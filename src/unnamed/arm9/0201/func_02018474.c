#include "nitro/os/system.h"
#include "nitro/fs/file.h"

extern int myFS_OpenFileFast(FSFile *p_file, FSFileID id);
extern int _Z13func_020184e0P6FSFilejj(FSFile *p_file, unsigned int dst, unsigned int size);

/* Open the file fileId names (func_02018434 passes it by value), crash if
   it will not open, and read up to size bytes of it into dst
   (func_020184e0). */
int func_02018474(FSFileID fileId, int dst, int size) {
    FSFile file;
    FS_InitFile(&file);
    if (myFS_OpenFileFast(&file, fileId) == 0) {
        _Z12OS_Terminatev();
        return 0;
    }
    return _Z13func_020184e0P6FSFilejj(&file, dst, size);
}
