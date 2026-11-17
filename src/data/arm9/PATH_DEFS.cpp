//cpp
/* arm9 .bss 0x020a0d84..0x020a0db8: the path tables (PATH_DEFS/PATH_NODES/NUM_PATHS), their node buffer and the
   crash screen's saved registers.

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   PATH_NODES, NUM_PATHS, data_020a0d90, data_020a0db0, data_020a0db4
*/
#include "types.h"

u8 PATH_DEFS[0x34];
