#ifndef SKETCH_PROXY_H_
#define SKETCH_PROXY_H_

#include "onyx/data/sketch_utils.h"
#include "onyx/data/sketch_graphic_context.h"
#include "onyx/data/sketch_document.h"

namespace sketch
{

class SketchProxy : public QObject
{
    Q_OBJECT
public:
    SketchProxy();
    ~SketchProxy();

    // Operate the document
    bool open(const QString & doc_path);
    bool close(const QString & doc_path);
    void close();
    bool save(const QString & doc_path);
    void save();

    bool exportDatabase(const QString & doc_path);
    bool loadFromDatabase(const QString & db_name);

    bool loadPage(const QString & doc_path,
                  const PageKey & page_key,
                  const QString & background_image);
    bool insertPage(const QString & doc_path,
                    const PageKey & page_key,
                    const QString & background_image);
    bool removePage(const QString & doc_path,
                    const PageKey & page_key);
    bool isPageExisting(const QString & doc_path,
                        const PageKey & page_key);
    bool isEmptyPage(const QString & doc_path,
                     const PageKey & page_key);
    QString getBackgroundImage(const QString & doc_path,
                               const PageKey & page_key);
    bool updateBackgroundImage(const QString & doc_path,
                               const PageKey & page_key,
                               const QString & background_id);
    int getPageCount(const QString & doc_path);
    PageKey getLastPage(const QString & doc_path);
    PageKey getFirstPage(const QString & doc_path);

    // configure
    void attachWidget(QWidget *w);
    void deattachWidget(QWidget *w);
    void setDrawingArea(QWidget *w);

    void setMode(SketchMode mode);
    void setContentOrient(const RotateDegree r);
    void setWidgetOrient(const RotateDegree r);
    void setShape(const SketchShape s);
    void setColor(const SketchColor c);
    void setZoom(const ZoomFactor z);
    void setDrawLayer(const int l);
    inline SketchColor getColor() { return sketch_ctx_.color_; }
    inline SketchShape getShape() { return sketch_ctx_.shape_; }

    // paint
    void paintPage(const QString & doc_path, const PageKey & page_key, QPainter & painter);

Q_SIGNALS:
    void requestUpdateScreen();

    void strokeStarted();
    void pointAdded(SketchPoint point);
    void strokeAdded(const Points & points);

public Q_SLOTS:
    bool activateDocument(const QString & doc_path);
    bool activatePage(const QString & doc_path, const PageKey & page_key);
    bool deactivateDocument(const QString & doc_path);
    bool deactivatePage(const QString & doc_path, const PageKey & page_key);
    void deactivateAll();
    bool updatePageDisplayRegion(const QString & doc_path, const PageKey & page_key, const QRect & region);

private Q_SLOTS:
    void onUpdateScreenTimeout();
    void onForceDriverDrawLines();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    typedef QMap<QString, SketchDocPtr>    Documents;
    typedef Documents::iterator            DocumentIter;

    typedef bool (SketchProxy::*SketchFunc)(QMouseEvent *e);
    typedef QMap<SketchStatus, SketchFunc> SketchHandler;
    typedef QMap<SketchMode, SketchHandler> SketchHandlers;

private:
    SketchDocPtr getDocument(const QString & doc_path);

    // operation
    void sketchPenDown(QMouseEvent *e);
    void sketchPenMove(QMouseEvent *e);
    void sketchPenUp(QMouseEvent *e);

    // Sketch
    bool beginSketch(QMouseEvent *e);
    bool moveSketch(QMouseEvent *e);
    bool endSketch(QMouseEvent *e);

    // Erase
    bool beginErase(QMouseEvent *e);
    bool moveErase(QMouseEvent *e);
    bool endErase(QMouseEvent *e);

    // stroke related
    bool hitTest(QMouseEvent *e, SketchDocPtr & hit_doc,
                 SketchPagePtr & hit_page, SketchPosition & pos);
    void beginStroke(SketchDocPtr doc, SketchPagePtr page, const SketchPosition & pos);
    void addPoint(SketchPagePtr page, const SketchPosition & pos, bool is_last_point = false);
    void endStroke(SketchPagePtr page, const SketchPosition & pos);

    // driver related
    void driverDrawLine(const QPoint & p1,
                        const QPoint & p2,
                        const SketchContext & ctx,
                        bool is_last_point = false);
    void driverDrawLines(StrokeLines &lines,
                         const SketchContext & ctx,
                         bool is_last_point = false);
    void driverDrawStrokes(SketchPagePtr page,
                           const Strokes & strokes,
                           const SketchPosition & pos);

    // last position
    void resetLastPosition();
    void keepLastPosition(const SketchPosition & pos);
    void keepLastPage(SketchDocPtr doc, SketchPagePtr page);
    bool isLastPositionValid();

private:
    Documents       docs_;               // supports multiple documents, for virtual document
    Documents       activated_docs_;     // activated documents
    QWidget         *attached_widget_;   // attached widget

    SketchHandlers  sketch_handlers_;    // sketch/erase handlers
    SketchStrokePtr stroke_;             // current stroke
    SketchDocPtr    last_doc_;           // last document
    SketchPagePtr   last_page_;          // last page
    SketchPosition  last_pos_;           // position of the last point

    SketchContext   sketch_ctx_;         // current sketching context
    GraphicContext  gc_;                 // graphic context is used for drawing the stroke
    StrokeLines     stroke_lines_;       // list of small stroke lines, used for fast drawing

    QTimer          erase_update_timer_; // timer controling the update of current screen
    QTimer          driver_draw_timer_;  // timer controling the driver update
    bool            need_update_once_;   // need update the screen of viewer at once
    SketchMode      mode_;               // current sketch mode
    SketchStatus    status_;             // current sketch status
};

};

#endif
