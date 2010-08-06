
#include "onyx/base/base.h"
#include "onyx/ui/ui.h"
#include "onyx/data/bookmark.h"
#include "gtest/gtest.h"



/// Store a bookmark into stream and load it again to see
/// if they are equal.
TEST(BookmarkTest, DataStream)
{
    using namespace vbf;

    // Construct stream on buffer.
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream write(&buffer);

    // Prepare bookmark and store it in the stream
    QString title = "bookmark 1";
    QVariant data(3.1415926f);
    Bookmark a(title, data);
    write << a;

    // Another stream.
    buffer.seek(0);
    QDataStream read(&buffer);
    Bookmark b;
    read >> b;

    EXPECT_TRUE(a == b);
}

/// Store a list bookmark into stream and load them again to see
/// if they are equal.
TEST(BookmarkTest, Bookmarks)
{
    using namespace vbf;

    // Construct stream on buffer.
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    QDataStream write(&buffer);

    // Prepare bookmark list and store them in the stream.
    static const int SIZE = 10;
    Bookmarks a;
    for(int i = 0; i < SIZE; ++i)
    {
        QString title("Bookmark %1");
        title = title.arg(i);
        QVariant data(i);
        Bookmark tmp(title, data);
        a.push_back(tmp);
    }
    write << a;

    // Another stream.
    buffer.seek(0);
    QDataStream read(&buffer);
    Bookmarks b;
    read >> b;


    EXPECT_TRUE(a.size() == b.size());

    for(int i = 0; i < SIZE; ++i)
    {
        EXPECT_TRUE(a[i] == b[i]);
    }
}

