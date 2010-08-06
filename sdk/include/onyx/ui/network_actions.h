
#ifndef ONYX_NETWORK_ACTIONS_H_
#define ONYX_NETWORK_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

class NetworkActions : public BaseActions
{
public:
    NetworkActions(void);
    ~NetworkActions(void);

public:
    /// Generate or re-generate the setting actions group.
    void generateActions(const vector<NetworkType> & values);

    /// Set the status of reading tool
    void setActionStatus(const NetworkType type, bool selected);

    /// Retrieve the action according to the type.
    QAction * action(const NetworkType type);

    /// Retrieve the selected font size.
    NetworkType selected();

};  // NetworkActions

}  // namespace ui

#endif  // ONYX_NETWORK_ACTIONS_H_
