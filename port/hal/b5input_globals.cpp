// THE GAME LOOP'S INPUT RING (run linkfull, lane B5INPUT). Phase 0x17 of the
// ROM's frame loop, src/func_020197b8.c:41-46:
//
//     data_0209d50c = 0x17; func_0203bc7c();
//     v = *(unsigned short*)((char*)data_020a0e58 + (data_020a0e40 << 2));
//     data_0209d534[data_0209d4e8] = v;
//     data_0209d51c = v;
//     data_0209d4e8 = (data_0209d4e8 + 1) & 0x1f;
//
// The three ring words were hosted nowhere (0 map rows on INT53). They are .bss
// on the cartridge (config/arm9/symbols.txt), so they start at zero here too,
// and each one gets its ROM span (next symbol minus this one):
//
//   data_0209d4e8   0x0209d4e8, 4 bytes   the ring cursor, a byte (& 0x1f)
//   data_0209d51c   0x0209d51c, 4 bytes   this frame's held keys, an int
//   data_0209d534   0x0209d534, 0x40      the ring: 32 halfwords of held keys
//
// INSIDE THE .dsstate BRACKET, for rom_frame.cpp's reason: a save state taken
// mid-session has a ring position, and a restore that came back with a
// different one would have changed the ROM's state. The captured span grows by
// these words, so a disk save state written by an older build refuses to load
// (by design, the same as every hosted word before).
//
// Nothing in src/ reads the ring but func_020197b8 itself, which the port does
// not run yet (tests/walk_window.cpp's level loop stands in for it phase by
// phase and writes these at phase 0x17), so the words the cartridge keeps are
// the words the port keeps, and the day the ROM's loop drives the frame they
// are already here.
#include "dsstate_seg.h"

DSSTATE_BEGIN
extern "C" {
unsigned char data_0209d4e8[4];
int data_0209d51c;
unsigned short data_0209d534[32];
}
DSSTATE_END
