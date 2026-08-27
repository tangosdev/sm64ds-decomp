#include "types.h"
/* func_02053fe0 @ 0x02053fe0 -- clear BG ext-palette enable bit in main-engine
 * DISPCNT, then disable the VRAM bank assignment via DisableVramBanks. */
extern u16 DisableVramBanks(u16 *bankBitsPtr);
extern u16 data_020a6096;

u16 func_02053fe0(void) {
    *(vu32 *)0x04000000 &= ~0x40000000;
    return DisableVramBanks(&data_020a6096);
}
