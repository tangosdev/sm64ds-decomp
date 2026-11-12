//cpp
/* ov006/dScMgD3DBase_c -- the 3D scene base shared by Jump, Jump2,
 * Trampoline and Trampoline2 (.text 0x020e6c28..0x020e7660).
 *
 * Functions run in reverse of ROM order; do not reorder. The destructor is
 * inline in the header, so BeforeInitResources is the key function that
 * emits the vtable and the D1 and D0 pair. The ten free helpers are unnamed
 * in symbols.txt, and so are most of the arm9 and ov004 calls below.
 *
 * deslop
 * Leftover: the ten free helpers and the Oam/data views keep linker
 *   names; naming belongs at shared headers.
 */

#include "dScMgD3DBase_c.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_light.h"
#include "decl_common.h"
#include "types.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/os/cache.h"

/* An OAM entry, with attributes 0 and 1 stored as one word the way this
 * code writes them. */
typedef struct Oam {
    u32 attr01;
    u16 attr2;
    u16 aff;
} Oam;

extern "C" {

/* ov006's own data. These six objects are this file's .bss
 * (0x02141a40..0x02141e54). The OAM shadow is a C++ array, so mwcc emits
 * __sinit_d_s_mg_d3d_base.cpp as a bare bx lr and one .ctor word. */
int sMgCurCamera;
void *sD3DSubObjPalette;
MgCamera *sMgSubCamera;
MgCamera *sMgMainCamera;
void *sD3DLanguageFile;
Oam sD3DOamShadow[128];
extern unsigned char sD3DSoundPanOffsets[];

/* arm9 / ov004 globals with no project header. */
extern unsigned char gSubScreenLayers;
extern unsigned char gMainScreenLayers;
extern unsigned char data_0209e660;
extern unsigned char data_0209f5f8;
extern dGraph_c::callback_c *gGraphCallback;
extern int data_020a0db0;
extern int data_0208ee44;
extern dScMgBase_c::graphCallback_c sMgGraphCallback;

/* arm9 / ov004 helpers with no project header. */
void GXS_LoadOAM(const void *src, unsigned int offset, unsigned int count);
int   func_020126e8(int a);
void  func_020126ac(int a0, int a1, int a2, int a3, int s0);
void  func_020127ec(int a0, int a1, int a2, int a3, int a4, int a5);
void *Ov004_LoadFile(int id);
void  Mg_ResetOverlays(void);
void  Mg_ResetBgOffsets(void);
void  Mg_DisableAllBanks(void);
int   GetGameLanguage(void);
void  DecompressLZ16(void *src, void *dst);
/* The file ID is really u16, but passing it narrowed grows InitSubScreenBg by
 * 16 bytes. Keep it wide. */
u32 LoadCompressedFileAt(int fileID, void *target);
void  Ov004_Deallocate(void *x);

/* Defined below. */
void D3D_InitDualScreen(void);
void D3D_ClearCaptureTable(void);
void D3D_DrawSpriteTable(void);
void D3D_CaptureToSub(void);
void D3D_CaptureToMain(void);
void D3D_ReleaseSubBg1(void *self);
int  D3D_PanForX(int a);

}

namespace Particle { void RenderAll(); }

extern "C" void D3D_CaptureToMain(void) {
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    GX_ResetBankForSubOBJ();
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForLCDC(GX_VRAM_LCDC_D);
    REG_DISPCAPCNT = 0x80330010;
    gSubScreenLayers &= ~0x10;
    gSubScreenLayers |= 4;
    REG_DISPCNT_SUB = (REG_DISPCNT_SUB & ~0x1f00) | (gSubScreenLayers << 8);
    G2S_SetBG2ControlDCBmp(GX_BG_SCRSIZE_DCBMP_256x256, GX_BG_AREAOVER_XLU,
                           GX_BG_BMPSCRBASE_0x08000);
    G2S_SetBG2Priority(0);
    G2S_BG2Mosaic(false);
}

extern "C" void D3D_CaptureToSub(void)
{
    GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
    GX_ResetBankForSubBG();
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForLCDC(GX_VRAM_LCDC_C);
    *(volatile int *)0x4000064 = 0x80360010;
    GXS_SetGraphicsMode(GX_BGMODE_5);
    gSubScreenLayers |= 0x10;
    gSubScreenLayers &= ~4;
    *(volatile int *)0x4001000 = (*(volatile int *)0x4001000 & ~0x1f00) | (gSubScreenLayers << 8);
}

extern "C" void D3D_DrawSpriteTable(void)
{
    int entry;
    int x;
    int y;
    int sprite;
    int tileY;
    int tileX;
    u32 displayControl;

    displayControl = REG_DISPCNT_SUB;
    REG_DISPCNT_SUB = (displayControl & 0xffbfff9f) | 0x20;

    /* Keep the index round trips through long long: without them this
     * function grows from 0xe0 to 0xe8 bytes. */
    sprite = 0;
    for (entry = 0; entry < 0x80; entry++) {
        sD3DOamShadow[entry].attr01 = 0;
        *(u32 *)&sD3DOamShadow[(int)(((long long)entry))].attr2 = 0;
    }

    y = 0;
    tileY = 0;
    for (; y < 0xc0; y += 0x40) {
        x = 0;
        tileX = 0;
        for (; x < 0x100; x += 0x40) {
            sD3DOamShadow[sprite].attr01 = ((y & 0xff) | 0xc0000c00) | ((x & 0x1ff) << 16);
            sD3DOamShadow[sprite].attr2 =
                (u16)(((int)(((long long)tileX)) + ((int)(((long long)tileY)) << 5)) | 0xf000);
            tileX += 8;
            sprite++;
        }
        tileY += 8;
    }

    DC_FlushRange(sD3DOamShadow, 0x400);
    GXS_LoadOAM(sD3DOamShadow, 0, 0x400);
}

extern "C" void D3D_ClearCaptureTable(void)
{
    GXS_LoadOAM(sD3DOamShadow, 0, 0x400u);
}

extern "C" void D3D_InitDualScreen(void)
{
    GX_ResetBankForSubBG();
    GX_ResetBankForSubOBJ();
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    *(unsigned int *)0x4001000 &= ~0x300010;
}

/* Slot 31: loads the sub-screen BG and OBJ graphics, then restores the
 * banks it borrowed. Keep the destinations as base plus offset; one folded
 * constant changes the code. */
int dScMgD3DBase_c::InitSubScreenBg()
{
    mSavedSubObjBank = GX_GetBankForSubOBJ();
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);
    LoadCompressedFileAt(sD3DCommonNcgByLanguage[GetGameLanguage()], G2S_GetOBJCharPtr());
    DecompressLZ16((void *)sD3DSubScreenBgByLanguage[GetGameLanguage()],
                   (u8 *)G2S_GetOBJCharPtr() + 0x2000);
    GX_SetBankForSubOBJ(mSavedSubObjBank);
    mSubBgBank = GX_GetBankForSubBG();
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x4000,
                      GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2S_SetBG1Priority(0);
    G2S_BG1Mosaic(false);
    SetSubBg1Offset(0, 0);
    gSubScreenLayers &= ~2;
    {
        s32 language = GetGameLanguage();
        LoadCompressedFileAt(sD3DOptionNcgByLanguage[language], (void *)G2S_GetBG1CharPtr());
    }
    LoadCompressedFileAt(0x5b, (void *)G2S_GetBG1ScrPtr());
    GX_SetBankForSubBG(mSubBgBank);
}

/* Slot 33: sets up the 3D display and VRAM banks, keeps the loaded
 * buffers for later palette and menu work, and publishes the two
 * cameras. */
void dScMgD3DBase_c::InitEngines()
{
    void *file;

    GXS_DispOn();
    gMainScreenLayers = DISPLAY_LAYER_OBJ;
    gSubScreenLayers = 0x10;
    Mg_ResetBgOffsets();
    Mg_DisableAllBanks();
    data_0209e660 = 1;
    GX::DisableAllBanks();
    GX_SetBankForTex(GX_VRAM_TEX_0_A);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G);
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    GX_SetBankForBGExtPltt(GX_VRAM_BGEXTPLTT_NONE);
    GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    GX_SetBankForBG(GX_VRAM_BG_128_B);
    file = Ov004_LoadFile(sD3DCommonNcgByLanguage[GetGameLanguage()]);
    sD3DLanguageFile = file;
    DecompressLZ16(file, (u8 *)G2_GetOBJCharPtr() + 0x4000);
    file = Ov004_LoadFile(0xc3);
    sD3DSubObjPalette = file;
    GX_LoadOBJPltt(file, 0x100u, 0x100u);
    GXS_LoadOBJPltt(sD3DSubObjPalette, 0x100u, 0x100u);
    InitialiseVramGlobals();
    Mg_ResetOverlays();
    D3D_DrawSpriteTable();
    data_0208ee44 = 1;
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GXS_SetGraphicsMode(GX_BGMODE_5);
    *(vu32 *)0x4000000u &= 0xffcfffefu;
    sMgCurCamera = (int)&mCameras[0];
    sMgMainCamera = &mCameras[0];
    sMgSubCamera = &mCameras[1];
    sMgGraphCallback.mScene = this;
    gGraphCallback = &sMgGraphCallback;
}

/* Clears the sub-screen BG1 claim. The caller passes this, which is
 * unused. */
extern "C" void D3D_ReleaseSubBg1(void *) { data_0209e660 = 0; }

/* The key function: the first virtual declared but not defined inline, so
   defining it here emits the vtable, the RTTI and the D1 and D0 pair. */
bool dScMgD3DBase_c::BeforeInitResources()
{
    if (dScMgBase_c::BeforeInitResources() == 0) return 0;
    unk_5000 = 0;
    return 1;
}

void dScMgD3DBase_c::AfterInitResources(unsigned int vfSuccess)
{
    dScMgBase_c::AfterInitResources(vfSuccess);
    mParticle.Initialise();
}

int dScMgD3DBase_c::BeforeBehavior()
{
    if (dScMgBase_c::BeforeBehavior() == 0) return 0;
    if (data_020a0db0 & 1)
        mParticle.Update();
    return 1;
}

int dScMgD3DBase_c::BeforeRender()
{
    if (dScMgBase_c::BeforeRender() == 0) {
        return 0;
    }

    Particle::RenderAll();
    return 1;
}

void dScMgD3DBase_c::AfterRender(unsigned int arg)
{
    int line = REG_VCOUNT;

    if (line > 0xb9 && line <= 0xc0) {
        while ((int)REG_VCOUNT < 0xc0) {
        }
    }

    dScene_c::AfterRender(arg);
}

void dScMgD3DBase_c::AfterCleanupResources(unsigned int vfSuccess)
{
    if (vfSuccess == 2) {
        CleanCommonModelDataArr();
        G3_LightVector(GX_LIGHTID_0, 0, -0xb50, -0xb50);
        G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
        G3_LightVector(GX_LIGHTID_1, 0, -0xb50, -0xb50);
        G3_LightColor(GX_LIGHTID_1, GX_RGB(31, 31, 31));
        D3D_ReleaseSubBg1(this);
        Ov004_Deallocate(sD3DLanguageFile);
        Ov004_Deallocate(sD3DSubObjPalette);
    }
    data_0209f5f8 = 0;
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

/* Slot 24, inherited by Jump and Jump2: switches between the two
 * 0xbc-byte cameras. Keep the if/else toggle; `mDrawSubScreen = mDrawSubScreen == 0`
 * compiles differently. */
int dScMgD3DBase_c::OnGraphCallback2()
{
    if (dScMgBase_c::OnGraphCallback2() == 0) return 0;
    if (mMenuOpen == 0) {
        if (data_0209d464 == 0) return 0;
        if (mDrawSubScreen == 0)
            mDrawSubScreen = 1;
        else
            mDrawSubScreen = 0;
        MgCamera *camera = &mCameras[mDrawSubScreen];
        sMgCurCamera = (int)camera;
        Camera_UpdateMatrices(camera);
        if (mDrawSubScreen == 1) {
            D3D_CaptureToSub();
        } else {
            D3D_CaptureToMain();
            if (data_0209f5f8 == 0) {
                *(int*)0x4001000 &= ~0xe000;
                data_0209f5f8 = 1;
            }
        }
    }
    return 1;
}

/* Slot 25. */
int dScMgD3DBase_c::OnGraphCallback3()
{
    return dScMgBase_c::OnGraphCallback3() != 0;
}

/* Slot 26: marks the 3D scenes with 2. */
int dScMgD3DBase_c::GetDisplayMode()
{
    return 2;
}

/* Returns whatever func_02012718 leaves in r0; decl_common.h declares it
   void, so there is no return statement. */
extern "C" int D3D_PlaySeAtX(int a, int b)
{
    func_02012718(a, b + 0x80000);
}

extern "C" void Sound_PlayBank1Panned(int a0, int a1, int a2) {
    a1 += sD3DSoundPanOffsets[a0];
    int pan = D3D_PanForX(a2);
    func_020127ec(1, a1, 4, 0, 0, pan);
}

extern "C" void D3D_PlaySePannedLoop(int a0, int a1, int a2) {
    int pan = func_020126e8(a1 + 0x80000);
    func_020126ac(a0, 6, 0, a2, pan);
}

/* Thunk: func_020126e8(a + 0x80000). */
extern "C" int D3D_PanForX(int a)
{
    return func_020126e8(a + 0x80000);
}

/* Slot 27: a long-branch veneer to the base. */
void dScMgD3DBase_c::OpenMenu()
{
    dScMgBase_c::OpenMenu();
}

/* Slot 28. */
int dScMgD3DBase_c::CloseMenu()
{
    return dScMgBase_c::CloseMenu();
}

/* Slot 29: claims the sub-screen banks before the base opens the menu. */
int dScMgD3DBase_c::SaveScreenForMenu()
{
    D3D_InitDualScreen();
    GXS_LoadOBJPltt(sD3DSubObjPalette, 0x100, 0x100);
    data_0209e660 = 0;
    mSubBgBank = GX_GetBankForSubBG();
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H);
    mSavedSubObjBank = GX_GetBankForSubOBJ();
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);
    return dScMgBase_c::SaveScreenForMenu();
}

/* Slot 30: gives the banks back around the base's menu teardown. */
void dScMgD3DBase_c::RestoreScreenFromMenu()
{
    GX_SetBankForSubBG(mSubBgBank);
    GX_SetBankForSubOBJ(mSavedSubObjBank);
    data_0209e660 = 1;
    D3D_ClearCaptureTable();
    dScMgBase_c::RestoreScreenFromMenu();
    if (mDrawSubScreen == 1) {
        D3D_CaptureToSub();
    } else {
        D3D_CaptureToMain();
        if (data_0209f5f8 == 0) data_0209f5f8 = 1;
    }
}

/* The destructor is inline in the header (see the banner). */
