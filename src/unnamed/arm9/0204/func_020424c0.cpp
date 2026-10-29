//cpp
#include "types.h"
#include "CP15.h"
#include "IRQ.h"

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

extern WorkInfo data_020a1fc0;
extern int data_02099e6c;

extern void* func_0205d23c(void* a, int b);
extern void FS_InitFile(int* s);
extern int func_0205d5e8(char* self, int a1, int a2, int a3, int a4);
extern int FS_ReadFile(int a, int b, int c);
extern int FS_CloseFile(char* self);

void func_020424c0(void)
{
    int sizes[4];
    void* ptrs[4];
    int local[18];
    u32 sl;
    u32 irq;
    void* fp;
    int i;

    ptrs[0] = (void*)0x27ffe48;
    ptrs[1] = (void*)0x27ffe40;
    ptrs[2] = (void*)0x27ffe50;
    ptrs[3] = (void*)0x27ffe58;

    i = 0;
    sl = i + 4;
    do {
        sizes[i] = (((int*)ptrs[i])[1] + 0x1f) & ~0x1f;
        sl += sizes[i] + 4;
        i++;
    } while (i < 4);

    irq = IRQ::Disable();
    fp = data_020a1fc0.mAllocFunc(data_020a1fc0.mAllocArg, sl, 0);
    IRQ::Restore(irq);

    if (data_020a1fc0.mIsParent != 0) {
        void* sb;
        char* buf;
        int roundedSize;
        int j;

        sb = func_0205d23c(&data_02099e6c, 3);
        buf = (char*)fp + 4;
        FS_InitFile(local);
        j = 0;

        do {
            int* p = (int*)ptrs[j];
            int origSize = p[1];
            int data0;
            int end;

            roundedSize = sizes[j];
            *(int*)buf = origSize;
            buf += 4;
            data0 = p[0];
            end = roundedSize + data0;
            func_0205d5e8((char*)local, (int)sb, data0, end, -1);
            FS_ReadFile((int)local, (int)buf, roundedSize);
            FS_CloseFile((char*)local);
            buf += roundedSize;
            j++;
        } while (j < 4);

        {
            Entry* entries = (Entry*)((char*)fp + 8);
            int count = (int)((u32)(((int*)ptrs[0])[1]) >> 3);
            u32 max = sl;
            int k;
            k = 0;
            if (count > 0) {
                do {
                    u32 diff = (u32)entries[k].end - (u32)entries[k].start;
                    if (max < diff) max = diff;
                    k++;
                } while (k < count);
            }
            *(int*)fp = max;
        }

        CP15::FlushAndInvalidateDataCache((u32)fp, sl);
        CP15::DrainWriteBuffer();
    }

    data_020a1fc0.mTable = fp;
    data_020a1fc0.mTableSize = sl;
}
}
