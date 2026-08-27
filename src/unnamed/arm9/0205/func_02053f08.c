/* func_02053f08 @ 0x02053f08 -- clear BG ext-palette enable bit in sub-engine
 * DISPCNT, then disable the VRAM bank assignment via DisableVramBanks. */
#include "nitro/hw/registers.h"

extern u16 DisableVramBanks(u16 *bankBitsPtr);
extern u16 data_020a609e;

u16 func_02053f08(void) {
    REG_DISPCNT_SUB &= ~DISPLAY_CONTROL_BG_EXT_PALETTE;
    return DisableVramBanks(&data_020a609e);
}
