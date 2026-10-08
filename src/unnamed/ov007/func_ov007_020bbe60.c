#include "types.h"
extern void func_ov007_020c6784(void *self, int count, int p2, int p3, int p4, int p5, int mode);

extern char *data_ov007_0210342c;
extern int data_ov007_020d7e98[];
extern int data_ov007_020d7eb0[];
extern int data_ov007_020cdb34[];
extern int data_ov007_020d7e68[];
extern int data_ov007_020d7e80[];
extern char *data_ov007_02104ba0;

void func_ov007_020bbe60(int idx) {
    int r = idx * 3 + *(u8 *)(data_ov007_0210342c + idx + 0xfc);
    func_ov007_020c6784(*(void **)data_ov007_02104ba0,
                        data_ov007_020cdb34[r],
                        data_ov007_020d7e68[r],
                        data_ov007_020d7e80[r],
                        data_ov007_020d7e98[r],
                        data_ov007_020d7eb0[r],
                        1);
    *(int *)(data_ov007_02104ba0 + 0x38) = 0x78;
    if (idx == 0 && *(u8 *)(data_ov007_0210342c + idx + 0xfc) == 2) {
        int *o = (int *)data_ov007_02104ba0;
        int mid = (int)(((s64)(o[0x38 / 4] << 12) * 0x1800 + 0x800) >> 12);
        o[0x38 / 4] = mid >> 12;
    }
    ((u8 *)(data_ov007_0210342c + 0xfc))[idx]++;
    {
        u8 *p = (u8 *)(data_ov007_0210342c + 0xfc);
        if (p[idx] >= 3) p[idx] = 0;
    }
}
