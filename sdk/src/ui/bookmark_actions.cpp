#include "onyx/ui/bookmark_actions.h"

namespace ui
{

BookmarkActions::BookmarkActions()
    : BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/bookmark.png")));
}

BookmarkActions::~BookmarkActions(void)
{
}

void BookmarkActions::generateActions()
{
    category()->setText(QCoreApplication::tr("Bookmark"));
    actions_.clear();

    shared_ptr<QAction> add(new QAction(exclusiveGroup()));
    add->setCheckable(true);
    add->setText(QCoreApplication::tr("Add Bookmark"));
    add->setIcon(QIcon(QPixmap(":/images/add_bookmark.png")));
    add->setData(ADD_BOOKMARK);
    actions_.push_back(add);

    shared_ptr<QAction> del(new QAction(exclusiveGroup()));
    del->setCheckable(true);
    del->setText(QCoreApplication::tr("Delete Bookmark"));
    del->setIcon(QIcon(QPixmap(":/images/delete_bookmark.png")));
    del->setData(DELETE_BOOKMARK);
    actions_.push_back(del);

    shared_ptr<QAction> show(new QAction(exclusiveGroup()));
    show->setCheckable(true);
    show->setText(QCoreApplication::tr("Show Bookmark"));
    show->setIcon(QIcon(QPixmap(":/images/show_all_bookmarks.png")));
    show->setData(SHOW_ALL_BOOKMARKS);
    actions_.push_back(show);
}

QAction * BookmarkActions::action(const ReadingToolsType action)
{
    int index = static_cast<int>(action);
    if (index >= 0 && index < static_cast<int>(actions_.size()))
    {
        return actions_[index].get();
    }
    return 0;
}

ReadingToolsType BookmarkActions::selected()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        return static_cast<ReadingToolsType>(act->data().toInt());
    }
    return UNDEFINED_TOOL;
}

}
