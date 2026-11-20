//cpp
/* arm9 .bss 0x020a4bb8..0x020a7fc4: the model and render globals: model components, texture sequences, material
   changers and the actor spawn tables.

   Zero-initialized, so the cartridge carries no bytes here. One storage
   object covers the run because mwcc emits one .bss section per symbol
   in size order and pads each to a word, which cannot express the ROM's
   byte layout; the configured labels inside it keep their symbols.txt
   addresses (the linker script defines them at their ROM address).

   data_020a4bbc, data_020a4bc0, data_020a4bc4, sVramTex4x4TexelDataStart, sVramPalStart,
   data_020a4bd0, data_020a4bd4, sVramPalEnd, ...and 113 more
*/
#include "types.h"

u8 data_020a4bb8[0x340c];
