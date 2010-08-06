#include "onyx/data/annotation_page.h"

namespace anno
{

AnnotationPage::AnnotationPage(void)
: global_id_( -1 )
, loaded_( false )
{
}

AnnotationPage::~AnnotationPage(void)
{
}

QDataStream& operator << (QDataStream & out, const AnnotationPage & page)
{
    out << page.annotations_;
    return out;
}

QDataStream& operator >> (QDataStream & in, AnnotationPage & page)
{
    in >> page.annotations_;
    return in;
}

}
