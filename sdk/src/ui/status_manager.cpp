#include "onyx/ui/status_manager.h"

using namespace ui;
namespace vbf
{
// Status Action Group
StatusActionGroup::StatusActionGroup()
: group_(new QActionGroup(0))
{
    group_->setExclusive(true);
}

StatusActionGroup::~StatusActionGroup()
{
}

void StatusActionGroup::setExclusive(bool exclusive)
{
    group_->setExclusive(exclusive);
}

void StatusActionGroup::addAction(StatusActionPtr & action)
{
    action->setCheckable(true);
    action->setActionGroup(actionGroup());
    actions_.push_back(action);
}

void StatusActionGroup::removeAction(const int id)
{
    QList< StatusActionPtr >::iterator iter = actions_.begin();
    for (; iter != actions_.end(); ++iter)
    {
        if (id == (*iter)->data().toInt())
        {
            group_->removeAction((*iter).get());
            actions_.erase(iter);
            break;
        }
    }
}

FunctionStatus StatusActionGroup::getStatus(const int id) const
{
    FunctionStatus ret = FUNC_DISABLE;
    QList< StatusActionPtr >::const_iterator iter = actions_.begin();
    StatusActionPtr action;
    for (; iter != actions_.end(); ++iter)
    {
        if (id == (*iter)->data().toInt())
        {
            action = *iter;
            break;
        }
    }

    if (action.get() != 0 && action->isEnabled())
    {
        if (action->isChecked())
        {
            ret = FUNC_SELECTED;
        }
        else
        {
            ret = FUNC_NORMAL;
        }
    }
    return ret;
}

void StatusActionGroup::setStatus(const int id, const FunctionStatus status)
{
    QList< StatusActionPtr >::iterator iter = actions_.begin();
    StatusActionPtr action;
    for (; iter != actions_.end(); ++iter)
    {
        if (id == (*iter)->data().toInt())
        {
            action = *iter;
            break;
        }
    }

    if (action.get() != 0 && action->isEnabled())
    {
        switch(status)
        {
        case FUNC_DISABLE:
            action->setEnabled(false);
            break;
        case FUNC_SELECTED:
            action->setChecked(true);
            break;
        case FUNC_NORMAL:
            action->setChecked(false);
            break;
        default:
            break;
        }
    }
}

// Status Manager
StatusManager::StatusManager()
{
    handlers_[ID_ZOOM_IN] = &StatusManager::setZoomIn;
    handlers_[ID_ZOOM_BEST_FIT] = &StatusManager::setZoomBestFit;
    handlers_[ID_ZOOM_TO_WIDTH] = &StatusManager::setZoomToWidth;
    handlers_[ID_ZOOM_TO_HEIGHT] = &StatusManager::setZoomToHeight;
    handlers_[ID_PAN] = &StatusManager::setPan;
    handlers_[ID_FREE_PEN] = &StatusManager::setFreePen;
    handlers_[ID_ADD_ANNOTATION] = &StatusManager::setAddAnnotation;
    handlers_[ID_DELETE_ANNOTATION] = &StatusManager::setDeleteAnnotation;
    handlers_[ID_SLIDE_SHOW] = &StatusManager::setSlideShow;
    handlers_[ID_DISPLAY_HYPERLINKS] = &StatusManager::setDisplayHyperlinks;
    handlers_[ID_RETRIEVE_WORD] = &StatusManager::setRetrieveWord;
    handlers_[ID_WAIT_DISPLAY_SEARCH_RES] = &StatusManager::setWaitDispSearchRes;
    handlers_[ID_SKETCHING] = &StatusManager::setSketching;
    handlers_[ID_ERASING] = &StatusManager::setErasing;

    initReadingTools();
    intiZooming();
    initStylus();
    initAnnotation();
    initRunningStatus();
}

StatusManager::~StatusManager()
{
}

void StatusManager::initReadingTools()
{
    reading_tools_group_.setExclusive(false);

    // slide show
    StatusActionPtr slide_show_action( new QAction(0) );
    slide_show_action->setData(ID_SLIDE_SHOW);
    reading_tools_group_.addAction(slide_show_action);

    // display hyperlinks
    StatusActionPtr display_hyperlinks_action( new QAction(0) );
    display_hyperlinks_action->setData(ID_DISPLAY_HYPERLINKS);
    reading_tools_group_.addAction(display_hyperlinks_action);
    reading_tools_group_.setStatus(ID_DISPLAY_HYPERLINKS, FUNC_NORMAL);
}

void StatusManager::intiZooming()
{
    zooming_group_.setExclusive(true);

    // zooming by selection
    StatusActionPtr zoom_selection_action( new QAction(0) );
    zoom_selection_action->setData(ID_ZOOM_IN);
    zooming_group_.addAction(zoom_selection_action);

    // zooming best fit
    StatusActionPtr zoom_best_fit_action( new QAction(0) );
    zoom_best_fit_action->setData(ID_ZOOM_BEST_FIT);
    zooming_group_.addAction(zoom_best_fit_action);

    // zooming to width
    StatusActionPtr zoom_to_width_action( new QAction(0) );
    zoom_to_width_action->setData(ID_ZOOM_TO_WIDTH);
    zooming_group_.addAction(zoom_to_width_action);

    // zooming to height
    StatusActionPtr zoom_to_height_action( new QAction(0) );
    zoom_to_height_action->setData(ID_ZOOM_TO_HEIGHT);
    zooming_group_.addAction(zoom_to_height_action);
}

void StatusManager::initStylus()
{
    stylus_group_.setExclusive(true);

    // zooming by selection
    StatusActionPtr zoom_selection_action( new QAction(0) );
    zoom_selection_action->setData(ID_ZOOM_IN);
    stylus_group_.addAction(zoom_selection_action);

    // pan
    StatusActionPtr pan_action( new QAction(0) );
    pan_action->setData(ID_PAN);
    stylus_group_.addAction(pan_action);

    // pen
    StatusActionPtr free_pen_action( new QAction(0) );
    free_pen_action->setData(ID_FREE_PEN);
    stylus_group_.addAction(free_pen_action);

    // sketch
    StatusActionPtr sketch_action( new QAction(0) );
    sketch_action->setData(ID_SKETCHING);
    stylus_group_.addAction(sketch_action);

    // erase
    StatusActionPtr erase_action( new QAction(0) );
    erase_action->setData(ID_ERASING);
    stylus_group_.addAction(erase_action);

    // delete annotation
    StatusActionPtr delete_annotation_action( new QAction(0) );
    delete_annotation_action->setData(ID_DELETE_ANNOTATION);
    stylus_group_.addAction(delete_annotation_action);
}

void StatusManager::initAnnotation()
{
    annotation_group_.setExclusive(true);

    // add annotation
    StatusActionPtr add_annotation_action( new QAction(0) );
    add_annotation_action->setData(ID_ADD_ANNOTATION);
    annotation_group_.addAction(add_annotation_action);

    // delete annotation
    StatusActionPtr delete_annotation_action( new QAction(0) );
    delete_annotation_action->setData(ID_DELETE_ANNOTATION);
    annotation_group_.addAction(delete_annotation_action);
}

void StatusManager::initRunningStatus()
{
    running_status_group_.setExclusive(false);

    // slide show
    StatusActionPtr wait_display_search_result_action( new QAction(0) );
    wait_display_search_result_action->setData(ID_WAIT_DISPLAY_SEARCH_RES);
    running_status_group_.addAction(wait_display_search_result_action);
}

void StatusManager::setStatus(const FunctionID id, const FunctionStatus s)
{
    StatusHandlers::iterator iter = handlers_.find(id);
    if (iter != handlers_.end())
    {
        SetStatusFunc ptr = iter->second;
        (this->*ptr)(s);
    }
}

void StatusManager::setWaitDispSearchRes(FunctionStatus s)
{
    running_status_group_.setStatus(ID_WAIT_DISPLAY_SEARCH_RES, s);
}

void StatusManager::notify(FunctionID cur_id)
{
    emit stylusChanged(cur_id);
}

void StatusManager::resetAnnotationGroup()
{
    annotation_group_.setStatus(ID_ADD_ANNOTATION, FUNC_NORMAL);
    annotation_group_.setStatus(ID_DELETE_ANNOTATION, FUNC_NORMAL);
}

void StatusManager::setSlideShow(FunctionStatus s)
{
    reading_tools_group_.setStatus(ID_SLIDE_SHOW, s);
    if (s == FUNC_SELECTED)
    {
        notify(ID_SLIDE_SHOW);
    }
    else
    {
        setPan(FUNC_SELECTED);
    }
}

void StatusManager::setDisplayHyperlinks(FunctionStatus s)
{
    reading_tools_group_.setStatus(ID_DISPLAY_HYPERLINKS, s);
}

void StatusManager::setZoomIn(FunctionStatus s)
{
    stylus_group_.setStatus(ID_ZOOM_IN, s);
    zooming_group_.setStatus(ID_ZOOM_IN, s);
    if (s == FUNC_SELECTED)
    {
        resetAnnotationGroup();
        notify(ID_ZOOM_IN);
    }
    else
    {
        setPan(FUNC_SELECTED);
    }
}

void StatusManager::setZoomBestFit(FunctionStatus s)
{
    zooming_group_.setStatus(ID_ZOOM_BEST_FIT, s);
    if (s == FUNC_SELECTED)
    {
        notify(ID_PAN);
    }
}

void StatusManager::setZoomToWidth(FunctionStatus s)
{
    zooming_group_.setStatus(ID_ZOOM_TO_WIDTH, s);
    if (s == FUNC_SELECTED)
    {
        notify(ID_PAN);
    }
}

void StatusManager::setZoomToHeight(FunctionStatus s)
{
    zooming_group_.setStatus(ID_ZOOM_TO_HEIGHT, s);
    if (s == FUNC_SELECTED)
    {
        notify(ID_PAN);
    }
}

void StatusManager::setPan(FunctionStatus s)
{
    stylus_group_.setStatus(ID_PAN, s);
    if (s == FUNC_SELECTED)
    {
        resetAnnotationGroup();
        notify( ID_PAN );
    }
}

void StatusManager::setFreePen(FunctionStatus s)
{
    stylus_group_.setStatus(ID_FREE_PEN, s);
    if (s == FUNC_SELECTED)
    {
        notify(ID_FREE_PEN);
    }
    else
    {
        setPan(FUNC_SELECTED);
    }
}

/// TOOD. This function is deprecated. Keep it for Foxit PDF viewer.
void StatusManager::setRetrieveWord(FunctionStatus s)
{
}

void StatusManager::setSketching(FunctionStatus s)
{
    stylus_group_.setStatus(ID_SKETCHING, s);
    if (s == FUNC_SELECTED)
    {
        resetAnnotationGroup();
        notify(ID_SKETCHING);
    }
    else
    {
        setPan(FUNC_SELECTED);
    }
}

void StatusManager::setErasing(FunctionStatus s)
{
    stylus_group_.setStatus(ID_ERASING, s);
    if (s == FUNC_SELECTED)
    {
        resetAnnotationGroup();
        notify(ID_ERASING);
    }
    else
    {
        setPan(FUNC_SELECTED);
    }
}

void StatusManager::setAddAnnotation(FunctionStatus s)
{
    annotation_group_.setStatus(ID_ADD_ANNOTATION, s);
    if (s == FUNC_SELECTED)
    {
        setFreePen(FUNC_SELECTED);
        notify(ID_ADD_ANNOTATION);
    }
    else
    {
        annotation_group_.setStatus(ID_DELETE_ANNOTATION, FUNC_NORMAL);
        setPan(FUNC_SELECTED);
    }
}

void StatusManager::setDeleteAnnotation(FunctionStatus s)
{
    annotation_group_.setStatus(ID_DELETE_ANNOTATION, s);
    if (s == FUNC_SELECTED)
    {
        stylus_group_.setStatus(ID_DELETE_ANNOTATION, s);
        notify(ID_DELETE_ANNOTATION);
    }
    else
    {
        annotation_group_.setStatus(ID_ADD_ANNOTATION, FUNC_NORMAL);
        setPan(FUNC_SELECTED);
    }
}

}
