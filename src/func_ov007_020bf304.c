#include "types.h"
#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern u8 data_ov007_02104bd4[];
extern u8 data_ov007_0210342c[];

extern void func_ov007_020cbb04(void *c);
extern int func_ov007_020ca26c(void *c);
extern void func_ov007_020c3d1c(void *thiz);
extern void func_ov007_020c9fa0(void *c);
extern int func_ov007_020c44c4(void *r4);

void func_ov007_020bf304(void)
{
    int i;
    void *g;
    void *p;

    g = *(void **)data_ov007_02104bd4;
    if (g == 0) {
        return;
    }

    p = *(void **)g;
    if (p != 0) {
        func_ov007_020cbb04(p);
        *(int *)(*(void **)data_ov007_02104bd4) = 0;
    }

    p = *(void **)((char *)(*(void **)data_ov007_02104bd4) + 0xc);
    if (p != 0) {
        func_ov007_020ca26c(p);
        *(int *)((char *)(*(void **)data_ov007_02104bd4) + 0xc) = 0;
    }

    for (i = 0; i < 2; i++) {
        if (*(void **)((char *)(*(void **)data_ov007_02104bd4) + 4 + i * 4) != 0) {
            func_ov007_020c3d1c(*(void **)((char *)(*(void **)data_ov007_02104bd4) + 4 + i * 4));
            *(int *)((char *)(*(void **)data_ov007_02104bd4) + 4 + i * 4) = 0;
        }
    }

    for (i = 0; i < 8; i++) {
        func_ov007_020c9fa0(*(void **)((char *)(*(void **)data_ov007_02104bd4) + 0x10 + i * 4));
    }

    for (i = 0; i <= 2; i++) {
        if (*(void **)((char *)(*(void **)data_ov007_0210342c) + 0x174 + i * 4) != 0) {
            func_ov007_020c44c4(*(void **)((char *)(*(void **)data_ov007_0210342c) + 0x174 + i * 4));
            *(int *)((char *)(*(void **)data_ov007_0210342c) + 0x174 + i * 4) = 0;
        }
    }

    func_ov007_020c3d1c(*(void **)data_ov007_02104bd4);
    *(void **)data_ov007_02104bd4 = 0;
}
