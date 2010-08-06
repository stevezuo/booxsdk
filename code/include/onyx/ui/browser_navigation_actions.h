#ifndef BROWSER_NAVIGATION_ACTIONS_H_
#define BROWSER_NAVIGATION_ACTIONS_H_

#include <QWebHistory>

#include "onyx/base/base.h"
#include "onyx/ui/context_dialog_base.h"


namespace ui
{

enum NavigationType
{
    NAVIGATE_NONE,
    NAVIGATE_FORWARD,
    NAVIGATE_BACKWARD,
    NAVIGATE_HOME,
    NAVIGATE_CLEAR_HISTORY,
};

class BrowserNavigationActions : public BaseActions
{
public:
    BrowserNavigationActions(void);
    ~BrowserNavigationActions(void);

public:
    /// Generate or re-generate the view type actions group.
    void generateActions(QWebHistory *history);

    /// Retrieve the selected action.
    NavigationType selected();

private:
};

};

#endif //  BROWSER_NAVIGATION_ACTIONS_H_
