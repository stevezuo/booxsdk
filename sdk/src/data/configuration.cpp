
#include "onyx/data/configuration.h"

using namespace std;
using namespace cms;

namespace vbf
{

bool openDatabase(const QString &doc_path,
                  ContentManager & db)
{
    if (db.isOpen())
    {
        qWarning("Database already opened.");
        return true;
    }

    QString database_path;
    cms::getDatabasePath(doc_path,  database_path);
    if (!db.open(database_path))
    {
        qWarning("Could not open %s", qPrintable(database_path));
        return false;
    }
    return true;
}

bool loadDocumentOptions(ContentManager & db,
                         const QString &doc_path,
                         Configuration & conf)
{
    // Retrieve document information.
    ContentNode::fromPath(conf.info, doc_path);
    if (!db.getContentNode(conf.info))
    {
        db.createContentNode(conf.info);
        return true;
    }

    // Read blob from the database.
    cms_blob options;
    db.getContentOptions(conf.info.id(), options);
    if (options.size() <= 0)
    {
        return true;
    }

    QBuffer buffer(&options);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream >> conf.options;
    return true;
}

bool saveDocumentOptions(ContentManager & db,
                         const QString &doc_path,
                         Configuration & conf,
                         bool add_to_history)
{
    if (add_to_history)
    {
        conf.info.increaseCount();
        conf.info.updateLastAccess();
    }

    // Should create one if necessary as loadDocumentOptions maybe not
    // invoked before this function.
    if (conf.info.id() == cms::CMS_INVALID_ID)
    {
        db.getContentNode(conf.info, doc_path, true);
    }
    else
    {
        if (!db.updateContentNode(conf.info))
        {
            qWarning("Could not update info.");
        }
    }

    // Store the options.
    cms_blob data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << conf.options;
    db.updateOptions(conf.info.id(), data);

    // Add it to reading history.
    if (add_to_history)
    {
        db.addToRecentDocuments(conf.info.id());
    }

    return true;
}


}   // namespace vbf

