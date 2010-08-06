#ifndef ENCODING_ACTIONS_H_
#define ENCODING_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

/// Generate all supported encodings.
class EncodingActions : public BaseActions
{
public:
    EncodingActions();
    ~EncodingActions(void);

public:
    /// Generate all supported encodings.
    void generateActions(const base::string &current_encoding);

    /// Retrieve the selected encoding if any.
    base::string encoding();
};

}   // namespace ui

#endif      // ENCODING_ACTIONS_H_
