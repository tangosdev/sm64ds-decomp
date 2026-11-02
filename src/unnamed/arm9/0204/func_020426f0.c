/* The write callback func_020423dc gives the child's "rom" archive, a
   NitroSDK FS_ARCHIVE_WRITE_FUNC: 1 (FS_RESULT_FAILURE) for every write. */
#include "types.h"
#include "private/fs_views.h"

int func_020426f0(FSArchive *p_arc, const void *src, u32 pos, u32 size) {
    return 1;
}
