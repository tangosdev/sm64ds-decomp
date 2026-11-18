//cpp
/* arm9 .bss 0x020a0dd8..0x020a0de8: the touch-panel state: TPData (func_0203b9bc) and the calibration parameters
   (func_0203bbc0).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   data_020a0de0
*/
#include "types.h"

u8 data_020a0dd8[0x10];
