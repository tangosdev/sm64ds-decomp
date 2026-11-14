#include "private/arm9_archive_table.h"

struct S82; /* a mounted .narc archive (src/engine/sound/SoundEngine.cpp) */
extern void _Z13func_02018908P3S82Pv(struct S82 *archive, void *heap);

/* Unload every mounted archive. Unlike UnloadArchive it leaves heap set. */
void UnloadArchives(void) {
    unsigned int i;
    ArchiveTableEntry *p = data_0208ecf4;
    for (i = 0; i < ARCHIVE_TABLE_COUNT; i++) {
        if (p->archive != 0) {
            _Z13func_02018908P3S82Pv(p->archive, p->heap);
            p->archive = 0;
        }
        p++;
    }
}
