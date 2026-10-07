//cpp
/* Scene lifecycle, transitions and graphics setup: arm9:0x0202e140..0x0202ec9c.
 * The manifest records the inferred TU boundary and emitted metadata policy.
 * Definitions run in reverse ROM order under mwccarm 2004/b56. Defining the
 * key function emits both inline destructor variants through the vtable;
 * no destructor-forcing functions are needed.
 */
#include "dScene_c.h"
#include "dScStage_c.h"
#include "dFdColor_c.h"

struct Matrix2x2 { int m[4]; };

/* The transition calls pass a second word to fader slots 3/4, whereas the
 * shared class currently declares only the frame count. Preserve this local
 * ABI view until the fader-family contract is reconstructed. Direct calls to
 * dFdBrightness_c below use its existing native one-argument methods.
 */
struct FaderVTable {
    void (*D1)(void *);
    void (*D0)(void *);
    void (*AdvanceFade)(void *);
    void (*SetBackwardTime)(void *, u32, u32);
    void (*SetForwardTime)(void *, u32, u32);
    int  (*IsAtStart)(void *);
    int  (*IsAtEnd)(void *);
};
struct FaderObject { FaderVTable *vt; };

namespace GX { void DisableAllBanks(); }

extern "C" {
extern int  func_02053c10(int enable);
extern int  func_02053be0(int enable);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(volatile u16* p, Matrix2x2* m, int a, int b, int c, int d);
extern void func_02053a90(u16* out, int v);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern Matrix2x2 data_02092668;

extern u16 func_02053f58(void);
extern u16 func_02054018(void);
extern u16 func_02054004(void);
extern u16 func_02053f6c(void);
extern u16 func_02053fa8(void);
extern u16 func_02053f94(void);
extern u16 func_02053f80(void);
extern u16 func_02053fe0(void);
extern u16 func_02053fbc(void);
extern u16 func_02053f44(void);
extern u16 func_02053f30(void);
extern u16 func_02053f08(void);
extern u16 func_02053ee0(void);

extern void _ZN2GX13SetBankForTexEt(u16);
extern void _ZN2GX17SetBankForTexPlttEt(u16);
extern void _ZN2GX12SetBankForBGEt(u16);
extern void _ZN2GX13SetBankForOBJEt(u16);
extern void _ZN2GX15SetBankForSubBGEt(u16);
extern void _ZN2GX22SetBankForSubBGExtPlttEt(u16);
extern void _ZN2GX16SetBankForSubOBJEt(u16);

extern void func_0205583c(void);

extern void Initialise3dGraphics(int arg);

extern fBase_c *data_0209f5c0;

extern dFdColor_c data_0209f5e8;

extern void *data_0209f1e4;
extern void func_02011b7c(void);

extern char data_0209b53c[];
extern void func_02011974(void *object);

extern u8 data_02092660;

extern u8   data_0209f1e0;

extern dFdBrightness_c *data_0209f5bc;
extern dFdBrightness_c data_0209f5d0;
extern u16  data_02092664;

extern void func_02023544(void);
extern int  func_020431c4(fBase_c *self);

extern u32 data_0209f5b8;

extern int func_0203d9b4(void);

extern int func_02013edc(u32 sceneID, u32 param, int a);

extern dFdBrightness_c *data_0209d4ac;
}

// @symbol _ZN8dScene_c22ResetHardwareRegistersEv
void dScene_c::ResetHardwareRegisters()
{
    Matrix2x2 m;

    m = data_02092668;

    *(volatile u16*)0x4000304 |= 0x8000;
    *(volatile u16*)0x4000304 = (*(volatile u16*)0x4000304 & 0xfffffdf1) | 0x20e;
    *(volatile u16*)0x4000304 |= 1;

    GX::DisableAllBanks();
    func_02053c10(0);
    func_02053be0(1);
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);

    *(volatile u32*)0x4000000 &= ~0x1f00;
    *(volatile u32*)0x4000000 &= ~0xe000;
    *(volatile u32*)0x4000000 &= ~0x38000000;
    *(volatile u32*)0x4000000 &= ~0x7000000;
    *(volatile u32*)0x4000000 &= 0xffcfffef;
    *(volatile u32*)0x4000000 &= 0xffbfff9f;
    *(volatile u32*)0x4000000 &= ~0x800000;
    *(volatile u32*)0x4000064 = 0x80000000;

    *(volatile u16*)0x4000008 &= 0x43;
    *(volatile u16*)0x400000a &= 0x43;
    *(volatile u16*)0x400000c &= 0x43;
    *(volatile u16*)0x400000e &= 0x43;
    *(volatile u16*)0x4000008 &= ~0x40;
    *(volatile u16*)0x400000a &= ~0x40;
    *(volatile u16*)0x400000c &= ~0x40;
    *(volatile u16*)0x400000e &= ~0x40;
    *(volatile u16*)0x4000008 &= ~3;
    *(volatile u16*)0x400000a &= ~3;
    *(volatile u16*)0x400000c &= ~3;
    *(volatile u16*)0x400000e &= ~3;

    *(volatile u32*)0x4000010 = 0;
    *(volatile u32*)0x4000014 = 0;
    *(volatile u32*)0x4000018 = 0;
    *(volatile u32*)0x400001c = 0;

    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile u16*)0x4000020, &m, 0, 0, 0, 0);
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile u16*)0x4000030, &m, 0, 0, 0, 0);

    *(volatile u16*)0x4000048 &= ~0x3f;
    *(volatile u16*)0x4000048 &= ~0x3f00;
    *(volatile u16*)0x400004a &= ~0x3f;
    *(volatile u16*)0x400004a &= ~0x3f00;

    *(volatile u16*)0x4000040 = 0;
    *(volatile u16*)0x4000044 = 0;
    *(volatile u16*)0x4000042 = 0;
    *(volatile u16*)0x4000046 = 0;
    *(volatile u8*)0x400004c = 0;
    *(volatile u8*)0x400004d = 0;

    func_02053a90((u16*)0x400006c, 0);
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile u32*)0x4001000 &= ~0x1f00;
    *(volatile u32*)0x4001000 &= ~0xe000;
    *(volatile u32*)0x4001000 &= 0xffcfffef;
    *(volatile u32*)0x4001000 &= 0xffbfff9f;
    *(volatile u32*)0x4001000 &= ~0x800000;

    *(volatile u16*)0x4001008 &= 0x43;
    *(volatile u16*)0x400100a &= 0x43;
    *(volatile u16*)0x400100c &= 0x43;
    *(volatile u16*)0x400100e &= 0x43;
    *(volatile u16*)0x4001008 &= ~0x40;
    *(volatile u16*)0x400100a &= ~0x40;
    *(volatile u16*)0x400100c &= ~0x40;
    *(volatile u16*)0x400100e &= ~0x40;
    *(volatile u16*)0x4001008 &= ~3;
    *(volatile u16*)0x400100a &= ~3;
    *(volatile u16*)0x400100c &= ~3;
    *(volatile u16*)0x400100e &= ~3;

    *(volatile u32*)0x4001010 = 0;
    *(volatile u32*)0x4001014 = 0;
    *(volatile u32*)0x4001018 = 0;
    *(volatile u32*)0x400101c = 0;

    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile u16*)0x4001020, &m, 0, 0, 0, 0);
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile u16*)0x4001030, &m, 0, 0, 0, 0);

    *(volatile u16*)0x4001048 &= ~0x3f;
    *(volatile u16*)0x4001048 &= ~0x3f00;
    *(volatile u16*)0x400104a &= ~0x3f;
    *(volatile u16*)0x400104a &= ~0x3f00;

    *(volatile u16*)0x4001040 = 0;
    *(volatile u16*)0x4001044 = 0;
    *(volatile u16*)0x4001042 = 0;
    *(volatile u16*)0x4001046 = 0;
    *(volatile u8*)0x400104c = 0;
    *(volatile u8*)0x400104d = 0;

    func_02053a90((u16*)0x400106c, 0);
}

namespace GX {
// @symbol _ZN2GX15DisableAllBanksEv
void DisableAllBanks(){
 func_02053f58(); func_02054018(); func_02054004(); func_02053f6c();
 func_02053fa8(); func_02053f94(); func_02053f80(); func_02053fe0();
 func_02053fbc(); func_02053f44(); func_02053f30(); func_02053f08();
 func_02053ee0();
}
}

// @symbol _ZN10dScStage_c12SetVramBanksEv
void dScStage_c::SetVramBanks() {
    GX::DisableAllBanks();
    _ZN2GX13SetBankForTexEt(3);
    _ZN2GX17SetBankForTexPlttEt(0x30);
    _ZN2GX12SetBankForBGEt(8);
    _ZN2GX13SetBankForOBJEt(0x40);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX22SetBankForSubBGExtPlttEt(0x80);
    _ZN2GX16SetBankForSubOBJEt(0x100);
}

// @symbol _ZN8dScene_c20Initialise3dGraphicsEv
void dScene_c::Initialise3dGraphics()
{
    ResetHardwareRegisters();
    func_0205583c();
    _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
    *(volatile u32 *)0x40004c8 = 0x296a5800;
    *(volatile u32 *)0x40004cc = 0x7fff;
    *(volatile u32 *)0x40004c0 = 0x7fff;
    *(volatile u32 *)0x40004c4 = 0;
    ::Initialise3dGraphics(0);
}

// @symbol _ZN8dScene_c19ResetFadersAndSoundEv
bool dScene_c::ResetFadersAndSound()
{
    data_0209f5c0 = this;
    if (!fBase_c::BeforeInitResources())
        return 0;
    SetFaders(&data_0209f5e8);
    data_0209f1e4 = 0;
    func_02011b7c();
    return 1;
}

// @symbol _ZN8dScene_c19BeforeInitResourcesEv
bool dScene_c::BeforeInitResources()
{
    if (!ResetFadersAndSound())
        return false;
    Initialise3dGraphics();
    return true;
}

// @symbol _ZN8dScene_c18AfterInitResourcesEj
void dScene_c::AfterInitResources(u32 vfSuccess)
{
    dBase_c::AfterInitResources(vfSuccess);
}

// @symbol _ZN8dScene_c22BeforeCleanupResourcesEv
int dScene_c::BeforeCleanupResources()
{
    if (!fBase_c::BeforeCleanupResources())
        return 0;
    func_02011974(data_0209b53c);
    return 1;
}

// @symbol _ZN8dScene_c21AfterCleanupResourcesEj
void dScene_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2)
        data_02092660 = 0;
    fBase_c::AfterCleanupResources(vfSuccess);
}

// @symbol _ZN8dScene_c14BeforeBehaviorEv
int dScene_c::BeforeBehavior()
{
    if (!fBase_c::BeforeBehavior())
        return 0;

    if (data_0209f1e0 != 0) {
        int noActor = (data_0209f5c0->actorID == 0);
        if (noActor != 0) {
            func_02023544();
        } else {
            if (data_0209f1e4 == 0) {
                data_0209f5d0.currInterp = 0;
                data_0209f5d0.dFdBrightness_c::SetForwardTime(0x10);
                data_0209f1e4 = &data_0209f5d0;
            } else if (data_0209f5d0.dFdBrightness_c::IsAtEnd()) {
                StartSceneFade(1, 0, 0);
                ((FaderObject *)&data_0209f5e8)->vt->SetForwardTime((FaderObject *)&data_0209f5e8, 0, 0);
                MarkForDestruction();
            }
            return 0;
        }
    }

    if (data_02092664 != 0x187) {
        if (((FaderObject *)data_0209f5bc)->vt->IsAtStart(data_0209f5bc) != 0) {
            ((FaderObject *)data_0209f5bc)->vt->SetForwardTime(data_0209f5bc, 0x1e, 0);
        } else if (((FaderObject *)data_0209f5bc)->vt->IsAtEnd(data_0209f5bc) != 0) {
            MarkForDestruction();
        }
        return 1;
    }

    if ((pauseFlags & 1) != 0) {
        if (func_020431c4(this) == 0) {
            pauseFlags &= ~1;
            pauseFlags &= ~4;
        }
        return 0;
    } else {
        if (((FaderObject *)data_0209f5bc)->vt->IsAtEnd(data_0209f5bc) != 0) {
            ((FaderObject *)data_0209f5bc)->vt->SetBackwardTime(data_0209f5bc, 0x1e, 0);
        }
        return 1;
    }
}

// @symbol _ZN8dScene_c13AfterBehaviorEj
void dScene_c::AfterBehavior(u32 vfSuccess)
{
    fBase_c::AfterBehavior(vfSuccess);
}

// @symbol _ZN8dScene_c12BeforeRenderEv
int dScene_c::BeforeRender()
{
    return fBase_c::BeforeRender() != 0;
}

// @symbol _ZN8dScene_c11AfterRenderEj
void dScene_c::AfterRender(u32 vfSuccess)
{
    fBase_c::AfterRender(vfSuccess);
}

// @symbol _ZN8dScene_c15SetSceneToSpawnEjj
int dScene_c::SetSceneToSpawn(u32 sceneID, u32 param)
{
    if (sceneID != data_02092664) {
        data_02092664 = sceneID;
        data_0209f5b8 = param;
        return 1;
    }
    return 0;
}

// @symbol _ZN8dScene_c14StartSceneFadeEjjt
void dScene_c::StartSceneFade(u32 sceneID, u32 param, u16 fadeColor)
{
    if (SetSceneToSpawn(sceneID, param))
        data_0209f5e8.color = fadeColor;
}

// @symbol _ZN8dScene_c18PrepareToSpawnBootEv
void dScene_c::PrepareToSpawnBoot()
{
    if (func_0203d9b4())
        data_02092664 = 0;
    else
        data_02092664 = 0x168;
    data_02092660 = 0;
}

// @symbol _ZN8dScene_c16SpawnIfNecessaryEv
int dScene_c::SpawnIfNecessary()
{
    u16 sceneID;
    if (data_02092660 != 0 || (sceneID = data_02092664) == 0x187)
        return 0;
    {
        int spawned = func_02013edc(sceneID, data_0209f5b8, 1);
        if (spawned == 0)
            return 0;
        data_02092664 = 0x187;
        data_02092660 = 1;
        return spawned;
    }
}

// @symbol _ZN8dScene_c9SetFadersEP15dFdBrightness_c
void dScene_c::SetFaders(dFdBrightness_c *fader)
{
    if (data_0209f5bc) {
        if (data_0209f5bc->IsAtStart()) {
            fader->SetToStart();
        } else if (data_0209f5bc->IsAtEnd()) {
            fader->SetToEnd();
        }
    }
    data_0209f5bc = fader;
    data_0209d4ac = fader;
}

// @symbol _ZN8dScene_c20SetAndStopColorFaderEv
void dScene_c::SetAndStopColorFader()
{
    SetFaders(&data_0209f5e8);
    data_0209f5e8.speed = 0;
}
