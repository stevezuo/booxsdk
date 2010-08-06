
#ifndef READING_TOOLS_ACTIONS_H_
#define READING_TOOLS_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

class ReadingToolsActions : public BaseActions
{
public:
    ReadingToolsActions(void);
    ~ReadingToolsActions(void);

public:
    /// Generate or re-generate the setting actions group.
    void generateActions(const vector<ReadingToolsType> & values, bool append = false);

    /// Set the status of reading tool
    void setActionStatus(const ReadingToolsType type, bool selected);

    /// Retrieve the action according to the type.
    QAction * action(const ReadingToolsType type);

    /// Retrieve the selected font size.
    ReadingToolsType selectedTool();

};  // ReadingToolsActions

}  // namespace ui

#endif  // READING_TOOLS_ACTIONS_H_
