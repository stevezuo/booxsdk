// -*- mode: c++; c-basic-offset: 4; -*-
// Author: John

#include "onyx/cms/content_manager.h"
#include "onyx/cms/content_thumbnail.h"
#include "onyx/cms/content_options.h"
#include "onyx/cms/content_bookmarks.h"
#include "onyx/cms/content_shortcut.h"
#include "onyx/cms/notes_manager.h"


namespace cms
{

const cms_long ContentManager::ROOT_CATEGORY_ID         = 0;
const cms_long ContentManager::LOCAL_ROOT_CATEGORY_ID   = 1;
const cms_long ContentManager::SERVER_ROOT_CATEGORY_ID  = 2;
const cms_long ContentManager::RECENT_DOCUMENTS_ID      = 3;
const cms_long ContentManager::READING_LISTS_ID         = 4;

ContentManager::ContentManager()
: database_()
, root_category_()
, local_root_category_()
, server_root_category_()
, recent_read_category_()
, reading_lists_category_()
{
}

ContentManager::~ContentManager(void)
{
    close();
}

/// Open specified database from file system.
bool ContentManager::open(const QString & database_name)
{
    if (!database_)
    {
        database_.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "cms")));
    }

    if (!database_->isOpen())
    {
        QString db_path(database_name);
        if (db_path.isEmpty())
        {
            getDatabasePath("", db_path);
        }
        database_->setDatabaseName(db_path);
        if (!database_->open())
        {
            qDebug() << database_->lastError().text();
            return false;
        }
        initializeTables();
        initializeCategories();
    }
    return true;
}

bool ContentManager::isOpen() const
{
    if (database_)
    {
        return database_->isOpen();
    }
    return false;
}

void ContentManager::close()
{
    if (database_)
    {
        database_->close();
        database_.reset(0);
        QSqlDatabase::removeDatabase("cms");
    }
}

/// Retrieve database version.
void ContentManager::getVersion(unsigned int &major,
                                unsigned int &minor)
{
}

void ContentManager::initializeTables()
{
    // content table.
    ContentNode::makeSureTableExist(*database_);

    // category table.
    ContentCategory::makeSureTableExist(*database_);

    // Options table.
    ContentOptions::makeSureTableExist(*database_);

    // Bookmarks.
    ContentBookmarks::makeSureTableExist(*database_);

    // Shortcuts.
    ContentShortcut::makeSureTableExist(*database_);

    // Notes manager
    NotesManager::makeSureTableExist(*database_);
}

void ContentManager::initializeCategories()
{
    // Pre-defined category ids.
    // Not sure it's always correct. Just check the table contains
    // anything or not. If there is nothing in the table, we can insert
    // the two unique ids.

    root_category_.id_ = ROOT_CATEGORY_ID;
    root_category_.mutable_title() = "root";

    local_root_category_.id_ = LOCAL_ROOT_CATEGORY_ID;
    local_root_category_.mutable_title() = "Library";
    local_root_category_.mutable_criteria() = "";

    server_root_category_.id_ = SERVER_ROOT_CATEGORY_ID;
    server_root_category_.mutable_title() = "Server";
    server_root_category_.mutable_criteria() = "";

    recent_read_category_.id_ = RECENT_DOCUMENTS_ID;
    recent_read_category_.mutable_title() = "Recent documents";
    recent_read_category_.mutable_criteria() = "";

    reading_lists_category_.id_ = READING_LISTS_ID;
    reading_lists_category_.mutable_title() = "Reading list";
    reading_lists_category_.mutable_criteria() = "";

    // Initialize them.
    initializeRootCategory();
    initializeCategory(local_root_category_);
    initializeCategory(server_root_category_);
    initializeCategory(recent_read_category_);
    initializeCategory(reading_lists_category_);
}

void ContentManager::initializeRootCategory()
{
    if (!ContentCategory::isCategoryExist(*database_, root_category_.id()))
    {
        ContentCategory::addCategory(*database_, root_category_);
    }
}

void ContentManager::initializeCategory(ContentCategory &category)
{
    root_category_.addChildCategory(category.id());
    category.addParentCategory(root_category_.id());

    // Check if the category is already there.
    if (!ContentCategory::isCategoryExist(*database_, category.id()))
    {
        ContentCategory::addCategory(*database_, category);
        ContentCategory::addCategoryRelationShip(*database_,
                                                 category.id(),
                                                 root_category_.id());
        return;
    }

    // Read sub categories.
    ContentCategory::getCategory(*database_, category);
}


bool ContentManager::getContentNode(ContentNode & info,
                                    const QString & absolute_path_name,
                                    bool create)
{
    return ContentNode::getContentNode(*database_,
                                       info,
                                       absolute_path_name,
                                       create);
}

/// Retrieve content information for given file. For content
/// like Email and RSS, caller can use url as the index.
/// For normal files in file system, caller can use the path name.
/// This function is usually used when the name and the location
/// are available.
bool ContentManager::getContentNode(ContentNode & info)
{
    return ContentNode::getContentNode(*database_, info);
}

/// Retrieve content info from specified content node id. This function
/// is used when the content id is available.
bool ContentManager::getContentNode(const cms_long id, ContentNode & info)
{
    return ContentNode::getContentNode(*database_, id, info);
}

/// Create a new content node and insert it into database.
/// This function assigns a unique content id for the
/// input content node. Caller should fill all
/// necessary fileds of content node information.
bool ContentManager::createContentNode(ContentNode& info)
{
    return ContentNode::createContentNode(*database_, info);
}

/// Update the content node. The content node should already
/// exist in the database.
bool ContentManager::updateContentNode(const ContentNode & info)
{
    return ContentNode::updateContentNode(*database_, info);
}

/// Remove content metadata for given content node. Caller should no longer
/// use the content node. This function also removes the content node
/// from its parent categories.
bool ContentManager::removeContentNode(ContentNode& info)
{
    // Remove the options.
    ContentOptions::removeOptions(*database_, info.id());

    // Remove from content category table, which also removes
    // from recent document list.
    ContentCategory::removeContentCategory(*database_, info.id());

    // Remove the content itself.
    ContentNode::removeContentNode(*database_, info);
    return true;
}

/// Retrieve specified category.
bool ContentManager::getCategory(const cms_long id,
                                 ContentCategory & category)
{
    return ContentCategory::getCategory(*database_, category);
}

/// Update specified category.
bool ContentManager::updateCategory(ContentCategory & category)
{
    return ContentCategory::updateCategory(*database_, category);
}

/// Remove specified category.
bool ContentManager::removeCategory(ContentCategory & category)
{
    return ContentCategory::removeCategory(*database_, category);
}

/// Create a new category and add child category node into
/// parent category.
bool ContentManager::createNewCategory(ContentCategory & parent,
                                       ContentCategory & child)
{
    ContentCategory::addCategory(*database_, child);


    child.addParentCategory(parent.id());
    parent.addChildCategory(child.id());

    ContentCategory::addCategoryRelationShip(*database_, child.id(),
                                             parent.id());
    // TODO, update content_category table.
    return true;
}

/// Retrieve parent category id list that contains the content.
bool ContentManager::getParentCategories(const ContentNode &node,
                                         cms_ids & categories)
{
    return ContentCategory::getContentParentCategories(*database_,
                                                       node.id(),
                                                       categories);
}

bool ContentManager::getChildrenCategories(const ContentCategory &parent,
                                           cms_ids & children)
{
    // select from category_category table.
    return ContentCategory::getChildCategories(*database_,
                                               parent.id(),
                                               children);

}

/// Add child content into specified category.
bool ContentManager::addChildContent(ContentCategory & category,
                                     const ContentNode & node)
{
    // Check it's already in the category or not.
    return ContentCategory::addChildContent(*database_, category, node.id());
}

bool ContentManager::getChildrenContent(const ContentCategory &parent,
                                        cms_ids & children)
{
    // select  database.
    return ContentCategory::getChildrenContent(*database_,
                                               parent,
                                               children);
}

/// Remove child content from specified category.
bool ContentManager::removeChildContent(ContentCategory & category,
                                        const ContentNode & node)
{
    return ContentCategory::removeChildContent(*database_,
                                               category,
                                               node.id());
}

bool ContentManager::removeChildContent(ContentNode & node)
{
    // Retrieve parent categories from content_category table.
    return ContentCategory::removeChildContent(*database_, node.id());
}

/// Add to parent category. Seems we don't need this function.
/// It can be replaced by CreateNewCategory.
bool ContentManager::addChildCategory(ContentCategory & parent,
                                      ContentCategory & child)
{
    // Make sure the child category exist.
    if (!getCategory(child.id(), child))
    {
        return false;
    }

    //TODO. STILL NEED TO CONSIDER IT.
    parent.addChildCategory(child.id());
    child.addParentCategory(parent.id());

    // Update database.
    return ContentCategory::addCategoryRelationShip(*database_,
                                                    child.id(),
                                                    parent.id());
}

/// Remove specified category from the parent category.
/// This function only removes the relationship between
/// parent category and child category. It does not remove
/// the categories themselves.
// Should we also remove the child category from category table?
bool ContentManager::removeChildCategory(ContentCategory & parent,
                                         ContentCategory & child)
{
    parent.removeChildCategory(child.id());
    child.removeParentCategory(parent.id());

    // Update database.
    return ContentCategory::removeCategoryRelationShip(*database_,
                                                       child.id(),
                                                       parent.id());
}

/// Create a new content options and return the unique options id.
bool ContentManager::createContentOptions(const cms_long id,
                                          const cms_blob & options)
{
    return ContentOptions::createOptions(*database_, id, options);
}

/// Retrieve options.
bool ContentManager::getContentOptions(const cms_long id, cms_blob &options)
{
    return ContentOptions::getOptions(*database_, id, options);
}

/// Remove content options from options table.
bool ContentManager::removeOptions(const cms_long id)
{
    return ContentOptions::removeOptions(*database_, id);
}

/// Change the Options data.
bool ContentManager::updateOptions(const cms_long id, const cms_blob & options)
{
    return ContentOptions::updateOptions(*database_, id, options);
}

bool ContentManager::createBookmarks(const cms_long id, const cms_blob & bookmarks)
{
    return ContentBookmarks::createBookmarks(*database_, id, bookmarks);
}

bool ContentManager::getBookmarks(const cms_long id, cms_blob &bookmarks)
{
    return ContentBookmarks::getBookmarks(*database_, id, bookmarks);
}

bool ContentManager::removeBookmarks(const cms_long id)
{
    return ContentBookmarks::removeBookmarks(*database_, id);
}

bool ContentManager::updateBookmarks(const cms_long id, const cms_blob & bookmarks)
{
    return ContentBookmarks::updateBookmarks(*database_, id, bookmarks);
}

bool ContentManager::addToRecentDocuments(const QString &doc_path)
{
    ContentNode node;
    getContentNode(node, doc_path);

    if (node.id() == CMS_INVALID_ID)
    {
        return false;
    }

    return addToRecentDocuments(node.id());
}

bool ContentManager::removeRecentDocument(const QString &doc_path)
{
    ContentNode node;
    getContentNode(node, doc_path);

    if (node.id() == CMS_INVALID_ID)
    {
        return false;
    }

    if (!ContentCategory::removeChildContent(*database_,
                                             recent_read_category_,
                                             node.id()))
    {
        return false;
    }
    return true;
}

/// Add to recent document list. Simple wrap of AddChildContent.
/// Not sure how many documents should keep.
bool ContentManager::addToRecentDocuments(const cms_long id)
{
    // Reuse the AddChildContent.
    return ContentCategory::addChildContent(*database_,
                                            recent_read_category_,
                                            id);
}

QString ContentManager::latestReading()
{
    QString path;
    cms_ids all;
    if (!getRecentDocuments(all))
    {
        return path;
    }

    if (all.size() <= 0)
    {
        return path;
    }

    ContentNode node;
    if (!getContentNode(all.front(), node))
    {
        return path;
    }

    return node.nativeAbsolutePath();
}

/// Get the recent opened docuemnts.
bool ContentManager::getRecentDocuments(cms_ids & documents)
{
    // Have to sort them by last access time.
    if (!getChildrenContent(recent_read_category_, documents))
    {
        return false;
    }

    sortRecentDocuments(documents);

    return true;
}

bool ContentManager::clearRecentDocuments()
{
    if (!ContentCategory::removeAllChildContent(*database_,
                                                recent_read_category_))
    {
        return false;
    }
    return true;
}

void ContentManager::sortRecentDocuments(cms_ids & documents)
{
    ContentNodePtrs nodes;
    for(cms_ids_iter iter = documents.begin(); iter != documents.end(); ++iter)
    {
        ContentNode *node  = new ContentNode;
        getContentNode(*iter, *node);
        nodes.push_back(node);
    }

    std::sort(nodes.begin(), nodes.end(), LessByAccessTime());

    documents.clear();
    for(ContentNodePtrs::iterator iter = nodes.begin();
        iter != nodes.end();
        ++iter)
    {
        documents.push_back((*iter)->id());
        delete (*iter);
    }
}

bool ContentManager::allShortcuts(QStringList & srcs,
                                  QStringList & targets)
{
    return ContentShortcut::all(*database_, srcs, targets);
}

QString ContentManager::target(const QString &src)
{
    return ContentShortcut::target(*database_, src);
}

int ContentManager::links(const QString &src)
{
    return ContentShortcut::links(*database_, src);
}

bool ContentManager::link(const QString &src,
                          const QString & target)
{
    return ContentShortcut::link(*database_, src, target);
}

bool ContentManager::unlinkBySource(const QString &src)
{
    return ContentShortcut::unlinkBySource(*database_, src);
}

bool ContentManager::unlinkByTarget(const QString & target)
{
    return ContentShortcut::unlinkByTarget(*database_, target);
}

QString ContentManager::suggestedNoteName()
{
    return NotesManager::suggestedName(*database_);
}

int  ContentManager::allNotes(Notes &notes)
{
    return NotesManager::all(*database_, notes);
}

bool ContentManager::addNoteIndex(const NoteInfo & note)
{
    QString path = getSketchDB(note.name());

    // Ensure note has been created.
    ContentNode info;
    getContentNode(info, path, true);

    return NotesManager::addIndex(*database_, note);
}

bool ContentManager::removeNote(const QString & name)
{
    // Remove from configuration.
    QString path = getSketchDB(name);
    ContentNode info;
    if (getContentNode(info, path, false))
    {
        removeContentNode(info);
    }

    // Remove index.
    if (!NotesManager::removeIndex(*database_, name))
    {
        qDebug() << "Failed to remove index";
        return false;
    }

    // Remove the database too.
    if (!QFile::remove(path))
    {
        qDebug() << "Failed to remove file: " << path;
        return false;
    }

    return true;
}

/// Remove all notes index and related notes.
bool ContentManager::removeAllNotes()
{
    cms::Notes notes;
    allNotes(notes);
    foreach(NoteInfo n, notes)
    {
        removeNote(n.name());
    }
    return true;
}

/// Remove all notes index. This function does not really remove the
/// notes database.
bool ContentManager::removeAllNotesIndex()
{
   return NotesManager::removeAll(*database_);
}

void ContentManager::dropAllTables()
{
    ContentNode::removeTable(*database_);
    ContentCategory::removeTable(*database_);

}

}   // namespace cms
