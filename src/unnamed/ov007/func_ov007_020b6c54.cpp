//cpp
#include "types.h"
#include "nitro/gx/g3x.h"

extern "C" {
void func_ov007_020b6c54(void) {
    volatile u16 *reg = (volatile u16 *)0x4000060;
    *reg = (*reg & ~0x3000) | 0x10;
    *reg = (*reg & ~0x3000) | 8;
    *reg = *reg & ~0x3002;
    *(int *)0x4000540 = 2;
    G3X_SetClearColor(0, 0, 0x7fff, 0x3f, 0);
}
}
