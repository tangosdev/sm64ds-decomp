//cpp
/* arm9 .bss 0x0209f1e8..0x0209f1ec: sBootJobDone, the boot scene's job latch (dScBoot_c, dScMB_c).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).
*/
#include "types.h"

u8 sBootJobDone[0x4];
