
#ifndef IMAGE_RENDER_SETTING_ACTIONS_H_
#define IMAGE_RENDER_SETTING_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

class ImageRenderSettingActions : public BaseActions
{
    Q_OBJECT
public:
    ImageRenderSettingActions(void);
    ~ImageRenderSettingActions(void);

public:
    /// Generate or re-generate the setting actions group.
    void GenerateActions(const vector<ImageRenderSetting> & values);

    /// Set the status of reading tool
    void SetActionStatus(const ImageRenderSetting setting, bool selected);

    /// Retrieve the selected font size.
    ImageRenderSetting GetSelectedSetting(bool &checked);

private Q_SLOTS:
    void OnDitherTriggered(bool checked);
    void OnConvertTriggered(bool checked);
    void OnSmoothTriggered(bool checked);

private:
    ImageRenderSetting active_setting_;
    bool               active_checked_status_;

};  // ImageRenderSettingActions

};  // namespace ui

#endif  // IMAGE_RENDER_SETTING_ACTIONS_H_
