#include "onyx/sys/sys.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/time_zone_dialog.h"

namespace ui
{

TimeZoneDialog::TimeZoneDialog(QWidget *parent)
    : TreeViewDialog(parent)
{
    setTitleIcon(":/images/time_zone.png");
}

TimeZoneDialog::~TimeZoneDialog(void)
{
}

QString TimeZoneDialog::selectedTimeZone()
{
    QString time_zone;
    QModelIndex index = selectedItem();
    if (index.isValid())
    {
        QStandardItemModel * time_zones = &time_zone_model_;
        QStandardItem *item = time_zones->itemFromIndex( index );
        time_zone = item->data().toString();
    }
    return time_zone;
}

int TimeZoneDialog::popup(const QString &title,
                          const QModelIndex &index)
{
    QStandardItemModel * time_zones = getTimeZones();
    setModel(time_zones);
    if (index.isValid())
    {
        selected_ = index;
    }
    else
    {
        QString time_zone = sys::SystemConfig::currentTimezone();
        for (int i = 0; i < time_zones->rowCount(); ++i)
        {
            QStandardItem *item = time_zones->item(i);
            if (item != 0 && time_zone.contains(item->data().toString()))
            {
                selected_ = time_zones->indexFromItem(item);
            }
        }
    }

    title_label_.setText(title);
    status_bar_.setProgress(tree_.currentPage(), tree_.pages());
    tree_.select(selected_);

    showMaximized();
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC);
    return exec();
}

QStandardItemModel* TimeZoneDialog::getTimeZones()
{
    QStringList time_zone_paths;
    QStringList time_zone_names;
    if (time_zone_paths.empty())
    {
        time_zone_paths.push_back("Etc/GMT+12");
        time_zone_paths.push_back("Etc/GMT+11");
        time_zone_paths.push_back("Etc/GMT+10");
        time_zone_paths.push_back("Etc/GMT+10_DST");
        time_zone_paths.push_back("Etc/GMT+9");
        time_zone_paths.push_back("Etc/GMT+9_DST");
        time_zone_paths.push_back("Etc/GMT+8");
        time_zone_paths.push_back("Etc/GMT+8_DST");
        time_zone_paths.push_back("Etc/GMT+7");
        time_zone_paths.push_back("Etc/GMT+7_DST");
        time_zone_paths.push_back("Etc/GMT+6");
        time_zone_paths.push_back("Etc/GMT+6_DST");
        time_zone_paths.push_back("Etc/GMT+5");
        time_zone_paths.push_back("Etc/GMT+5_DST");
        time_zone_paths.push_back("Etc/GMT+4");
        time_zone_paths.push_back("Etc/GMT+4_DST");
        time_zone_paths.push_back("Etc/GMT+3");
        time_zone_paths.push_back("Etc/GMT+3_DST");
        time_zone_paths.push_back("Etc/GMT+2");
        time_zone_paths.push_back("Etc/GMT+1");
        time_zone_paths.push_back("Etc/GMT+1_DST");
        time_zone_paths.push_back("Etc/Greenwich");
        time_zone_paths.push_back("Etc/Greenwich_DST");
        time_zone_paths.push_back("Etc/GMT-1");
        time_zone_paths.push_back("Etc/GMT-1_DST");
        time_zone_paths.push_back("Etc/GMT-2");
        time_zone_paths.push_back("Etc/GMT-2_DST");
        time_zone_paths.push_back("Etc/GMT-3");
        time_zone_paths.push_back("Etc/GMT-3_DST");
        time_zone_paths.push_back("Etc/GMT-4");
        time_zone_paths.push_back("Etc/GMT-4_DST");
        time_zone_paths.push_back("Etc/GMT-5");
        time_zone_paths.push_back("Etc/GMT-5_DST");
        time_zone_paths.push_back("Etc/GMT-6");
        time_zone_paths.push_back("Etc/GMT-6_DST");
        time_zone_paths.push_back("Etc/GMT-7");
        time_zone_paths.push_back("Etc/GMT-7_DST");
        time_zone_paths.push_back("Etc/GMT-8");
        time_zone_paths.push_back("Etc/GMT-8_DST");
        time_zone_paths.push_back("Etc/GMT-9");
        time_zone_paths.push_back("Etc/GMT-9_DST");
        time_zone_paths.push_back("Etc/GMT-10");
        time_zone_paths.push_back("Etc/GMT-10_DST");
        time_zone_paths.push_back("Etc/GMT-11");
        time_zone_paths.push_back("Etc/GMT-11_DST");
        time_zone_paths.push_back("Etc/GMT-12");
        time_zone_paths.push_back("Etc/GMT-12_DST");
        time_zone_paths.push_back("Etc/GMT-13");
    }

    if (time_zone_names.empty())
    {
        time_zone_names.push_back(QCoreApplication::tr("GMT-12:00 International Date Line West"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-11:00 Midway Island, Samoa"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-10:00 Hawaii"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-10:00 Hawaii (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-09:00 Alaska"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-09:00 Alaska (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-08:00 Tijuana, Baja California"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-08:00 Tijuana, Baja California (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-07:00 Arizona"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-07:00 Arizona (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-06:00 Central America"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-06:00 Central America (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-05:00 Bogota, Lima, Quito, Rio Branco"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-05:00 Bogota, Lima, Quito, Rio Branco (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-04:00 Atlantic Time(Canada)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-04:00 Atlantic Time(Canada) (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-03:00 Brasilia"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-03:00 Brasilia (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-02:00 Mid-Atlantic"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-01:00 Azores"));
        time_zone_names.push_back(QCoreApplication::tr("GMT-01:00 Azores (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT Greenwich Mean Time"));
        time_zone_names.push_back(QCoreApplication::tr("GMT Greenwich Mean Time (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+01:00 Amsterdam, Berlin, Bern, Rome"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+01:00 Amsterdam, Berlin, Bern, Rome (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+02:00 Amman"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+02:00 Amman (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+03:00 Baghdad"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+03:00 Baghdad (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+04:00 Abu Dhabi, Muscat"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+04:00 Abu Dhabi, Muscat (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+05:00 Tashkent"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+05:00 Tashkent (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+06:00 Almaty, Novosibirsk"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+06:00 Almaty, Novosibirsk (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+07:00 Bangkok, Hanoi, Jakarta"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+07:00 Bangkok, Hanoi, Jakarta (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+08:00 Beijing, Chongqing, Hong Kong, Urumqi"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+08:00 Beijing, Chongqing, Hong Kong, Urumqi (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+09:00 Osaka, Sapporo, Tokyo"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+09:00 Osaka, Sapporo, Tokyo (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+10:00 Canberra, Melbourne, Sydney"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+10:00 Canberra, Melbourne, Sydney (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+11:00 Magadan, Solomon Is., New Caledonia"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+11:00 Magadan, Solomon Is., New Caledonia (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+12:00 Auckland, Wellington"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+12:00 Auckland, Wellington (Daylight Savings Time)"));
        time_zone_names.push_back(QCoreApplication::tr("GMT+13:00 Nuku'alofa"));
    }

    time_zone_model_.clear();
    if (time_zone_model_.rowCount() <= 0)
    {
        time_zone_model_.setColumnCount(1);
        for(int i = 0; i < time_zone_paths.size(); ++i)
        {
            QStandardItem *time_zone_item = new QStandardItem( time_zone_names.at(i) );
            time_zone_item->setData( time_zone_paths.at(i) );
            time_zone_item->setEditable( false );
            time_zone_model_.setItem( i, 0, time_zone_item );
        }
        time_zone_model_.setHeaderData(0, Qt::Horizontal, QVariant::fromValue(QCoreApplication::tr("Time Zone")), Qt::DisplayRole);
    }
    return &time_zone_model_;
}

} // namespace ui
