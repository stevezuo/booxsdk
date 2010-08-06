#ifndef SKETCH_ACTIONS_H_
#define SKETCH_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

typedef QVector<AnnotationMode> AnnotationModes;
typedef QVector<SketchMode>     SketchModes;
typedef QVector<SketchColor>    SketchColors;
typedef QVector<SketchShape>    SketchShapes;

enum SketchActionsType
{
    INVALID_SKETCH_TYPE = -1,
    ANNOTATION_MODE = 0,
    SKETCH_MODE,
    SKETCH_SHAPE,
    SKETCH_COLOR
};

struct SketchActionsContext
{
    SketchActionsType active_type;
    int               cur_anno_mode;
    int               cur_sketch_mode;
    int               cur_color;
    int               cur_shape;
    bool              active_checked_status;

    SketchActionsContext()
        : active_type(INVALID_SKETCH_TYPE)
        , cur_anno_mode(INVALID_ANNOTATION_MODE)
        , cur_sketch_mode(MODE_UNKNOWN)
        , cur_color(INVALID_SKETCH_COLOR)
        , cur_shape(INVALID_SKETCH_SHAPE)
        , active_checked_status(false) {}
};

class SketchActions : public BaseActions
{
    Q_OBJECT
public:
    SketchActions(void);
    ~SketchActions(void);

public:
    void generateAnnotationMode(const AnnotationModes & modes,
                                const AnnotationMode selected_mode = INVALID_ANNOTATION_MODE);
    void generateSketchMode(const SketchModes & modes,
                            const SketchMode selected_mode = MODE_UNKNOWN);
    void generateSketchColors(const SketchColors & colors,
                              const SketchColor selected_color = SKETCH_COLOR_BLACK);
    void generateSketchShapes(const SketchShapes & shapes,
                              const SketchShape selected_shape = SKETCH_SHAPE_2);

    void setAnnotationMode(const AnnotationMode mode, bool checked);
    void setSketchMode(const SketchMode mode, bool checked);
    void setSketchShape(const SketchShape shape);
    void setSketchColor(const SketchColor color);

    /// Retrieve the selected sketch mode.
    SketchActionsType getSelectedValue(int & value, bool & checked);

private Q_SLOTS:
    void onSketchTriggered(bool checked);
    void onEraseTriggered(bool checked);
    void onAddAnnotationTriggered(bool checked);
    void onDeleteAnnotationTriggered(bool checked);
    void onDisplayAnnotationsTriggered(bool checked);

    void onColorTriggered(QAction *action);
    void onShapeTriggered(QAction *action);

private:
    SketchActionsContext ctx_;

    scoped_ptr<QActionGroup> anno_modes_;
    scoped_ptr<QActionGroup> sketch_modes_;
    scoped_ptr<QActionGroup> colors_;
    scoped_ptr<QActionGroup> shapes_;
};  // SketchActions

}  // namespace ui

#endif  // SKETCH_ACTIONS_H_
