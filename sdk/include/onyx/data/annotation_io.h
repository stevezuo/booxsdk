#ifndef ANNOTATION_IO_H_
#define ANNOTATION_IO_H_

#include "onyx/data/database.h"
#include "onyx/data/annotation.h"
#include "onyx/data/annotation_page.h"
#include "onyx/data/annotation_document.h"

namespace anno
{

typedef QMap<int, PagePosition> IDMap;
typedef IDMap::iterator         IDMapIter;
typedef QByteArray blob;

class AnnotationIO
{
public:
    AnnotationIO();
    ~AnnotationIO();

    bool open(const QString doc_name, bool create = true);
    void close();

    // save
    bool savePage( AnnotationPagePtr page );
    bool saveDocument( AnnotationDocumentPtr doc );

    // load
    bool loadDocument( const QString & path, AnnotationDocumentPtr doc );
    bool loadPage( AnnotationPagePtr page );
    void loadPagesID( IDMap & map );

    static shared_ptr<AnnotationIO> getIO( const QString & doc_name, bool create = true );

private:
    // initialize
    bool initialTable();

    // save
    bool createPage( AnnotationPagePtr page, const blob & data );
    bool updatePage( AnnotationPagePtr page, const blob & data );
    bool removePage( AnnotationPagePtr page );

private:
    typedef QMap< QString, shared_ptr<AnnotationIO> > IOMap;
    typedef IOMap::iterator                           IOMapIter;

private:
    shared_ptr<DataBase> db_;
    static IOMap         io_map_;    ///< map of all the io instances
};

};

#endif
