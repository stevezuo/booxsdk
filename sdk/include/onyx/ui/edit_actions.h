#ifndef EDIT_ACTIONS_H_
#define EDIT_ACTIONS_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"
#include "context_dialog_base.h"

namespace ui
{

class EditActions : public BaseActions
{
public:
    EditActions(void);
    ~EditActions(void);

public:
    /// Generate or re-generate the edit actions.
    void generateActions();
    QAction * action(const EditType type);
    EditType selected();

};

}   // namespace ui

#endif //  EDIT_ACTIONS_H_
