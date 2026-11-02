//cpp
#include "types.h"
#include "CP15.h"
#include "IRQ.h"
#include "private/fs_views.h"

typedef struct FSFile FSFile;

extern "C" {

#pragma opt_propagation off
#pragma opt_loop_invariants off
/* data_020a1fc0, the file-sharing work block, under FileShareWork's names
 * (include/private/arm9_a1fc0_file_share.h); typed here for the allocator
 * call. Only the fields this function touches are spelled out. */
typedef struct {
    int mIsParent; /* 0x00 */
    char pad_04[0x10];
    void *(*mAllocFunc)(void *, u32, u32); /* 0x14 */
    void *mAllocArg;                       /* 0x18 */
    char pad_1c[4];
    void *mTable;   /* 0x20 -- the card header's FAT, FNT and overlay tables */
    u32 mTableSize; /* 0x24 */
} WorkInfo;

typedef struct {
    int start;
    int end;
} Entry;

/* An (offset, size) pair of the card header copy at 0x027ffe00. */
typedef struct {
    int offset;
    int size;
} RomRegion;

extern WorkInfo data_020a1fc0;
extern char data_02099e6c[]; /* "rom" */

extern FSArchive *FS_FindArchive(const char *name, int name_len);
extern void FS_InitFile(int *s);
extern int FS_OpenFileDirect(char *p_file, FSArchive *p_arc, u32 image_top, u32 image_bottom,
                             u32 file_index);
extern int FS_ReadFile(int a, int b, int c);
extern int FS_CloseFile(char *self);

/* Allocate one block for the card's FAT, FNT and both overlay tables (each
 * as its size word followed by the data rounded up to 0x20 bytes, after one
 * leading word) and store it as mTable / mTableSize. The parent fills it from
 * the ROM archive: it reads each region, then sets the leading word to the
 * larger of the block size and the largest FAT file. The child only
 * allocates. Its caller func_02040c34 then registers the parent's block as
 * region 0x20000, or on the child passes mTableSize to func_02041224 to grow
 * the receive work. */
void func_020424c0(void) {
    int sizes[4];
    RomRegion *ptrs[4];
    FSFile file;
    u32 sl;
    u32 irq;
    void *fp;
    int i;

    ptrs[0] = (RomRegion *)0x27ffe48; /* FAT */
    ptrs[1] = (RomRegion *)0x27ffe40; /* FNT */
    ptrs[2] = (RomRegion *)0x27ffe50; /* ARM9 overlay table */
    ptrs[3] = (RomRegion *)0x27ffe58; /* ARM7 overlay table */

    i = 0;
    sl = i + 4;
    do {
        sizes[i] = (ptrs[i]->size + 0x1f) & ~0x1f;
        sl += sizes[i] + 4;
        i++;
    } while (i < 4);

    irq = IRQ::Disable();
    fp = data_020a1fc0.mAllocFunc(data_020a1fc0.mAllocArg, sl, 0);
    IRQ::Restore(irq);

    if (data_020a1fc0.mIsParent != 0) {
        FSArchive *sb;
        char *buf;
        int roundedSize;
        int j;

        sb = FS_FindArchive(data_02099e6c, 3);
        buf = (char *)fp + 4;
        FS_InitFile((int *)&file);
        j = 0;

        do {
            RomRegion *p = ptrs[j];
            int origSize = p->size;
            int data0;
            int end;

            roundedSize = sizes[j];
            *(int *)buf = origSize;
            buf += 4;
            data0 = p->offset;
            end = roundedSize + data0;
            FS_OpenFileDirect((char *)&file, sb, data0, end, -1);
            FS_ReadFile((int)&file, (int)buf, roundedSize);
            FS_CloseFile((char *)&file);
            buf += roundedSize;
            j++;
        } while (j < 4);

        {
            Entry *entries = (Entry *)((char *)fp + 8);
            int count = (int)((u32)ptrs[0]->size >> 3);
            u32 max = sl;
            int k;
            k = 0;
            if (count > 0) {
                do {
                    u32 diff = (u32)entries[k].end - (u32)entries[k].start;
                    if (max < diff)
                        max = diff;
                    k++;
                } while (k < count);
            }
            *(int *)fp = max;
        }

        CP15::FlushAndInvalidateDataCache((u32)fp, sl);
        CP15::DrainWriteBuffer();
    }

    data_020a1fc0.mTable = fp;
    data_020a1fc0.mTableSize = sl;
}
}
