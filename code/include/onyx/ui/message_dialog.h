
#ifndef ONYX_UI_MESSAGE_DIALOG_H_
#define ONYX_UI_MESSAGE_DIALOG_H_

#include <QtGui/QtGui>
#include "onyx_dialog.h"

namespace ui
{

class MessageDialog : public OnyxDialog
{
    Q_OBJECT
public:
    MessageDialog(QMessageBox::Icon icon,
                  const QString & title,
                  const QString & text,
                  QMessageBox::StandardButtons buttons = QMessageBox::NoButton,
                  QWidget * parent = 0);
    ~MessageDialog();

public:
    int exec();
    void updateInformation(const QString & text);

protected:
    bool event(QEvent * event);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    OnyxPushButton *button(int button_id);
    void resizeEvent(QResizeEvent *e);

private Q_SLOTS:
    void onButtonClicked();

private:
    void createLayout();
    void createButtons(QMessageBox::StandardButtons buttons);
    void addButton(OnyxPushButton *button);
    QPixmap pixmap(QMessageBox::Icon icon);
    void navigate(int key);

private:
    QMessageBox::Icon icon_type_;
    QMessageBox::StandardButtons buttons_;

    QVBoxLayout  content_vbox_;
    QHBoxLayout  info_hbox_;
    QHBoxLayout  button_hbox_;

    OnyxLabel info_text_;
    QList<OnyxPushButton *> button_list_;

};

};  // namespace ui

#endif  // ONYX_UI_MESSAGE_DIALOG_H_
