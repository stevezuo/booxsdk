
#include "onyx/sys/locale_conf.h"

namespace sys
{

LocaleConfig::LocaleConfig()
{
}

LocaleConfig::~LocaleConfig()
{
}

bool LocaleConfig::makeSureTableExist(QSqlDatabase& database)
{
    QSqlQuery query(database);
    bool ok = query.exec("create table if not exists locale_conf ("
                         "key text primary key, "
                         "value text "
                         ")");
    if (ok)
    {
        return query.exec("create index if not exists key_index on locale_conf (key) ");
    }
    return false;

}

QLocale LocaleConfig::locale(QSqlDatabase& database)
{
    return QLocale::system();

    /*
    Not necessary anymore.
    QLocale ret = QLocale::system().name();
    QLocale::Language language = ret.language();
    QLocale::Country country = ret.country();

    QSqlQuery query(database);
    query.prepare( "select value from locale_conf where key = ?");
    query.addBindValue("language");
    if (query.exec() && query.next())
    {
        language = static_cast<QLocale::Language>(query.value(0).toInt());
    }

    query.prepare( "select value from locale_conf where key = ?");
    query.addBindValue("country");
    if (query.exec() && query.next())
    {
        country = static_cast<QLocale::Country>(query.value(0).toInt());
    }

    return QLocale(language, country);
    */
}

bool LocaleConfig::setLocale(QSqlDatabase & database,
                             const QLocale &locale)
{
    // The encoding is supported by glibc locale archive.
    QLocale::setDefault(locale);
    QString value = locale.name() + ".UTF-8";
    qputenv("LC_ALL", value.toAscii());
    qputenv("LANG", value.toAscii());

    // Actually, it's not really necessary to call setlocale(LC_ALL), as the
    // QSystemLocale reads settings from env. It's better to call setlocale
    // so that the other applications that use gettext directly can work well.
    setlocale(LC_ALL, value.toAscii().constData());
    QSystemLocale system; // Override system locale now.

    // write to /root/Settings/language. Need a better way to determind the
    // file path name. So far, just hardcode in application.
    const QString PATH = "/root/Settings/language";
    QFile file(PATH);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        qWarning("Could not open %s for writing", qPrintable(PATH));
        return false;
    }
    QByteArray data = value.toAscii().prepend("export LANG=");
    qDebug("Data writting to file %s", data.constData());
    file.write(data);
    file.flush();
    file.close();
    return true;

    /*
    Don't need to write to database anymore.
    QSqlQuery query(database);
    query.prepare( "INSERT OR REPLACE into locale_conf (key, value) values(?, ?)");
    query.addBindValue("language");
    query.addBindValue(locale.language());
    if (!query.exec())
    {
        return false;
    }

    query.prepare( "INSERT OR REPLACE into locale_conf (key, value) values(?, ?)");
    query.addBindValue("country");
    query.addBindValue(locale.country());
    return query.exec();
    */
}

}   // namespace sys
