// -*- mode: c++; c-basic-offset: 4; -*-

#include <QtGui/QtGui>

#include "onyx/application.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/sys_status.h"

class MyWidget : public QWidget
{
public:
    MyWidget(QWidget *parent = 0) : QWidget(parent) {}
    ~MyWidget() {}

  public:
    void refreshScreen() {
        onyx::screen::ScreenProxy::instance().flush(this, onyx::screen::ScreenProxy::GC);
    }

private:
    void paintEvent(QPaintEvent *e) {
        QPainter painter(this);
        QFont font("", 20, QFont::Normal);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, tr("Hello World From Onyx."));
    }

    bool event(QEvent * event)
    {
        bool ret = QWidget::event(event);
        if (event->type() == QEvent::UpdateRequest)
        {
            refreshScreen();
        }
        return ret;
    }

    void keyReleaseEvent(QKeyEvent *ke)
    {
        ke->accept();
        if (ke->key() == Qt::Key_Escape)
        {
            qApp->exit();
        }
    }
};

class MyApplication : public onyx::Application {
  public:
    MyApplication() : widget_(NULL) {}

    virtual ~MyApplication() {
        delete widget_;
    }

    virtual int exec() {
        sys::SysStatus::instance().setSystemBusy(false); 
        widget_ = new MyWidget;
        widget_->show();
        widget_->refreshScreen();
        return 0;
    }
  private:
    MyWidget* widget_;
};

ONYX_APP_CLASS(MyApplication)
