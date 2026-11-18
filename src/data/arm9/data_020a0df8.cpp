//cpp
/* arm9 .bss 0x020a0df8..0x020a0eb0: the touch calibration samples, the active player slot and the pad input
   records (func_0203b9bc, func_0203bc7c, PlayerInput).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   gActivePlayerSlot, data_020a0e44, data_020a0e48, data_020a0e50,
   data_020a0e58, data_020a0e5a, data_020a0e68, data_020a0e98, ...and 5
   more
*/
#include "types.h"

u8 data_020a0df8[0xb8];
