#include "private/fs_views.h"

extern int myFS_ConvertPathToFileID(FSFileID *p_file_id, const char *path);
/* the ROM archive (see func_02018598.c) */
extern FSArchive *data_0209d3b4;

/* Resolve a ROM path to its FAT file id, keeping the archive the FSFileID
   names for func_02018598. myFS_ConvertPathToFileID crashes on a path that
   does not resolve. */
int func_020189f0(const char *path) {
    FSFileID file_id;
    myFS_ConvertPathToFileID(&file_id, path);
    data_0209d3b4 = file_id.arc;
    return file_id.file_id;
}
