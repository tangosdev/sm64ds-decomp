//cpp
/* arm9 .bss 0x020a0c78..0x020a0ce0: the sphere-collision CLPS block and dBgW's handle table (dBgCh_SphCrr_Query,
   dBgW).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   data_020a0c80
*/
#include "types.h"

u8 data_020a0c78[0x68];
