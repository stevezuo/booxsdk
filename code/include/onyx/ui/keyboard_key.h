#ifndef ONYX_KEYBOARD_KEY_H_
#define ONYX_KEYBOARD_KEY_H_

#include "keyboard_utils.h"

namespace ui
{

class KeyboardLayout;
class KeyBoardKey : public QPushButton
{
    Q_OBJECT
public:
    KeyBoardKey(KeyboardLayout *layout, QWidget *parent = 0);
    virtual ~KeyBoardKey();

    void setCode(const int code, const int location, const KeyboardDirection direction);
    inline int code();
    inline void setDirection(KeyboardDirection direction) { direction_ = direction; }

    bool isShiftKey();

Q_SIGNALS:
    void directionKeyPressed(KeyboardDirection direction);

public Q_SLOTS:
    void onShifted(bool);
    void onCapLocked(bool);
    void onDisplayArrow(bool display);

protected:
    bool event(QEvent*);
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

private:
    void updateText();
    void handleLayoutBySpecialChar(bool shifted);

private:
    bool              locked_;
    bool              shifted_;
    bool              display_arrow_;
    int               code_;
    int               shift_code_;
    KeyboardDirection direction_;
    KeyboardLayout    *layout_;
};

inline int KeyBoardKey::code()
{
    return shift_code_;
}

}   // namespace ui

#endif      // ONYX_KEYBOARD_KEY_H_
