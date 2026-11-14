#include "private/arm9_archive_table.h"
#include "nitro/os/system.h"

extern int LoadArchive(int idx);
extern int _Z13func_0204ede8P3S82j(void *o, unsigned int i);

/* The address of archived file val's bytes inside its mounted archive
   (func_0204ede8), mounting the archive first if needed. 0 for a FAT id
   (below 0x8000), which has no in-memory copy. */
int func_020186c0(unsigned int val) {
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
                    _Z12OS_Terminatev();
                    return 0;
                }
            }
            return _Z13func_0204ede8P3S82j(e->archive, val - e->firstFileId);
        }
        e++;
    }
    _Z12OS_Terminatev();
    return 0;
}
