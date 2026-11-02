#include "private/fs_views.h"

extern int FSi_SendCommand(void *self, int arg);

/* NitroSDK FS_CloseFile: send FS_COMMAND_CLOSEFILE (8), then clear arc, set
 * command to FS_COMMAND_INVALID (0xc) and drop FS_FILE_STATUS_IS_FILE |
 * FS_FILE_STATUS_IS_DIR (0x30). */
typedef struct FSFile FSFile;

int FS_CloseFile(char *self)
{
    FSFile *p_file = (FSFile *)self;

    if (FSi_SendCommand(self, 8) == 0)
        return 0;

    p_file->arc = 0;
    p_file->command = 0xc;
    p_file->stat &= ~0x30;
    return 1;
}
