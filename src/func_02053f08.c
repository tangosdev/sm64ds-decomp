/* func_02053f08 @ 0x02053f08 -- clear BG ext-palette enable bit in sub-engine
 * DISPCNT, then disable the VRAM bank assignment via DisableVramBanks. */
typedef unsigned short u16;
typedef volatile unsigned long vu32;

extern u16 DisableVramBanks(u16 *bankBitsPtr);
extern u16 data_020a609e;

u16 func_02053f08(void) {
    *(vu32 *)0x04001000 &= ~0x40000000;
    return DisableVramBanks(&data_020a609e);
}
