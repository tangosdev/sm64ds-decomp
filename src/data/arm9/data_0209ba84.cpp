//cpp
/* arm9 .bss 0x0209ba84..0x0209d4b8: the sound driver's mixer state, SaveBlock (gSaveBlock), the save-file
   tables and the minigame records (Sound.cpp, SaveData.cpp, KuppaScript,
   dScDSMT_c).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   data_0209baa0, gSaveBlock, data_0209cab4, data_0209cad2, data_0209cae4,
   data_0209caf4, data_0209cdcc, data_0209cdd0, ...and 44 more
*/
#include "types.h"

u8 data_0209ba84[0x1a34];
