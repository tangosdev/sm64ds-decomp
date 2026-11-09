#ifndef SM64DS_GX_H
#define SM64DS_GX_H

/* GX -- the three display-library functions that still go by a coined C++
 * namespace. The rest of the family carries its NitroSDK name with C linkage:
 * see nitro/gx/gx.h, nitro/gx/gx_vramcnt.h and nitro/gx/gx_load.h.
 *
 * DisableAllBanks (src/game/stages/d_scene.cpp) is game code: it switches off
 * every VRAM user's banks through the SDK's GX_DisableBankFor* calls.
 * GetSizeOfTex and GetSizeOfTexPltt (src/engine/gx/GX_VramCnt.cpp) return the
 * size in bytes of the texture and texture-palette banks; pret's
 * GX_vramcnt.c carries no GX_GetSizeOf* function to name them from. */
namespace GX {
void DisableAllBanks(void);
int GetSizeOfTex(void);
int GetSizeOfTexPltt(void);
}

#endif
