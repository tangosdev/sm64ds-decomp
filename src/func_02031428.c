#include "types.h"
#pragma opt_common_subs off
extern int func_02054d88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);

extern int data_0209fd00;
extern u8 data_0209fc78;
extern u8 data_0209fc7c;
extern u8 data_0209fcd8;
extern u8 data_0209fc70;
extern u8 data_0209fd20[];
extern u32 data_0209287c[];
extern u32 data_0209fd5c[];
extern u32 data_0209283c[];

void func_02031428(void)
{
    int cnt;
    int i;
    int rows;
    int j;
    int base;

    base = func_02054d88() + 0x4000;

    cnt = (int)(*(u8 *)(data_0209fd00 + 6)) << 1;
    i = 0;
    if (cnt > 0) {
        do {
            rows = data_0209fc78;
            j = 0;
            if (rows > 0) {
                do {
                    MultiCopy_Int((int *)(base + 0x10), (int *)base, 0x10);
                    if (i == (((int)(*(u8 *)(data_0209fd00 + 6)) << 1) - 1)) {
                        *(int *)(base + 0x10) = 0;
                        *(int *)(base + 0x14) = 0;
                        *(int *)(base + 0x18) = 0;
                        *(int *)(base + 0x1c) = 0;
                    } else {
                        int rc2 = data_0209fc78;
                        MultiCopy_Int((int *)(base + rc2 * 0x20), (int *)(base + 0x10), 0x10);
                    }
                    rows = data_0209fc78;
                    j++;
                    base += 0x20;
                } while (j < rows);
            }
            cnt = (int)(*(u8 *)(data_0209fd00 + 6)) << 1;
            i++;
        } while (i < cnt);
    }

    {
        u32 col;
        int rowIdx;
        u8 rem;
        int count;
        int j;
        int blk;
        int wordIdx;
        u8 *inc;
        u32 *dst;
        int i2;
        int i3;

        dst = (u32 *)(func_02054d88() + (((data_0209fc78 * (cnt - 1)) + 0x200) << 5) + 0x10);
        col = data_0209fc7c;
        count = data_0209fcd8;

        j = 0;
        if (count > 0) {
            rowIdx = data_0209fc70;
            inc = data_0209fd20;
            do {
                rem = col & 7;
                blk = col >> 3;
                wordIdx = blk << 3;
                dst[wordIdx] = (data_0209287c[rem] & data_0209fd5c[rowIdx + 0]) | dst[wordIdx];
                *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 4) & ~0ULL)) = (data_0209287c[rem] & data_0209fd5c[rowIdx + 1]) | *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 4) & ~0ULL));
                i2 = rowIdx + 2;
                *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 8) & ~0ULL)) = (data_0209287c[rem] & data_0209fd5c[i2]) | *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 8) & ~0ULL));
                i3 = rowIdx + 3;
                *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 12) & ~0ULL)) = (data_0209287c[rem] & data_0209fd5c[i3]) | *(u32 *)(((u64)(int)((char *)&dst[wordIdx] + 12) & ~0ULL));
                if (rem != 0) {
                    *(u32 *)((char *)&dst[wordIdx] + 0x20) = data_0209283c[rem] & data_0209fd5c[rowIdx + 0];
                    *(u32 *)((char *)&dst[wordIdx] + 0x24) = data_0209283c[rem] & data_0209fd5c[rowIdx + 1];
                    *(u32 *)((char *)&dst[wordIdx] + 0x28) = data_0209283c[rem] & data_0209fd5c[i2];
                    *(u32 *)((char *)&dst[wordIdx] + 0x2c) = data_0209283c[rem] & data_0209fd5c[i3];
                }
                col = (col + *inc) & 0xff;
                rowIdx += 0x10;
                inc++;
                j++;
            } while (j < count);
        }
    }

    data_0209fc70 = data_0209fc70 + 3;
}
