#ifndef ANNOTATION_PAGE_H_
#define ANNOTATION_PAGE_H_

#include "onyx/data/annotation.h"

using namespace ui;
namespace anno
{

typedef int PagePosition;

class AnnotationPage
{
public:
    AnnotationPage(void);
    ~AnnotationPage(void);

    inline Annotations & annotations();
    inline PagePosition position();
    inline void setPosition( PagePosition pos );

    inline void setGlobalID( const int id );
    inline int globalID() { return global_id_; }

    inline bool isLoaded() const { return loaded_; }
    inline void setLoaded();

private:
    friend QDataStream& operator<<(QDataStream & out, const AnnotationPage & page);
    friend QDataStream& operator>>(QDataStream & in, AnnotationPage & page);

private:
    PagePosition   position_;    /// position of the page
    Annotations    annotations_;
    int            global_id_;
    bool           loaded_;      /// the data of this page has been loaded
                                 /// it is unnecessary to load again
};

/// return the annotations, the client manages appending and removing
Annotations & AnnotationPage::annotations()
{
    return annotations_;
}

PagePosition AnnotationPage::position()
{
    return position_;
}

void AnnotationPage::setPosition( PagePosition pos )
{
    position_ = pos;
}

void AnnotationPage::setGlobalID( const int id )
{
    global_id_ = id;
}

void AnnotationPage::setLoaded()
{
    loaded_ = true;
}

typedef shared_ptr<AnnotationPage> AnnotationPagePtr;
typedef QHash<PagePosition, AnnotationPagePtr> Pages;
typedef Pages::iterator PagesIter;

QDataStream& operator<<(QDataStream & out, const AnnotationPage & page);
QDataStream& operator>>(QDataStream & in, AnnotationPage & page);

};
#endif
