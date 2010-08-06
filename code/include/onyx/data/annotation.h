#ifndef ANNOTATION_H_
#define ANNOTATION_H_

#include "onyx/base/base.h"
#include "onyx/base/down_cast.h"
#include "onyx/ui/ui.h"

using namespace ui;

namespace anno
{

/// The Annotation class maintains the title and data of an annotation object
class Annotation
{
public:
    Annotation();
    Annotation(const Annotation & right);

    /// Create an annotation by given title and data.
    Annotation(const QString & title, const QVariant & data);
    ~Annotation(void);

public:
    /// Return the title of an annotation object.
    const QString & title() const { return title_; }

    /// Return the reference of title. Caller can update the title by this function.
    QString & mutable_title() { return title_; }

    /// Return the data of an annotation object. The data can be anything user wants
    /// to put in. Usually the data contains the location information of an annotation.
    const QVariant & data() const { return data_; }

    /// Return the reference of data. Caller can update the data by this function.
    QVariant & mutable_data() { return data_; }

    inline bool operator == (const Annotation & right) const;
    inline Annotation & operator = (const Annotation & right);

private:
    friend QDataStream & operator << ( QDataStream & out, const Annotation & annotation );
    friend QDataStream & operator >> ( QDataStream & in, Annotation & annotation );

private:
    QString title_;
    QVariant data_;
};

/// Compare this annotation with right. Returns true if they are identical.
bool Annotation::operator == (const Annotation & right) const
{
    if (this != &right)
    {
        return (title_ == right.title_ &&
                data_ == right.data_);
    }
    return true;
}

/// Assign the value of right to this annotation.
Annotation & Annotation::operator = (const Annotation & right)
{
    if (this != &right)
    {
        title_ = right.title_;
        data_  = right.data_;
    }
    return *this;
}

/// A list of annotations.
typedef QList<Annotation>       Annotations;

/// Iterator of the annotation list.
typedef Annotations::iterator   AnnotationIter;

QDataStream & operator << ( QDataStream & out, const Annotation & annotation );
QDataStream & operator >> ( QDataStream & in, Annotation & annotation );

};
#endif
