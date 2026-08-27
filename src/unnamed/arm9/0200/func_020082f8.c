#include "types.h"
extern Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);
extern u16 data_0209b274;

int func_020082f8(char* self, u8* src, int lo, int hi) {
    Fix12i slope = _ZN4cstd4fdivEii((data_0209b274 - lo) << 12, (hi - lo) << 12);
    self[0x10c] = (u8)((Fix12i)(((long long)((int)src[0] << 12) * slope + 0x800) >> 12) >> 12);
    self[0x10d] = (u8)((Fix12i)(((long long)((int)src[1] << 12) * slope + 0x800) >> 12) >> 12);
    self[0x10e] = (u8)(((Fix12i)(((long long)(((int)src[2] - 0xff) << 12) * slope + 0x800) >> 12) >> 12) + 0xff);
    self[0x10f] = (u8)(((Fix12i)(((long long)(((int)src[3] - 0xbf) << 12) * slope + 0x800) >> 12) >> 12) + 0xbf);
    *(int*)(self + 0xf8) = _ZN4cstd4fdivEii(
        (((u8)self[0x10e] - (u8)self[0x10c]) + 1) << 12,
        (((u8)self[0x10f] - (u8)self[0x10d]) + 1) << 12);
    return 1;
}
