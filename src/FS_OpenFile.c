/* NitroSDK FS_OpenFile: look the path up, then open the file it names.
 *
 * The path rides through r1: the body saves the FSFile in r4, points r0 at
 * its own stack FSFileID and calls FS_ConvertPathToFileID without touching
 * r1, so the path its caller passed is the one that gets looked up. */
#include "nitro/fs/file.h"

int FS_OpenFile(FSFile *p_file, const char *path) {
    FSFileID file_id;
    return FS_ConvertPathToFileID(&file_id, path) && FS_OpenFileFast(p_file, file_id);
}
