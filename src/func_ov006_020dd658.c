#include "types.h"
struct Pair { int a; int b; };

extern int data_ov006_0212e418[];
extern void func_020127a4(int r0, int r1, int r2, int r3);
extern void func_ov004_020adfc4(int a, int b, struct Pair *p2, struct Pair *p3, struct Pair *p4);

void func_ov006_020dd658(char *self, int i) {
    int n = i * 0x1c;
    int old466c;
    struct Pair pa;
    struct Pair pb;

    old466c = *(s32 *)(self + 0x466c + n);
    *(s32 *)(self + 0x4664 + n) += old466c;
    *(s32 *)(self + 0x466c + n) += 0x600;

    if (old466c <= 0 && *(s32 *)(self + 0x466c + n) >= 0) {
        int v = ((*(s32 *)(self + 0x4660 + n) >> 12) - 0x80) >> 1;
        if (v >= 0x3c)
            v = 0x3c;
        if (v <= -0x3c)
            v = -0x3c;
        func_020127a4(2, 0xec, 0xffff, v);
    }

    {
        int y = *(s32 *)(self + 0x4664 + n) >> 12;
        u8 idx = *(u8 *)(self + 0x4674 + n);
        if (*(s32 *)(self + 0x466c + n) < 0)
            return;
        if (y <= data_ov006_0212e418[idx] - 0x20)
            return;

        *(u8 *)(self + 0x4675 + n) = 4;
        *(u8 *)(self + 0x4676 + n) = 0;
        *(u8 *)(self + 0x4677 + n) = 0;
        (*(u16 *)(((long long)(int)(self + 0x4d08))))++;

        {
            int v1 = *(s32 *)(self + 0x4664 + n);
            int v0 = *(s32 *)(self + 0x4660 + n);
            pb.a = 0x6c000;
            pb.b = -0x80000;
            pa.a = v0;
            pa.b = v1;
        }
        func_ov004_020adfc4(1, 0x20, &pa, &pa, &pb);
    }
}
