#include "types.h"
struct M48 { int w[12]; };
struct Vec3 { int x, y, z; };

extern int _ZN4cstd4fdivEii(int a, int b);
extern void _Z14ApproachLinearRiii(int *v, int target, int rate);
extern void func_ov006_021082c4(char *p);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct M48 *mF, int x, int y, int z);
extern s16 data_02082214[];
extern struct M48 data_020a0e68;

void func_ov006_021082fc(char *self)
{
    int A = *(int *)(self + 0xb8);
    int r4 = _ZN4cstd4fdivEii((int)(((s64)A * A + 0x800) >> 12), *(int *)(self + 0xb0)) - 0xc00;

    A = *(int *)(self + 0xb8);
    int fm2 = (int)(((s64)A * 0x500 + 0x800) >> 12);
    int t2 = _ZN4cstd4fdivEii(fm2, *(int *)(self + 0xb0));
    *(s16 *)(self + 0xc6) = (s16)t2;

    s16 *pC0 = (s16 *)(((int)self + 0xc0));
    *pC0 = *pC0 + *(s16 *)(self + 0xc6);

    *(int *)(self + 0xb4) = (int)(((s64)*(int *)(self + 0xb4) * 0xfe0 + 0x800) >> 12);

    *(int *)(((int)self + 0xb4)) = *(int *)(((int)self + 0xb4)) + r4;
    *(int *)(((int)self + 0xb0)) = *(int *)(((int)self + 0xb0)) + *(int *)(self + 0xb4);

    int B = *(int *)(self + 0xb0);
    if (B > 0x4200) {
        B = 0x4200;
        *(int *)(self + 0xb0) = B;
        *(int *)(self + 0xb4) = 0;
    } else if (B < 0x2c00) {
        _Z14ApproachLinearRiii((int *)(self + 0xb8), 0xfffff667, 0x28);
        func_ov006_021082c4(self);
    } else {
        if (*(s16 *)(self + 0xcc) == 1) {
            *(int *)(self + 0xbc) = 0;
            *(s16 *)(self + 0xcc) = 2;
        }
    }

    if (*(int *)(self + 0xd0) != 0) {
        _Z14ApproachLinearRiii((int *)(self + 0xb8), 0, 0x10);
    }

    u16 angle = *(u16 *)(self + 0xc0);
    int dist = *(int *)(self + 0xb0);
    int idx = angle >> 4;
    s16 val1 = data_02082214[idx * 2 + 1];
    s16 val0 = data_02082214[idx * 2];

    int x, y, z;
    struct Vec3 t;
    x = (int)(((s64)dist * val1 + 0x800) >> 12);
    y = (int)(((s64)dist * 0x400 + 0x800) >> 12) + 0x780;
    z = (int)(((s64)dist * val0 + 0x800) >> 12);
    ((int *)&t)[0] = x; ((int *)&t)[1] = y; ((int *)&t)[2] = z;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1], ((int *)&t)[2]);

    *(struct M48 *)(self + 0x7c) = data_020a0e68;
}
