// Authors: John

/// Public header of the system configuration library.
#include "onyx/sys/page_turning_conf.h"


namespace sys
{
int PageTurningConfig::direction_ = 1;
int PageTurningConfig::THRESHOLD = 30;

/// Page turning configuration. By default, from right to left or
/// from bottom to top is interpreted as next.
PageTurningConfig::PageTurningConfig()
{
}

PageTurningConfig::~PageTurningConfig()
{
}

bool PageTurningConfig::makeSureTableExist(QSqlDatabase& database)
{
    QSqlQuery query(database);
    return query.exec("create table if not exists page_turning_conf ("
                      "key text primary key, "
                      "value text "
                      ")");
}

bool PageTurningConfig::load(QSqlDatabase& db)
{
    // TODO.
    return true;
}

bool PageTurningConfig::save(QSqlDatabase& db)
{
    // TODO.
    return true;
}

int PageTurningConfig::direction(const QPoint & old_position, const QPoint & new_position)
{
    int delta_x = new_position.x() - old_position.x();
    int delta_y = new_position.y() - old_position.y();

    int dist = std::max(abs(delta_x), abs(delta_y));
    if (dist < distance())
    {
        return 0;
    }

    int delta = 0;
    if (abs(delta_x) > abs(delta_y))
    {
        delta = delta_x;
    }
    else
    {
        delta = delta_y;
    }

    if (delta < -THRESHOLD)
    {
        return direction_;
    }
    else if (delta > THRESHOLD)
    {
        return -direction_;
    }
    return 0;
}

void PageTurningConfig::setDirection(int conf)
{
    if (conf > 0)
    {
        direction_ = 1;
    }
    else if (conf < 0)
    {
        direction_ = -1;
    }
}

/// Return the distance threshold.
int PageTurningConfig::distance()
{
    return THRESHOLD;
}

}   // namespace sys.
