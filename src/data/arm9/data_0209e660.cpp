//cpp
/* arm9 .bss 0x0209e660..0x0209f1d8: the OAM state: OAM.cpp's manager words, its GXOamAttr buffers and the per-
   screen attribute tables.

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   OAM::objCountMain, OAM::curAffineSetMain, OAM::curAffineSetSub, OAM::objCountSub, OAM::bufferMain,
   data_0209e67c, data_0209e694, OAM::bufferSub, ...and 8 more
*/
#include "types.h"

u8 data_0209e660[0xb78];
