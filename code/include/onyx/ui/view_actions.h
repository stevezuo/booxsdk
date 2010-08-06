#ifndef ONYX_VIEW_ACTIONS_H_
#define ONYX_VIEW_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

typedef QVector<ViewSettingType> Rotations;
typedef QVector<PageLayoutType> PageLayouts;

enum ViewActionsType
{
    INVALID_VIEW_TYPE = -1,
    VIEW_ROTATION = 0,
    VIEW_PAGE_LAYOUT
};

struct ViewActionsContext
{
    ViewActionsType   active_type;
    int               cur_rotation;
    int               cur_layout;

    ViewActionsContext()
        : active_type(INVALID_VIEW_TYPE)
        , cur_rotation(INVALID_MODE)
        , cur_layout(INVALID_LAYOUT) {}
};

class ViewActions : public BaseActions
{
    Q_OBJECT
public:
    ViewActions(void);
    ~ViewActions(void);

public:
    void generateRotationActions(const Rotations & rotations,
                                 const ViewSettingType selected_value = PORTRAIT_MODE);
    void generatePageLayoutActions(const PageLayouts & layouts,
                                   const PageLayoutType selected_value = PAGE_LAYOUT);

    /// Set current view setting
    void setCurrentRotation(const ViewSettingType selected_value);
    void setCurrentPageLayout(const PageLayoutType selected_value);

    /// Get the selected value
    ViewActionsType getSelectedValue(int & value);

private Q_SLOTS:
    void onRotationTriggered(QAction *action);
    void onPageLayoutTriggered(QAction *action);

private:
    ViewActionsContext ctx_;

    scoped_ptr<QActionGroup> rotations_;
    scoped_ptr<QActionGroup> page_layouts_;

};  // ViewActions

}  // namespace ui

#endif  // ONYX_VIEW_ACTIONS_H_
