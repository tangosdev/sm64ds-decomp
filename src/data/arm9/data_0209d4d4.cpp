//cpp
/* arm9 .bss 0x0209d4d4..0x0209d528: the OS/IRQ boot flags and the two PM backlight switches (IRQ2, OS_Terminate,
   func_020197b8, func_02019dfc).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   data_0209d4d8, data_0209d4dc, data_0209d4e0, data_0209d4e4, data_0209d4e8,
   data_0209d4ec, data_0209d4f0, data_0209d4f4, ...and 12 more
*/
#include "types.h"

u8 data_0209d4d4[0x54];
