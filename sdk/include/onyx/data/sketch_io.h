#ifndef SKETCH_IO_H
#define SKETCH_IO_H

#include "onyx/data/database.h"
#include "onyx/data/sketch_page.h"
#include "onyx/data/sketch_document.h"

namespace sketch
{

typedef QByteArray blob;

class SketchIO;
typedef shared_ptr<SketchIO> SketchIOPtr;

class SketchIO
{
public:
    SketchIO();
    ~SketchIO();

    bool open(const QString & doc_name, bool create = true);
    void close();

    // save
    bool savePage(SketchPagePtr page);
    bool saveDocument(SketchDocPtr doc);

    // export
    bool exportDatabase(const QString & doc_name);

    // load
    bool loadDocument(const QString & path, SketchDocPtr doc);
    bool loadPageData(SketchPagePtr page);
    bool loadPages(Pages & pages);

    static SketchIOPtr getIO( const QString & doc_name, bool create = true );

private:
    // initialize
    bool initialTable();

    // check columns
    bool checkColumns();

    // save
    bool createPage(SketchPagePtr page, const blob & data);
    bool updatePageData(SketchPagePtr page, const blob & data);
    bool updatePageKey(SketchPagePtr page);
    bool updatePageBackground(SketchPagePtr page);
    bool removePage(SketchPagePtr page);

private:
    typedef QMap<QString, SketchIOPtr> IOMap;
    typedef IOMap::iterator            IOMapIter;

private:
    shared_ptr<DataBase> db_;
    static IOMap         io_map_;    ///< map of all the io instances
};

};

#endif
