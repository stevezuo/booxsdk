#include <QApplication>

#include "onyx/data/configuration.h"
#include "gtest/gtest.h"

using namespace vbf;
using namespace cms;


namespace
{

TEST(ConfTest, SaveAndLoad)
{
    static char* name;
    static int FAKE_ARGC = 1;
    static char** FAKE_ARGV = &name;
    QApplication app(FAKE_ARGC, FAKE_ARGV);
    QDir current = QDir::current();
    QString doc_path = current.filePath("temp.doc");

    ContentManager mgr;
    EXPECT_TRUE(openDatabase(doc_path, mgr));

    static const int SIZE = 1024;
    Configuration conf;
    conf.options["a"] = 1;
    conf.options["b"] = "b";
    cms_blob options;
    options.resize(SIZE);
    for(int i = 0; i < SIZE; ++i)
    {
        options[i] = qrand();
    }
    conf.options["c"] = options;

    EXPECT_TRUE(saveDocumentOptions(mgr, doc_path, conf, false));

    Configuration result;
    EXPECT_TRUE(loadDocumentOptions(mgr, doc_path, result));
    EXPECT_TRUE(conf.options == result.options);
}

}   // namespace vbf

