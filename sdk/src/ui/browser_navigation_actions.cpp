#include "onyx/ui/browser_navigation_actions.h"

namespace ui
{

BrowserNavigationActions::BrowserNavigationActions()
: BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/navigation.png")));
}

BrowserNavigationActions::~BrowserNavigationActions(void)
{
    actions_.clear();
}

void BrowserNavigationActions::generateActions(QWebHistory *history)
{
    category()->setText(QCoreApplication::tr("Navigation"));

    actions_.clear();
    shared_ptr<QAction> backward(new QAction(exclusiveGroup()));
    backward->setText(QCoreApplication::tr("Backward"));
    backward->setIcon(QIcon(QPixmap(":/images/backward.png")));
    backward->setData(NAVIGATE_BACKWARD);
    backward->setEnabled(history->canGoBack());
    backward->setCheckable(true);
    backward->setChecked(false);
    actions_.push_back(backward);


    shared_ptr<QAction> forward(new QAction(exclusiveGroup()));
    forward->setText(QCoreApplication::tr("Forward"));
    forward->setIcon(QIcon(QPixmap(":/images/forward.png")));
    forward->setData(NAVIGATE_FORWARD);
    forward->setEnabled(history->canGoForward());
    forward->setCheckable(true);
    forward->setChecked(false);
    actions_.push_back(forward);


    shared_ptr<QAction> home(new QAction(exclusiveGroup()));
    home->setText(QCoreApplication::tr("Home"));
    home->setIcon(QIcon(QPixmap(":/images/home.png")));
    home->setData(NAVIGATE_HOME);
    home->setEnabled(true);
    home->setCheckable(true);
    home->setChecked(false);
    actions_.push_back(home);


    shared_ptr<QAction> clear(new QAction(exclusiveGroup()));
    clear->setText(QCoreApplication::tr("Clear History"));
    clear->setIcon(QIcon(QPixmap(":/images/clear_history.png")));
    clear->setData(NAVIGATE_CLEAR_HISTORY);
    clear->setEnabled(true);
    clear->setCheckable(true);
    clear->setChecked(false);
    actions_.push_back(clear);

}

/// Retrieve the selected action.
NavigationType BrowserNavigationActions::selected()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        return static_cast<NavigationType>(act->data().toInt());
    }
    return NAVIGATE_NONE;
}

}

