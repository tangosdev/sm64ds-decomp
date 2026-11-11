/* NitroSDK FS_OpenFileDirect(p_file, p_arc, image_top, image_bottom,
 * file_index): store the archive and arg.openfiledirect, send
 * FS_COMMAND_OPENFILEDIRECT (7) through FSi_SendCommand, and
 * on success mark the handle FS_FILE_STATUS_IS_FILE (0x10), not
 * FS_FILE_STATUS_IS_DIR (0x20). Field names follow the SDK's FSFile, as
 * src/engine/fs/FS_CommandDefault.cpp spells it. */
#include "types.h"
#include "nitro/fs/file.h"

extern int FSi_SendCommand(FSFile *p_file, int command);

int FS_OpenFileDirect(FSFile *p_file, FSArchive *p_arc, u32 image_top, u32 image_bottom,
                      u32 file_index) {
    p_file->arc = p_arc;
    p_file->arg.openfiledirect.index = file_index;
    p_file->arg.openfiledirect.top = image_top;
    p_file->arg.openfiledirect.bottom = image_bottom;
    if (FSi_SendCommand(p_file, 7) == 0)
        return 0;
    p_file->stat |= 0x10;
    p_file->stat &= ~0x20;
    return 1;
}
