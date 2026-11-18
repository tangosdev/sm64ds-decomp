//cpp
/* arm9 .bss 0x020a0ef0..0x020a4b6c: the Wireless Manager / multiboot cluster: the WM state, the MB registry and
   its buffers (unit0203d7b8, MB_FileInfo, MB_Parent).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   data_020a0ef4, data_020a0ef8, data_020a0efc, data_020a0f00, data_020a0f04,
   data_020a0f08, data_020a0f0c, data_020a0f10, ...and 98 more
*/
#include "types.h"

u8 data_020a0ef0[0x3c7c];
