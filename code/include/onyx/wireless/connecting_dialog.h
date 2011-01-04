
#ifndef UI_CONNECTING_DIALOG_H_
#define UI_CONNECTING_DIALOG_H_

#include <QtGui/QtGui>
#include <QHostInfo>
#include "ap_item.h"


using namespace sys;

namespace ui
{

/// popup when CONNECT_TO_DEFAULT_APN is on.
class ConnectingDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectingDialog(QWidget *parent, SysStatus & sys);
    ~ConnectingDialog();

public:
    int  popup(bool show_profile = true);
    void connect(const QString & file, const QString & username, const QString &password);

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual bool event(QEvent * event);

private Q_SLOTS:
    void onPppConnectionChanged(const QString &, int);
    void onCloseClicked();

private:
    void createLayout();

private:
    QVBoxLayout content_layout_;
    OnyxLabel state_widget_;
    SysStatus & sys_;
};

}

#endif // UI_WIFI_DIALOG_H_
