#include "private/fs_views.h"

extern void FS_InitFile(int *s);
extern int myFS_OpenFileFast(int buf, FSFileID ab);
extern void Crash(void);
extern int func_020184e0(int *buf, unsigned int c, unsigned int d);

/* Open the file fileId names (func_02018434 passes it by value), crash if
   it will not open, and read up to size bytes of it into dst
   (func_020184e0). */
int func_02018474(FSFileID fileId, int dst, int size) {
    int file[0x11];
    FS_InitFile(file);
    if (myFS_OpenFileFast((int)file, fileId) == 0) {
        Crash();
        return 0;
    }
    return func_020184e0(file, dst, size);
}
