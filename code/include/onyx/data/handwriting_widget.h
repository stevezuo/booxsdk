#ifndef NABOO_LIB_HANDWRITING_WIDGET_H_
#define NABOO_LIB_HANDWRITING_WIDGET_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"

namespace sketch
{
    class SketchProxy;
};

namespace handwriting
{

/// Handwriting widget.
class HandwritingWidget : public QWidget
{
    Q_OBJECT
public:
    HandwritingWidget(QWidget *parent);
    ~HandwritingWidget();

    void attachSketchProxy(sketch::SketchProxy *proxy);
    void deattachSketchProxy();

public Q_SLOTS:
    void onClearSketches();

private:
    void paintEvent(QPaintEvent *pe);
    bool event(QEvent *e);

    void updateSketchProxy();

private:
    sketch::SketchProxy *sketch_proxy_;    ///< sketch proxy
};

}   // namespace handwriting

#endif  // NABOO_LIB_HANDWRITING_WIDGET_H_
