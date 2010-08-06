#ifndef STATUS_MANAGER_H
#define STATUS_MANAGER_H

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"

using namespace ui;
namespace vbf
{

typedef shared_ptr<QAction>         StatusActionPtr;
typedef shared_ptr< QActionGroup >  StatusActionGroupPtr;

// Status Action Group
class StatusActionGroup
{
public:
    StatusActionGroup();
    ~StatusActionGroup();

    void setExclusive(bool exclusive);
    void addAction(StatusActionPtr & action);
    void removeAction(const int id);
    FunctionStatus getStatus(const int id) const;
    void setStatus(const int id, const FunctionStatus status);

private:
    inline QActionGroup * actionGroup();

private:
    StatusActionGroupPtr       group_;
    QList< StatusActionPtr >   actions_;
};


class StatusManager : public QObject
{
    Q_OBJECT
public:
    StatusManager();
    ~StatusManager();

    void setStatus(const FunctionID id, const FunctionStatus s);

    inline bool isZoomIn() const;
    inline bool isPan() const;
    inline bool isSlideShow() const;
    inline bool isDisplayHyperlinks() const;
    inline bool isRetreiveWord() const;
    inline bool isWaitDispSearchRes() const;
    inline bool isAddAnnotation() const;
    inline bool isDeleteAnnotation() const;
    inline bool isSketching() const;
    inline bool isErasing() const;
    inline bool isFreePen() const;

Q_SIGNALS:
    void stylusChanged(const int);

private:
    // Intialization
    void initReadingTools();
    void intiZooming();
    void initStylus();
    void initAnnotation();
    void initRunningStatus();

    // Set status
    void setZoomIn(FunctionStatus s);
    void setZoomBestFit(FunctionStatus s);
    void setZoomToWidth(FunctionStatus s);
    void setZoomToHeight(FunctionStatus s);
    void setPan(FunctionStatus s);
    void setFreePen(FunctionStatus s);
    void setAddAnnotation(FunctionStatus s);
    void setDeleteAnnotation(FunctionStatus s);
    void setSlideShow(FunctionStatus s);
    void setDisplayHyperlinks(FunctionStatus s);
    void setRetrieveWord(FunctionStatus s);
    void setWaitDispSearchRes(FunctionStatus s);
    void setSketching(FunctionStatus s);
    void setErasing(FunctionStatus s);

    void notify(FunctionID cur_id);
    void resetAnnotationGroup();

private:
    typedef void (StatusManager::*SetStatusFunc)(FunctionStatus s);
    typedef std::map<FunctionID, SetStatusFunc> StatusHandlers;

private:
    StatusHandlers    handlers_;           ///< the status handlers table

    StatusActionGroup reading_tools_group_;
    StatusActionGroup zooming_group_;
    StatusActionGroup stylus_group_;
    StatusActionGroup annotation_group_;
    StatusActionGroup running_status_group_;
};

inline QActionGroup * StatusActionGroup::actionGroup()
{
    return group_.get();
}

inline bool StatusManager::isZoomIn() const
{
    return (zooming_group_.getStatus(ID_ZOOM_IN) == FUNC_SELECTED &&
            stylus_group_.getStatus(ID_ZOOM_IN) == FUNC_SELECTED);
}

inline bool StatusManager::isPan() const
{
    return stylus_group_.getStatus(ID_PAN) == FUNC_SELECTED;
}

inline bool StatusManager::isRetreiveWord() const
{
    // TODO. Remove this function
    return false;
}

inline bool StatusManager::isWaitDispSearchRes() const
{
    return running_status_group_.getStatus(ID_WAIT_DISPLAY_SEARCH_RES) == FUNC_SELECTED;
}

inline bool StatusManager::isSketching() const
{
    return stylus_group_.getStatus(ID_SKETCHING) == FUNC_SELECTED;
}

inline bool StatusManager::isErasing() const
{
    return stylus_group_.getStatus(ID_ERASING) == FUNC_SELECTED;
}

inline bool StatusManager::isSlideShow() const
{
    return reading_tools_group_.getStatus(ID_SLIDE_SHOW) == FUNC_SELECTED;
}

inline bool StatusManager::isDisplayHyperlinks() const
{
    return reading_tools_group_.getStatus(ID_DISPLAY_HYPERLINKS) == FUNC_SELECTED;
}

inline bool StatusManager::isFreePen() const
{
    return stylus_group_.getStatus(ID_FREE_PEN) == FUNC_SELECTED;
}

inline bool StatusManager::isAddAnnotation() const
{
    return ( annotation_group_.getStatus(ID_ADD_ANNOTATION) == FUNC_SELECTED &&
             stylus_group_.getStatus(ID_FREE_PEN) == FUNC_SELECTED );
}

inline bool StatusManager::isDeleteAnnotation() const
{
    return ( annotation_group_.getStatus(ID_DELETE_ANNOTATION) == FUNC_SELECTED &&
             stylus_group_.getStatus(ID_DELETE_ANNOTATION) == FUNC_SELECTED );
}

};  // namespace vbf
#endif
