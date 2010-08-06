#ifndef WIFI_AP_ITEM_H_
#define WIFI_AP_ITEM_H_

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include "onyx/ui/ui.h"
#include "onyx/sys/sys.h"

using namespace sys;

namespace ui
{

class WifiTitleItem : public QWidget
{
    Q_OBJECT

public:
    WifiTitleItem(QWidget *parnet);
    ~WifiTitleItem();

public Q_SLOTS:
    void setState(const QString &);
    void onCustomizedClicked(bool);

Q_SIGNALS:
    void refreshClicked();
    void customizedClicked();

protected:
    virtual bool event(QEvent *e);
    void createLayout();

private:
    QHBoxLayout layout_;
    OnyxLabel title_label_;
    OnyxPushButton customize_button_;
    OnyxPushButton refresh_button_;
};


/// Represents a wifi access point. It displays the access point
/// state to end user.
class WifiAPItem : public QWidget
{
    Q_OBJECT

public:
    WifiAPItem(QWidget *parent);
    ~WifiAPItem(void);

public:
    void setProfile(WifiProfile & profile);
    WifiProfile & profile();

Q_SIGNALS:
    void clicked(WifiProfile & profile);
    void config(WifiProfile & profile);

protected:
    virtual bool event(QEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    void createLayout();
    void updateByProfile(WifiProfile & profile);
    bool isSelected();

private Q_SLOTS:
    void onConfigButtonClicked();

private:
    QHBoxLayout hor_layout_;
    OnyxLabel ssid_label_;
    OnyxPushButton config_button_;
    OnyxLabel lock_icon_label_;

    static const int SIGNAL_ICONS = 5;
    OnyxLabel signal_labels_[SIGNAL_ICONS];
    WifiProfile profile_;


    static WifiAPItem *selected_item_;
    static scoped_ptr<QPixmap> selected_pixmap_;
};

typedef QVector<WifiAPItem *> WifiAPItems;
typedef WifiAPItems::iterator WifiAPItemsIter;

}    // namespace ui

#endif
