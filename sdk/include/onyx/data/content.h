#ifndef ONYX_CONTENT_H_
#define ONYX_CONTENT_H_

#include <QtCore/QtCore>

namespace content
{

/// Represent a single book.
struct Book
{
    QString bookid;
    QString url;
    int internalid;
};
typedef QVector<Book> Books;

QDataStream & operator<< ( QDataStream & out, const Book & book );
QDataStream & operator>> ( QDataStream & in, Book & book );

bool save( QByteArray & data, const Books & books );
bool load( QByteArray & data, Books & books );

};

#endif
