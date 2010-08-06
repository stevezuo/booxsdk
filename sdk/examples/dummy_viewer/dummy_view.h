#ifndef BASE_VIEW_H_
#define BASE_VIEW_H_

#include "onyx/base/base.h"
#include "onyx/base/ui.h"

using namespace ui;
using namespace base;

namespace dummy
{

/// BaseView displays the content of associated document.
/// TODO: NOT SURE we inherit from widget.
class DummyView : public Widget
{
    Q_OBJECT
public:
    explicit DummyView(Widget *parent = 0);
    virtual ~DummyView(void);

    void SetPath(const QString &path);

protected:
    void paintEvent(QPaintEvent *);

private:
    QString path_;

private:
    NO_COPY_AND_ASSIGN(DummyView);

};  // class BaseView

} //namespace vbf

#endif
