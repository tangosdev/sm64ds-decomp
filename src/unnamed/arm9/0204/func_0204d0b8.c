#include "types.h"
void func_0204d0b8(void *p, int a, int b) {
    u16 *c = (u16 *)((char *)p + 0x40);
    u16 g = ((0xff - b) * 0x1f) / 0xff;
    *c = (*c & ~0x3e0) | ((g & 0x1f) << 5);
}
