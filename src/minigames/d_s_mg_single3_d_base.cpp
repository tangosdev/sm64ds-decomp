//cpp
/* Shared scene for the single-camera 3D minigames: nine functions in
 * [0x0210a4b0, 0x0210a8c0), including the compiler-emitted destructor pair.
 * Keep this definition order: the compiler emits functions in reverse
 * source order.
 *
 * Ov004_Deallocate (ov004, 0x020adc5c) and Deallocate (arm9, 0x02018144)
 * are different functions; each call site keeps the one the ROM calls.
 */

#include "dScMgSingle3DBase_c.h"
#include "types.h"
#include "decl_common.h"

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
void func_ov004_020b290c(void);
void func_ov004_020b2980(void);
s32 GetGameLanguage(void);
/* Forwards to LoadFile. Its own definition is still declared without the
 * argument and result, so this file keeps a local declaration. */
void *func_ov004_020adc68(int fileId);
void func_ov004_020b0d30(void);
void InitialiseVramGlobals(void);
void FreeGfxSlotsById(int id);
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern u32 data_020a0db0;
extern int data_ov004_020beb6c;
extern int data_ov006_0213e42c[];
extern int data_0208ee44;
extern int data_ov004_020beb74[];
extern void **data_0209d4a8;
}

// @symbol _ZN19dScMgSingle3DBase_c9Virtual84Ev
/* Slot 33 brings up both graphics engines, assigns VRAM banks and loads
 * their character data and OBJ palettes before publishing this scene, then
 * sets lights 0 and 1 to white. All thirteen children inherit this override. */
void dScMgSingle3DBase_c::Virtual84()
{
    void *fileData;

    *(vu32 *)0x4001000u |= 0x10000u;
    data_ov004_020beb6c = 0;
    func_ov004_020b290c();
    func_ov004_020b2980();
    data_0209d45c = 0x10;
    data_0209d454 = 0x10;
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    GX::SetBankForBG(2);
    GX::SetBankForOBJ(0x10);
    GX::SetBankForTex(1);
    GX::SetBankForTexPltt(0x20);
    GX::SetGraphicsMode(1, 0, 1);
    *(vu32 *)0x4000000u &= 0xffcfffefu;
    GX::SetBankForSubBG(4);
    GX::SetBankForSubOBJ(8);
    fileData = func_ov004_020adc68(data_ov006_0213e42c[GetGameLanguage()]);
    {
        char *dst = (char *)0x6400000; dst += 0x4000;
        DecompressLZ16(fileData, dst);
    }
    {
        char *dst = (char *)0x6600000; dst += 0x4000;
        DecompressLZ16(fileData, dst);
    }
    Ov004_Deallocate(fileData);
    fileData = func_ov004_020adc68(0xc3);
    CP15::FlushAndInvalidateDataCache((u32)fileData, 0x100u);
    GX::LoadOBJPltt(fileData, 0x100u, 0x100u);
    GXS::LoadOBJPltt(fileData, 0x100u, 0x100u);
    Ov004_Deallocate(fileData);
    func_ov004_020b0d30();
    data_0208ee44 = 1;
    G3X::SetFog(false, 0, 2, 0x1000);
    InitialiseVramGlobals();
    FreeGfxSlotsById(0x1d);
    data_ov004_020beb74[1] = (int)this;
    data_0209d4a8 = (void **)data_ov004_020beb74;
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
/* Slot 5 resets the light vector and light color registers and frees the
 * common model data on teardown. The volatile stores are hardware writes;
 * keep their width and order. */
void dScMgSingle3DBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2) {
        *(volatile int *)0x40004c8 = 0x296a5800;
        *(volatile int *)0x40004cc = 0x7fff;
        CleanCommonModelDataArr();
    }
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

// @symbol _ZN19dScMgSingle3DBase_c24OnHitByCannonBlastedCharEv
/* Slot 26 identifies this branch of minigame scenes with the value 1. */
int dScMgSingle3DBase_c::OnHitByCannonBlastedChar()
{
    return 1;
}

// @symbol func_ov006_0210a534
/* Load the shared sub-screen background assets, releasing each buffer. */
extern "C" void func_ov006_0210a534(void)
{
    void *fileData = (void *)LoadFile(0x26);
    data_0209d454 |= 8;
    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & 0x43) | 0x214;
    DecompressLZ16(fileData, (void *)G2S::GetBG3CharPtr());
    Ov004_Deallocate(fileData);
    fileData = (void *)LoadFile(0x27);
    GXS::LoadBGPltt(fileData, 0xa0, 0x160);
    Deallocate(fileData);
    fileData = (void *)LoadFile(0x28);
    func_020562b4(fileData, 0, 0x800);
    Deallocate(fileData);
    *(volatile u16*)0x400100e &= ~0x40;
    *(volatile u32*)0x400101c = 0;
    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & ~3) | 1;
}

/* The inline destructor plus AfterInitResources as key function emit D1
 * and D0 in cartridge order. Children inline this base teardown, so there
 * is no standalone D2. */
