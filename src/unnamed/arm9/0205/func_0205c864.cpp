//cpp
#include "IRQ.h"

extern "C" {

extern void OS_SleepThread(void *p);

/* NitroSDK FS_SuspendArchive(p_arc): with interrupts off, unless the archive
 * is already FS_ARCHIVE_FLAG_SUSPEND (8): if it is running a command
 * (FS_ARCHIVE_FLAG_RUNNING, 0x10) set FS_ARCHIVE_FLAG_SUSPENDING (0x40) and
 * sleep on stat_q until that clears, otherwise set SUSPEND at once. Returns
 * whether the archive was not suspended before. Field names follow the SDK's
 * FSArchive, as func_0205d044.cpp spells it; only the fields this function
 * touches are spelled out. */
typedef struct FSArchive {
    char pad_00[0xe];
    unsigned short stat_q; /* 0x0e -- woken once a suspend takes effect */
    int flag;              /* 0x10 */
} FSArchive;

static inline int FS_IsArchiveSuspended(volatile const FSArchive *p_arc) {
    return (p_arc->flag & 8) ? 1 : 0;
}

static inline int FS_IsArchiveRunning(volatile const FSArchive *p_arc) {
    return (p_arc->flag & 0x10) ? 1 : 0;
}

static inline int FS_IsArchiveSuspending(volatile const FSArchive *p_arc) {
    return (p_arc->flag & 0x40) ? 1 : 0;
}

int func_0205c864(int *thiz)
{
    FSArchive *p_arc = (FSArchive *)thiz;
    unsigned int saved;
    int retval;
    saved = (unsigned int)IRQ::Disable();
    retval = !FS_IsArchiveSuspended(p_arc);
    if (retval) {
        if (FS_IsArchiveRunning(p_arc)) {
            p_arc->flag |= 0x40;
            do {
                OS_SleepThread(&p_arc->stat_q);
            } while (FS_IsArchiveSuspending(p_arc));
        } else {
            p_arc->flag |= 8;
        }
    }
    IRQ::Restore(saved);
    return retval;
}
}
