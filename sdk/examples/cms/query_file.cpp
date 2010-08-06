#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "onyx/cms/content_node.h"
#include "onyx/cms/content_manager.h"


using namespace cms;

void ShowUsage()
{
    printf("\nquery file_path\n");
}

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        ShowUsage();
        return 0;
    }

    QString file_path(argv[1]);
    QString db_path;
    cms::getDatabasePath(file_path, db_path);

    ContentManager mgr;
    if (!mgr.open(db_path))
    {
        qFatal("Could not open db %s", qPrintable(db_path));
        return 0;
    }

    bool found = false;
    QTimer t; t.start();
    ContentNode node;
    found = mgr.getContentNode(node, file_path, false);

    if (found)
    {
        qDebug( "id            %lld ", node.id() );
        qDebug( "name          %s ", qPrintable(node.name()));
        qDebug( "size          %lld ", node.size() );
        qDebug( "location      %s ", qPrintable(node.location()));
        qDebug( "title         %s ", qPrintable(node.title()));
        qDebug( "authors       %s ", qPrintable(node.authors()));
        qDebug( "description   %s ", qPrintable(node.description()));
        qDebug( "last_access   %s ", qPrintable(node.last_access()));
        qDebug( "publisher     %s ", qPrintable(node.publisher()));
        qDebug( "md5           %s ", qPrintable(node.md5()));
    }
    else
    {
        qDebug( " Not found " );
    }

    return 0;
}

