//cpp
/* Shared scene for the single-camera 3D minigames: nine functions in
 * [0x0210a4b0, 0x0210a8c0), including the compiler-emitted destructor pair.
 * Keep this definition order: the compiler emits functions in reverse
 * source order.
 *
 * Ov004_Deallocate (ov004, 0x020adc5c) and Deallocate (arm9, 0x02018144)
 * are different functions; each call site keeps the one the ROM calls.
 *
 * leftovers:
 * - The GX/GXS/G2S/CP15 engine calls stay file-local namespace
 *   declarations: no shared header declares them.
 * - Ov004_LoadFile keeps its local declaration: its own definition
 *   is still declared without the argument and result.
 * - The ov004 registry words stay raw: sMgTopScreenYOffset and the
 *   file-id table sSingle3DCommonNcgByLanguage name no struct this TU can see.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_s_mg_single3_d_base_AssetId {
    ASSET_D_2D_MG_BG_CARD2_NCG_BIN = 0x0026,
    ASSET_D_2D_MG_BG_CARD2_NCL_BIN = 0x0027,
    ASSET_D_2D_MG_BG_CARD2_NSC_BIN = 0x0028
};

#include "dScMgSingle3DBase_c.h"
#include "types.h"
#include "decl_common.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_light.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/g3x.h"

/* SDK calls this file makes. */
#include "nitro/os/cache.h"

extern "C" {

void *LoadFile(int fileId);
void DecompressLZ16(void *src, void *dst);
void Ov004_Deallocate(void *p);
void Mg_ResetBgOffsets(void);
void Mg_DisableAllBanks(void);
s32 GetGameLanguage(void);
/* Forwards to LoadFile. Its own definition is still declared without the
 * argument and result, so this file keeps a local declaration. */
void *Ov004_LoadFile(int fileId);
void Mg_ResetOverlays(void);
void FreeGfxSlotsById(int id);
extern u8 gSubScreenLayers;
extern u8 gMainScreenLayers;
extern s32 data_020a0db0;
extern int sMgTopScreenYOffset;
extern dScMgBase_c::graphCallback_c sMgGraphCallback;
extern int sSingle3DCommonNcgByLanguage[];
extern int data_0208ee44;
extern dGraph_c::callback_c *gGraphCallback;
}


/* Slot 33 brings up both graphics engines, assigns VRAM banks and loads
 * the language's OBJ characters and the OBJ palettes into both engines
 * before publishing this scene, then sets lights 0 and 1 to white. All
 * thirteen children inherit this override. */
void dScMgSingle3DBase_c::InitEngines()
{
    void *fileData;

    GXS_DispOn();
    sMgTopScreenYOffset = 0;
    Mg_ResetBgOffsets(); /* zero all eight BG scroll offsets */
    Mg_DisableAllBanks(); /* GX::DisableAllBanks */
    gMainScreenLayers = DISPLAY_LAYER_OBJ;
    gSubScreenLayers = 0x10;
    GX_SetBGCharOffset(0);
    GX_SetBGScrOffset(0);
    GX_SetBankForBG(GX_VRAM_BG_128_B);
    GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    GX_SetBankForTex(GX_VRAM_TEX_0_A);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_F);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    fileData = Ov004_LoadFile(sSingle3DCommonNcgByLanguage[GetGameLanguage()]);
    DecompressLZ16(fileData, (u8 *)G2_GetOBJCharPtr() + 0x4000);
    DecompressLZ16(fileData, (u8 *)G2S_GetOBJCharPtr() + 0x4000);
    Ov004_Deallocate(fileData);
    fileData = Ov004_LoadFile(0xc3);
    DC_FlushRange(fileData, 0x100u);
    GX_LoadOBJPltt(fileData, 0x100u, 0x100u);
    GXS_LoadOBJPltt(fileData, 0x100u, 0x100u);
    Ov004_Deallocate(fileData);
    Mg_ResetOverlays();
    data_0208ee44 = 1;
    G3X_SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();
    FreeGfxSlotsById(0x1d);
    sMgGraphCallback.mScene = this;
    gGraphCallback = &sMgGraphCallback;
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    G3_LightColor(GX_LIGHTID_1, GX_RGB(31, 31, 31));
}

/* Slot 2 runs the base, then initializes particles. */
void dScMgSingle3DBase_c::AfterInitResources(u32 vfSuccess)
{
    dScMgBase_c::AfterInitResources(vfSuccess);
    mParticle.Initialise();
}

/* Slot 7 steps the particle tracker only when the global update flag is set. */
int dScMgSingle3DBase_c::BeforeBehavior()
{
    if (dScMgBase_c::BeforeBehavior() == 0)
        return 0;
    if (data_020a0db0 & 1)
        mParticle.Update();
    return 1;
}

/* Slot 10 renders particles after the common scene is ready. */
int dScMgSingle3DBase_c::BeforeRender()
{
    if (!dScMgBase_c::BeforeRender())
        return 0;
    Particle::RenderAll();
    return 1;
}

/* Slot 5: when vfSuccess is 2, put light 0 back to the scene default (white,
 * from (0, -1/sqrt2, -1/sqrt2)) and free the common model data, then run the
 * base. */
void dScMgSingle3DBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2) {
        G3_LightVector(GX_LIGHTID_0, 0, -0xb50, -0xb50);
        G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
        CleanCommonModelDataArr();
    }
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

/* Slot 26 identifies this branch of minigame scenes with the value 1. */
int dScMgSingle3DBase_c::GetDisplayMode()
{
    return 1;
}

/* Load the sub screen's BG3 characters, palette and map, releasing each
 * buffer. */
extern "C" void LoadCard2SubBg3(void *) {
    void *fileData = LoadFile(ASSET_D_2D_MG_BG_CARD2_NCG_BIN);
    gSubScreenLayers |= 8;
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                          GX_BG_CHARBASE_0x14000);
    DecompressLZ16(fileData, (void *)G2S_GetBG3CharPtr());
    Ov004_Deallocate(fileData);
    fileData = LoadFile(ASSET_D_2D_MG_BG_CARD2_NCL_BIN);
    GXS_LoadBGPltt(fileData, 0xa0, 0x160);
    Deallocate(fileData);
    fileData = LoadFile(ASSET_D_2D_MG_BG_CARD2_NSC_BIN);
    GXS_LoadBG3Scr(fileData, 0, 0x800);
    Deallocate(fileData);
    G2S_BG3Mosaic(false);
    G2S_SetBG3Offset(0, 0);
    G2S_SetBG3Priority(1);
}

/* The inline destructor plus AfterInitResources as key function emit D1
 * and D0 in cartridge order. Children inline this base teardown, so there
 * is no standalone D2. */
