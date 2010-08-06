#include "onyx/data/annotation_agent.h"
#include "onyx/data/annotation_io.h"

namespace anno
{

AnnotationAgent::AnnotationAgent()
{
}

AnnotationAgent::~AnnotationAgent()
{
}

/// Open an annotation document by given path.
/// \param doc_path The path of the annotation document.
bool AnnotationAgent::open( const QString & doc_path )
{
    // get the document
    AnnotationDocumentPtr doc = getDocument( doc_path );
    if (doc.get() == NULL)
    {
        // the document is invalid
        return false;
    }
    return true;
}

/// Close an annotation document by given path.
/// \param doc_path The path of the annotation document.
bool AnnotationAgent::close( const QString & doc_path )
{
    DocumentIter iter = docs_.find( doc_path );
    if (iter == docs_.end())
    {
        // if the document is not here, return false
        return false;
    }

    docs_.erase(iter);
    return true;
}

/// Close all of the opened annotation documents.
void AnnotationAgent::close()
{
    DocumentIter doc_iter = docs_.begin();
    for (; doc_iter != docs_.end(); doc_iter++)
    {
        shared_ptr<AnnotationIO> io = AnnotationIO::getIO(doc_iter.value()->path(), false);
        if (io.get())
        {
            io->close();
        }
    }
    docs_.clear();
}

/// Save the annotation data of given document into database.
/// \param doc_path Path of the saving document.
bool AnnotationAgent::save( const QString & doc_path )
{
    // get the document
    AnnotationDocumentPtr doc = getDocument(doc_path);
    if (doc.get() == NULL)
    {
        // the document is invalid
        return false;
    }

    // save every dirty pages
    Pages & dirty_pages = doc->getDirtyPages();
    if ( dirty_pages.empty() )
    {
        return true;
    }

    shared_ptr<AnnotationIO> io = AnnotationIO::getIO( doc_path, this );
    if (io.get() == NULL)
    {
        // the database cannot be built
        return false;
    }

    PagesIter iter = dirty_pages.begin();
    for (; iter != dirty_pages.end(); iter++)
    {
        if ( !io->savePage( iter.value() ) )
        {
            continue;
        }
    }
    dirty_pages.clear();
    return true;
}

/// Save annotation data of all opened documents into database.
void AnnotationAgent::save()
{
    DocumentIter iter = docs_.begin();
    for (; iter != docs_.end(); iter++)
    {
        AnnotationDocumentPtr doc = *iter;

        // save every dirty pages
        Pages & dirty_pages = doc->getDirtyPages();
        if (dirty_pages.empty())
        {
            continue;
        }

        shared_ptr<AnnotationIO> io = AnnotationIO::getIO( doc->path(), this );
        if (io.get() == NULL)
        {
            continue;
        }

        PagesIter iter = dirty_pages.begin();
        for (; iter != dirty_pages.end(); iter++)
        {
            if ( !io->savePage( iter.value() ) )
            {
                continue;
            }
        }

        dirty_pages.clear();
    }
}

/// Load all of the annotation pages in a given document. Returns true if loading succeeds.
/// \param doc_path The path of given document.
bool AnnotationAgent::loadAllPages( const QString & doc_path )
{
    // get the document
    AnnotationDocumentPtr doc = getDocument(doc_path);
    if (doc.get() == NULL)
    {
        // the document is invalid
        return false;
    }

    Pages & pages = doc->pages();
    if ( pages.empty() )
    {
        return true;
    }

    // get the io
    shared_ptr<AnnotationIO> io = AnnotationIO::getIO( doc_path, this );
    if (io.get() == NULL)
    {
        return false;
    }

    Pages::iterator iter = pages.begin();
    for ( ; iter != pages.end(); ++iter )
    {
        AnnotationPagePtr page = iter.value();
        if ( page->globalID() > 0 && page->annotations().empty() )
        {
            // get the document
            if ( !io->loadPage( page ) )
            {
                return false;
            }
        }
    }
    return true;
}

/// Load an annotation page by given document path and page position. Returns true
/// if the page can be successfully loaded.
/// \param doc_path The path of given document.
/// \param page_position The position of given page.
bool AnnotationAgent::loadPage( const QString & doc_path,
                                const PagePosition & page_position )
{
    // get the document
    AnnotationDocumentPtr doc = getDocument(doc_path);
    if (doc.get() == NULL)
    {
        // the document is invalid
        return false;
    }

    AnnotationPagePtr page = doc->getPage( page_position );
    if ( page->isLoaded() )
    {
        return true;
    }

    if ( page->globalID() > 0 && page->annotations().empty() )
    {
        // get the io
        shared_ptr<AnnotationIO> io = AnnotationIO::getIO( doc_path, this );
        if (io.get() == NULL)
        {
            return false;
        }

        return io->loadPage( page );
    }
    return true;
}

/// Returns an annotation document by given path.
AnnotationDocumentPtr AnnotationAgent::getDocument( const QString & doc_path )
{
    DocumentIter idx = docs_.find( doc_path );
    if (idx != docs_.end())
    {
        return idx.value();
    }

    // create a new document
    AnnotationDocumentPtr doc( new AnnotationDocument() );
    if ( !doc->open( doc_path ) )
    {
        return AnnotationDocumentPtr();
    }

    // put new document instance into list
    docs_[doc_path] = doc;

    // get the list of pages id and initialize all of the page instances
    shared_ptr<AnnotationIO> io = AnnotationIO::getIO( doc_path, false );
    if (io.get())
    {
        IDMap ids;
        io->loadPagesID( ids );
        IDMapIter iter = ids.begin();
        for (; iter != ids.end(); iter++)
        {
            AnnotationPagePtr page = doc->getPage( iter.value() );
            page->setGlobalID( iter.key() );
            qDebug("Load Annotation Page:%d, Global ID:%d", iter.value(), iter.key());
        }
    }
    return doc;
}

}
