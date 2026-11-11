/* NitroSDK FS_ConvertPathToFileID: resolve path to a file id through a
 * scratch FSFile. Its one-instruction caller myFS_ConvertPathToFileID reaches
 * it with both arguments still in the registers. */
#include "nitro/fs/file.h"

extern int FSi_FindPath(FSFile *p_dir, const char *path, FSFileID *p_file_id, FSDirPos *p_dir_pos);

int FS_ConvertPathToFileID(FSFileID *p_file_id, const char *path) {
    FSFile file;
    FS_InitFile(&file);
    return FSi_FindPath(&file, path, p_file_id, 0) != 0;
}
