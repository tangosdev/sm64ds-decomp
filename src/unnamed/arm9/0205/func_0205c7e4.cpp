//cpp
#include "IRQ.h"

extern "C" {

enum Bool { FALSE, TRUE };

extern void* func_0205d044(void* self);
extern void func_0205cfa4(void* p);

/* NitroSDK FSArchive, the field touched here (func_0205d044.cpp's name). */
typedef struct FSArchive {
    char pad_00[0x10];
    int flag; /* 0x10 */
} FSArchive;

/* NitroSDK FS_ResumeArchive(p_arc): if the archive is suspended
 * (FS_ARCHIVE_FLAG_SUSPEND, 8), clear the flag and pick up its next command
 * (func_0205d044 = FSi_NextCommand); after interrupts are restored, run a
 * command that came back (func_0205cfa4). Returns whether the archive was
 * not suspended. */
int func_0205c7e4(char* self)
{
    FSArchive *p_arc = (FSArchive *)self;
    void* r = 0;
    unsigned int saved;
    enum Bool bit;
    enum Bool notset;

    saved = IRQ::Disable();
    bit = (enum Bool)((p_arc->flag & 8) != 0);
    notset = (enum Bool)(bit == FALSE);
    if (notset == FALSE) {
        p_arc->flag &= ~8;
        r = func_0205d044(self);
    }
    IRQ::Restore(saved);
    if (r != 0) {
        func_0205cfa4(r);
    }
    return notset;
}
}
