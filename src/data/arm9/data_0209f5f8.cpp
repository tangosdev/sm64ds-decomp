//cpp
/* arm9 .bss 0x0209f5f8..0x0209f610: the fade/wipe flags and their scanline-table pointers (dWipe_c,
   dScMgD3dBase_c, dScMgLuigi_c).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   data_0209f5fc, data_0209f600, data_0209f604, data_0209f608, data_0209f60c
*/
#include "types.h"

u8 data_0209f5f8[0x18];
