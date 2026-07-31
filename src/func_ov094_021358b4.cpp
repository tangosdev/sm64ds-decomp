//cpp
// @symbol func_ov094_021358b4
/* recovered: shared common types */
#include "common.h"

typedef int Fix12;
typedef unsigned char u8;
extern "C" {
extern void ApproachLinear(struct Vector3 *a, const struct Vector3 *b, Fix12 f);
extern int func_ov094_02136188(void *c, void *p);
extern int data_ov094_02136b60[];
}

extern "C" int func_ov094_021358b4(void *t) {
    char *c = (char*)t;
    if (*(int*)(c+0x3e8) == 0) {
        if (*(u8*)(c+0x3e4) != 0) {
            u8 *cd = (u8*)(((int)c + 0x3e4) & 0xFFFFFFFFFFFFFFFF);
            *cd = *cd - 1;
            *(short*)(c+0x92) = 0;
            *(int*)(c+0xa8) = 0x14000;
        } else {
            *(int*)(c+0x3e8) = 1;
            *(int*)(c+0xa4) = 0;
            *(int*)(c+0xa8) = 0;
            *(int*)(c+0xac) = 0;
            int *p = (int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF);
            *(int*)(c+0x5c) = *(int*)(c+0x3d8);
            *(int*)(c+0x60) = *(int*)(c+0x3dc);
            *(int*)(c+0x64) = *(int*)(c+0x3e0);
            *p += 0x64000;
        }
    } else {
        ApproachLinear((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x3d8), 0x2000);
        if (*(u8*)(c+0x3e4) < 0x1f) {
            u8 *cd = (u8*)(((int)c + 0x3e4) & 0xFFFFFFFFFFFFFFFF);
            *cd = *cd + 1;
        } else {
            *(u8*)(c+0x3e4) = 0x1f;
            func_ov094_02136188(c, data_ov094_02136b60);
        }
    }
    return 1;
}
