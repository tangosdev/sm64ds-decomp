#include "private/arm9_archive_table.h"

struct S82; /* a mounted .narc archive (src/engine/sound/SoundEngine.cpp) */
extern void func_02018908(struct S82 *archive, void *heap);

/* Unload every mounted archive. Unlike UnloadArchive it leaves heap set. */
void UnloadArchives(void) {
    unsigned int i;
    ArchiveTableEntry *p = data_0208ecf4;
    for (i = 0; i < ARCHIVE_TABLE_COUNT; i++) {
        if (p->archive != 0) {
            func_02018908(p->archive, p->heap);
            p->archive = 0;
        }
        p++;
    }
}
