#include "onyx/sys/sys.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if (argc < 2)
    {
        return -1;
    }

    int color = atoi(argv[1]);
    sys::SysStatus::instance().setGrayScale(color);
    return 0;
}

