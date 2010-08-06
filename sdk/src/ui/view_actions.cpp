#include "onyx/ui/view_actions.h"

namespace ui
{

ViewActions::ViewActions(void)
{
    rotations_.reset(new QActionGroup(0));
    rotations_->setExclusive(true);

    page_layouts_.reset(new QActionGroup(0));
    page_layouts_->setExclusive(true);
}

ViewActions::~ViewActions(void)
{
    actions_.clear();
}

void ViewActions::onRotationTriggered(QAction *action)
{
    ctx_.active_type = VIEW_ROTATION;
    ctx_.cur_rotation = action->data().toInt();
}

void ViewActions::onPageLayoutTriggered(QAction *action)
{
    ctx_.active_type = VIEW_PAGE_LAYOUT;
    ctx_.cur_layout = action->data().toInt();
}

void ViewActions::generateRotationActions(const Rotations & rotations,
                                          const ViewSettingType selected_value)
{
    category()->setText(QCoreApplication::tr("Layout"));
    category()->setIcon(QIcon(QPixmap(":/images/view_setting.png")));

    Rotations::const_iterator begin = rotations.begin();
    Rotations::const_iterator end   = rotations.end();
    for(Rotations::const_iterator iter = begin; iter != end; ++iter)
    {
        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(rotations_.get()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);

        if (*iter == selected_value)
        {
            act->setChecked(true);
        }

        switch (*iter)
        {
        case PORTRAIT_MODE:
            {
                act->setText(QCoreApplication::tr("Portrait Mode"));
                act->setIcon(QIcon(QPixmap(":/images/portrait.png")));
            }
            break;
        case LANDSCAPE_MODE:
            {
                act->setText(QCoreApplication::tr("Landscape Mode"));
                act->setIcon(QIcon(QPixmap(":/images/landscape.png")));
            }
            break;
        default:
            break;
        }
        actions_.push_back(act);
    }

    // add separator
    shared_ptr<QAction> separator(new QAction(rotations_.get()));
    separator->setSeparator(true);
    actions_.push_back(separator);

    connect(rotations_.get(),
            SIGNAL(triggered(QAction*)),
            this,
            SLOT(onRotationTriggered(QAction*)));
}

void ViewActions::generatePageLayoutActions(const PageLayouts & layouts,
                                            const PageLayoutType selected_value)
{
    category()->setText(QCoreApplication::tr("Layout"));
    category()->setIcon(QIcon(QPixmap(":/images/view_setting.png")));

    actions_.clear();
    PageLayouts::const_iterator begin = layouts.begin();
    PageLayouts::const_iterator end   = layouts.end();
    for(PageLayouts::const_iterator iter = begin; iter != end; ++iter)
    {
        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(page_layouts_.get()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);

        if (*iter == selected_value)
        {
            act->setChecked(true);
        }

        switch (*iter)
        {
        case PAGE_LAYOUT:
            {
                act->setText(QCoreApplication::tr("Single Page"));
                act->setIcon(QIcon(QPixmap(":/images/single_page.png")));
            }
            break;
        case CONTINUOUS_LAYOUT:
            {
                act->setText(QCoreApplication::tr("Scroll Pages"));
                act->setIcon(QIcon(QPixmap(":/images/continuous_page.png")));
            }
            break;
        case FACE_TO_FACE_LAYOUT:
            {
                act->setText(QCoreApplication::tr("Face To Face"));
                act->setIcon(QIcon(QPixmap(":/images/continuous_page.png")));
            }
            break;
        case SCROLL_LAYOUT:
            {
                act->setText(QCoreApplication::tr("Scroll Content"));
                act->setIcon(QIcon(QPixmap(":/images/continuous_page.png")));
            }
            break;
        case REFLOWABLE_LAYOUT:
            {
                act->setText(QCoreApplication::tr("Reflowable"));
                act->setIcon(QIcon(QPixmap(":/images/single_page.png")));
            }
            break;
        default:
            break;
        }
        actions_.push_back(act);
    }

    // add separator
    shared_ptr<QAction> separator(new QAction(page_layouts_.get()));
    separator->setSeparator(true);
    actions_.push_back(separator);

    connect(page_layouts_.get(),
            SIGNAL(triggered(QAction*)),
            this,
            SLOT(onPageLayoutTriggered(QAction*)));
}

ViewActionsType ViewActions::getSelectedValue(int & value)
{
    switch (ctx_.active_type)
    {
    case VIEW_ROTATION:
        value = ctx_.cur_rotation;
        break;
    case VIEW_PAGE_LAYOUT:
        value = ctx_.cur_layout;
        break;
    default:
        value = -1;
        break;
    }
    return ctx_.active_type;
}

void ViewActions::setCurrentRotation(const ViewSettingType selected_value)
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ((*iter)->data().toInt() == static_cast<int>(selected_value))
        {
            (*iter)->setChecked(true);
        }
    }
}

void ViewActions::setCurrentPageLayout(const PageLayoutType selected_value)
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ((*iter)->data().toInt() == static_cast<int>(selected_value))
        {
            (*iter)->setChecked(true);
        }
        else
        {
            (*iter)->setChecked(false);
        }
    }
}

} // namespace ui
