/// Author: John

#ifndef BASE_BOOKMARK_H_
#define BASE_BOOKMARK_H_

#include "onyx/base/base.h"
#include <QString>
#include <QVariant>
#include "onyx/cms/content_manager.h"

namespace vbf
{

class Bookmark
{
public:
    Bookmark();
    Bookmark(const Bookmark & right);
    Bookmark(const QString & title, const QVariant & data);
    ~Bookmark(void);

public:
    const QString & title() const { return title_; }
    QString & mutable_title() { return title_; }

    const QVariant & data() const { return data_; }
    QVariant & mutable_data() { return data_; }

    bool operator == (const Bookmark & right) const
    {
        if (this != &right)
        {
            return (title_    == right.title_ &&
                    data_ == right.data_);
        }
        return true;
    }

    Bookmark & operator = (const Bookmark & right)
    {
        if (this != &right)
        {
            title_ = right.title_;
            data_  = right.data_;
        }
        return *this;
    }

private:
    QString title_;
    QVariant data_;
};

QDataStream & operator<< ( QDataStream & out, const Bookmark & bookmark );
QDataStream & operator>> ( QDataStream & in, Bookmark & bookmark );


typedef QVector<Bookmark> Bookmarks;
typedef Bookmarks::iterator BookmarksIter;
typedef Bookmarks::const_iterator ConstBookmarksIter;

/// Insert a bookmark into bookmark vector and make sure
/// the new inserted bookmark is in correct position(pre-order).
/// Caller is able to define its own compare function.
/// \return This function returns false when the same bookmark is found
/// in the bookmark list. Otherwise it returns true.
/// \notice For template reason, I have to implement this function in the
/// header file.
template <class Comp>
bool insertBookmark(Bookmarks & bookmarks,
                    const Bookmark & new_bookmark,
                    Comp cmp)
{
    BookmarksIter begin = bookmarks.begin();
    BookmarksIter end   = bookmarks.end();
    BookmarksIter iter  = begin;
    for(; iter != end; ++iter)
    {
        if (cmp(new_bookmark, *iter))
        {
            break;
        }
        else if (new_bookmark == *iter)
        {
            return false;
        }
    }

    bookmarks.insert(iter, new_bookmark);
    return true;
}

bool removeBookmark(Bookmarks & bookmarks, const Bookmark & bookmark);


/// Store bookmark for the document.
bool loadBookmarks(cms::ContentManager & db,
                   const QString &document_path,
                   Bookmarks & bookmarks);

/// Load bookmarks for the document.
bool saveBookmarks(cms::ContentManager & db,
                   const QString &document_path,
                   const Bookmarks & bookmarks);

}

#endif
