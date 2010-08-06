#ifndef ONYX_SIGN_IN_DIALOG_H_
#define ONYX_SIGN_IN_DIALOG_H_

#include "onyx_dialog.h"
#include "label.h"
#include "line_edit.h"
#include "buttons.h"
#include "keyboard.h"

namespace ui
{

class SignInDialog : public OnyxDialog
{
    Q_OBJECT
public:
    SignInDialog(QWidget *parent, const QString & title);
    ~SignInDialog(void);

public:
    int popup(const QString & id, const QString & password);
    QString password();
    QString id();

protected:
    void mouseMoveEvent(QMouseEvent *me);
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void keyReleaseEvent(QKeyEvent *);
    void keyPressEvent(QKeyEvent * ke);
    bool event(QEvent * event);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
    void onShowPlainTextClicked();
    void onOKClicked();
    void onCloseClicked();
    void onTimeout();
    void onIDChanged(const QString & text);
    void onPasswordChanged(const QString & text);
    void onGetFocus(OnyxLineEdit *object);

private:
    void createLayout();
    void setReceiver(QWidget *receiver) { keyboard_receiver_ = receiver; }
    QWidget * receiver() { return keyboard_receiver_; }

private:
    QGridLayout     input_layout_;
    QHBoxLayout     hbox_;

    OnyxLabel       id_label_;
    OnyxLineEdit    id_edit_;           ///< Input ID

    OnyxLabel       password_label_;
    OnyxLineEdit    password_edit_;     ///< Input Password
    OnyxPushButton  ok_button_;
    OnyxPushButton  clear_button_;
    OnyxCheckBox    show_plain_text_;   ///< Show plain text or star.
    QWidget         *keyboard_receiver_;

    KeyBoard        keyboard_;          ///< Keyboard.
    QTimer          timer_;             ///< Timer to update the screen.
};

};

#endif // ONYX_SIGN_IN_DIALOG
