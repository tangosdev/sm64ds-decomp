#include "nitro/os/system.h"
#include "nitro/fs/file.h"

extern int myFS_OpenFileFast(FSFile *p_file, FSFileID id);
extern void *_Z13func_0201834cP6FSFilei(FSFile *p_file, int flush);
extern int myFS_ConvertPathToFileID(FSFileID *p_file_id, const char *path);

/* Load the file an FSFileID names (see func_0201834c); flush nonzero
   flushes the result from the data cache. */
void *func_020182ec(FSFileID fileId, int flush) {
    FSFile file;
    void *result = 0;
    FS_InitFile(&file);
    if (myFS_OpenFileFast(&file, fileId) == 0)
        _Z12OS_Terminatev();
    else
        result = _Z13func_0201834cP6FSFilei(&file, flush);
    return result;
}

/* Load the ROM file at path into a new heap block, decompressed if it is
   LZ77-tagged, and flush it from the data cache (Ov004_LoadFileByPath). */
void *func_020182bc(const char *path) {
    FSFileID fileId;
    myFS_ConvertPathToFileID(&fileId, path);
    return func_020182ec(fileId, 1);
}
