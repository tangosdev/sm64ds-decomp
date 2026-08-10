#include "types.h"
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void func_0203568c(int *value, int target);
extern void func_02035684(int *value, int target);

#define LAUNDER_PTR(ptr) \
    ((int *)(int)(ptr))
#define LAUNDER_32(base, offset) \
    ((int *)(int)((long long)((int)(base) + (offset))))

void func_ov102_0214baa0(char *self)
{
    u8 variant;

    *LAUNDER_32(self, 0xb0) &= ~0x10000000;
    variant = *(u8 *)(self + 0x3f5);
    if (!(variant < 2 || variant == 5)) {
        _ZN5Actor24KillAndTrackInDeathTableEv(self);
        return;
    }

    *(int *)(self + 0x3dc) = 5;
    *(int *)(self + 0x98) = 0;
    _ZN12CylinderClsn5ClearEv(self + 0x110);

    *LAUNDER_PTR(self + 0xb0) &= 0xfff1fffe;
    *(int *)(self + 0x5c) = *(int *)(self + 0x3c4);
    *(int *)(self + 0x60) = *(int *)(self + 0x3c8);
    *(int *)(self + 0x64) = *(int *)(self + 0x3cc);
    *(u8 *)(self + 0x3f3) = 0;
    *(int *)(self + 0x38c) = 0;
    *(int *)(self + 0x390) = 0;
    *(s16 *)(self + 0x3ea) = 0;
    *(int *)(self + 0x80) = 0x1000;
    *(int *)(self + 0x84) = 0x1000;
    *(int *)(self + 0x88) = 0x1000;
    *(s16 *)(self + 0x8c) = 0;
    *(s16 *)(self + 0x8e) = 0;
    *(s16 *)(self + 0x90) = 0;
    *(s16 *)(self + 0x92) = 0;
    *(s16 *)(self + 0x94) = 0;
    *(s16 *)(self + 0x96) = 0;

    {
        s16 angle = *(s16 *)(self + 0x3f0);
        *(s16 *)(self + 0x8e) = angle;
        s16 copy = *(s16 *)(self + 0x8e);
        *(s16 *)(self + 0x94) = copy;
    }

    *LAUNDER_PTR(self + 0x128) &= ~0x4002;
    *LAUNDER_PTR(self + 0x128) |= 4;
    *(int *)(self + 0x114) = 0x3c000;
    *(int *)(self + 0x118) = 0x50000;

    func_0203568c((int *)(self + 0x144), 0x32000);
    func_02035684((int *)(self + 0x144), 0x32000);
    *(int *)(self + 8) = 0;
}
