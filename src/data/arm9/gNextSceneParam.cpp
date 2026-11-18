//cpp
/* arm9 .bss 0x0209f5b8..0x0209f5c4: the next scene's id and parameter, the fader that owns the transition and
   the scene the graph is running (dScene_c, KuppaScript).

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   gSceneFader, gCurrentScene
*/
#include "types.h"

u8 gNextSceneParam[0xc];
