//cpp
/* arm9 .bss 0x020a805c..0x020a80c8: the FS ROM/overlay state: the card lock, the DMA channel, the two overlay
   tables and the ROM archive record (FS_Rom, FS_Overlay).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   fsi_default_dma_no, fsi_ovt9, fsi_ovt7, fsi_arc_rom, data_020a80c4
*/
#include "types.h"

u8 fsi_card_lock_id[0x6c];
