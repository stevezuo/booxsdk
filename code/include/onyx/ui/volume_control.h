#ifndef ONYX_VOLUME_CONTROL_H_
#define ONYX_VOLUME_CONTROL_H_

#include <QMap>
#include <QImage>
#include "onyx/base/base.h"
#include "ui_global.h"

namespace ui
{

class VolumeControlDialog : public QDialog
{
    Q_OBJECT

public:
    VolumeControlDialog(QWidget *parent);
    ~VolumeControlDialog();

public Q_SLOTS:
    int ensureVisible();
    virtual void done(int r);
    void onScreenUpdateRequest();

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

public Q_SLOTS:
    void setVolume(int volume, bool is_mute);
    void resetTimer();

Q_SIGNALS:
    void finished(const int value, const int total);

private Q_SLOTS:
    void stopTimer();
    void onTimeout();

public:
    QHBoxLayout        layout_;
    QImage image_;

    int current_;
    int min_;
    int max_;
    int pressing_value_;
    QTimer timer_;
    QMap<int, int>     map_;

    QLabel label_;
};

};  // namespace ui

#endif    // ONYX_VOLUME_CONTROL_H_
