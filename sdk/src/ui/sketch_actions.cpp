#include "onyx/ui/sketch_actions.h"

namespace ui
{

SketchActions::SketchActions(void)
{

}

SketchActions::~SketchActions(void)
{
    actions_.clear();
}

void SketchActions::onSketchTriggered(bool checked)
{
    ctx_.active_type           = SKETCH_MODE;
    ctx_.cur_sketch_mode       = MODE_SKETCHING;
    ctx_.active_checked_status = checked;
}

void SketchActions::onEraseTriggered(bool checked)
{
    ctx_.active_type           = SKETCH_MODE;
    ctx_.cur_sketch_mode       = MODE_ERASING;
    ctx_.active_checked_status = checked;
}

void SketchActions::onAddAnnotationTriggered(bool checked)
{
    ctx_.active_type           = ANNOTATION_MODE;
    ctx_.cur_anno_mode         = ADD_ANNOTATION;
    ctx_.active_checked_status = checked;
}

void SketchActions::onDeleteAnnotationTriggered(bool checked)
{
    ctx_.active_type           = ANNOTATION_MODE;
    ctx_.cur_anno_mode         = DELETE_ANNOTATION;
    ctx_.active_checked_status = checked;
}

void SketchActions::onDisplayAnnotationsTriggered(bool checked)
{
    ctx_.active_type           = ANNOTATION_MODE;
    ctx_.cur_anno_mode         = DIAPLAY_ALL_ANNOTATIONS;
    ctx_.active_checked_status = checked;
}

void SketchActions::onColorTriggered(QAction *action)
{
    ctx_.active_type           = SKETCH_COLOR;
    ctx_.cur_color             = action->data().toInt();
    ctx_.active_checked_status = true;
}

void SketchActions::onShapeTriggered(QAction *action)
{
    ctx_.active_type           = SKETCH_SHAPE;
    ctx_.cur_shape             = action->data().toInt();
    ctx_.active_checked_status = true;
}

void SketchActions::generateAnnotationMode(const AnnotationModes & modes,
                                           const AnnotationMode selected_mode)
{
    anno_modes_.reset(new QActionGroup(0));
    anno_modes_->setExclusive(false);

    AnnotationModes::const_iterator begin = modes.begin();
    AnnotationModes::const_iterator end   = modes.end();
    for(AnnotationModes::const_iterator iter = begin; iter != end; ++iter)
    {
        // Add to group automatically.
        shared_ptr<QAction> act(new QAction(anno_modes_.get()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);
        switch (*iter)
        {
        case ADD_ANNOTATION:
            {
                act->setText(QCoreApplication::tr("Add Annotate"));
                connect(act.get(),
                        SIGNAL(toggled(bool)),
                        this,
                        SLOT(onAddAnnotationTriggered(bool)));
                act->setIcon(QIcon(QPixmap(":/images/add_annotation.png")));
            }
            break;
        case DELETE_ANNOTATION:
            {
                act->setText(QCoreApplication::tr("Erase Annotation"));
                connect(act.get(),
                        SIGNAL(toggled(bool)),
                        this,
                        SLOT(onDeleteAnnotationTriggered(bool)));
                act->setIcon(QIcon(QPixmap(":/images/delete_annotation.png")));
            }
            break;
        case DIAPLAY_ALL_ANNOTATIONS:
            {
                act->setText(QCoreApplication::tr("All Annotations"));
                connect(act.get(),
                        SIGNAL(toggled(bool)),
                        this,
                        SLOT(onDisplayAnnotationsTriggered(bool)));
                act->setIcon(QIcon(QPixmap(":/images/show_all_annotations.png")));
            }
            break;
        default:
            break;
        }

        // Set current mode
        if (*iter == selected_mode)
        {
            act->setChecked(true);
        }
        actions_.push_back(act);
    }

    // add separator
    shared_ptr<QAction> separator(new QAction(anno_modes_.get()));
    separator->setSeparator(true);
    actions_.push_back(separator);
}

void SketchActions::generateSketchMode(const SketchModes & modes,
                                       const SketchMode selected_mode)
{
    category()->setText(QCoreApplication::tr("Annotations"));
    category()->setIcon(QIcon(QPixmap(":/images/annotations.png")));

    sketch_modes_.reset(new QActionGroup(0));
    sketch_modes_->setExclusive(false);

    SketchModes::const_iterator begin = modes.begin();
    SketchModes::const_iterator end   = modes.end();
    for(SketchModes::const_iterator iter = begin; iter != end; ++iter)
    {
        // Add to group automatically.
        shared_ptr<QAction> act(new QAction(sketch_modes_.get()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);
        switch (*iter)
        {
        case MODE_SKETCHING:
            {
                act->setText(QCoreApplication::tr("Sketch"));
                connect(act.get(),
                        SIGNAL(toggled(bool)),
                        this,
                        SLOT(onSketchTriggered(bool)));
                act->setIcon(QIcon(QPixmap(":/images/sketch_mode_sketch.png")));
            }
            break;
        case MODE_ERASING:
            {
                act->setText(QCoreApplication::tr("Erase Sketch"));
                connect(act.get(),
                        SIGNAL(toggled(bool)),
                        this,
                        SLOT(onEraseTriggered(bool)));
                act->setIcon(QIcon(QPixmap(":/images/sketch_mode_erase.png")));
            }
            break;
        default:
            break;
        }

        // Set current mode
        if (*iter == selected_mode)
        {
            act->setChecked(true);
        }
        actions_.push_back(act);
    }

    // add separator
    shared_ptr<QAction> separator(new QAction(sketch_modes_.get()));
    separator->setSeparator(true);
    actions_.push_back(separator);
}

void SketchActions::generateSketchColors(const SketchColors & colors,
                                         const SketchColor selected_color)
{
    colors_.reset(new QActionGroup(0));
    colors_->setExclusive(true);

    SketchColors::const_iterator begin = colors.begin();
    SketchColors::const_iterator end   = colors.end();
    for(SketchColors::const_iterator iter = begin; iter != end; ++iter)
    {
        // Add to group automatically.
        shared_ptr<QAction> act(new QAction(colors_.get()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);

        switch (*iter)
        {
        case SKETCH_COLOR_WHITE:
            {
                act->setText(QCoreApplication::tr("White"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_color_white.png")));
            }
            break;
        case SKETCH_COLOR_LIGHT_GRAY:
            {
                act->setText(QCoreApplication::tr("Light Gray"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_color_light_grey.png")));
            }
            break;
        case SKETCH_COLOR_DARK_GRAY:
            {
                act->setText(QCoreApplication::tr("Dark Gray"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_color_dark_grey.png")));
            }
            break;
        case SKETCH_COLOR_BLACK:
            {
                act->setText(QCoreApplication::tr("Black"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_color_black.png")));
            }
        default:
            break;
        }

        // Set current mode
        if (*iter == selected_color)
        {
            act->setChecked(true);
        }
        actions_.push_back(act);
    }

    // add separator
    shared_ptr<QAction> separator(new QAction(colors_.get()));
    separator->setSeparator(true);
    actions_.push_back(separator);

    connect(colors_.get(),
            SIGNAL(triggered(QAction*)),
            this,
            SLOT(onColorTriggered(QAction*)));
}

void SketchActions::generateSketchShapes(const SketchShapes & shapes,
                                         const SketchShape selected_shape)
{
    shapes_.reset(new QActionGroup(0));
    shapes_->setExclusive(true);

    SketchShapes::const_iterator begin = shapes.begin();
    SketchShapes::const_iterator end   = shapes.end();
    for(SketchShapes::const_iterator iter = begin; iter != end; ++iter)
    {
        // Add to group automatically.
        shared_ptr<QAction> act(new QAction(shapes_.get()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);

        // Set current mode
        if (*iter == selected_shape)
        {
            act->setChecked(true);
        }

        switch (*iter)
        {
        case SKETCH_SHAPE_0:
            {
                act->setText(QCoreApplication::tr("Shape 1"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_shape_1.png")));
            }
            break;
        case SKETCH_SHAPE_1:
            {
                act->setText(QCoreApplication::tr("Shape 2"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_shape_2.png")));
            }
            break;
        case SKETCH_SHAPE_2:
            {
                act->setText(QCoreApplication::tr("Shape 3"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_shape_3.png")));
            }
            break;
        case SKETCH_SHAPE_3:
            {
                act->setText(QCoreApplication::tr("Shape 4"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_shape_4.png")));
            }
            break;
        case SKETCH_SHAPE_4:
            {
                act->setText(QCoreApplication::tr("Shape 5"));
                act->setIcon(QIcon(QPixmap(":/images/sketch_shape_5.png")));
            }
            break;
        default:
            break;
        }
        actions_.push_back(act);
    }

    // add separator
    shared_ptr<QAction> separator(new QAction(shapes_.get()));
    separator->setSeparator(true);
    actions_.push_back(separator);

    connect(shapes_.get(),
            SIGNAL(triggered(QAction*)),
            this,
            SLOT(onShapeTriggered(QAction*)));
}

SketchActionsType SketchActions::getSelectedValue(int & value, bool & checked)
{
    switch (ctx_.active_type)
    {
    case ANNOTATION_MODE:
        value = ctx_.cur_anno_mode;
        break;
    case SKETCH_MODE:
        value = ctx_.cur_sketch_mode;
        break;
    case SKETCH_COLOR:
        value = ctx_.cur_color;
        break;
    case SKETCH_SHAPE:
        value = ctx_.cur_shape;
        break;
    default:
        value = -1;
        break;
    }

    checked = ctx_.active_checked_status;
    return ctx_.active_type;
}

void SketchActions::setSketchShape(const SketchShape shape)
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ((*iter)->data().toInt() == static_cast<int>(shape))
        {
            (*iter)->setChecked(true);
        }
        else if ((*iter)->data().toInt() >= INVALID_SKETCH_SHAPE &&
                 (*iter)->data().toInt() < UNKNOWN_SKETCH_SHAPE)
        {
            (*iter)->setChecked(false);
        }
    }
}

void SketchActions::setSketchColor(const SketchColor color)
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ((*iter)->data().toInt() == static_cast<int>(color))
        {
            (*iter)->setChecked(true);
        }
        else if ((*iter)->data().toInt() >= INVALID_SKETCH_COLOR &&
                 (*iter)->data().toInt() < UNKNOWN_SKETCH_COLOR)
        {
            (*iter)->setChecked(false);
        }
    }
}

void SketchActions::setAnnotationMode(const AnnotationMode mode, bool checked)
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ((*iter)->data().toInt() == static_cast<int>(mode))
        {
            (*iter)->setChecked(checked);
        }
        else if ((*iter)->data().toInt() > INVALID_ANNOTATION_MODE &&
                 (*iter)->data().toInt() < UNKNOWN_ANNOTATION_MODE)
        {
            (*iter)->setChecked(false);
        }
    }
}

void SketchActions::setSketchMode(const SketchMode mode, bool checked)
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ((*iter)->data().toInt() == static_cast<int>(mode))
        {
            (*iter)->setChecked(checked);
        }
        else if ((*iter)->data().toInt() >= MODE_SKETCHING &&
                 (*iter)->data().toInt() < MODE_UNKNOWN)
        {
            (*iter)->setChecked(false);
        }
    }
}

}
