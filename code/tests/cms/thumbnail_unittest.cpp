// Copyright 2007 Onyx International Inc.
// All Rights Reserved.
// Author: John

#include "gtest/gtest.h"
#include "onyx/base/base.h"
#include "onyx/cms/content_thumbnail.h"

using namespace cms;

namespace
{

/// Open database that does not exist.
/// Results: It can create a new database.
TEST(ThumbnailTest, SaveAndLoad)
{
    QDir current = QDir::current();

    ContentThumbnail thumbs(current.absolutePath());
    EXPECT_TRUE(thumbs.open());

    // Create test image.
    static const int WIDTH = 100;
    static const int HEIGHT = 100;
    QImage image(WIDTH, HEIGHT, QImage::Format_ARGB32);
    unsigned char *data = image.bits();
    for(int i = 0; i < HEIGHT; ++i)
    {
        for(int j = 0; j < WIDTH; ++j)
        {
            *data++ = j & 0xff;
            *data++ = i & 0xff;
            *data++ = j & 0xff;
            *data++ = i & 0xff;
        }
    }

    static const QString FILE_NAME = "sample.png";
    image.save(FILE_NAME, "png");

    QImage result;
    EXPECT_FALSE(thumbs.loadThumbnail(FILE_NAME, THUMBNAIL_LARGE, result));
    EXPECT_TRUE(thumbs.storeThumbnail(FILE_NAME, THUMBNAIL_LARGE, image));
    EXPECT_TRUE(thumbs.loadThumbnail(FILE_NAME, THUMBNAIL_LARGE, result));
    EXPECT_TRUE(result == image);
    EXPECT_TRUE(thumbs.close());

    current.remove(FILE_NAME);
    current.remove(current.filePath(thumbs.databaseName()));
}

TEST(ThumbnailTest, hasThumb)
{
    QFileInfo info("C:\\onyx\\sdk\\doc\\html\\3rdparty.html");
    cms::ContentThumbnail thumbdb(info.absolutePath());
    EXPECT_FALSE(thumbdb.hasThumbnail(info.fileName(), cms::THUMBNAIL_LARGE));

    if (!thumbdb.hasThumbnail(info.fileName(), cms::THUMBNAIL_LARGE))
    {
        thumbdb.storeThumbnail(info.fileName(),
                               cms::THUMBNAIL_LARGE,
                               QImage());
    }

    EXPECT_TRUE(thumbdb.hasThumbnail(info.fileName(), cms::THUMBNAIL_LARGE));
}

}   // end of namespace
