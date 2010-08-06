#include "onyx/data/annotation_io.h"
#include "onyx/data/annotation_agent.h"

namespace anno
{

AnnotationIO::IOMap AnnotationIO::io_map_;

AnnotationIO::AnnotationIO()
{
}

AnnotationIO::~AnnotationIO()
{
    close();
}

bool AnnotationIO::open(const QString doc_name, bool create)
{
    bool exist = DataBase::isDBExist( doc_name );
    if (exist || create)
    {
        db_ = DataBase::getDB( doc_name );
        if (db_ != 0 && db_->database() != 0)
        {
            bool ret = initialTable();
            if (!ret)
            {
                qWarning("Cannot Initialize the Annotation Table");
            }
            return ret;
        }
    }

    return false;
}

void AnnotationIO::close()
{
    if (db_.get())
    {
        db_->close();
    }
}

bool AnnotationIO::initialTable()
{
    QSqlQuery query(*(db_->database()));
    return query.exec("create table if not exists annotation ("
                      "id integer primary key, "
                      "page_position integer,"
                      "data blob"
                      ")");
}

bool AnnotationIO::savePage( AnnotationPagePtr page )
{
    if ( !page->annotations().empty() )
    {
        // construct an empty QByteArray to store the data of annotations
        blob data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);
        QDataStream stream(&buffer);

        // save the data of page into the stream
        stream << *page;

        if ( page->globalID() >= 0 )
        {
            updatePage( page, data );
        }
        else
        {
            createPage( page, data );
        }
    }
    else
    {
        if ( page->globalID() >= 0 )
        {
            removePage(page);
        }
    }
    return true;
}

bool AnnotationIO::createPage( AnnotationPagePtr page, const blob & data )
{
    if (db_.get() == NULL)
    {
        return false;
    }

    QSqlQuery statement( *(db_->database()) );
    statement.prepare( "insert into annotation (page_position, data)"
                       " values (?, ?)" );

    statement.addBindValue( page->position() );
    statement.addBindValue( data );

    if (statement.exec())
    {
        page->setGlobalID( statement.lastInsertId().toInt() );
        qDebug("Save page:%d ID:%d succeeds", page->position(), page->globalID());
        return true;
    }
    return false;
}

bool AnnotationIO::updatePage( AnnotationPagePtr page, const blob & data )
{
    if (db_.get() == NULL)
    {
        return false;
    }

    QSqlQuery statement( *(db_->database()) );
    statement.prepare( "update annotation set "
                       "data = ? where id = ?" );
    statement.addBindValue( data );
    statement.addBindValue( page->globalID() );
    return statement.exec();
}

bool AnnotationIO::removePage( AnnotationPagePtr page )
{
    if (db_.get() == NULL)
    {
        return false;
    }

    QSqlQuery statement( *(db_->database()) );
    statement.prepare( "delete from annotation where "
                       "id = ?" );
    statement.addBindValue( page->globalID() );
    if ( statement.exec() )
    {
        page->setGlobalID(-1);
        return true;
    }
    return false;
}

bool AnnotationIO::saveDocument( AnnotationDocumentPtr doc )
{
    // TODO. Implement Me
    return false;
}

bool AnnotationIO::loadDocument( const QString & path,
                                 AnnotationDocumentPtr doc )
{
    // TODO. Implement Me
    return false;
}

bool AnnotationIO::loadPage( AnnotationPagePtr page )
{
    if (db_.get() == NULL)
    {
        return false;
    }

    QSqlQuery query( *(db_->database()) );
    query.prepare( "select data from annotation "
                   "where id = ?" );
    query.addBindValue( page->globalID() );
    if ( query.exec() && query.next() )
    {
        // construct a data stream
        // shallow copy
        blob bytes = query.value(0).toByteArray();
        QDataStream stream(&bytes, QIODevice::ReadOnly);
        stream >> *page;
        page->setLoaded();
        return true;
    }
    return false;
}

void AnnotationIO::loadPagesID( IDMap & map )
{
    if (db_.get() == NULL)
    {
        return;
    }

    QSqlQuery query( *(db_->database()) );
    query.prepare("select id, page_position from annotation");
    if (query.exec())
    {
        map.clear();
        while (query.next())
        {
            int idx = 0;
            int id = query.value(idx++).toInt();
            PagePosition page_position = query.value(idx++).toInt();
            map[id] = page_position;
        }
    }
}

shared_ptr<AnnotationIO> AnnotationIO::getIO( const QString & doc_name, bool create )
{
    IOMapIter pos = io_map_.find( doc_name );
    if (pos != io_map_.end())
    {
        return pos.value();
    }

    // construct a new IO instance
    shared_ptr<AnnotationIO> io( new AnnotationIO() );
    if (!io->open(doc_name, create))
    {
        return shared_ptr<AnnotationIO>();
    }
    io_map_[doc_name] = io;
    return io;
}

}
