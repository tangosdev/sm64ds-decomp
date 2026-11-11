#include "nitro/fs/file.h"

extern int FSi_SendCommand(FSFile *p_file, int command);

/* NitroSDK FS_CloseFile: send FS_COMMAND_CLOSEFILE (8), then clear arc, set
 * command to FS_COMMAND_INVALID (0xc) and drop FS_FILE_STATUS_IS_FILE |
 * FS_FILE_STATUS_IS_DIR (0x30). */
int FS_CloseFile(FSFile *p_file) {
    if (FSi_SendCommand(p_file, 8) == 0)
        return 0;

    p_file->arc = 0;
    p_file->command = 0xc;
    p_file->stat &= ~0x30;
    return 1;
}
