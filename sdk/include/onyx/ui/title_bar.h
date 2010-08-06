#ifndef ONYX_TITLE_BAR_H_
#define ONYX_TITLE_BAR_H_

#include <QtGui/QtGui>

namespace ui
{

/// Title bar for eink device. Remove unnecessary updates.
/// It contains a image and necessary text.
class OnyxTitleBar : public QWidget
{
public:
    OnyxTitleBar(const QImage & image, QWidget *parent);
    ~OnyxTitleBar();

    /// The function is necessary as the title may be changed when
    /// it's visible.
    void setTitle(const QString & title);
    QString title() const;

    QFont & font() { return font_; }

protected:
    void paintEvent(QPaintEvent *pe);

private:
    void updateLayout();

private:
    QFont font_;
    QImage image_;
    QPoint image_pos_;
    QTextLayout text_layout_;
    bool layout_dirty_;
};

};  // namespace ui

#endif  // ONYX_TITLE_BAR_H_
