/* The write callback func_020423dc gives the child's "rom" archive, a
   NitroSDK FS_ARCHIVE_WRITE_FUNC: every write fails. */
#include "types.h"
#include "nitro/fs/archive.h"

int func_020426f0(FSArchive *p_arc, const void *src, u32 pos, u32 size) {
    return FS_RESULT_FAILURE;
}
