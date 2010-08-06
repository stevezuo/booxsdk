#ifndef SKETCH_DOCUMENT_H_
#define SKETCH_DOCUMENT_H_

#include "onyx/data/sketch_utils.h"
#include "onyx/data/sketch_page.h"

using namespace ui;
namespace sketch
{

class SketchIO;
typedef shared_ptr<SketchIO> SketchIOPtr;

class SketchDocument
{
public:
    SketchDocument(void);
    ~SketchDocument(void);

    // open & close
    bool open(const QString & path);
    bool close();
    const QString & path() { return path_; }

    // retrive a sketch page
    SketchPagePtr getPage(const PageKey & key, bool create, const QString & background_image);
    SketchPagePtr insertPage(const PageKey & key, const QString & background_image);
    bool removePage(const PageKey & key, SketchIOPtr io);

    // load all pages
    bool loadAllPages(SketchIOPtr io);

    // hit test
    bool hitTest(QMouseEvent *e, SketchPagePtr & page, SketchPosition & pos);

    // sketch context
    void updateSketchContext(const SketchContext & ctx);
    void updateEraseContext(const EraseContext & ctx);
    SketchContext & sketchContext() { return sketch_ctx_; }
    EraseContext & eraseContext() { return erase_ctx_; }

    // has dirty pages
    bool hasDirtyPages();

    // active pages
    bool isPageActive(const SketchPagePtr page);
    bool activatePage(const PageKey & page_key);
    bool deactivatePage(const PageKey & page_key);
    void deactivateAll();
    Pages & pages() { return pages_; }
    Pages & activatedPages() { return activated_pages_; }

    // count
    int getPageCount() { return pages_.size(); }
    PageKey getLastPage();
    PageKey getFirstPage();

private:
    void clearPages();

private:
    QString         path_;              /// path of the document

    SketchContext   sketch_ctx_;        /// context of current sketching
    EraseContext    erase_ctx_;         /// context of erasing

    Pages           pages_;             /// sketch pages
    Pages           activated_pages_;   /// activated pages
};

typedef shared_ptr<SketchDocument> SketchDocPtr;

};
#endif
