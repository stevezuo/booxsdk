#ifndef BASE_VIEW_H_
#define BASE_VIEW_H_

#include "onyx/base/base.h"
#include <QtCore/QtCore>
#include "onyx/base/base_model.h"

using namespace ui;
using namespace base;

namespace vbf
{

/// BaseView displays the content of associated document.
/// TODO: NOT SURE we inherit from widget.
class BaseView : public QWidget
{
    Q_OBJECT
public:
    explicit BaseView(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~BaseView(void);

public Q_SLOTS:
    /// Attach a document model to this view.
    virtual void attachModel(BaseModel *model) = 0;

    /// Deattach this view with model
    virtual void deattachModel() = 0;

    /// Query supported interface.
    // virtual bool Query(const char *name, void ** ptr) = 0;

Q_SIGNALS:
    void activated(BaseView *view);
    void deactivate(BaseView *view);
    void modelAttachedSignal();
    void modelDetachedSignal();
    void pageChangedSignal(const int current, const int total);
    void fontSizeChangedSignal(const int size);
    void fontFamilyChanged(const QString &family);
    void zoomFactorChanged(const int factor);
    void orientationChanged(const int roation);

private:
    NO_COPY_AND_ASSIGN(BaseView);

};  // class BaseView

} //namespace vbf

#endif
