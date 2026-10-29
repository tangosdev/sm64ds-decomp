//cpp
#include "IRQ.h"

extern "C" {

extern void func_0205c864(int *thiz);
extern void func_0205c788(int *p, int n);

/* NitroSDK FSFile, the link field followed here (func_0205d044.cpp's name). */
typedef struct FSFile {
    struct FSFile *prev; /* 0x00 -- link.prev */
    struct FSFile *next; /* 0x04 -- link.next */
} FSFile;

/* NitroSDK FSArchive, the fields cleared here; names follow the SDK's, as
 * func_0205d044.cpp and func_0205c9b8.c spell them. */
typedef struct FSArchive {
    char pad_00[0x10];
    unsigned int flag; /* 0x10 */
    struct {
        FSFile *prev; /* 0x14 */
        FSFile *next; /* 0x18 -- the command list's head */
    } list;
    int base;     /* 0x1c */
    int fat;      /* 0x20 */
    int fat_size; /* 0x24 */
    int fnt;      /* 0x28 */
    int fnt_size; /* 0x2c */
    int fat_bak;  /* 0x30 */
    int fnt_bak;  /* 0x34 */
} FSArchive;

/* NitroSDK FS_UnloadArchive(p_arc): if the archive is loaded
 * (FS_ARCHIVE_FLAG_LOADED, 2), suspend it (func_0205c864), mark it
 * UNLOADING (0x80), cancel every queued command (func_0205c788 =
 * FSi_ReleaseCommand with FS_RESULT_CANCELED, 3), clear its table and base
 * addresses and drop CANCELING | LOADED | UNLOADING (0xa2). */
int func_0205caa8(int *thiz)
{
    FSArchive *p_arc = (FSArchive *)thiz;
    int saved = IRQ::Disable();
    int b = (int)((p_arc->flag & 2) != 0);
    if (b != 0) {
        FSFile *p;
        /* a dead volatile read of flag the ROM keeps (dropping it is 4 bytes short) */
        *(volatile unsigned int *)&p_arc->flag;
        func_0205c864(thiz);
        p_arc->flag |= 0x80;
        p = p_arc->list.next;
        if (p != 0) {
            FSFile *n;
            do {
                n = p->next;
                func_0205c788((int *)p, 3);
                p = n;
            } while (n != 0);
        }
        p_arc->list.next = 0;
        p_arc->base = 0;
        p_arc->fat = 0;
        p_arc->fat_size = 0;
        p_arc->fnt = 0;
        p_arc->fnt_size = 0;
        p_arc->fnt_bak = 0;
        p_arc->fat_bak = p_arc->fnt_bak;
        p_arc->flag &= ~0xa2;
    }
    IRQ::Restore(saved);
    return 1;
}
}
