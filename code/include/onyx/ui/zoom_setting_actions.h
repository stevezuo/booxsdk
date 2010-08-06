
#ifndef ZOOM_SETTING_ACTIONS_H_
#define ZOOM_SETTING_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

class ZoomSettingActions : public BaseActions
{
public:
    ZoomSettingActions(void);
    ~ZoomSettingActions(void);

public:
    void generateActions( const std::vector<ZoomFactor> & values );
    void setCurrentZoomValue( const ZoomFactor selected_value );
    ZoomFactor getSelectedZoomValue();

};  // ZoomSettingActions

}  // namespace ui

#endif  // ZOOM_SETTING_ACTIONS_H_
