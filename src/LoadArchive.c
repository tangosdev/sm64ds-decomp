#include "types.h"
#include "private/arm9_archive_table.h"

extern void *data_020a0ea0;

extern void *_Z13func_02018934PcS_Pv(char *name, char *path, void *heap);

/* Mount archive idx from its .narc on first use; nonzero once it is mounted. */
int LoadArchive(int idx) {
    ArchiveTableEntry *e = &data_0208ecf4[idx];
    if (e->archive == 0) {
        e->heap = data_020a0ea0;
        e->archive = _Z13func_02018934PcS_Pv(e->name, e->path, e->heap);
    }
    return e->archive != 0;
}
