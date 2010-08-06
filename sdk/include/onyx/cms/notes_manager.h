#ifndef CMS_NOTES_MANAGER_H_
#define CMS_NOTES_MANAGER_H_

#include <QtGui/QtGui>
#include <QtSql/QtSql>
#include "onyx/base/base.h"

namespace cms
{

class NoteInfo
{
public:
    NoteInfo();
    NoteInfo(const NoteInfo & right);
    ~NoteInfo();

    const QString & name() const { return name_; }
    QString & mutable_name() { return name_; }

    const QImage & thumbnail() const { return image_; }
    QImage & mutable_thumbnail() { return image_; }
    void thumbnail(QByteArray & ba) const;

    NoteInfo & operator = (const NoteInfo & right);

private:
    QString name_;
    QImage image_;
};
typedef QVector<NoteInfo> Notes;


/// Store all notes index. It does not really store the notes data.
/// It maintains all notes index in center database.
class NotesManager
{
    friend class ContentManager;
public:
    NotesManager(void);
    ~NotesManager(void);

private:
    static bool makeSureTableExist(QSqlDatabase &);
    static bool removeTable(QSqlDatabase &database);

    static QString suggestedName(QSqlDatabase &);
    static int  all(QSqlDatabase &, Notes &notes);
    static bool addIndex(QSqlDatabase &, const NoteInfo & note);
    static bool removeIndex(QSqlDatabase &, const QString & name);
    static bool removeAll(QSqlDatabase &);

};

}

#endif  // CMS_NOTES_MANAGER_H_
