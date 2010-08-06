#include "onyx/sys/sys.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDir dir(QDir::current());
    QString path("snapshot.png");
    if (argc > 1)
    {
        path = dir.absoluteFilePath(argv[1]);
    }
    else
    {
        path = dir.absoluteFilePath(path);
    }

    sys::SysStatus::instance().snapshot(path);
    return 0;
}

