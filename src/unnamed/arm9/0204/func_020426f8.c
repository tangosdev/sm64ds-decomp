/* The read callback func_020423dc gives the child's "rom" archive, a
   NitroSDK FS_ARCHIVE_READ_FUNC: the archive's tables are in memory, so a
   read copies size bytes from address pos to dst and succeeds. */
#include "types.h"
#include "nitro/fs/archive.h"
#include "nitro/mi/memory.h"

int func_020426f8(FSArchive *p_arc, void *dst, u32 pos, u32 size) {
    MI_CpuCopy8((const void *)pos, dst, size);
    return FS_RESULT_SUCCESS;
}
