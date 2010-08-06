
#ifndef READING_STYLE_ACTIONS_H_
#define READING_STYLE_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

enum ReadingStyleType
{
    STYLE_INVALID = -1,
    STYLE_LINE_SPACING_8,
    STYLE_LINE_SPACING_9,
    STYLE_LINE_SPACING_10,
    STYLE_LINE_SPACING_11,
    STYLE_LINE_SPACING_12,
    STYLE_LINE_SPACING_13,
    STYLE_LINE_SPACING_14,
    STYLE_LINE_SPACING_15,
    STYLE_LINE_SPACING_16,
    STYLE_LINE_SPACING_17,
    STYLE_LINE_SPACING_18,
    STYLE_LINE_SPACING_19,
    STYLE_LINE_SPACING_20,

    STYLE_PAGE_MARGINS_SMALL,
    STYLE_PAGE_MARGINS_MIDDLE,
    STYLE_PAGE_MARGINS_LARGE,

    STYLE_END,
};

class ReadingStyleActions : public BaseActions
{
public:
    ReadingStyleActions(void);
    ~ReadingStyleActions(void);

public:
    /// Generate or re-generate the setting actions group.
    void generateActions(ReadingStyleType selected);

    /// Set the status of reading style
    void setActionStatus(const ReadingStyleType type, bool selected);

    /// Retrieve the action according to the type.
    QAction * action(const ReadingStyleType type);

    /// Retrieve the selected type.
    ReadingStyleType selected();

};  // ReadingStyleActions

}  // namespace ui

#endif  // READING_STYLE_ACTIONS_H_
