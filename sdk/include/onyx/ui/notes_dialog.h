#ifndef ONYX_NOTES_DIALOG_H_
#define ONYX_NOTES_DIALOG_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"
#include "keyboard.h"

namespace ui
{

class NotesDialog : public OnyxDialog
{
    Q_OBJECT
public:
    NotesDialog(const QString & text = QString(), QWidget *parent = 0);
    ~NotesDialog();

    int popup(const QString & text);
    QSize sizeHint() const;
    QSize minimumSize () const;
    QSize maximumSize () const;

    QString inputText() const;

protected:
    void mouseMoveEvent(QMouseEvent *me);
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void keyReleaseEvent(QKeyEvent *);
    void keyPressEvent(QKeyEvent * ke);
    bool event(QEvent * event);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);
    void hideEvent(QHideEvent * e);

Q_SIGNALS:
    void closeClicked();
    void okClicked(const QString & content);

private:
    void createLayout();
    bool eventFilter(QObject *obj, QEvent *event);
    bool adjustPosition();

private Q_SLOTS:
    void onCloseClicked();
    void onOKClicked();

private:
    QHBoxLayout     hbox_;
    QVBoxLayout     vbox1_;

    OnyxPushButton  ok_button_;
    OnyxPushButton  clear_button_;

    OnyxTextEdit    content_;   ///< The lookup result.
    KeyBoard        keyboard_;  ///< Keyboard.

    QButtonGroup    button_group_;
};

};  // namespace ui

#endif // ONYX_NOTES_DIALOG_H_
