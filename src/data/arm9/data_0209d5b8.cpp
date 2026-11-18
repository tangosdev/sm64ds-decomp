//cpp
/* arm9 .bss 0x0209d5b8..0x0209d64c: the OSThread func_0201a1bc runs the card/IRQ work on.

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).
*/
#include "types.h"

u8 data_0209d5b8[0x94];
