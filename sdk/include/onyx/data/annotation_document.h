#ifndef ANNOTATION_DOCUMENT_H_
#define ANNOTATION_DOCUMENT_H_

#include "onyx/data/annotation_page.h"

using namespace ui;
namespace anno
{

class AnnotationDocument
{
public:
    AnnotationDocument(void);
    ~AnnotationDocument(void);

    inline const QString & path() { return path_; }
    inline Pages & pages() { return pages_; }

    bool open( const QString & path );
    bool close();

    AnnotationPagePtr getPage( const PagePosition & position,
                               bool create = true );

    bool isPageDirty( const AnnotationPagePtr page );
    void setPageDirty( AnnotationPagePtr page );
    Pages & getDirtyPages() { return dirty_pages_; }

private:
    void clearPages();

private:
    QString path_;            /// path of the document
    Pages   pages_;           /// sketch pages
    Pages   dirty_pages_;     /// dirty page is the page which contains
                              /// new strokes and is not saved.
};

typedef shared_ptr<AnnotationDocument>        AnnotationDocumentPtr;
typedef QHash<QString, AnnotationDocumentPtr> Documents;
typedef Documents::iterator                   DocumentIter;
};
#endif
