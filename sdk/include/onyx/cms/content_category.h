// Author: John

#ifndef CMS_CONTENT_CATEGORY_H_
#define CMS_CONTENT_CATEGORY_H_

#include "onyx/base/base.h"
#include "content_node.h"

namespace cms
{

/// Category is a container for sub category and content nodes.
/// User can NOT construct a valid category without using Content
/// Manager.
class ContentCategory
{
    friend class ContentManager;
public:
    ContentCategory(void);
    ContentCategory(const ContentCategory& right);
    ~ContentCategory(void);

    /// Retrieve the category id.
    cms_long id() const { return id_; }

    /// Serves as SQL query statement to retrieve content list
    /// from content_category table.
    const QString & criteria() const { return criteria_; }
    QString & mutable_criteria() { return criteria_; }

    /// Category title.
    const QString & title() const { return title_; }
    QString & mutable_title() { return title_; }

    /// Retreive all children content ids.
    const cms_ids & children_content_nodes() const { return child_content_; }

    /// Retrieve all children category ids.
    const cms_ids & children_categories() const { return child_categories_; }

    /// Retrieve all parent category ids. NOT SURE.
    const cms_ids & parent_categories() const { return parent_categories_; }

    /// When protected, user is not allowed to change the title.
    bool is_protected() const { return is_protected_; }
    bool canAddContent() const { return can_add_content_; }

private:
    bool addContentNode(const cms_long node_id, bool force = false);
    bool removeContentNode(const cms_long node_id);

    bool addChildCategory(const cms_long child_id);
    bool removeChildCategory(const cms_long child_id);
    bool addParentCategory(const cms_long parent_id);
    bool removeParentCategory(const cms_long parent_id);

    /// Reset the category.
    void clear();

    // TODO. Need to review the following functions carefully.
    static bool makeSureTableExist(QSqlDatabase &);
    static bool removeTable(QSqlDatabase &database);

    static bool isCategoryExist(QSqlDatabase &, const cms_long);
    static bool addCategory(QSqlDatabase&, ContentCategory&);
    static bool getCategory(QSqlDatabase&, ContentCategory &category);
    static bool updateCategory(QSqlDatabase &, ContentCategory &);
    static bool removeCategory(QSqlDatabase&, ContentCategory &category);

    // Category relationship.
    static bool addCategoryRelationShip(QSqlDatabase & database,
                                        const cms_long child,
                                        const cms_long parent);

    static bool removeCategoryRelationShip(QSqlDatabase & database,
                                           const cms_long child,
                                           const cms_long parent);

    static bool getContentParentCategories(QSqlDatabase & database,
                                           const cms_long content_id,
                                           cms_ids & categories);

    static bool getChildCategories(QSqlDatabase &database,
                                   const cms_long id,
                                   cms_ids & categories);

    static bool addChildContent(QSqlDatabase & database,
                                ContentCategory & category,
                                const cms_long id);

    static bool getChildrenContent(QSqlDatabase & database,
                                   const ContentCategory &parent,
                                   cms_ids & children);

    static bool removeChildContent(QSqlDatabase & database,
                                   ContentCategory & category,
                                   const cms_long id);

    static bool removeAllChildContent(QSqlDatabase & database,
                                      ContentCategory & category);

    static bool removeChildContent(QSqlDatabase & database,
                                   const cms_long content_id);

    static bool removeContentCategory(QSqlDatabase&, const cms_long);


private:
    cms_long id_;               ///< Category id.
    QString criteria_;     ///< How to generate this category.
    QString title_;        ///< The category title.
    cms_ids parent_categories_; ///< All parent branch ids.
    cms_ids child_categories_;  ///< All children branch.
    cms_ids child_content_;     ///< All children content.
    bool is_protected_;         ///< Not allowed to change.
    bool can_add_content_;      ///< Allow to add content node or not.

};
typedef ContentCategory * ContentCategoryPtr;
typedef std::vector<ContentCategoryPtr> ContentCategories;

}  // namespace cms

#endif  // CMS_CATEGORY_H_
