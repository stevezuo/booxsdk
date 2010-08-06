#ifndef BOOKMARK_ACTIONS_H_
#define BOOKMARK_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

class BookmarkActions : public BaseActions
{
public:
    BookmarkActions(void);
    ~BookmarkActions(void);

public:
    /// Generate or re-generate the bookmark actions.
    void generateActions();
    QAction * action(const ReadingToolsType action);
    ReadingToolsType selected();
};

}   // namespace ui

#endif //  BOOKMARK_ACTIONS_H_
