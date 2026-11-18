//cpp
/* arm9 .bss 0x020a8104..0x020a89a8: the power-management and card state: PMi_* work and mutex, cardi_* and the
   ROM statistics (PM.c, card.c).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   PMi_SyncFlag, PMi_SleepEndFlag, PMi_LCDCount, PMi_Work, PMi_Mutex,
   PMi_RegisterBuffer, data_020a813c, cardi_arg, ...and 8 more
*/
#include "types.h"

u8 PMi_IsInit[0x8a4];
