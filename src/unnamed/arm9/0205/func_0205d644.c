/* NitroSDK FS_ConvertPathToFileID: resolve path to a file id through a
 * scratch FSFile. Its one-instruction caller myFS_ConvertPathToFileID reaches
 * it with both arguments still in the registers. */
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern void FS_InitFile(void *p_file);
extern int FSi_FindPath(FSFile *p_dir, const char *path, FSFileID *p_file_id, FSDirPos *p_dir_pos);

int func_0205d644(FSFileID *p_file_id, const char *path) {
    FSFile file;
    FS_InitFile(&file);
    return FSi_FindPath(&file, path, p_file_id, 0) != 0;
}
