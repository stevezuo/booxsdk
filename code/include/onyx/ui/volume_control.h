#ifndef ONYX_VOLUME_CONTROL_H_
#define ONYX_VOLUME_CONTROL_H_

#include "onyx/base/base.h"
#include "ui_global.h"

namespace ui
{

class VolumeControl : public QWidget
{
    Q_OBJECT

public:
    VolumeControl(QWidget *parent);
    virtual ~VolumeControl(void);

public Q_SLOTS:
    void onVolumeChanged(int volume, bool is_mute);

Q_SIGNALS:
    void clicked(const int percent, const int value);
    void changing(const int value, const int total);

private Q_SLOTS:
    void onTimeout();
    void onClicked(const int percent, const int value);
    void onChanged(const int value, const int total);

private:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void resizeEvent(QResizeEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *);

    void createLayout();
    void updatefgPath(int value);
    void updatePath(QPainterPath & path, const QRect & rect);

private:
    int current_;
    int min_;
    int max_;
    int pressing_value_;
    QTimer timer_;

    QPainterPath bk_path_;
    QPainterPath fg_path_;
};

class VolumeControlDialog : public QDialog
{
    Q_OBJECT
    VolumeControlDialog();
    VolumeControlDialog(QWidget *parent);
public:
    ~VolumeControlDialog();
    static VolumeControlDialog * instance()
    {
        static VolumeControlDialog dialog(0);
        return &dialog;
    }

    inline void setAlwaysActive(bool always_active) { always_active_ = always_active; }
    inline bool alwaysActive() { return always_active_; }
    void ensureVisible();

protected:
    bool event(QEvent *e);
    void mousePressEvent(QMouseEvent *me);
    void mouseMoveEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void keyPressEvent(QKeyEvent *ke);
    void keyReleaseEvent(QKeyEvent *ke);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    void createLayout();

public:
    QHBoxLayout        layout_;
    VolumeControl      control_;
    bool               update_parent_;
    bool               always_active_;
};

};  // namespace ui

#endif    // ONYX_VOLUME_CONTROL_H_
