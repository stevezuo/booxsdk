
#include "onyx/ui/zoom_setting_actions.h"

namespace ui
{

ZoomSettingActions::ZoomSettingActions(void)
: BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/zoom_settings.png")));
}

ZoomSettingActions::~ZoomSettingActions(void)
{
}

/// Set current zoom value
void ZoomSettingActions::setCurrentZoomValue(const ZoomFactor selected_value)
{
    QAction * checked_action = exclusive_group_->checkedAction();
    if (checked_action != 0)
    {
        checked_action->setChecked(false);
    }

    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ((*iter)->data().toDouble() == selected_value &&
            selected_value != ZOOM_SELECTION)
        {
            (*iter)->setChecked(true);
            return;
        }
    }
}

/// Generate or re-generate the setting actions group.
void ZoomSettingActions::generateActions( const std::vector<ZoomFactor> & values )
{
    category()->setText(QCoreApplication::tr("Zooming"));
    actions_.clear();

    vector<ZoomFactor>::const_iterator begin = values.begin();
    vector<ZoomFactor>::const_iterator end   = values.end();
    for(vector<ZoomFactor>::const_iterator iter = begin; iter != end; ++iter)
    {
         // Add to category automatically.
        shared_ptr<QAction> act(new QAction(exclusiveGroup()));

        // Change font and make it as checkable.
        act->setCheckable(true);

        if (*iter == ZOOM_HIDE_MARGIN)
        {
            // hide margin
            act->setText(QCoreApplication::tr("Hide Margin"));
            act->setIcon(QIcon(QPixmap(":/images/zoom_hide_margin.png")));
        }
        else if (*iter == ZOOM_TO_PAGE)
        {
            // zoom to page
            act->setText(QCoreApplication::tr("To Page"));
            act->setIcon(QIcon(QPixmap(":/images/zoom_to_page.png")));
        }
        else if (*iter == ZOOM_TO_WIDTH)
        {
            // zoom to width
            act->setText(QCoreApplication::tr("To Width"));
            act->setIcon(QIcon(QPixmap(":/images/zoom_to_width.png")));
        }
        else if (*iter == ZOOM_TO_HEIGHT)
        {
            // zoom to height
            act->setText(QCoreApplication::tr("To Height"));
            act->setIcon(QIcon(QPixmap(":/images/zoom_to_height.png")));
        }
        else if (*iter == ZOOM_SELECTION)
        {
            // selection zoom
            act->setText(QCoreApplication::tr("Selection Zoom"));
            act->setIcon(QIcon(QPixmap(":/images/zoom_selection_zoom.png")));
        }
        else if (*iter >= ZOOM_MIN)
        {
            act->setText(QString("%1%").arg(*iter));
            QString icon_name = QString(":/images/zoom_%1%.png").arg(*iter);
            act->setIcon(QIcon(QPixmap(icon_name)));
        }
        else
        {
            act->setText(QString("Zoom Index %1").arg(*iter + 1));
            QString icon_name = QString(":/images/zoom_%1%.png").arg((*iter + 1) * 100);
            act->setIcon(QIcon(QPixmap(icon_name)));
        }

        act->setData(*iter);
        actions_.push_back(act);
    }
}

/// Retrieve the selected font size.
ZoomFactor ZoomSettingActions::getSelectedZoomValue()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        return act->data().toDouble();
    }
    return INVALID_ZOOM;
}

}   // namespace ui
