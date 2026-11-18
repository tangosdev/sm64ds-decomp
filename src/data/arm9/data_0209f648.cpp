//cpp
/* arm9 .bss 0x0209f648..0x020a0c5c: the iris-wipe scanline table (1536 bytes), the Kuppa-script cursors and the
   message-system state (dWipe_c, KuppaScript, Message, dEntObj).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   gRunningKuppaScript, gSavedKuppaScript, data_0209fc50, data_0209fc54,
   data_0209fc58, data_0209fc5c, data_0209fc5d, data_0209fc60, ...and 50
   more
*/
#include "types.h"

u8 data_0209f648[0x1614];
