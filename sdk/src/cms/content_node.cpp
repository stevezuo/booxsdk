// Author: John

#include "onyx/cms/content_node.h"

namespace cms
{
// TODO: Move to global header file.
const QString DATE_FORMAT  = "yyyy-MM-dd hh:mm:ss";

ContentNode::ContentNode(void)
: id_(CMS_INVALID_ID)
, name_()
, location_()
, title_()
, authors_()
, description_()
, last_access_()
, publisher_()
, md5_()
, size_(0)
, rating_(0)
, read_time_(0)
, read_count_(0)
, progress_()
{
}

ContentNode::ContentNode(const ContentNode & right)
: id_(right.id_)
, name_(right.name_)
, location_(right.location_)
, title_(right.title_)
, authors_(right.authors_)
, description_(right.description_)
, last_access_(right.last_access_)
, publisher_(right.publisher_)
, md5_(right.md5_)
, size_(right.size_)
, rating_(right.rating_)
, read_time_(right.read_time_)
, read_count_(right.read_count_)
, progress_(right.progress_)
, attributes_(right.attributes_)
{
}

ContentNode::ContentNode(const QString & path)
: id_(CMS_INVALID_ID)
, name_()
, location_()
, title_()
, authors_()
, description_()
, last_access_()
, publisher_()
, md5_()
, size_(0)
, rating_(0)
, read_time_(0)
, read_count_(0)
, progress_()
{
    QFileInfo info(path);
    mutable_name() = info.fileName();
    mutable_location() = info.path();
    mutable_size() = info.size();
}

ContentNode::~ContentNode(void)
{
}

/// Use current time as the the last access time.
const QString & ContentNode::updateLastAccess()
{
    mutable_last_access() = QDateTime::currentDateTime().toString(DATE_FORMAT);
    return last_access();
}

bool ContentNode::contains(const QString &pattern)
{
    // Search in the title, authors, description, access time
    // and publisher.
    if (pattern.isEmpty())
    {
        return true;
    }
    else if (title_.contains(pattern))
    {
        return true;
    }
    else if (authors_.contains(pattern))
    {
        return true;
    }
    else if (description_.contains(pattern))
    {
        return true;
    }
    else if (last_access_.contains(pattern))
    {
        return true;
    }
    else if (publisher_.contains(pattern))
    {
        return true;
    }
    return false;
}

QString ContentNode::nativeAbsolutePath()
{
    QFileInfo info(QDir(location()), name());
    return info.absoluteFilePath();
}

/// Retrieve all attributes associated with the node.
void ContentNode::attributes(QVariantMap & att)
{
    QBuffer buffer(&attributes_);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream >> att;
}

/// Store attributes.
void ContentNode::setAttributes(QVariantMap & att)
{
    attributes_.clear();
    QBuffer buffer(&attributes_);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << att;
}

void ContentNode::clear()
{
    id_ = CMS_INVALID_ID;
    name_.clear();
    location_.clear();
    title_.clear();
    authors_.clear();
    description_.clear();
    last_access_.clear();
    publisher_.clear();
    size_ = 0;
    rating_ = 0;
    read_time_ = 0;
    read_count_ = 0;
    progress_.clear();
    attributes_.clear();
}

void ContentNode::fromPath(ContentNode & node,const QString &path)
{
    node.clear();
    QFileInfo info(path);
    node.mutable_name() = info.fileName();
    node.mutable_location() = info.path();
    node.mutable_size() = info.size();
}

bool ContentNode::makeSureTableExist(QSqlDatabase &database)
{
    QSqlQuery query(database);
    bool ok = query.exec("create table if not exists content ("
                        "id integer primary key, "
                        "name text,"
                        "location text,"
                        "title text,"
                        "authors text,"
                        "description text, "
                        "last_access text, "
                        "publisher text, "
                        "md5 text, "
                        "size integer, "
                        "rating integer, "
                        "read_time integer, "
                        "read_count integer, "
                        "progress text, "
                        "attributes blob "
                        ")");
    if (ok)
    {
        return query.exec("create index if not exists name_index on content (name) ");
    }
    return false;
}


bool ContentNode::removeTable(QSqlDatabase &database)
{
    QSqlQuery query(database);
    return query.exec("drop table content");
}

// Need to define the search policy.
// - Match the name, location and size.
// - Otherwise, we just ignore the request.
// - We can also use the hash code to match the file.
/// Query the content node in the specified database.
/// \param database The sqlitepp wrapper of database.
/// \param info The content node information returned by this function.
/// \param absolute_path The absolute path of the node.
/// \param create Create the content node automatically if not found.
/// \return This function returns true if the content node is already in
/// the database, otherwise it returns false.
bool
ContentNode::getContentNode(QSqlDatabase &database,
                            ContentNode & node,
                            const QString & absolute_path,
                            bool create)
{
    // Initialize query parameters and initialize all stuff
    // that does not rely on the database.
    QFileInfo info(absolute_path);

    node.mutable_size() = info.size();
    node.mutable_location() = info.path();
    node.mutable_name() = info.fileName();

    // Query by name, location and size.
    QSqlQuery query(database);
    query.prepare( "select id, title, authors, description, "
                   "last_access, publisher, md5, "
                   "rating, read_time, read_count, progress, attributes "
                   "from content where name = :name and location = :location "
                   "and size = :size" );
    query.bindValue(":name", node.name());
    query.bindValue(":location", node.location());
    query.bindValue(":size", node.size());

    if (query.exec() && query.next())
    {
        int index = 0;
        node.id_ = query.value(index++).toInt();
        node.title_ = query.value(index++).toString();
        node.mutable_authors() = query.value(index++).toString();
        node.mutable_description() = query.value(index++).toString();
        node.mutable_last_access() = query.value(index++).toString();
        node.mutable_publisher() = query.value(index++).toString();
        node.mutable_md5() = query.value(index++).toString();
        node.mutable_rating() = query.value(index++).toInt();
        node.mutable_read_time() = query.value(index++).toInt();
        node.mutable_read_count() = query.value(index++).toInt();
        node.mutable_progress() = query.value(index++).toString();
        node.mutable_attributes() = query.value(index++).toByteArray();
        return true;
    }

    // File has been moved, check the name and size.
    /*
    {
        statement st(database);
        st  << "select id, title, authors, description, "
            << "last_access, publisher, md5, "
            << "rating, read_time, read_count "
            << "from content where name = :name and size = :size",
            node.id_),
            into(info.mutable_title()),
            into(info.mutable_authors()),
            into(info.mutable_description()),
            into(info.mutable_last_access()),
            into(info.mutable_publisher()),
            into(info.mutable_md5()),
            into(info.mutable_rating()),
            into(info.mutable_read_time()),
            into(info.mutable_read_count()),
            use(info.name()),
            use(info.size());
        if (st.exec())
        {
            return true;
        }
    }

    // File name has been changed, check the md5 only.
    // Check if the md5 is ready or not. Make sure md5
    // is calculated only once.
    // TODO, maybe just ignore the node as it's not important.
    if (info.md5().empty() &&
        FileMd5Sum(absolute_path, info.mutable_md5()))
    {
        statement st(database);
        st  << "select id, title, authors, description, "
            << "last_access, publisher, "
            << "rating, read_time, read_count "
            << "from content where md5 = :md5",
            into(info.id_),
            into(info.mutable_title()),
            into(info.mutable_authors()),
            into(info.mutable_description()),
            into(info.mutable_last_access()),
            into(info.mutable_publisher()),
            into(info.mutable_rating()),
            into(info.mutable_read_time()),
            into(info.mutable_read_count()),
            use(info.md5());
        if (st.exec())
        {
            return true;
        }
    }
    */

    // Insert into the database. Should change the function name.
    if (create)
    {
        createContentNode(database, node);
    }
    return false;
}

/// TODO, need a better way to identify the file.
/// Lookup the file name.
/// Check the location.
/// File content hash. (Which part to hash and length).
bool ContentNode::getContentNode(QSqlDatabase & database,
                                 ContentNode &node)
{
    QSqlQuery query(database);
    query.prepare( "select id, title, authors, description, "
                   "last_access, publisher, md5, "
                   "rating, read_time, read_count, progress, attributes "
                   "from content where name = :name and location = :location "
                   "and size = :size" );
    query.bindValue(":name", node.name());
    query.bindValue(":location", node.location());
    query.bindValue(":size", node.size());

    if (query.exec() && query.next())
    {
        int index = 0;
        node.id_ = query.value(index++).toInt();
        node.title_ = query.value(index++).toString();
        node.mutable_authors() = query.value(index++).toString();
        node.mutable_description() = query.value(index++).toString();
        node.mutable_last_access() = query.value(index++).toString();
        node.mutable_publisher() = query.value(index++).toString();
        node.mutable_md5() = query.value(index++).toString();
        node.mutable_rating() = query.value(index++).toInt();
        node.mutable_read_time() = query.value(index++).toInt();
        node.mutable_read_count() = query.value(index++).toInt();
        node.mutable_progress() = query.value(index++).toString();
        node.mutable_attributes() = query.value(index++).toByteArray();
        return true;
    }
    return false;
}

bool ContentNode::getContentNode(QSqlDatabase & database,
                                 const cms_long id,
                                 ContentNode & node)
{
    QSqlQuery query(database);
    query.prepare( "select name, location, title, authors, description, "
                   "last_access, publisher, md5, "
                   "rating, read_time, read_count, progress, attributes "
                   "from content where id = :id" );
    query.bindValue(":id", id);

    if (query.exec() && query.next())
    {
        int index = 0;
        node.id_ = id;
        node.name_ = query.value(index++).toString();
        node.location_ = query.value(index++).toString();
        node.title_ = query.value(index++).toString();
        node.mutable_authors() = query.value(index++).toString();
        node.mutable_description() = query.value(index++).toString();
        node.mutable_last_access() = query.value(index++).toString();
        node.mutable_publisher() = query.value(index++).toString();
        node.mutable_md5() = query.value(index++).toString();
        node.mutable_rating() = query.value(index++).toInt();
        node.mutable_read_time() = query.value(index++).toInt();
        node.mutable_read_count() = query.value(index++).toInt();
        node.mutable_progress() = query.value(index++).toString();
        node.mutable_attributes() = query.value(index++).toByteArray();
        return true;
    }
    return false;
}

bool ContentNode::createContentNode(QSqlDatabase& database,
                                    ContentNode & node)
{
    QSqlQuery query(database);
    query.prepare ("insert into content "
                   "(name, location, title, authors, description, "
                   "last_access, publisher, md5, "
                   "size, rating, read_time, read_count, progress, attributes ) "
                   " values "
                   "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(node.name());
    query.addBindValue(node.location());
    query.addBindValue(node.title());
    query.addBindValue(node.authors());
    query.addBindValue(node.description());
    query.addBindValue(node.last_access());
    query.addBindValue(node.publisher());

    query.addBindValue(node.md5());
    query.addBindValue(node.size());
    query.addBindValue(node.rating());
    query.addBindValue(node.read_time());
    query.addBindValue(node.read_count());
    query.addBindValue(node.progress());
    query.addBindValue(node.attributes());

    if (query.exec())
    {
        node.id_ = query.lastInsertId().toInt();
        return true;
    }
    return false;
}

bool ContentNode::updateContentNode(QSqlDatabase& database,
                                    const ContentNode & node)
{
    QSqlQuery query(database);
    query.prepare ("update content set "
                   " name = ?, location = ?, title = ?, authors = ?, "
                   " description = ?, last_access = ?, publisher = ?, md5 = ?, "
                   " size = ?, rating = ?, read_time = ?, read_count = ?, "
                   " progress = ?, attributes = ? "
                   " where id = ?");

    query.addBindValue(node.name());
    query.addBindValue(node.location());
    query.addBindValue(node.title());
    query.addBindValue(node.authors());
    query.addBindValue(node.description());
    query.addBindValue(node.last_access());
    query.addBindValue(node.publisher());

    query.addBindValue(node.md5());
    query.addBindValue(node.size());
    query.addBindValue(node.rating());
    query.addBindValue(node.read_time());
    query.addBindValue(node.read_count());

    query.addBindValue(node.progress());
    query.addBindValue(node.attributes());

    query.addBindValue(node.id());

    return query.exec();
}

bool ContentNode::removeContentNode(QSqlDatabase& database,
                                    ContentNode& node)
{
    QSqlQuery query(database);
    query.prepare( "delete from content where id = ?");
    query.addBindValue(node.id());

    if (query.exec())
    {
        node.clear();
        return true;
    }
    return false;
}

void ContentNode::increaseCount()
{
    ++read_count_;
}

}   // namespace cms
