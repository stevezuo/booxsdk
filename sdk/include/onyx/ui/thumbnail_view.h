#ifndef ONYX_THUMBNAIL_VIEW_H__
#define ONYX_THUMBNAIL_VIEW_H__

#include "ui.h"
#include "thumbnail_item.h"

namespace ui
{

enum ThumbnailElement
{
    THUMB_IMAGE = Qt::UserRole + 10
};

class ThumbnailView : public OnyxDialog
{
    Q_OBJECT
public:
    explicit ThumbnailView(QWidget *parent = 0);
    ~ThumbnailView(void);

    void clear();
    void attachModel(QStandardItemModel *model, QModelIndex index = QModelIndex());
    void updateThumbnail(QStandardItem *item);
    void jumpToScreen(int screen_index);

Q_SIGNALS:
    void needThumbnail(const QSize & size_limit, QStandardItem *item);
    void clicked(QStandardItem *selected_item);
    void escape();

    bool positionChanged(const int current, const int total);
    void needFullScreen(bool enable);

public Q_SLOTS:
    void onSlideClicked(const int percent, const int value);

protected Q_SLOTS:
    virtual void onCloseClicked();

private:
    // message handlers
    void paintEvent(QPaintEvent *pe);
    void resizeEvent(QResizeEvent *re);
    void mousePressEvent(QMouseEvent *me);
    void mouseMoveEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void keyReleaseEvent(QKeyEvent *ke);

    // paint elements
    void paintBoundingRect(QPainter & painter);

    void moveThumb(int thumb_idx);
    void nextScreen();
    void prevScreen();

    void requestNextWaitingThumbnails();
    void resetWaitingThumbnails();
    void resetPosition();

    bool updateScreens();
    bool createLayout();

    shared_ptr<ThumbnailItem> hitTest(QMouseEvent *me, int & idx);

private:
    typedef QVector<QStandardItem*>     DisplayedThumbs;
    typedef DisplayedThumbs::iterator   DisplayedThumbIter;

    typedef QSet<int>                   WaitingIndexes;
    typedef WaitingIndexes::iterator    WaitingIndexIter;

private:
    QStandardItemModel*     model_;
    DisplayedThumbs         displayed_thumbs_;
    WaitingIndexes          waiting_indexes_;
    QModelIndex             current_index_;

    scoped_ptr<QGridLayout>              thumbs_layout_;
    QVector< shared_ptr<ThumbnailItem> > thumbnails_;
    QPoint                  mouse_press_pos_;

    int                     current_screen_;
    int                     current_thumb_in_screen_;
    int                     total_screens_;
};

};  // namespace ui

#endif  // ONYX_THUMBNAIL_VIEW_H__
