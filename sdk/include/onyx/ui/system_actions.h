#ifndef SYSTEM_ACTIONS_H_
#define SYSTEM_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

class SystemActions : public BaseActions
{
public:
    SystemActions(void);
    ~SystemActions(void);

public:
    /// Generate or re-generate the system actions.
    void generateActions(const std::vector<int> & actions = std::vector<int>());
    void addAboutAction();

    SystemAction selected();

};

}   // namespace ui

#endif //  SYSTEM_ACTIONS_H_
