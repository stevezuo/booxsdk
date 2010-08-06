// Author: John

#ifndef CMS_CONTENT_MANAGER_H_
#define CMS_CONTENT_MANAGER_H_

#include <iostream>
#include <stdexcept>
#include <vector>
#include "onyx/base/base.h"
#include <QString>
#include <QtSql/QtSql>
#include "content_node.h"
#include "content_category.h"
#include "cms_utils.h"
#include "notes_manager.h"

namespace cms
{


/// Content manager provides operations for caller to use
/// data stroed in database. Content manager takes response to
/// make sure the data are synchronous between user and database.
/// We would add merge between two database later.
class ContentManager
{
public:
    static const cms_long ROOT_CATEGORY_ID;
    static const cms_long LOCAL_ROOT_CATEGORY_ID;
    static const cms_long SERVER_ROOT_CATEGORY_ID;
    static const cms_long RECENT_DOCUMENTS_ID;
    static const cms_long READING_LISTS_ID;

public:
    explicit ContentManager();
    ~ContentManager(void);

public:
    bool open(const QString & database_name = QString());
    bool isOpen() const;
    void close();

    void getVersion(unsigned int &major, unsigned int &minor);

    // Content session.
    bool createContentNode(ContentNode& info);
    bool getContentNode(ContentNode & info,
                        const QString & absolute_path_name,
                        bool create = true);
    bool getContentNode(ContentNode & info);
    bool getContentNode(const cms_long id, ContentNode & info);
    bool updateContentNode(const ContentNode& info);
    bool removeContentNode(ContentNode& info);

    // Category session.
    bool getCategory(const cms_long id, ContentCategory & category);
    bool updateCategory(ContentCategory & category);
    bool removeCategory(ContentCategory & category);
    const ContentCategory & root_category() const { return root_category_; }

    // Local root category.
    const ContentCategory & local_root_category() const;
    ContentCategory & mutable_local_root_category();

    // Server side root category.
    const ContentCategory& server_root_category()const;
    ContentCategory & mutable_server_root_category();

    // Category-category.
    bool createNewCategory(ContentCategory & parent,
                           ContentCategory & child);

    // Content-category.
    bool getParentCategories(const ContentNode &node, cms_ids & categories);
    bool addChildContent(ContentCategory & category, const ContentNode & node);
    bool removeChildContent(ContentCategory & category,
                            const ContentNode & node);
    bool removeChildContent(ContentNode & node);
    bool addChildCategory(ContentCategory & parent,
                          ContentCategory & child);
    bool removeChildCategory(ContentCategory & parent,
                             ContentCategory & child);

    // Options session.
    bool createContentOptions(const cms_long id, const cms_blob & options);
    bool getContentOptions(const cms_long id, cms_blob &options);
    bool removeOptions(const cms_long id);
    bool updateOptions(const cms_long id, const cms_blob & Options);

    // Bookmark session.
    bool createBookmarks(const cms_long id, const cms_blob & options);
    bool getBookmarks(const cms_long id, cms_blob &options);
    bool removeBookmarks(const cms_long id);
    bool updateBookmarks(const cms_long id, const cms_blob & Options);

    // Recent docuemnts.
    bool addToRecentDocuments(const QString &doc_path);
    bool removeRecentDocument(const QString &doc_path);
    bool addToRecentDocuments(const cms_long id);
    QString latestReading();
    bool getRecentDocuments(cms_ids & documents);
    bool clearRecentDocuments();
    void sortRecentDocuments(cms_ids & documents);

    // Shortcut
    bool allShortcuts(QStringList & src, QStringList & targets);
    QString target(const QString &src);
    int links(const QString &src);
    bool link(const QString &src, const QString & target);
    bool unlinkBySource(const QString &src);
    bool unlinkByTarget(const QString & target);

    // Notes.
    QString suggestedNoteName();
    int  allNotes(Notes &notes);
    bool addNoteIndex(const NoteInfo & note);
    bool removeNote(const QString & name);
    bool removeAllNotes();
    bool removeAllNotesIndex();

    /// Remove all tables in the database. Just for test.
    void dropAllTables();

private:
    void initializeTables();

    /// Initialize all pre-defined categories.
    void initializeCategories();

    /// Initialize root category();
    void initializeRootCategory();

    /// Initialize all second level categories.
    void initializeCategory(ContentCategory &category);

    bool getChildrenCategories(const ContentCategory &parent,
                               cms_ids & children);

    /// Retrieve child
    bool getChildrenContent(const ContentCategory &parent,
                            cms_ids & children);

private:
    scoped_ptr<QSqlDatabase> database_;     ///< sqlite qt wrapper.

    // Not very clear yet.
    ContentCategory root_category_;
    ContentCategory local_root_category_;
    ContentCategory server_root_category_;
    ContentCategory recent_read_category_;
    ContentCategory reading_lists_category_;
};


/// Local root category.
inline const ContentCategory & ContentManager::local_root_category() const
{
    return local_root_category_;
}

inline ContentCategory & ContentManager::mutable_local_root_category()
{
    return local_root_category_;
}

/// Server side root category.
inline const ContentCategory& ContentManager::server_root_category()const
{
    return server_root_category_;
}

inline ContentCategory & ContentManager::mutable_server_root_category()
{
    return server_root_category_;
}


};  // namespace cms

#endif
