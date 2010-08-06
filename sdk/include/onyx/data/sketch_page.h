#ifndef SKETCH_PAGE_H_
#define SKETCH_PAGE_H_

#include "onyx/data/sketch_stroke.h"

using namespace ui;

namespace sketch
{

class SketchPage
{
public:
    SketchPage();
    ~SketchPage(void);

    void setID(const int id);
    const int id() const { return id_; }
    bool isIDValid() const { return id_ >= 0; }

    void setKey(const PageKey & k);
    const PageKey & key() const { return key_; }

    void setOrient(const RotateDegree r);
    RotateDegree orient() const { return orient_; }

    void setContentArea(const QRect & c);
    const QRect & contentArea() const { return content_area_; }

    void setDisplayArea(const QRect & d);
    const QRect & displayArea() const { return display_area_; }

    // background ID
    void setBackgroundImage(const QString & path) { background_image_ = path; }
    const QString & backgroundImage() { return background_image_; }

    // sketch
    bool hitTest(QMouseEvent *e, SketchPosition &position);

    // stroke
    void appendStroke(SketchStrokePtr stroke);
    void clearStrokes();
    void removeStrokes(const Strokes & strokes);
    int  getStrokeCount();
    Strokes & strokes() { return strokes_; }

    // erase
    bool hitTest(const QPoint & p, EraseContext & ctx, Strokes & strokes);

    // io
    int  length();

    // dirty
    void setDataDirty(bool dirty) { is_data_dirty_ = dirty; }
    bool isDataDirty() { return is_data_dirty_; }
    void setKeyDirty(bool dirty) { is_key_dirty_ = dirty; }
    bool isKeyDirty() { return is_key_dirty_; }
    void setBackgroundDirty(bool dirty) { is_background_dirty_ = dirty; }
    bool isBackgroundDirty() { return is_background_dirty_; }
    void setDataLoaded(bool loaded) { data_loaded_ = loaded; }
    bool dataLoaded() const { return data_loaded_; }

    // painting
    void paint(const SketchContext & sketch_ctx, GraphicContext & gc, QPainter & painter);

private:
    bool hitTestStrokes(const QPoint & p, const EraseContext & ctx, Strokes & strokes);
    bool hitTestStrokes(const QLine & line, const EraseContext & ctx, Strokes & strokes);

    // io
    int  getLengthOfAttributes();
    bool dumpAttributes(QDataStream & out) const;
    bool dumpStrokes(QDataStream & out) const;
    bool loadAttributes(QDataStream & in, int & num_strokes);
    bool loadStrokes(QDataStream & in, const int num_strokes);

    friend QDataStream& operator<<(QDataStream & out, const SketchPage & page);
    friend QDataStream& operator>>(QDataStream & in, SketchPage & page);

private:
    // attributes that need to be saved
    RotateDegree    orient_;                /// rotation degree of the page
    QColor          bk_color_;              /// background color
    QRect           content_area_;          /// content area of the page
    QRect           display_area_;          /// display area of the page, the coordination is calculated by the widget

    int             id_;                    /// id of the page, which is retrieved from DB
    QString         background_image_;      /// the path of background image
    PageKey         key_;                   /// key of the page
    QTime           timestamp_;             /// timestamp of the access
    bool            is_data_dirty_;         /// is the data of page dirty?
    bool            is_key_dirty_;          /// is the key of page dirty?
    bool            is_background_dirty_;   /// is the background dirty?
    bool            data_loaded_;           /// is the data of page loaded?
    Strokes         strokes_;               /// the strokes in this page
    SketchPoint     last_erase_point_;      /// last erased point
};

QDataStream& operator<<(QDataStream & out, const SketchPage & page);
QDataStream& operator>>(QDataStream & in, SketchPage & page);

typedef shared_ptr<SketchPage> SketchPagePtr;
typedef QMap<PageKey, SketchPagePtr> Pages;
typedef Pages::iterator PagesIter;

};
#endif
