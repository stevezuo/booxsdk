#include "onyx/sys/sys.h"
#include "onyx/sys/sys_conf.h"

#include "onyx/data/sketch_io.h"

namespace sketch
{

SketchIO::IOMap SketchIO::io_map_;

SketchIO::SketchIO()
{
}

SketchIO::~SketchIO()
{
    close();
}

bool SketchIO::open(const QString & doc_name, bool create)
{
    bool exist = DataBase::isDBExist( doc_name );
    if (exist || create)
    {
        db_ = DataBase::getDB( doc_name );
        if (db_ != 0 && db_->database() != 0)
        {
            return initialTable();
        }
    }
    return false;
}

void SketchIO::close()
{
    if (db_.get())
    {
        db_->close();
    }
}

bool SketchIO::initialTable()
{
    if (!checkColumns())
    {
        qDebug("Check null columns. Create a new sketch table");
    }

    QSqlQuery query(*(db_->database()));
    bool ret = query.exec("create table if not exists sketch ("
                          "id integer primary key, "
                          "page_id text,"
                          "data blob,"
                          "background_id text"
                          ")");
    if (ret)
    {
        ret = query.exec("create index if not exists id_index on sketch (id) ");
    }
    return ret;
}

bool SketchIO::checkColumns()
{
    QSqlQuery query(*(db_->database()));
    query.prepare("select sql from sqlite_master where name = ?");
    query.addBindValue("sketch");
    bool found = false;
    if (query.exec())
    {
        while (query.next())
        {
            QString sql = query.value(0).toString();
            if (sql.contains("background_id"))
            {
                found = true;
                break;
            }
        }
    }

    if (!found)
    {
        query.prepare("alter table sketch add background_id text");
        return query.exec();
    }
    return true;
}

bool SketchIO::savePage(SketchPagePtr page)
{
    bool ret = true;
    if (page->isKeyDirty())
    {
        ret = updatePageKey(page);
        if (ret)
        {
            page->setKeyDirty(false);
        }
    }

    if (page->isBackgroundDirty() && !page->backgroundImage().isEmpty())
    {
        if (!page->isIDValid())
        {
            blob empty_data;
            createPage(page, empty_data);
        }
        ret = updatePageBackground(page);
        if (ret)
        {
            page->setBackgroundDirty(false);
        }
    }

    if (page->isDataDirty())
    {
        if (page->getStrokeCount() > 0)
        {
            // construct an empty QByteArray to store the page data
            int len = page->length();
            blob data(len, 0);

            // construct a data stream based on the bytes array
            QDataStream stream(&data, QIODevice::WriteOnly);

            // dump the data of page into the stream
            stream << *page;
            if (page->isIDValid())
            {
                ret = updatePageData(page, data);
            }
            else
            {
                ret = createPage(page, data);
            }
        }
        else
        {
            // the page can only be removed when there is record in db and
            // the blob data had been loaded/saved
            if (page->isIDValid())
            {
                ret = removePage(page);
            }
        }

        if (ret)
        {
            page->setDataDirty(false);
        }
    }
    return ret;
}

bool SketchIO::createPage(SketchPagePtr page, const blob & data)
{
    if (db_ == 0)
    {
        return false;
    }

    QSqlQuery statement(*(db_->database()));
    statement.prepare("insert into sketch (page_id, background_id, data)"
                      " values (?, ?, ?)");

    statement.addBindValue(page->key());
    statement.addBindValue(page->backgroundImage());
    statement.addBindValue(data);
    if (statement.exec())
    {
        page->setID(statement.lastInsertId().toInt());
        return true;
    }
    return false;
}

bool SketchIO::updatePageData(SketchPagePtr page, const blob & data)
{
    if (db_ == 0)
    {
        return false;
    }

    QSqlQuery statement(*(db_->database()));
    statement.prepare("update sketch set "
                      "data = ? where id = ?");
    statement.addBindValue(data);
    statement.addBindValue(page->id());
    return statement.exec();
}

bool SketchIO::updatePageKey(SketchPagePtr page)
{
    if (db_ == 0)
    {
        return false;
    }
    QSqlQuery statement(*(db_->database()));
    statement.prepare("update sketch set "
                      "page_id = ? where id = ?");
    statement.addBindValue(page->key());
    statement.addBindValue(page->id());
    return statement.exec();
}

bool SketchIO::updatePageBackground(SketchPagePtr page)
{
    if (db_ == 0)
    {
        return false;
    }
    QSqlQuery statement(*(db_->database()));
    statement.prepare("update sketch set "
                      "background_id = ? where id = ?");
    statement.addBindValue(page->backgroundImage());
    statement.addBindValue(page->id());
    return statement.exec();
}

bool SketchIO::removePage(SketchPagePtr page)
{
    if (db_.get() == NULL)
    {
        return false;
    }

    QSqlQuery statement(*(db_->database()));
    statement.prepare("delete from sketch where "
                      "id = ?");
    statement.addBindValue(page->id());
    if (statement.exec())
    {
        page->setID(-1);
        return true;
    }
    return false;
}

bool SketchIO::saveDocument(SketchDocPtr doc)
{
    return false;
}

bool SketchIO::loadDocument(const QString & path, SketchDocPtr doc)
{
    return false;
}

bool SketchIO::loadPageData(SketchPagePtr page)
{
    if (db_.get() == NULL)
    {
        return false;
    }

    QSqlQuery query(*(db_->database()));
    query.prepare("select data from sketch "
                  "where id = ?");
    query.addBindValue(page->id());
    if (query.exec() && query.next())
    {
        // construct a data stream
        // shallow copy
        QByteArray bytes = query.value(0).toByteArray();
        QDataStream stream(&bytes, QIODevice::ReadOnly);
        stream >> *page;
        return true;
    }
    return false;
}

bool SketchIO::loadPages(Pages & pages)
{
    if (db_ == 0)
    {
        return false;
    }

    QSqlQuery query(*(db_->database()));
    query.prepare("select id, page_id, background_id from sketch");
    if (query.exec())
    {
        pages.clear();
        while (query.next())
        {
            SketchPagePtr page(new SketchPage());
            int idx = 0;
            int id = query.value(idx++).toInt();
            page->setID(id);

            PageKey page_key = query.value(idx++).toString();
            page->setKey(page_key);
            pages[page_key] = page;

            QString background_image = query.value(idx++).toString();
            page->setBackgroundImage(background_image);
        }
    }
    return true;
}

shared_ptr<SketchIO> SketchIO::getIO(const QString & doc_name, bool create)
{
    IOMapIter pos = io_map_.find(doc_name);
    if (pos != io_map_.end())
    {
        return pos.value();
    }

    // construct a new IO instance
    shared_ptr<SketchIO> io( new SketchIO() );
    if (!io->open(doc_name, create))
    {
        return shared_ptr<SketchIO>();
    }
    io_map_[doc_name] = io;
    return io;
}

bool SketchIO::exportDatabase(const QString & doc_name)
{
    if (db_ == 0)
    {
        qDebug("Database does NOT exist");
        return false;
    }

    QDir export_dir;
    if (sys::SystemConfig::notesExportDirectory(doc_name, export_dir))
    {
        QFileInfo db_info(db_->name());
        QString export_path = export_dir.absoluteFilePath(db_info.fileName());
        return sys::SystemConfig::exportNotes(db_->name(), export_path);
    }
    return false;
}

}
