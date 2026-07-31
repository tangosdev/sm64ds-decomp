// @symbol func_ov006_02126948
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
struct V3 { int x, y; volatile int z; };

extern int _ZN4cstd4fdivEii(int a, int b);
extern void func_ov006_020c0134(void *self);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short ang);
extern struct Matrix4x3 data_020a0e68;

#pragma opt_lifetimes off
void func_ov006_02126948(char *c) {
    int t = _ZN4cstd4fdivEii(0xc0000, data_02082314);
    struct V3 v;
    int x, y;
    if (*(unsigned char *)(c + 0xb9f8) == 0) {
        *(int *)(c + 0x4700) = 0;
        *(int *)(c + 0x4704) = 0xd0000;
        *(int *)(c + 0x4708) = 0;
        *(int *)(c + 0x470c) = 0;
        *(int *)(c + 0x4710) = 0;
        *(int *)(c + 0x4714) = t;
        *(short *)(c + 0x4718) = 0x200;
        func_ov006_020c0134(c + 0x4660);
    } else {
        *(int *)(c + 0x4700) = 0;
        *(int *)(c + 0x4704) = -0x60000;
        *(int *)(c + 0x4708) = 0;
        *(int *)(c + 0x470c) = 0;
        *(int *)(c + 0x4710) = 0;
        *(int *)(c + 0x4714) = t;
        *(short *)(c + 0x4718) = 0x200;
        func_ov006_020c0134(c + 0x4660);
    }
    y = *(int *)(c + 0xab6c) - *(int *)(c + 0xab3c) + 0x110000;
    x = *(int *)(c + 0xab38) - 0x80000;
    v.x = x; v.y = y; v.z = 0;
    Matrix4x3_FromTranslation(&data_020a0e68, x, y, 0);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short *)(c + 0xab7c));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short *)(c + 0xab78));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short *)(c + 0xab7a));
    *(struct Matrix4x3 *)(c + 0xabc0) = data_020a0e68;
}
