#include "types.h"
/* func_020540f0 @ 0x020540f0 -- clear OBJ ext-palette enable bit in sub-engine
 * DISPCNT, then unmap the VRAM bank back to LCDC via func_020541b8. */
extern u16 func_020541b8(u16 *bankBitsPtr);
extern u16 data_020a60a0;

u16 func_020540f0(void) {
    *(vu32 *)0x04001000 &= ~0x80000000;
    return func_020541b8(&data_020a60a0);
}
