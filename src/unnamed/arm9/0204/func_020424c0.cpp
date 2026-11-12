//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/fs/file.h"
#include "nitro/card/card_rom.h"
#include "private/file_share_alloc.h"

extern "C" {

/* Measured: without propagation 22 of 106 words differ. for-loop forms are
 * 0x1b0. Plain for loops match everything except the first loop (22 words):
 * the ROM seeds the length from the zeroed counter (`mov r0, #0` ...
 * `add sl, r0, #4`), which only opt_propagation off reproduces; `sl = 4`,
 * `sl = sizeof(u32)`, `for (sl = 4, i = 0; ...)` and `i = 0; for (sl = i + 4;
 * ...)` all fold the seed to a constant. (The opt_loop_invariants off this file
 * also carried is inert now: the function is exact without it.) */
#pragma opt_propagation off
typedef struct {
    int start;
    int end;
} Entry;

extern char data_02099e6c[]; /* "rom" */

extern FSArchive *FS_FindArchive(const char *name, int name_len);

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
    const CARDRomRegion *ptrs[4];
    FSFile file;
    u32 sl;
    void *fp;
    int i;

    ptrs[0] = CARD_GetRomRegionFAT();
    ptrs[1] = CARD_GetRomRegionFNT();
    ptrs[2] = CARD_GetRomRegionOVT(MI_PROCESSOR_ARM9);
    ptrs[3] = CARD_GetRomRegionOVT(MI_PROCESSOR_ARM7);

    i = 0;
    sl = i + 4;
    do {
        sizes[i] = (int)((ptrs[i]->length + 0x1f) & ~0x1f);
        sl += sizes[i] + 4;
        i++;
    } while (i < 4);

    fp = FileShareAlloc(sl);

    if (data_020a1fc0.mIsParent != 0) {
        FSArchive *sb;
        char *buf;
        int roundedSize;
        int j;

        sb = FS_FindArchive(data_02099e6c, 3);
        buf = (char *)fp + 4;
        FS_InitFile(&file);
        j = 0;

        do {
            const CARDRomRegion *p = ptrs[j];
            int origSize = (int)p->length;
            int data0;
            int end;

            roundedSize = sizes[j];
            *(int *)buf = origSize;
            buf += 4;
            data0 = (int)p->offset;
            end = roundedSize + data0;
            FS_OpenFileDirect(&file, sb, data0, end, -1);
            FS_ReadFile(&file, buf, roundedSize);
            FS_CloseFile(&file);
            buf += roundedSize;
            j++;
        } while (j < 4);

        {
            Entry *entries = (Entry *)((char *)fp + 8);
            int count = (int)(ptrs[0]->length >> 3);
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

        DC_FlushRange(fp, sl);
        DC_WaitWriteBufferEmpty();
    }

    data_020a1fc0.mTable = fp;
    data_020a1fc0.mTableSize = sl;
}
}
