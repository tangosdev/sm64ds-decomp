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
#include "decl_common.h"
#include "types.h"
#include "GX.h"
#include "G2.h"
#include "CP15.h"

/* An OAM entry, with attributes 0 and 1 stored as one word the way this
 * code writes them. */
typedef struct Oam {
    u32 attr01;
    u16 attr2;
    u16 aff;
} Oam;

extern "C" {

/* ov006's own data. */
extern void *sD3DSubObjPalette;
extern void *sMgSubCamera;
extern Oam sD3DOamShadow[];
extern unsigned char sD3DSoundPanOffsets[];

/* arm9 / ov004 globals with no project header. */
extern unsigned char data_0209d454;
extern unsigned char data_0209d45c;
extern unsigned char data_0209e660;
extern unsigned char data_0209f5f8;
extern void *data_0209d4a8;
extern unsigned int data_020a0db0;
extern int data_0208ee44;
extern int sMgGraphCallback[];

/* arm9 / ov004 helpers with no project header. */
int   func_02053ea0(void);
int   func_02053eb0(void);
void  func_02054140(void);
void  func_02054154(void);
void  func_02054430(int);
void  func_02054748(int);
void  func_02056674(const void *src, unsigned int offset, unsigned int count);
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
u32   LoadCompressedFileAt(unsigned int fileID, void *target);
void  Ov004_Deallocate(void *x);
struct dCamera_c;
void  Camera_UpdateMatrices(dCamera_c *camera);

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

// @symbol D3D_CaptureToMain
extern "C" void D3D_CaptureToMain(void) {
    *(volatile unsigned short *)0x4000304 |= 0x8000;
    func_02054140();
    GX::SetBankForSubBG(4);
    func_02054430(8);
    *(volatile unsigned int *)0x4000064 = 0x80330010;
    data_0209d454 &= ~0x10;
    data_0209d454 |= 4;
    *(volatile unsigned int *)0x4001000 =
        (*(volatile unsigned int *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
    *(volatile unsigned short *)0x400100c =
        (*(volatile unsigned short *)0x400100c & 0x43) | 0x4284;
    *(volatile unsigned short *)0x400100c &= ~3;
    *(volatile unsigned short *)0x400100c &= ~0x40;
}

// @symbol D3D_CaptureToSub
extern "C" void D3D_CaptureToSub(void)
{
    *(volatile unsigned short *)0x4000304 &= ~0x8000;
    func_02054154();
    GX::SetBankForSubOBJ(8);
    func_02054430(4);
    *(volatile int *)0x4000064 = 0x80360010;
    GXS::SetGraphicsMode(5);
    data_0209d454 |= 0x10;
    data_0209d454 &= ~4;
    *(volatile int *)0x4001000 = (*(volatile int *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
}

// @symbol D3D_DrawSpriteTable
extern "C" void D3D_DrawSpriteTable(void)
{
    int entry;
    int x;
    int y;
    int sprite;
    int tileY;
    int tileX;
    u32 displayControl;

    displayControl = *(volatile u32 *)0x4001000;
    *(volatile u32 *)0x4001000 = (displayControl & 0xffbfff9f) | 0x20;

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

    CP15::FlushAndInvalidateDataCache((u32)sD3DOamShadow, 0x400);
    func_02056674(sD3DOamShadow, 0, 0x400);
}

// @symbol D3D_ClearCaptureTable
extern "C" void D3D_ClearCaptureTable(void)
{
    func_02056674(sD3DOamShadow, 0, 0x400u);
}

// @symbol D3D_InitDualScreen
extern "C" void D3D_InitDualScreen(void)
{
    func_02054154();
    func_02054140();
    GX::SetBankForSubOBJ(8);
    GX::SetBankForSubBG(4);
    *(unsigned int *)0x4001000 &= ~0x300010;
}

// @symbol _ZN14dScMgD3DBase_c15InitSubScreenBgEv
/* Slot 31: loads the sub-screen BG and OBJ graphics, then restores the
 * banks it borrowed. Keep the destinations as base plus offset; one folded
 * constant changes the code. */
int dScMgD3DBase_c::InitSubScreenBg()
{
    mSavedSubObjBank = func_02053ea0();
    GX::SetBankForSubOBJ(0x100);
    LoadCompressedFileAt(sD3DCommonNcgByLanguage[GetGameLanguage()], (void *)0x6600000);
    {
        char *dst = (char*)0x6600000; dst += 0x2000;
        DecompressLZ16((void *)sD3DSubScreenBgByLanguage[GetGameLanguage()], dst);
    }
    GX::SetBankForSubOBJ(mSavedSubObjBank);
    mSubBgBank = func_02053eb0();
    GX::SetBankForSubBG(0x80);
    {
        volatile u16 *p = (volatile u16*)0x400100a;
        *p = (*p & 0x43) | 0x800;
        *p = *p & ~3;
        *p = *p & ~0x40;
    }
    SetSubBg1Offset(0, 0);
    data_0209d454 &= ~2;
    {
        s32 language = GetGameLanguage();
        LoadCompressedFileAt(sD3DOptionNcgByLanguage[language], (void *)G2S::GetBG1CharPtr());
    }
    LoadCompressedFileAt(0x5b, (void*)G2S::GetBG1ScrPtr());
    GX::SetBankForSubBG(mSubBgBank);
}

// @symbol _ZN14dScMgD3DBase_c11InitEnginesEv
/* Slot 33: sets up the 3D display and VRAM banks, keeps the loaded
 * buffers for later palette and menu work, and publishes the two
 * cameras. */
void dScMgD3DBase_c::InitEngines()
{
    void *file;

    *(vu32 *)0x4001000u |= 0x10000u;
    data_0209d45c = 0x10;
    data_0209d454 = 0x10;
    Mg_ResetBgOffsets();
    Mg_DisableAllBanks();
    data_0209e660 = 1;
    GX::DisableAllBanks();
    GX::SetBankForTex(1);
    GX::SetBankForTexPltt(0x40);
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    func_02054748(0);
    GX::SetBankForOBJ(0x10);
    GX::SetBankForBG(2);
    file = Ov004_LoadFile(sD3DCommonNcgByLanguage[GetGameLanguage()]);
    sD3DLanguageFile = file;
    {
        char *dst = (char *)0x6400000;
        dst += 0x4000;
        DecompressLZ16(file, dst);
    }
    file = Ov004_LoadFile(0xc3);
    sD3DSubObjPalette = file;
    GX::LoadOBJPltt(file, 0x100u, 0x100u);
    GXS::LoadOBJPltt(sD3DSubObjPalette, 0x100u, 0x100u);
    InitialiseVramGlobals();
    Mg_ResetOverlays();
    D3D_DrawSpriteTable();
    data_0208ee44 = 1;
    GX::SetGraphicsMode(1, 0, 1);
    GXS::SetGraphicsMode(5);
    *(vu32 *)0x4000000u &= 0xffcfffefu;
    sMgCurCamera = (int)&mCameras[0];
    sMgMainCamera = &mCameras[0];
    sMgSubCamera = &mCameras[1];
    sMgGraphCallback[1] = (int)this;
    data_0209d4a8 = (void *)sMgGraphCallback;
}

// @symbol D3D_ReleaseSubBg1
/* Clears the sub-screen BG1 claim. The caller passes this, which is
 * unused. */
extern "C" void D3D_ReleaseSubBg1(void *) { data_0209e660 = 0; }

// @symbol _ZN14dScMgD3DBase_c19BeforeInitResourcesEv
/* The key function: the first virtual declared but not defined inline, so
   defining it here emits the vtable, the RTTI and the D1 and D0 pair. */
bool dScMgD3DBase_c::BeforeInitResources()
{
    if (dScMgBase_c::BeforeInitResources() == 0) return 0;
    unk_5000 = 0;
    return 1;
}

// @symbol _ZN14dScMgD3DBase_c18AfterInitResourcesEj
void dScMgD3DBase_c::AfterInitResources(unsigned int vfSuccess)
{
    dScMgBase_c::AfterInitResources(vfSuccess);
    mSysTracker.Initialise();
}

// @symbol _ZN14dScMgD3DBase_c14BeforeBehaviorEv
int dScMgD3DBase_c::BeforeBehavior()
{
    if (dScMgBase_c::BeforeBehavior() == 0) return 0;
    if (data_020a0db0 & 1)
        mSysTracker.Update();
    return 1;
}

// @symbol _ZN14dScMgD3DBase_c12BeforeRenderEv
int dScMgD3DBase_c::BeforeRender()
{
    if (dScMgBase_c::BeforeRender() == 0) {
        return 0;
    }

    Particle::RenderAll();
    return 1;
}

// @symbol _ZN14dScMgD3DBase_c11AfterRenderEj
void dScMgD3DBase_c::AfterRender(unsigned int arg)
{
    volatile unsigned short *reg = (volatile unsigned short *)0x04000006;
    int line = *reg;

    if (line > 0xb9 && line <= 0xc0) {
        while ((int)*reg < 0xc0) {
        }
    }

    dScene_c::AfterRender(arg);
}

// @symbol _ZN14dScMgD3DBase_c21AfterCleanupResourcesEj
void dScMgD3DBase_c::AfterCleanupResources(unsigned int vfSuccess)
{
    if (vfSuccess == 2) {
        CleanCommonModelDataArr();
        *(int*)0x40004c8 = 0x296a5800;
        *(int*)0x40004cc = 0x7fff;
        *(int*)0x40004c8 = 0x696a5800;
        *(int*)0x40004cc = 0x40007fff;
        D3D_ReleaseSubBg1(this);
        Ov004_Deallocate(sD3DLanguageFile);
        Ov004_Deallocate(sD3DSubObjPalette);
    }
    data_0209f5f8 = 0;
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

// @symbol _ZN14dScMgD3DBase_c16OnGraphCallback2Ev
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
        int cameraAddress = (int)&mCameras[mDrawSubScreen];
        sMgCurCamera = cameraAddress;
        Camera_UpdateMatrices((dCamera_c *)cameraAddress);
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

// @symbol _ZN14dScMgD3DBase_c16OnGraphCallback3Ev
/* Slot 25. */
int dScMgD3DBase_c::OnGraphCallback3()
{
    return dScMgBase_c::OnGraphCallback3() != 0;
}

// @symbol _ZN14dScMgD3DBase_c14GetDisplayModeEv
/* Slot 26: marks the 3D scenes with 2. */
int dScMgD3DBase_c::GetDisplayMode()
{
    return 2;
}

// @symbol D3D_PlaySeAtX
/* Returns whatever func_02012718 leaves in r0; decl_common.h declares it
   void, so there is no return statement. */
extern "C" int D3D_PlaySeAtX(int a, int b)
{
    func_02012718(a, b + 0x80000);
}

// @symbol Sound_PlayBank1Panned
extern "C" void Sound_PlayBank1Panned(int a0, char *a1, void *a2) {
    a1 += sD3DSoundPanOffsets[a0];
    int pan = D3D_PanForX((int)a2);
    func_020127ec(1, (int)a1, 4, 0, 0, pan);
}

// @symbol D3D_PlaySePannedLoop
extern "C" void D3D_PlaySePannedLoop(int a0, int a1, int a2) {
    int pan = func_020126e8(a1 + 0x80000);
    func_020126ac(a0, 6, 0, a2, pan);
}

// @symbol D3D_PanForX
/* Thunk: func_020126e8(a + 0x80000). */
extern "C" int D3D_PanForX(int a)
{
    return func_020126e8(a + 0x80000);
}

// @symbol _ZN14dScMgD3DBase_c8OpenMenuEv
/* Slot 27: a long-branch veneer to the base. */
void dScMgD3DBase_c::OpenMenu()
{
    dScMgBase_c::OpenMenu();
}

// @symbol _ZN14dScMgD3DBase_c9CloseMenuEv
/* Slot 28. */
int dScMgD3DBase_c::CloseMenu()
{
    return dScMgBase_c::CloseMenu();
}

// @symbol _ZN14dScMgD3DBase_c17SaveScreenForMenuEv
/* Slot 29: claims the sub-screen banks before the base opens the menu. */
int dScMgD3DBase_c::SaveScreenForMenu()
{
    D3D_InitDualScreen();
    GXS::LoadOBJPltt(sD3DSubObjPalette, 0x100, 0x100);
    data_0209e660 = 0;
    mSubBgBank = func_02053eb0();
    GX::SetBankForSubBG(0x80);
    mSavedSubObjBank = func_02053ea0();
    GX::SetBankForSubOBJ(0x100);
    return dScMgBase_c::SaveScreenForMenu();
}

// @symbol _ZN14dScMgD3DBase_c21RestoreScreenFromMenuEv
/* Slot 30: gives the banks back around the base's menu teardown. */
void dScMgD3DBase_c::RestoreScreenFromMenu()
{
    GX::SetBankForSubBG(mSubBgBank);
    GX::SetBankForSubOBJ(mSavedSubObjBank);
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
