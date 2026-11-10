#ifndef SM64DS_GX_H
#define SM64DS_GX_H

/* GX::DisableAllBanks, the one display function that still goes by a coined
 * C++ namespace. The display library itself carries its NitroSDK names with
 * C linkage: see nitro/gx/gx.h, nitro/gx/gx_vramcnt.h and nitro/gx/gx_load.h.
 *
 * DisableAllBanks (src/game/stages/d_scene.cpp) is game code: it switches off
 * every VRAM user's banks through the SDK's GX_DisableBankFor* calls. No
 * source names it. */
namespace GX {
void DisableAllBanks(void);
}

#endif
