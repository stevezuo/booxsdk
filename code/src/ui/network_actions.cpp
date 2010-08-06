
#include "onyx/ui/network_actions.h"

namespace ui
{

NetworkActions::NetworkActions(void)
: BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/network_management.png")));
}

NetworkActions::~NetworkActions(void)
{
}

void NetworkActions::generateActions(const vector<NetworkType> & values)
{
    category()->setText(QCoreApplication::tr("Network"));
    actions_.clear();

    vector<NetworkType>::const_iterator begin = values.begin();
    vector<NetworkType>::const_iterator end   = values.end();
    for(vector<NetworkType>::const_iterator iter = begin;
        iter != end; ++iter)
    {
        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(exclusiveGroup()));

        act->setCheckable(true);
        act->setData(*iter);
        switch (*iter)
        {
        case NETWORK_WIFI:
            {
                act->setText(QCoreApplication::tr("Wifi"));
                act->setIcon(QIcon(QPixmap(":/images/wifi.png")));
            }
            break;
        case NETWORK_PROXY:
            {
                act->setText(QCoreApplication::tr("Proxy"));
                act->setIcon(QIcon(QPixmap(":/images/network_proxy.png")));
            }
            break;
        default:
            break;
        }
        actions_.push_back(act);
    }
}

void NetworkActions::setActionStatus(const NetworkType type, bool selected)
{
    if (type < INVALID_TYPE || type > NETWORK_WCDMA)
    {
        return;
    }

    QAction *act = 0;
    for (int i = 0; i < static_cast<int>(actions_.size()); ++i)
    {
        if (type == actions_[i]->data().toInt())
        {
            act = actions_[i].get();
            break;
        }
    }

    if (act == 0)
    {
        return;
    }

    act->setChecked(selected);
}

QAction * NetworkActions::action(const NetworkType type)
{
    for (int i = 0; i < static_cast<int>(actions_.size()); ++i)
    {
        if (type == actions_[i]->data().toInt())
        {
            return actions_[i].get();
        }
    }
    return 0;
}


NetworkType NetworkActions::selected()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        return static_cast<NetworkType>(act->data().toInt());
    }
    return INVALID_TYPE;
}

}   // namespace ui
