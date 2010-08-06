
#include <QBuffer>
#include "onyx/data/content.h"


namespace content
{


QDataStream & operator<< ( QDataStream & out, const Book & book )
{
    out << book.bookid;
    out << book.url;
    out << book.internalid;
    return out;
}

QDataStream & operator>> ( QDataStream & in, Book & book )
{
    in >> book.bookid;
    in >> book.url;
    in >> book.internalid;
    return in;
}

bool save( QByteArray & data, const QVector<Book> & books )
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << books;
    return true;
}

bool load( QByteArray & data, QVector<Book> & books )
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream >> books;
    return true;
}


}
