#ifndef ONYX_UI_LIB_POPUP_MENU_H_
#define ONYX_UI_LIB_POPUP_MENU_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"

namespace ui
{

typedef shared_ptr<QActionGroup> ActionGroupPtr;
typedef vector<ActionGroupPtr> ActionGroups;
typedef vector<ActionGroupPtr>::iterator ActionGroupsIter;

typedef shared_ptr<QButtonGroup> ButtonGroupPtr;
typedef vector<ButtonGroupPtr> ButtonGroups;
typedef vector<ButtonGroupPtr>::iterator ButtonGroupsIter;

typedef shared_ptr<QFrame> FramePtr;
typedef vector<FramePtr> Frames;
typedef vector<FramePtr>::iterator FramesIter;

typedef shared_ptr<QAbstractButton> AbstractButtonPtr;
typedef vector<AbstractButtonPtr> AbstractButtons;
typedef vector<AbstractButtonPtr>::iterator AbstractButtonsIter;

/// BaseActions represents a menu group and all its children item.
class BaseActions : public QObject
{
    Q_OBJECT
public:
    BaseActions()
        : category_action_(0)
        , exclusive_group_()
        , actions_()
    {
        category_action_.setCheckable(true);

        exclusive_group_.reset(new QActionGroup(0));
        exclusive_group_->setExclusive(true);
    }

    virtual ~BaseActions()
    {
        clear();
    }

public:
    /// Retrieve the generated actions.
    inline QActionGroup * exclusiveGroup() { return exclusive_group_.get(); }

    /// The group action.
    inline QAction * category() { return &category_action_; }

    /// Get all children actions reference.
    vector<shared_ptr<QAction> > & actions() { return actions_; }

    /// Clear all.actions.
    void clear() { actions_.clear(); }

protected:
    QAction category_action_;                   ///< The left category.
    scoped_ptr<QActionGroup> exclusive_group_; ///< Used to make items exclusive. The child class
                                                ///< can choose to use other exclusive group if not suitable.
    vector<shared_ptr<QAction> > actions_;
};

}       // namespace ui

#endif  // ONYX_UI_LIB_POPUP_MENU_H_
