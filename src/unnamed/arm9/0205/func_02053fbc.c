#include "types.h"
/* func_02053fbc @ 0x02053fbc -- clear OBJ ext-palette enable bit in main-engine
 * DISPCNT, then disable the VRAM bank assignment via DisableVramBanks. */
extern u16 DisableVramBanks(u16 *bankBitsPtr);
extern u16 data_020a6098;

u16 func_02053fbc(void) {
    *(vu32 *)0x04000000 &= ~0x80000000;
    return DisableVramBanks(&data_020a6098);
}
