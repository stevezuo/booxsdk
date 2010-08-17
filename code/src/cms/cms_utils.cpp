// Author: John

#include "onyx/cms/cms_utils.h"

namespace cms
{

static const QString DATABASE_DIR  = ".odb";
static const QString DATABASE_NAME = "content.db";
static const QString SKETCH_POSTFIX = ".sketch";

const QString & dateFormat()
{
    static const QString DATE_FORMAT  = "yyyy-MM-dd hh:mm:ss";
    return DATE_FORMAT;
}

static QString onyxDBDir()
{
    // By default ".onyx", svn style.
    static QString ONYX_DB_DIR;
    if (ONYX_DB_DIR.isEmpty())
    {
        ONYX_DB_DIR = qgetenv("ONYX_DB_DIR");
    }
    if (ONYX_DB_DIR.isEmpty())
    {
        ONYX_DB_DIR = ".onyx";
    }
    return ONYX_DB_DIR;

}

/// Retrieve database path name from content file path.
/// Currently, it uses the root path of content path as the working
/// directory. TODO. It may not work correctly for Linux when using
/// mount point.
/// @param content_path The absolute content file path.
/// @param database_path The returned database path.
/// @return It returns false if it the database does not exist.
bool getDatabasePath(const QString & content_path,
                     QString & database_path)
{
    database_path = QDir::home().filePath(DATABASE_NAME);
    return true;
}

QString getThumbDB(const QString & dir_path)
{
    QString db_path;
    QDir dir(dir_path);
    if (!dir.exists(onyxDBDir()))
    {
        if (!dir.mkdir(onyxDBDir()))
        {
            qWarning("Could not create the db %s. Filesystem readonly or no enough space.",
                     qPrintable(onyxDBDir()));
            return db_path;
        }
    }

    dir.cd(onyxDBDir());
    return dir.absoluteFilePath(".thumbs.db");
}

QString getSketchDB(const QString & file_path)
{
    QFileInfo info(file_path);

    // If the file path does not exist, it's regarded as a note name.
    // The internal sketch database will be used.
    if (!info.exists())
    {
        return internalSketchDBPath(file_path);
    }

    // File exists, use normal sketch database.
    QDir dir(info.dir());
    if (!dir.exists(onyxDBDir()))
    {
        if (!dir.mkdir(onyxDBDir()))
        {
            qWarning("Could not create the db %s. Filesystem readonly or no enough space.",
                     qPrintable(onyxDBDir()));
            return QString();
        }
    }

    dir.cd(onyxDBDir());
    return dir.absoluteFilePath(info.fileName() + SKETCH_POSTFIX);
}

QString internalSketchDBPath(const QString &name)
{
    const QString ONYX_NOTES_DIR = "notes";

    QDir home = QDir::home();
    if (!home.exists(ONYX_NOTES_DIR))
    {
        home.mkdir(ONYX_NOTES_DIR);
    }

    home.cd(ONYX_NOTES_DIR);
    return home.filePath(name).append(SKETCH_POSTFIX);
}

QString sketchPostfix()
{
    return SKETCH_POSTFIX;
}

/// Check if file specified by the path may contain metadata or not.
/// \path The file absolute path.
/// \return This function returns true if it could contain metadata
/// otherwise it returns false.
bool couldContainMetadata(const QString &path)
{
    if (path.endsWith(".pdf", Qt::CaseInsensitive) ||
        path.endsWith(".epub", Qt::CaseInsensitive))
    {
        return true;
    }
    return false;
}

}  // namespace cms


