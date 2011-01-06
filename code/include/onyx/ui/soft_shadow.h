#ifndef ONYX_SOFT_SHADOW_H_
#define ONYX_SOFT_SHADOW_H_

#include <QtGui/QtGui>

namespace ui
{

/// Soft shadow for eink device.
class OnyxShadowWidget : public QDialog
{
public:
    OnyxShadowWidget(QWidget *parent, Qt::Orientation o);
    ~OnyxShadowWidget();

protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent * event);

private:
    QPainterPath getHorizontalPath();
    QPainterPath getVerticalPath();

private:
    Qt::Orientation orient_;
};

/// Reprents horizontal and vertical shadows.
class Shadows : public QObject
{
public:
    Shadows(QWidget *parent);
    ~Shadows();

public:
    void show(bool show = true);
    void onWidgetMoved(QWidget *sibling);
    void onWidgetResized(QWidget *sibling);

    OnyxShadowWidget & hor_shadow() { return hor_shadow_; }
    OnyxShadowWidget & ver_shadow() { return ver_shadow_; }

public:
    static const int PIXELS;

private:
    OnyxShadowWidget hor_shadow_;
    OnyxShadowWidget ver_shadow_;

};

};  // namespace ui

#endif  // ONYX_SOFT_SHADOW_H_
