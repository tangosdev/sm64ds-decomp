//cpp
/* arm9 .bss 0x0209b2e4..0x0209b468: the entry-scene, title-screen, download
   and actor globals above the camera's and the Kuppa script's state.
   Zero-initialized, so the cartridge carries no bytes; this is one storage
   object for the whole run because the ROM starts some of these on addresses
   that are not word-aligned, and mwcc emits one .bss section per symbol in
   size order, which cannot express that. The configured labels inside it -
   data_0209b2e4, data_0209b2e8, data_0209b2ec, data_0209b2ed, data_0209b2f0,
   gTitleCursor, gTitleListTop, data_0209b2fc, gMgSaveDirty, gMgMenuPage,
   gMgMenuRow, gMgMenuRowMid, data_0209b33c (Ov007BootArgs), data_0209b340,
   data_0209b34b, data_0209b34e, data_0209b3d8, the view matrix data_0209b3ec
   and its inverse data_0209b41c, data_0209b44c, data_0209b450,
   gNextActorUpdateMask, data_0209b458, data_0209b45c, data_0209b460 and
   gActorUpdateMask - keep their symbols.txt addresses. */
#include "types.h"

u8 data_0209b2e4[0x184];
