//cpp
/* arm9 .bss 0x020a94c0..0x020aa420: the tail of the ARM9 .bss: the WBT child-thread state, a WM block and
   newlib's __cs/__atexit/signal state.

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   data_020a94c4, data_020a94c8, data_020a94cc, data_020a94d0, data_020a94d4,
   data_020a9570, data_020a9d28, data_020a9d2c, ...and 31 more
*/
#include "types.h"

u8 data_020a94c0[0xf60];
