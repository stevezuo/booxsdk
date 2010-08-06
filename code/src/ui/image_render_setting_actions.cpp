
#include "onyx/ui/image_render_setting_actions.h"

namespace ui
{

ImageRenderSettingActions::ImageRenderSettingActions(void)
: BaseActions()
, active_setting_(INVALID_IMAGE_RENDER_SETTING)
, active_checked_status_(false)
{
    category()->setIcon(QIcon(QPixmap(":/images/config.png")));
    exclusiveGroup()->setExclusive(false);
}

ImageRenderSettingActions::~ImageRenderSettingActions(void)
{
}

void ImageRenderSettingActions::GenerateActions(const vector<ImageRenderSetting> & values)
{
    category()->setText(QCoreApplication::tr("Image Settings"));
    actions_.clear();

    vector<ImageRenderSetting>::const_iterator begin = values.begin();
    vector<ImageRenderSetting>::const_iterator end   = values.end();
    for(vector<ImageRenderSetting>::const_iterator iter = begin;
        iter != end; ++iter)
    {
        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(exclusiveGroup()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);
        act->setIcon(QIcon(QPixmap(":/images/inbox.png")));

        // The text.
        QString text;
        switch (*iter)
        {
        case IMAGE_NEED_DITHER:
            {
                text = QCoreApplication::tr("Dither");
                act->setText(text);
                connect(act.get(),
                        SIGNAL(toggled(bool)),
                        this,
                        SLOT(OnDitherTriggered(bool)));
            }
            break;
        case IMAGE_NEED_CONVERT:
            {
                text = QCoreApplication::tr("Convert");
                act->setText(text);
                connect(act.get(),
                        SIGNAL(toggled(bool)),
                        this,
                        SLOT(OnConvertTriggered(bool)));
            }
            break;
        case IMAGE_NEED_SMOOTH:
            {
                text = QCoreApplication::tr("Smooth");
                act->setText(text);

                connect(act.get(), SIGNAL(toggled(bool)),
                    this, SLOT(OnSmoothTriggered(bool)));
            }
            break;
        default:
            break;
        }

        actions_.push_back(act);
    }
}

void ImageRenderSettingActions::SetActionStatus(const ImageRenderSetting setting,
                                                bool selected)
{
    assert(setting > INVALID_IMAGE_RENDER_SETTING &&
           setting < IMAGE_SETTING_UNKNOWN);

    int idx = -1;
    for (unsigned int i = 0; i < actions_.size(); ++i)
    {
        if (setting == actions_[i]->data().toInt())
        {
            idx = i;
            break;
        }
    }

    if (idx < 0)
    {
        return;
    }

    actions_[idx]->setChecked(selected);
}

ImageRenderSetting ImageRenderSettingActions::GetSelectedSetting(bool &checked)
{
    checked = active_checked_status_;
    return active_setting_;
}

void ImageRenderSettingActions::OnDitherTriggered(bool checked)
{
    active_setting_ = IMAGE_NEED_DITHER;
    active_checked_status_ = checked;
}

void ImageRenderSettingActions::OnConvertTriggered(bool checked)
{
    active_setting_ = IMAGE_NEED_CONVERT;
    active_checked_status_ = checked;
}

void ImageRenderSettingActions::OnSmoothTriggered(bool checked)
{
    active_setting_ = IMAGE_NEED_SMOOTH;
    active_checked_status_ = checked;
}

}   // namespace ui
