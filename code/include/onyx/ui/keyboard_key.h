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

    void setCode(const int code, const int direction);
    inline int code();

    bool isShiftKey();

public Q_SLOTS:
    void onShifted(bool);
    void onCapLocked(bool);

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
    bool  locked_;
    bool  shifted_;
    int   code_;
    int   shift_code_;
    KeyboardLayout *layout_;
};

inline int KeyBoardKey::code()
{
    return shift_code_;
}

}   // namespace ui

#endif      // ONYX_KEYBOARD_KEY_H_
