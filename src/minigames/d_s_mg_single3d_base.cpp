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
 * - The GX/GXS/G2S/CP15/G3X engine calls stay file-local namespace
 *   declarations: no shared header declares them.
 * - Ov004_LoadFile keeps its local declaration: its own definition
 *   is still declared without the argument and result.
 * - The ov004 registry words stay raw: sMgTopScreenYOffset and the
 *   file-id table sSingle3DCommonNcgByLanguage name no struct this TU can see.
 *   sMgGraphCallback keeps the int[] spelling: the typed
 *   dScMgBase_c::graphCallback_c form matches (9/9 still MATCH) but trips
 *   the decl ratchet against src/minigames/d_s_mg_d3d_base.cpp's banked int[],
 *   and that file is outside this job's area.
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

/* SDK calls this file makes. */
namespace GX {
void SetBankForBG(u16);
void SetBankForOBJ(u16);
void SetBankForTex(u16);
void SetBankForTexPltt(u16);
void SetGraphicsMode(int, int, int);
void SetBankForSubBG(u16);
void SetBankForSubOBJ(u16);
void LoadOBJPltt(const void*, u32, u32);
}
namespace GXS {
void LoadBGPltt(const void*, u32, u32);
void LoadOBJPltt(const void*, u32, u32);
}
namespace G2S { u32 GetBG3CharPtr(); }
namespace CP15 { void FlushAndInvalidateDataCache(u32 address, u32 length); }
namespace G3X { void SetFog(bool enable, int, int, int); }

extern "C" {
int LoadFile(int fileId);
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
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern u32 data_020a0db0;
extern int sMgTopScreenYOffset;
extern int sMgGraphCallback[];
extern int sSingle3DCommonNcgByLanguage[];
extern int data_0208ee44;
extern void **data_0209d4a8;
}

// @symbol _ZN19dScMgSingle3DBase_c11InitEnginesEv
/* Slot 33 brings up both graphics engines, assigns VRAM banks and loads
 * the language's OBJ characters and the OBJ palettes into both engines
 * before publishing this scene, then sets lights 0 and 1 to white. All
 * thirteen children inherit this override. */
void dScMgSingle3DBase_c::InitEngines()
{
    void *fileData;

    REG_DISPCNT_SUB |= 0x10000u;
    sMgTopScreenYOffset = 0;
    Mg_ResetBgOffsets(); /* zero all eight BG scroll offsets */
    Mg_DisableAllBanks(); /* GX::DisableAllBanks */
    data_0209d45c = 0x10;
    data_0209d454 = 0x10;
    REG_DISPCNT &= ~0x7000000u;
    REG_DISPCNT &= ~0x38000000u;
    GX::SetBankForBG(2);
    GX::SetBankForOBJ(0x10);
    GX::SetBankForTex(1);
    GX::SetBankForTexPltt(0x20);
    GX::SetGraphicsMode(1, 0, 1);
    REG_DISPCNT &= ~0x300010u;
    GX::SetBankForSubBG(4);
    GX::SetBankForSubOBJ(8);
    fileData = Ov004_LoadFile(sSingle3DCommonNcgByLanguage[GetGameLanguage()]);
    /* Each split `dst += 0x4000` keeps the ROM's mov and add pair; a single
     * constant becomes a literal-pool load. */
    {
        char *dst = (char *)0x6400000; dst += 0x4000;
        DecompressLZ16(fileData, dst);
    }
    {
        char *dst = (char *)0x6600000; dst += 0x4000;
        DecompressLZ16(fileData, dst);
    }
    Ov004_Deallocate(fileData);
    fileData = Ov004_LoadFile(0xc3);
    CP15::FlushAndInvalidateDataCache((u32)fileData, 0x100u);
    GX::LoadOBJPltt(fileData, 0x100u, 0x100u);
    GXS::LoadOBJPltt(fileData, 0x100u, 0x100u);
    Ov004_Deallocate(fileData);
    Mg_ResetOverlays();
    data_0208ee44 = 1;
    G3X::SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();
    FreeGfxSlotsById(0x1d);
    sMgGraphCallback[1] = (int)this;
    data_0209d4a8 = (void **)sMgGraphCallback;
    *(vu32 *)0x40004ccu = 0x7fff;
    *(vu32 *)0x40004ccu = 0x40007fff;
}

// @symbol _ZN19dScMgSingle3DBase_c18AfterInitResourcesEj
/* Slot 2 runs the base, then initializes particles. */
void dScMgSingle3DBase_c::AfterInitResources(u32 vfSuccess)
{
    dScMgBase_c::AfterInitResources(vfSuccess);
    mSysTracker.Initialise();
}

// @symbol _ZN19dScMgSingle3DBase_c14BeforeBehaviorEv
/* Slot 7 steps the particle tracker only when the global update flag is set. */
int dScMgSingle3DBase_c::BeforeBehavior()
{
    if (dScMgBase_c::BeforeBehavior() == 0)
        return 0;
    if (data_020a0db0 & 1)
        mSysTracker.Update();
    return 1;
}

// @symbol _ZN19dScMgSingle3DBase_c12BeforeRenderEv
/* Slot 10 renders particles after the common scene is ready. */
int dScMgSingle3DBase_c::BeforeRender()
{
    if (!dScMgBase_c::BeforeRender())
        return 0;
    Particle::RenderAll();
    return 1;
}

// @symbol _ZN19dScMgSingle3DBase_c21AfterCleanupResourcesEj
/* Slot 5: when vfSuccess is 2, reset light 0's vector and color and free
 * the common model data, then run the base. Keep the hardware stores'
 * width and order. */
void dScMgSingle3DBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2) {
        *(vu32 *)0x40004c8u = 0x296a5800;
        *(vu32 *)0x40004ccu = 0x7fff;
        CleanCommonModelDataArr();
    }
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

// @symbol _ZN19dScMgSingle3DBase_c14GetDisplayModeEv
/* Slot 26 identifies this branch of minigame scenes with the value 1. */
int dScMgSingle3DBase_c::GetDisplayMode()
{
    return 1;
}

// @symbol LoadCard2SubBg3
/* Load the sub screen's BG3 characters, palette and map, releasing each
 * buffer. */
extern "C" void LoadCard2SubBg3(void)
{
    void *fileData = (void *)LoadFile(ASSET_D_2D_MG_BG_CARD2_NCG_BIN);
    data_0209d454 |= 8;
    *(vu16 *)0x400100eu = (*(vu16 *)0x400100eu & 0x43) | 0x214;
    DecompressLZ16(fileData, (void *)G2S::GetBG3CharPtr());
    Ov004_Deallocate(fileData);
    fileData = (void *)LoadFile(ASSET_D_2D_MG_BG_CARD2_NCL_BIN);
    GXS::LoadBGPltt(fileData, 0xa0, 0x160);
    Deallocate(fileData);
    fileData = (void *)LoadFile(ASSET_D_2D_MG_BG_CARD2_NSC_BIN);
    func_020562b4(fileData, 0, 0x800);
    Deallocate(fileData);
    *(vu16 *)0x400100eu &= ~0x40;
    *(vu32 *)0x400101cu = 0;
    *(vu16 *)0x400100eu = (*(vu16 *)0x400100eu & ~3) | 1;
}

/* The inline destructor plus AfterInitResources as key function emit D1
 * and D0 in cartridge order. Children inline this base teardown, so there
 * is no standalone D2. */
