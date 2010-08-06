#ifndef ONYX_TIME_ZONE_DIALOG_H_
#define ONYX_TIME_ZONE_DIALOG_H_

#include "tree_view_dialog.h"

namespace ui
{

class TimeZoneDialog : public TreeViewDialog
{
    Q_OBJECT
public:
    explicit TimeZoneDialog(QWidget *parent);
    ~TimeZoneDialog(void);

public:
    int popup(const QString & title = "", const QModelIndex &idx = QModelIndex());
    QString selectedTimeZone();

private:
    QStandardItemModel * getTimeZones();

private:
    QStandardItemModel time_zone_model_;
    NO_COPY_AND_ASSIGN(TimeZoneDialog);
};

} //namespace ui

#endif  // ONYX_TIME_ZONE_DIALOG_H_
