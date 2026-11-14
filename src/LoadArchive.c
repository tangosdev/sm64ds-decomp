#include "types.h"
#include "private/arm9_archive_table.h"

extern void *data_020a0ea0;

extern void *func_02018934(char *name, char *path, void *heap);

/* Mount archive idx from its .narc on first use; nonzero once it is mounted. */
int LoadArchive(int idx) {
    ArchiveTableEntry *e = &data_0208ecf4[idx];
    if (e->archive == 0) {
        e->heap = data_020a0ea0;
        e->archive = func_02018934(e->name, e->path, e->heap);
    }
    return e->archive != 0;
}
