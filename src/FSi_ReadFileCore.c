/* NitroSDK FSi_ReadFileCore(p_file, dst, len, async): clamp len to what is
 * left between prop.file.pos and prop.file.bottom (and to 0), store the
 * request in arg.readfile, and send FS_COMMAND_READFILE (0) through
 * FSi_SendCommand. A synchronous caller (async == 0; FS_ReadFile)
 * marks the file FS_FILE_STATUS_SYNC (4) first, waits in FS_WaitAsync and
 * returns the bytes the position moved, or -1 on failure;
 * FS_ReadFileAsync returns the clamped length at once.
 * The body follows the SDK's: pos, rest and the requested length are taken
 * up front as constants and len is clamped in place. */
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern int FSi_SendCommand(FSFile *p_file, int command);
extern int FS_WaitAsync(FSFile *p_file);
int FSi_ReadFileCore(FSFile *p_file, void *dst, int len, int async) {
    const int pos = p_file->prop.file.pos;
    const int rest = p_file->prop.file.bottom - pos;
    const unsigned int org = len;
    if (len > rest)
        len = rest;
    if (len < 0)
        len = 0;
    p_file->arg.readfile.dst = dst;
    p_file->arg.readfile.len_org = org;
    p_file->arg.readfile.len = len;
    if (!async)
        p_file->stat |= 4;
    FSi_SendCommand(p_file, 0);
    if (!async) {
        if (FS_WaitAsync(p_file))
            len = p_file->prop.file.pos - pos;
        else
            len = -1;
    }
    return len;
}
