#include "private/arm9_archive_table.h"

struct S82; /* a mounted .narc archive (src/engine/sound/SoundEngine.cpp) */
extern void _Z13func_02018908P3S82Pv(struct S82 *archive, void *heap);

/* Give archive i back to the heap it came from, if it is mounted. */
void UnloadArchive(int i) {
    ArchiveTableEntry *p = &data_0208ecf4[i];
    void *a = p->archive;
    if (a == 0)
        return;
    _Z13func_02018908P3S82Pv(a, p->heap);
    p->archive = 0;
    p->heap = 0;
}
