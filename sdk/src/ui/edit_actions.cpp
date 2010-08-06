#include "onyx/ui/edit_actions.h"

namespace ui
{

EditActions::EditActions()
: BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/edit.png")));
}

EditActions::~EditActions(void)
{
}

void EditActions::generateActions()
{
    category()->setText(QCoreApplication::tr("Edit"));
    actions_.clear();

    /*
    // Disable them for release 1.0
    shared_ptr<Action> cut(new Action(exclusiveGroup()));
    cut->setCheckable(true);
    cut->setText(QCoreApplication::tr("Cut"));
    cut->setIcon(QIcon(QPixmap(":/images/cut.png")));
    cut->setData(EDIT_CUT);
    actions_.push_back(cut);

    shared_ptr<Action> copy(new Action(exclusiveGroup()));
    copy->setCheckable(true);
    copy->setText(QCoreApplication::tr("Copy"));
    copy->setIcon(QIcon(QPixmap(":/images/copy.png")));
    copy->setData(EDIT_COPY);
    actions_.push_back(copy);

    shared_ptr<Action> paste(new Action(exclusiveGroup()));
    paste->setCheckable(true);
    paste->setText(QCoreApplication::tr("Paste"));
    paste->setIcon(QIcon(QPixmap(":/images/paste.png")));
    paste->setData(EDIT_PASTE);
    actions_.push_back(paste);
    */

    shared_ptr<QAction> del(new QAction(exclusiveGroup()));
    del->setCheckable(true);
    del->setText(QCoreApplication::tr("Delete"));
    del->setIcon(QIcon(QPixmap(":/images/delete.png")));
    del->setData(EDIT_DELETE);
    actions_.push_back(del);
}

QAction * EditActions::action(const EditType type)
{
    for(int i = 0; i < static_cast<int>(actions_.size()); ++i)
    {
        if (actions_.at(i)->data().toInt() == type)
        {
            return actions_.at(i).get();
        }
    }
    return 0;
}

EditType EditActions::selected()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        return static_cast<EditType>(act->data().toInt());
    }
    return INVALID_EDIT;
}


}
