#ifndef ONYX_PASSWORD_WIDGET_H_
#define ONYX_PASSWORD_WIDGET_H_

#include "buttons.h"
#include "line_edit.h"
#include "onyx_dialog.h"
#include "keyboard.h"

namespace ui
{

/// Password widget to enable user to change search criteria.
class PasswordDialog : public OnyxDialog
{
    Q_OBJECT

public:
    PasswordDialog(QWidget *parent);
    ~PasswordDialog(void);

public:
    int popup(const QString &password);
    QString password();

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
    void onTextChanged(const QString& text);

private:
    void createLayout();

private:
    QHBoxLayout  hbox_;

    OnyxLineEdit text_edit_;    ///< Input edit.
    OnyxPushButton ok_button_;
    OnyxPushButton clear_button_;
    OnyxCheckBox   show_plain_text_;   ///< Show plain text or star.

    KeyBoard      keyboard_;     ///< Keyboard.
    QTimer        timer_;        ///< Timer to update the screen.
};


};  // namespace ui

#endif  // ONYX_PASSWORD_WIDGET_H_
