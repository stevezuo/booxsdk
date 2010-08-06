// Author: John

#ifndef CMS_CONTENT_NODE_H_
#define CMS_CONTENT_NODE_H_

#include "onyx/base/base.h"
#include <QString>
#include <QtSql/QtSql>


namespace cms
{

typedef long long cms_long;
typedef std::vector<cms_long> cms_ids;
typedef std::vector<cms_long>::iterator cms_ids_iter;
typedef std::vector<cms_long>::const_iterator cms_ids_const_iter;
typedef QByteArray cms_blob;

static const cms_long CMS_INVALID_ID = -1;

/// Usually, content node represents a content file.
/// It contains row information stored in content table.
/// Caller can NOT construct a valid content node directly.
/// Caller should ask Content Manager to create a node for itself.
/// All information represented by string must use UTF-8 encoding.
class ContentNode
{
    friend class ContentManager;
    friend class ContentCategory;
public:
    ContentNode(void);
    ContentNode(const ContentNode & right);
    ContentNode(const QString & path);
    ~ContentNode(void);

    /// Retrieve the unique content id.
    cms_long id() const { return id_; }

    /// The name serves as an index of the content node.
    const QString & name() const { return name_; }
    QString & mutable_name() { return name_; }

    /// The location serves as an index of the content node.
    const QString & location() const { return location_; }
    QString & mutable_location() { return location_; }

    /// Retrieve title. It can be file name when there is format
    /// is not supported. It can also be meaningful string when
    /// the format contains metadata.
    const QString & title() const { return title_; }
    QString & mutable_title() { return title_; }

    /// Retrieve authors information.
    const QString & authors() const { return authors_; }
    QString & mutable_authors() { return authors_; }
    bool authors(std::vector<QString> & lst) const;

    /// Brief description of the content node. It's possible to
    /// use rich text format as the description.
    const QString & description() const { return description_; }
    QString & mutable_description() { return description_; }

    /// The last access time. Caller should provide the time
    /// in correct format, usually, it should be:
    /// YYYY-MM-DD HH:MM:SS so that they can be compared in
    /// string format.
    const QString & last_access() const { return last_access_; }
    QString & mutable_last_access() { return last_access_; }

    /// Update the last access time. This function uses the current
    /// time as the last access time.
    const QString & updateLastAccess();

    /// Publisher.
    const QString & publisher() const { return publisher_; }
    QString & mutable_publisher() { return publisher_; }

    /// Md5 digest.
    const QString & md5() const { return md5_; }
    QString & mutable_md5() { return md5_; }

    /// The content size.
    const cms_long size() const { return size_; }
    cms_long & mutable_size() { return size_; }

    /// The rating.
    const int rating() const { return rating_; }
    int & mutable_rating() { return rating_; }

    /// Read times in seconds.
    const int read_time() const { return read_time_; }
    int & mutable_read_time() { return read_time_; }

    /// Read count. When the read count is zero, it means the content
    /// has not been accessed.
    const int read_count() const { return read_count_; }
    int & mutable_read_count() { return read_count_; }
    void increaseCount();

    // Read progress.
    const QString & progress() const { return progress_; }
    QString & mutable_progress() { return progress_; }

    void attributes(QVariantMap & attributes);
    void setAttributes(QVariantMap & attributes);

    /// Convert to native path.
    QString nativeAbsolutePath();

    ContentNode & operator = (const ContentNode & right)
    {
        if (this != &right)
        {
            id_          = right.id_;
            name_        = right.name_;
            location_    = right.location_;
            title_       = right.title_;
            authors_     = right.authors_;
            description_ = right.description_;
            last_access_ = right.last_access_;
            publisher_   = right.publisher_;
            md5_         = right.md5_;
            size_        = right.size_;
            rating_      = right.rating_;
            read_time_   = right.read_time_;
            read_count_  = right.read_count_;
            progress_    = right.progress_;
            attributes_  = right.attributes_;
        }
        return *this;
    }

    // Not sure to compare md5 or not. TODO.
    bool operator == (const ContentNode & right) const
    {
        return (id_          == right.id_    &&
                name_        == right.name_ &&
                location_    == right.location_);
    }

    /// Check if the node matches the input pattern or not.
    /// It tries to search the input pattern in all fields.
    bool contains(const QString &pattern);

    /// Update the information from path.
    static void fromPath(ContentNode &, const QString &);

private:
    void clear();

    /// Make sure the table exist.
    static bool makeSureTableExist(QSqlDatabase &);
    static bool removeTable(QSqlDatabase &);

    static bool getContentNode(QSqlDatabase &, ContentNode &,
                               const QString &, bool);
    static bool getContentNode(QSqlDatabase&, ContentNode &);
    static bool getContentNode(QSqlDatabase&, const cms_long, ContentNode &);

    static bool createContentNode(QSqlDatabase&, ContentNode&);
    static bool updateContentNode(QSqlDatabase&, const ContentNode &);
    static bool removeContentNode(QSqlDatabase&, ContentNode&);

    cms_blob & mutable_attributes() { return attributes_; }
    const cms_blob & attributes() const { return attributes_; }

private:
    cms_long id_;
    QString name_;
    QString location_;
    QString title_;
    QString authors_;           ///< All authors.
    QString description_;
    QString last_access_;       ///< Last access time.
    QString publisher_;
    QString md5_;               ///< Md5 digest.
    cms_long size_;             ///< Content size.
    int rating_;                ///< The rating.
    int read_time_;             ///< In seconds.
    int read_count_;
    QString progress_;          ///< Reading progress.
    cms_blob attributes_;       ///< Additional attributes for flexibility.
};
typedef ContentNode * ContentNodePtr;
typedef std::vector<ContentNodePtr> ContentNodePtrs;
typedef std::vector<ContentNode> ContentNodes;

/// Compare functors.
struct LessByName
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->name() < b->name();
    }
};

struct GreaterByName
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->name() > b->name();
    }
};

struct LessByAuthors
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->authors() < b->authors();
    }
};

struct GreaterByAuthors
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->authors() > b->authors();
    }
};

struct LessByTitle
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->title() < b->title();
    }
};

struct GreaterByTitle
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->title() > b->title();
    }
};

struct LessByDescription
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->description() < b->description();
    }
};

struct GreaterByDescription
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->description() > b->description();
    }
};

struct LessByAccessTime
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->last_access() > b->last_access();
    }
};

struct GreaterByAccessTime
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->last_access() < b->last_access();
    }
};

struct LessByPublisher
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->publisher() > b->publisher();
    }
};

struct GreaterByPublisher
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->publisher() < b->publisher();
    }
};

struct LessBySize
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->size() > b->size();
    }
};

struct GreaterBySize
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->size() < b->size();
    }
};

struct LessByRating
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->rating() > b->rating();
    }
};

struct GreaterByRating
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->rating() < b->rating();
    }
};

struct LessByReadTime
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->read_time() > b->read_time();
    }
};

struct GreaterByReadTime
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->read_time() < b->read_time();
    }
};

struct LessByReadCount
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->read_count() > b->read_count();
    }
};

struct GreaterByReadCount
{
    bool operator()( const ContentNodePtr a, const ContentNodePtr b ) const
    {
        return a->read_count() < b->read_count();
    }
};


}  // namespace cms

#endif
