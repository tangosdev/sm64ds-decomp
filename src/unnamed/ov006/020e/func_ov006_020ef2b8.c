#include "types.h"
void func_0203cd80(int* m, int x);
void func_0203ccd4(int *m, short angle);

void func_ov006_020ef2b8(void) {
    int v[3];
    int w[3];
    *(volatile int*)0x40004cc = 0x7fff;
    *(volatile int*)0x40004cc = 0x40007fff;
    v[0] = 0;
    v[1] = 0;
    v[2] = 0xfffff008;
    func_0203cd80(v, 0x2000);
    func_0203ccd4(v, 0);
    *(volatile int*)0x40004c8 =
        (((short)v[0] >> 3) & 0x3ff) |
        ((((short)v[1] >> 3) & 0x3ff) << 10) |
        ((((short)v[2] >> 3) & 0x3ff) << 20);
    w[2] = 0xfffff008;
    w[0] = 0;
    w[1] = 0;
    *(volatile int*)0x4000440 = 2;
    *(volatile int*)0x4000454 = 0;
    func_0203cd80(w, 0x2000);
    func_0203ccd4(w, 0);
    *(volatile int*)0x40004c8 =
        ((((short)w[0] >> 3) & 0x3ff) |
        ((((short)w[1] >> 3) & 0x3ff) << 10) |
        ((((short)w[2] >> 3) & 0x3ff) << 20)) | 0x40000000;
}
