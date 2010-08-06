#include "onyx/ui/thumbnail_view.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/sys_conf.h"
#include "onyx/cms/content_thumbnail.h"

Q_DECLARE_METATYPE( QImage );

namespace ui
{

static const int NUM_ROW       = 2;
static const int NUM_COLUMN    = 2;

ThumbnailView::ThumbnailView(QWidget *parent)
: OnyxDialog(parent)
, model_(0)
, thumbs_layout_(new QGridLayout(&content_widget_))
{
    resetPosition();
    createLayout();
}

ThumbnailView::~ThumbnailView(void)
{
    clear();
}

void ThumbnailView::clear()
{
    thumbnails_.clear();
}

void ThumbnailView::resetPosition()
{
    current_screen_ = 0;
    current_thumb_in_screen_ = 0;
    total_screens_ = 0;
}

bool ThumbnailView::createLayout()
{
    clear();

    // TODO. set the title and title icon in parent application
    updateTitle(QApplication::tr("History"));
    updateTitleIcon(QPixmap(":/images/about_dialog_icon.png"));

    content_widget_.setBackgroundRole(QPalette::Button);

    thumbs_layout_->setSpacing(THUMB_SPACING);
    for (int r = 0; r < NUM_ROW; ++r)
    {
        for (int c = 0; c < NUM_COLUMN; ++c)
        {
            shared_ptr<ThumbnailItem> thumb(new ThumbnailItem(0));
            thumbnails_.push_back(thumb);
            thumbs_layout_->addLayout(thumb->layout(), r, c);
        }
    }
    return true;
}

void ThumbnailView::attachModel(QStandardItemModel *model, QModelIndex index)
{
    if (model == 0)
    {
        qWarning("Invalid Model");
        return;
    }

    resetPosition();
    model_         = model;
    current_index_ = index;

    if (!current_index_.isValid() && model_->rowCount() > 0)
    {
        QStandardItem * first = model_->item(0);
        current_index_ = model_->indexFromItem(first);
    }

    resetWaitingThumbnails();
}

void ThumbnailView::updateThumbnail(QStandardItem *item)
{
    int index = displayed_thumbs_.indexOf(item);
    if (index < 0)
    {
        qDebug("The returned thumbnial is out of date");
        return;
    }

    QVariant img_data = item->data(THUMB_IMAGE);
    if (!img_data.isValid())
    {
        return;
    }

    // update the image of thumbnail
    thumbnails_[index]->setImage(img_data.value< QImage >(), true);

    WaitingIndexIter waiting_idx = waiting_indexes_.find(index);
    if (waiting_idx != waiting_indexes_.end())
    {
        waiting_indexes_.erase(waiting_idx);
        requestNextWaitingThumbnails();
    }
}

void ThumbnailView::requestNextWaitingThumbnails()
{
    WaitingIndexIter begin = waiting_indexes_.begin();
    if (begin == waiting_indexes_.end())
    {
        return;
    }

    /*QSize size_limit = thumbnails_[*begin]->imageRect().size();
    size_limit -= QSize(THUMB_SPACING, THUMB_SPACING);*/
    QSize size_limit = thumbnailSize(cms::THUMBNAIL_HUGE);
    emit needThumbnail(size_limit,
                       displayed_thumbs_[*begin]);
}

bool ThumbnailView::updateScreens()
{
    if (model_ == 0)
    {
        return false;
    }

    if (!current_index_.isValid() && model_->rowCount() > 0)
    {
        QStandardItem * first = model_->item(0);
        current_index_ = model_->indexFromItem(first);
    }

    bool need_update = false;
    int thumb_num = NUM_ROW * NUM_COLUMN;

    int row = current_index_.row();
    int next_screen = row / thumb_num;
    need_update = (next_screen != current_screen_);
    current_screen_ = next_screen;

    int row_count = model_->rowCount();
    int total_screens = total_screens_;
    if (row_count % thumb_num != 0)
    {
        total_screens = row_count / thumb_num + 1;
    }
    else
    {
        total_screens = row_count / thumb_num;
    }
    need_update |= (total_screens != total_screens_);
    total_screens_ = total_screens;

    current_thumb_in_screen_ = row - (current_screen_ * thumb_num);
    return need_update;
}

void ThumbnailView::jumpToScreen(int screen_index)
{
    if (model_ == 0)
    {
        return;
    }

    int thumb_num = NUM_ROW * NUM_COLUMN;
    int first_thumb_index = screen_index * thumb_num;
    QStandardItem * item = model_->item(first_thumb_index);
    if (item == 0)
    {
        qDebug("Invalid Screen");
        return;
    }

    current_index_ = model_->indexFromItem(item);
    resetWaitingThumbnails();
}

/// Reset the displaying thumbnails by current index
void ThumbnailView::resetWaitingThumbnails()
{
    if (model_ == 0)
    {
        return;
    }

    // update the screen information
    bool need_full_update = updateScreens();

    // update page bar
    if (need_full_update)
    {
        emit positionChanged(current_screen_ + 1, total_screens_);
    }

    int num   = NUM_ROW * NUM_COLUMN;
    int first = current_screen_ * num;
    int end   = (current_screen_ + 1) * num;
    int total_rows = model_->rowCount();
    if (end > total_rows)
    {
        end = total_rows;
    }

    if (need_full_update)
    {
        displayed_thumbs_.clear();
        waiting_indexes_.clear();

        // update the whole screen
        for (int i = first; i < end; ++i)
        {
            QStandardItem * display_item = model_->item(i);
            displayed_thumbs_.push_back(display_item);

            thumbnails_[i - first]->setText(display_item->text());

            QVariant img_data = display_item->data(THUMB_IMAGE);
            if (!img_data.isValid())
            {
                waiting_indexes_.insert(i - first);
            }
            else
            {
                thumbnails_[i - first]->setImage(img_data.value< QImage >(), false);
            }
        }

        if ((end - first) < num)
        {
            // clear the images of empty thumbnail widgets
            for (int empty_idx = (end - first); empty_idx < num; ++empty_idx)
            {
                thumbnails_[empty_idx]->setImage(QImage(), false);
                thumbnails_[empty_idx]->setText("");
            }
        }
        content_widget_.update();
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
    }
    else
    {
        // fastest update the screen
        content_widget_.update();
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::DW);
    }

    requestNextWaitingThumbnails();
}

void ThumbnailView::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    paintBoundingRect(painter);
}

void ThumbnailView::paintBoundingRect(QPainter & painter)
{
    if ( current_thumb_in_screen_ < 0 ||
         current_thumb_in_screen_ >= NUM_ROW * NUM_COLUMN )
    {
        return;
    }

    // draw the bounding rectangle of current page
    painter.drawRect(thumbnails_[current_thumb_in_screen_]->imageRect());
}

void ThumbnailView::resizeEvent(QResizeEvent *re)
{
    if (!re->size().isValid())
    {
        return;
    }

    resetWaitingThumbnails();
}

void ThumbnailView::mousePressEvent(QMouseEvent *me)
{
    switch (me->button())
    {
    case Qt::LeftButton:
        {
            mouse_press_pos_ = me->pos();
        }
        break;
    default:
        break;
    }
    me->accept();
}

void ThumbnailView::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void ThumbnailView::mouseReleaseEvent(QMouseEvent *me)
{
    switch (me->button())
    {
    case Qt::LeftButton:
        {
            if (!mouse_press_pos_.isNull())
            {
                int sys_offset = sys::SystemConfig::direction(mouse_press_pos_, me->pos());
                if (sys_offset != 0)
                {
                    sys_offset > 0 ? nextScreen() : prevScreen();
                    me->accept();
                    return;
                }
            }

            int thumb_idx = 0;
            shared_ptr<ThumbnailItem> thumb = hitTest(me, thumb_idx);
            if (thumb.get() != 0 && thumb_idx < displayed_thumbs_.size())
            {
                QStandardItem * item = displayed_thumbs_[thumb_idx];
                emit clicked(item);
            }
        }
        break;
    case Qt::RightButton:
        {
        }
        break;
    default:
        break;
    }
    me->accept();
}

void ThumbnailView::keyReleaseEvent(QKeyEvent *ke)
{
    switch(ke->key())
    {
    case Qt::Key_PageDown:
        {
            nextScreen();
        }
        break;
    case Qt::Key_PageUp:
        {
            prevScreen();
        }
        break;
    case Qt::Key_Left:
        {
            int prev = current_index_.row() - 1;
            moveThumb(prev);
        }
        break;
    case Qt::Key_Right:
        {
            int next = current_index_.row() + 1;
            moveThumb(next);
        }
        break;
    case Qt::Key_Up:
        {
            int up = current_index_.row() - NUM_COLUMN;
            moveThumb(up);
        }
        break;
    case Qt::Key_Down:
        {
            int down = current_index_.row() + NUM_COLUMN;
            moveThumb(down);
        }
        break;
    case Qt::Key_Escape:
        {
            onCloseClicked();
        }
        break;
    case Qt::Key_Return:
        {
            QStandardItem * cur_item = model_->itemFromIndex(current_index_);
            emit clicked(cur_item);
        }
        break;
    default:
        break;
    }
    ke->accept();
}

void ThumbnailView::onCloseClicked()
{
    emit escape();
}

void ThumbnailView::moveThumb(int thumb_idx)
{
    if (model_ == 0)
    {
        return;
    }

    if (thumb_idx < 0 || thumb_idx >= model_->rowCount())
    {
        qDebug("Reach the terminal thumbnail");
        return;
    }

    QStandardItem * item = model_->item(thumb_idx);
    current_index_ = model_->indexFromItem(item);
    resetWaitingThumbnails();
}

void ThumbnailView::nextScreen()
{
    jumpToScreen(current_screen_ + 1);
}

void ThumbnailView::prevScreen()
{
    jumpToScreen(current_screen_ - 1);
}

void ThumbnailView::onSlideClicked(const int percent, const int value)
{
    jumpToScreen(value - 1);
}

shared_ptr<ThumbnailItem> ThumbnailView::hitTest(QMouseEvent *me, int & idx)
{
    QPoint mouse_pos = me->pos();
    for (int i = 0; i < thumbnails_.size(); ++i)
    {
        if (thumbnails_[i]->rect().contains(mouse_pos))
        {
            idx = i;
            return thumbnails_[i];
        }
    }
    return shared_ptr<ThumbnailItem>();
}

}
