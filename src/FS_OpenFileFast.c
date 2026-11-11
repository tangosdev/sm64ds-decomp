/* NitroSDK FS_OpenFileFast: open the file a FSFileID names, through the
 * archive it carries (FS_COMMAND_OPENFILEFAST, 6), and mark the FSFile a
 * file (FS_FILE_STATUS_IS_FILE, 0x10) rather than a directory (0x20). */
#include "nitro/fs/file.h"

extern int FSi_SendCommand(FSFile *p_file, int command);

int FS_OpenFileFast(FSFile *p_file, FSFileID file_id) {
    if (!file_id.arc)
        return 0;
    p_file->arc = file_id.arc;
    p_file->arg.openfilefast.id = file_id;
    if (!FSi_SendCommand(p_file, 6))
        return 0;
    p_file->stat |= 0x10;
    p_file->stat &= ~0x20;
    return 1;
}
