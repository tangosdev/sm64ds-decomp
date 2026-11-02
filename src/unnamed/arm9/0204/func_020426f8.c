/* The read callback func_020423dc gives the child's "rom" archive, a
   NitroSDK FS_ARCHIVE_READ_FUNC: the archive's tables are in memory, so a
   read copies size bytes from address pos to dst and succeeds (0). */
#include "types.h"
#include "private/fs_views.h"

extern void CpuCopy8(const void *src, void *dst, u32 size);

int func_020426f8(FSArchive *p_arc, void *dst, u32 pos, u32 size) {
    CpuCopy8((const void *)pos, dst, size);
    return 0;
}
