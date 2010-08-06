
#include "onyx/sys/fat.h"
#ifdef BUILD_FOR_ARM
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/msdos_fs.h>
#include <dirent.h>
#include <stdio.h>
#endif

int changeToHidden(const char * folder)
{
#ifdef BUILD_FOR_ARM
    DIR *dir = opendir(folder);
    if (dir == 0)
    {
        printf("Could not open dir %s\n", folder);
        return 0;
    }

    int dir_fd = dirfd(dir);
    if (dir_fd < 0)
    {
        printf("Invalid dir fd %d\n", dir_fd);
        return 0;
    }

    const int attr = 50;
    ioctl(dir_fd, FAT_IOCTL_SET_ATTRIBUTES, &attr);
    closedir(dir);
#endif
    return 1;
}
