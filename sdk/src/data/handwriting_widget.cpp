#include "onyx/data/handwriting_widget.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/data/sketch_proxy.h"
#ifdef BUILD_FOR_ARM
#include <QtGui/qwsdisplay_qws.h>
#include <QtGui/qscreen_qws.h>
#endif

using namespace sketch;

namespace handwriting
{

static const QString HAND_WRITING_PATH = "hand_writing";

static RotateDegree getSystemRotateDegree()
{
    int degree = 0;
#ifdef BUILD_FOR_ARM
    degree = QScreen::instance()->transformOrientation();
#endif
    return static_cast<RotateDegree>(degree);
}

HandwritingWidget::HandwritingWidget(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
    , sketch_proxy_(0)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);
}

HandwritingWidget::~HandwritingWidget()
{
}

void HandwritingWidget::paintEvent(QPaintEvent *pe)
{
    if (sketch_proxy_ != 0)
    {
        QPainter painter(this);

        // update zoom factor
        sketch_proxy_->setZoom(ZOOM_ACTUAL);
        sketch_proxy_->setContentOrient(ROTATE_0_DEGREE);
        sketch_proxy_->setWidgetOrient(getSystemRotateDegree());

        // draw sketches in this page
        // the page number of any image is 0
        sketch::PageKey page_key;
        page_key.setNum(0);
        QRect page_area(QPoint(0, 0), size());

        sketch_proxy_->updatePageDisplayRegion(HAND_WRITING_PATH, page_key, page_area);
        sketch_proxy_->paintPage(HAND_WRITING_PATH, page_key, painter);
    }
}

void HandwritingWidget::attachSketchProxy(SketchProxy *proxy)
{
    sketch_proxy_ = proxy;
    sketch_proxy_->setMode(MODE_SKETCHING);
    sketch_proxy_->attachWidget(this);
    updateSketchProxy();
}

void HandwritingWidget::deattachSketchProxy()
{
    if (sketch_proxy_ != 0)
    {
        sketch_proxy_->deattachWidget(this);
        sketch_proxy_ = 0;
    }
}

bool HandwritingWidget::event(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
        e->accept();
        return true;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        {
            QKeyEvent * ke = down_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Down ||
                ke->key() == Qt::Key_Up ||
                ke->key() == Qt::Key_Left ||
                ke->key() == Qt::Key_Right ||
                ke->key() == Qt::Key_PageDown ||
                ke->key() == Qt::Key_PageUp ||
                ke->key() == Qt::Key_Enter ||
                ke->key() == Qt::Key_Escape ||
                ke->key() == Qt::Key_VolumeDown ||
                ke->key() == Qt::Key_VolumeUp ||
                ke->key() == ui::Device_Menu_Key)
            {
                break;
            }
            e->accept();
            return true;
        }
    default:
        break;
    }
    return QWidget::event(e);
}

void HandwritingWidget::updateSketchProxy()
{
    if (sketch_proxy_ != 0)
    {
        // deactivate all pages
        sketch_proxy_->deactivateAll();

        // update zoom factor
        sketch_proxy_->setZoom(ZOOM_ACTUAL);
        sketch_proxy_->setContentOrient(ROTATE_0_DEGREE);
        sketch_proxy_->setWidgetOrient(getSystemRotateDegree());

        sketch::PageKey page_key;
        QRect page_area(QPoint(0, 0), size());

        // the page number of any image is 0
        page_key.setNum(0);
        sketch_proxy_->activatePage(HAND_WRITING_PATH, page_key);
        sketch_proxy_->updatePageDisplayRegion(HAND_WRITING_PATH, page_key, page_area);
    }
}

void HandwritingWidget::onClearSketches()
{
    // clear the sketches on the area
    if (sketch_proxy_ != 0)
    {
        sketch::PageKey page_key;
        page_key.setNum(0);
        sketch_proxy_->removePage(HAND_WRITING_PATH, page_key);
        updateSketchProxy();
    }
}

}
