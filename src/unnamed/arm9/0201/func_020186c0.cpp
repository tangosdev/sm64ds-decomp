//cpp
#include "private/arm9_archive_table.h"

#include "S82.h"
#include "nitro/os/system.h"

extern "C" int LoadArchive(int idx);

/* The address of archived file val's bytes inside its mounted archive
   (func_0204ede8), mounting the archive first if needed. 0 for a FAT id
   (below 0x8000), which has no in-memory copy. */
extern "C" int func_020186c0(unsigned int val) {
    unsigned int i;
    ArchiveTableEntry *e;
    if (val < 0x8000)
        return 0;
    e = data_0208ecf4;
    for (i = 0; i < ARCHIVE_TABLE_COUNT; i++) {
        if (e->firstFileId <= val && val < e->endFileId) {
            if (e->archive == 0) {
                if (LoadArchive(i))
                    data_0208eb54 = i;
                if (e->archive == 0) {
                    OS_Terminate();
                    return 0;
                }
            }
            return ((S82 *)e->archive)->func_0204ede8(val - e->firstFileId);
        }
        e++;
    }
    OS_Terminate();
    return 0;
}
