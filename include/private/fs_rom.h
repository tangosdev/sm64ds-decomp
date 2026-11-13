#ifndef PRIVATE_FS_ROM_H
#define PRIVATE_FS_ROM_H
#ifndef __cplusplus
#define FSi_InitRom _Z11FSi_InitRomj
#endif

/* The FS library's ROM-archive statics and entry points, as NitroSDK's
 * private libraries/fs/include/rom.h declares them: fs_rom.c (FS_Rom.cpp)
 * defines them, and fs_overlay.c (FS_Overlay.c) and the game's copy of the
 * overlay loader (func_02018c00.c) read them. */

#include "types.h"
#include "nitro/card/card_rom.h"
#include "private/fs_views.h"

#if defined(_MSC_VER) && defined(__cplusplus)
extern "C" {
#endif

extern u32 fsi_default_dma_no;
extern s32 fsi_card_lock_id;
extern CARDRomRegion fsi_ovt9;
extern CARDRomRegion fsi_ovt7;
extern FSArchive fsi_arc_rom;

void FSi_InitRom(u32 default_dma_no);

#if defined(_MSC_VER) && defined(__cplusplus)
}
#endif

#endif /* PRIVATE_FS_ROM_H */
