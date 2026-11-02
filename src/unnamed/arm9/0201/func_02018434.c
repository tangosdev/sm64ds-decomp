#include "private/fs_views.h"
extern int myFS_ConvertPathToFileID(void *out_file_id, const char *path);
extern int func_02018474(FSFileID fileId, int dst, int size);

/* Read up to size bytes of the ROM file at path into dst (func_02018474).
   func_0203ad84 reads "/BUILDTIME" this way. */
void func_02018434(const char *path, int dst, int size) {
    FSFileID fileId;
    myFS_ConvertPathToFileID(&fileId, path);
    func_02018474(fileId, dst, size);
}
