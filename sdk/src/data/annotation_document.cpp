#include "onyx/data/annotation_document.h"

namespace anno
{

AnnotationDocument::AnnotationDocument(void)
{
}

AnnotationDocument::~AnnotationDocument(void)
{
    close();
}

bool AnnotationDocument::open( const QString & path )
{
    if ( !close() )
    {
        return false;
    }

    path_ = path;
    return true;
}

bool AnnotationDocument::close()
{
    clearPages();
    path_.clear();
    return true;
}

AnnotationPagePtr AnnotationDocument::getPage( const PagePosition & position,
                                               bool create )
{
    // get a page by key
    PagesIter idx = pages_.find( position );
    if (idx != pages_.end())
    {
        return idx.value();
    }

    if (create)
    {
        // if the page is empty, create a new one
        AnnotationPagePtr page( new AnnotationPage() );
        page->setPosition( position );
        pages_[position] = page;
        return page;
    }
    return AnnotationPagePtr();
}

bool AnnotationDocument::isPageDirty( const AnnotationPagePtr page )
{
    return ( dirty_pages_.find(page->position()) != dirty_pages_.end() );
}

void AnnotationDocument::setPageDirty( AnnotationPagePtr page )
{
    dirty_pages_[page->position()] = page;
}

void AnnotationDocument::clearPages()
{
    pages_.clear();
}

}
