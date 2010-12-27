#ifndef ONYX_KEYBOARD_DIRECTION_DIALOG_H_
#define ONYX_KEYBOARD_DIRECTION_DIALOG_H_

#include "onyx/base/base.h"
#include "buttons.h"

namespace ui
{

enum KeyboardDirection
{
    KEYBOARD_UP = 0,
    KEYBOARD_DOWN,
    KEYBOARD_LEFT,
    KEYBOARD_RIGHT,
    KEYBOARD_CENTER
};

class KeyboardLayout;
class KeyboardDirectionDialog : public QDialog
{
    Q_OBJECT
public:
    KeyboardDirectionDialog(KeyboardDirection direction, QWidget *parent);
    virtual ~KeyboardDirectionDialog();

    void ensureVisible(KeyboardLayout *layout, bool show_widget);

Q_SIGNALS:
    void directionSelected(KeyboardDirection direction);

private Q_SLOTS:
    void onDirectionClicked(bool);

private:
    void createLayout();

    void keyPressEvent(QKeyEvent *ke);
    void keyReleaseEvent(QKeyEvent *ke);
    void paintEvent(QPaintEvent *pe);

    QPoint getDisplayPosition(KeyboardLayout *layout);
    QRect getKeyboardArea();

private:
    QHBoxLayout        layout_;
    ui::OnyxPushButton direction_button_;
    QIcon              direction_icon_;
    KeyboardDirection  direction_;
};

};

#endif
