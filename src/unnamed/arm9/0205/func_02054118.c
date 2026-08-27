#include "types.h"
/* func_02054118 @ 0x02054118 -- clear BG ext-palette enable bit in sub-engine
 * DISPCNT, then unmap the VRAM bank back to LCDC via func_020541b8. */
extern u16 func_020541b8(u16 *bankBitsPtr);
extern u16 data_020a609e;

u16 func_02054118(void) {
    *(vu32 *)0x04001000 &= ~0x40000000;
    return func_020541b8(&data_020a609e);
}
