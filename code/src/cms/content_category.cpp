// Author: John

#include <algorithm>
#include "onyx/cms/content_category.h"
#include "onyx/cms/cms_utils.h"

namespace cms
{

ContentCategory::ContentCategory(void)
: id_(CMS_INVALID_ID)
, is_protected_(false)
, can_add_content_(true)
{
}

ContentCategory::ContentCategory(const ContentCategory& right)
: id_(right.id_)
, criteria_(right.criteria_)
, title_(right.title_)
, parent_categories_(right.parent_categories_)
, child_categories_(right.child_categories_)
, child_content_(right.child_categories_)
, is_protected_(right.is_protected_)
{
}

ContentCategory::~ContentCategory(void)
{
}

/// Add a content node to this category.
/// When force is true, we will always add the node into the category.
/// It will remove the the node at first.
bool ContentCategory::addContentNode(const cms_long node_id, bool force)
{
    if (!force)
    {
        return addToContainer(child_content_, node_id);
    }
    else
    {
        removeContentNode(node_id);
        addToContainer(child_content_, node_id);
        return true;
    }
}

/// Remove content node from this category.
bool ContentCategory::removeContentNode(const cms_long node_id)
{
    return removeFromContainer(child_content_, node_id);
}


/// Add sub category to this category.
/// Caller should call add_parent_category as well.
bool ContentCategory::addChildCategory(const cms_long child_id)
{
    return addToContainer(child_categories_, child_id);
}

/// Remove sub category from this category.
bool ContentCategory::removeChildCategory(const cms_long child_id)
{
    return removeFromContainer(child_categories_, child_id);
}

/// Add parent category.
bool ContentCategory::addParentCategory(const cms_long parent_id)
{
    return addToContainer(parent_categories_, parent_id);
}

/// Add parent category.
bool ContentCategory::removeParentCategory(const cms_long parent_id)
{
    return removeFromContainer(parent_categories_, parent_id);
}

void ContentCategory::clear()
{
    id_ = CMS_INVALID_ID;
    criteria_.clear();
    title_.clear();
    parent_categories_.clear();
    child_categories_.clear();
    child_content_.clear();
    is_protected_ = false;
}

bool ContentCategory::makeSureTableExist(QSqlDatabase & database)
{
    // content category table.
    QSqlQuery query(database);
    query.exec( "create table if not exists category ("
                "id integer primary key, "
                "criteria text, "
                "tag text, "
                "title text"
                ")" );

    // Content_category relationship table.
    query.exec( "create table if not exists content_category ("
                "id integer primary key, "
                "content_id integer , "
                "category_id integer  "
                ")");

    // category_category relationship table.
    query.exec( "create table if not exists category_category ("
                "id integer primary key, "
                "child_category_id integer,"
                "parent_category_id integer"
                ")" );

    return true;
}

bool ContentCategory::removeTable(QSqlDatabase &database)
{
    QSqlQuery query(database);
    query.exec( "drop table category" );
    query.exec( "drop table content_category" );
    query.exec( "drop table category_category" );
    return true;
}

bool ContentCategory::isCategoryExist(QSqlDatabase &database,
                                      const cms_long id)
{
    cms_long temp = CMS_INVALID_ID;
    QSqlQuery query(database);
    query.prepare( "select id from category where id = ?" ),
    query.addBindValue(id);
    if (query.exec() && query.next())
    {
        temp = query.value(0).toInt();
        return (temp != CMS_INVALID_ID);
    }
    return false;
}

/// Add a new category to database.
bool ContentCategory::addCategory(QSqlDatabase & database,
                                  ContentCategory& category)
{
    QSqlQuery query(database);
    if (category.id() != CMS_INVALID_ID)
    {
        query.prepare( "INSERT OR REPLACE into category (id, title, criteria)"
                       "values(?, ?, ?)" );
        query.addBindValue(category.id());
        query.addBindValue(category.title());
        query.addBindValue(category.criteria());
        return query.exec();
    }
    else
    {
        // Insert into category table as the category does not exist.
        query.prepare( "insert into category (criteria, title) "
                       "values (?, ?)" );
        query.addBindValue(category.criteria());
        query.addBindValue(category.title());
        if (query.exec())
        {
            category.id_ =  query.lastInsertId().toInt();
            return true;
        }
    }
    return false;
}

bool ContentCategory::getCategory(QSqlDatabase& database,
                                  ContentCategory &category)
{
    // Retrieve category data.
    QSqlQuery query(database);
    query.prepare( "select criteria, title from category where id = ? ");
    query.addBindValue(category.id());
    if (query.exec() && query.next())
    {
        category.mutable_criteria() = query.value(0).toString();
        category.mutable_title() = query.value(1).toString();
    }
    else
    {
        return false;
    }

    // Retrieve child category id list.
    cms_long child_category_id = CMS_INVALID_ID;
    query.prepare( "select child_category_id from category_category "
                   "where parent_category_id = ?" );
    query.addBindValue(child_category_id);
    query.exec();
    while (query.next())
    {
        category.addChildCategory(query.value(0).toInt());
    }

    // Retrieve child content id list.
    category.child_content_.clear();
    query.prepare(  "select content_id from content_category where "
                    "category_id = ?" );
    query.addBindValue(category.id());
    query.exec();
    while (query.next())
    {
        category.child_content_.push_back(query.value(0).toInt());
    }

    // Retrieve parent category id list. may be not necessary.
    category.parent_categories_.clear();
    query.prepare(  "select parent_category_id from category_category where "
                    "child_category_id = ?" );
    query.addBindValue(category.id());
    query.exec();
    while (query.next())
    {
        category.parent_categories_.push_back(query.value(0).toInt());
    }
    return true;
}

bool ContentCategory::removeCategory(QSqlDatabase &database,
                                     ContentCategory &category)
{
    // Use transaction.
    database.transaction();

    // Step1: Remove from category_category table.
    QSqlQuery query(database);
    query.prepare( "delete from category_category where "
                   "child_category_id = ?" );
    query.addBindValue(category.id());
    query.exec();

    query.prepare( "delete from category_category where "
                   "parent_category_id = ?" );
    query.addBindValue(category.id());
    query.exec();

    // Step2: Remove from content_category table
    query.prepare( "delete from content_category where "
                   "category_id = ?" );
    query.addBindValue(category.id());
    query.exec();

    // Step3: Remove from category table
    query.prepare( "delete from category where "
                   "id = ?" );
    query.addBindValue(category.id());
    query.exec();

    database.commit();
    category.clear();
    return true;
}

bool ContentCategory::addCategoryRelationShip(QSqlDatabase & database,
                                              const cms_long child,
                                              const cms_long parent)
{
    QSqlQuery query(database);
    query.prepare( "insert into category_category "
                   "(child_category_id, parent_category_id) "
                   "values(?, ?)");
    query.addBindValue(child);
    query.addBindValue(parent);
    return query.exec();
}

bool ContentCategory::removeCategoryRelationShip(QSqlDatabase & database,
                                                 const cms_long child,
                                                 const cms_long parent)
{
    QSqlQuery query(database);
    query.prepare( "delete from category_category where "
                   "child_category_id = ? and "
                   "parent_category_id = ?");
    query.addBindValue(child);
    query.addBindValue(parent);
    return query.exec();
}

bool ContentCategory::getContentParentCategories(QSqlDatabase & database,
                                                 const cms_long content_id,
                                                 cms_ids & categories)
{
    QSqlQuery query(database);
    categories.clear();
    query.prepare( "select category_id from content_category "
                   "where content_id = ? " );
    query.addBindValue(content_id);

    query.exec();
    while (query.next())
    {
        categories.push_back(query.value(0).toInt());
    }
    return (categories.size() > 0);
}

bool ContentCategory::getChildCategories(QSqlDatabase &database,
                                         const cms_long parent,
                                         cms_ids & categories)
{
    QSqlQuery query(database);
    categories.clear();
    query.prepare( "select child_category_id from category_category "
                   "where parent_category_id = ? " );
    query.addBindValue(parent);

    query.exec();
    while (query.next())
    {
        categories.push_back(query.value(0).toInt());
    }
    return (categories.size() >= 0);
}

bool ContentCategory::removeContentCategory(QSqlDatabase & database,
                                            const cms_long content_id)
{
    // Remove from the content_category tabel.
    QSqlQuery query(database);
    query.prepare( "delete from content_category where "
                   "content_id = ?" );
    query.addBindValue(content_id);
    return query.exec();
}


bool ContentCategory::updateCategory(QSqlDatabase & database,
                                     ContentCategory & category)
{
    QSqlQuery query(database);
    query.prepare ("update category set "
                   "title = ? "
                   "where id = ?" );
    query.addBindValue(category.title());
    query.addBindValue(category.id());
    return query.exec();
}

bool ContentCategory::addChildContent(QSqlDatabase & database,
                                      ContentCategory & category,
                                      const cms_long id)
{
    // Check it's already in the category or not.
    if (category.addContentNode(id))
    {
        // Update database.
        QSqlQuery query(database);
        query.prepare( "insert into "
                       "content_category (content_id, category_id)"
                       "values (?, ?)" );
        query.addBindValue(id);
        query.addBindValue(category.id());
        return query.exec();
    }
    return false;
}

bool ContentCategory::getChildrenContent(QSqlDatabase & database,
                                         const ContentCategory &parent,
                                         cms_ids & children)
{
    children.clear();
    QSqlQuery query(database);
    query.prepare( "select content_id from content_category "
                   "where category_id = ? " );
    query.addBindValue(parent.id());
    query.exec();

    while (query.next())
    {
        children.push_back(query.value(0).toInt());
    }
    return true;
}

bool ContentCategory::removeChildContent(QSqlDatabase & database,
                                         ContentCategory & category,
                                         const cms_long id)
{
    if (category.removeContentNode(id))
    {
        // Update database.
        QSqlQuery query(database);
        query.prepare( "delete from content_category "
                       "where content_id = ?" );
        query.addBindValue(id);
        return query.exec();
    }
    return false;
}

bool ContentCategory::removeAllChildContent(QSqlDatabase & database,
                                            ContentCategory & category)
{
    // Update database.
    QSqlQuery query(database);
    query.prepare( "delete from content_category where category_id = ?" );
    query.addBindValue(category.id());
    return query.exec();
}

bool ContentCategory::removeChildContent(QSqlDatabase & database,
                                         const cms_long content_id)
{
    QSqlQuery query(database);
    query.prepare( "delete from content_category where "
                   "content_id = ?" );
    query.addBindValue(content_id);
    return query.exec();
}

}   // namespace cms
