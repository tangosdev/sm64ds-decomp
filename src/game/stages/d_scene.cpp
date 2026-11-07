//cpp
/* The scene base class: lifecycle, transitions and graphics setup for every
 * playable level and minigame scene.
 *
 * _ZTS8dScene_c 0x020914b0. _ZTI8dScene_c 0x020914d4, base word _ZTI8dBase_c
 * (fBase_c -> dBase_c -> dScene_c). _ZTV8dScene_c 0x02092680. The destructor is the key
 * function: D1 at 0x0202e140, D0 at 0x0202e170, both inline through the
 * vtable. 21 functions, .text 0x0202e140..0x0202ec9c, in reverse ROM order.
 */

// leftovers:
// - dScStage_c::SetVramBanks and GX::DisableAllBanks sit between the scene
//   graphics methods in this span; co-residence is inferred from the
//   contiguous run, not from an object map.
// - func_02053c10/02053be0/02053a90/02053fxx and data_02092660/68 keep
//   linker names: no header home proven for them yet.
#include "dScene_c.h"
#include "dScStage_c.h"
#include "dFdColor_c.h"
#include "G2x.h"
#include "GX.h"
#include "nitro/hw/registers.h"

namespace GX {
void SetGraphicsMode(int a, int b, int c);
}

namespace GXS {
void SetGraphicsMode(int a);
}

#include "Matrix2x2.h"

/* ResetHardwareRegisters copies the identity matrix with one four-word block
   move. A struct copy of Matrix2x2's four named fields scalarizes into
   separate loads and stores and grows the function by 20 bytes, so the copy
   goes through this words view -- the same reason UpdateMinimap keeps its
   MinimapDesc cast in d_s_stage_methods.cpp. */
struct Matrix2x2Words {
    int w[4];
};

namespace GX { void DisableAllBanks(); }

extern "C" {
/* src/engine/gx/GX_Init.cpp names these three in comments but they keep their
   address symbols: the H-blank and V-blank interrupt enables (each returns the
   previous bit) and the MASTER_BRIGHT writer (negative darkens). */
extern int  func_02053c10(int enable);
extern int  func_02053be0(int enable);

extern void func_02053a90(u16* out, int v);

extern Matrix2x2 data_02092668;

extern void func_0205583c(void);

extern void Initialise3dGraphics(int arg);

/* The running scene: ResetFadersAndSound stores `this`, and dActor_c::Spawn,
   LevelObjects and dScEntry_c spawn their children under it. */
extern fBase_c *gCurrentScene;

extern dFader_c *gSoftResetFader;
extern void func_02011b7c(void);

/* Sound.cpp's sound-object registry; only its address is needed here.
   local extern: SndRegTable is Sound.cpp's own type, no header declares it. */
struct SndRegTable;
extern SndRegTable data_0209b53c;
extern void func_02011974(void *object);

extern u8 data_02092660;

extern u8 gSoftResetPending;

/* The fader the scene change runs, installed by SetFaders. */
extern dFdBrightness_c *gSceneFader;
/* The scene SetSceneToSpawn queued, with gNextSceneParam its spawn
   parameter. 0x187 means none: SpawnIfNecessary stores it once it has
   spawned the queued scene, and BeforeBehavior fades the running scene out
   while any other value is pending. */
extern u16 gNextSceneId;

extern void func_02023544(void);

extern u32 gNextSceneParam;

extern int func_0203d9b4(void);

extern int func_02013edc(s32 sceneID, s32 param, int a);

extern dFdBrightness_c *data_0209d4ac;
}

/* The scene's two global faders: the colour fade every scene change runs
   (SetAndStopColorFader, StartSceneFade) and the plain brightness fade
   BeforeBehavior runs for a soft reset. Namespace-scope objects with inline
   constructors and real destructors, so mwcc emits __sinit_d_scene.cpp to
   build them in this order and register their destructors (was the
   handwritten __sinit_02074edc). Other TUs reach them by these names. */
dFdColor_c gSceneColorFader;
dFdBrightness_c gSoftResetBrightnessFader;

// @symbol _ZN8dScene_c22ResetHardwareRegistersEv
void dScene_c::ResetHardwareRegisters()
{
    Matrix2x2 m;

    *(Matrix2x2Words *)&m = *(Matrix2x2Words *)&data_02092668;

    REG_POWCNT1 |= 0x8000;
    REG_POWCNT1 = (REG_POWCNT1 & 0xfffffdf1) | 0x20e;
    REG_POWCNT1 |= 1;

    GX::DisableAllBanks();
    func_02053c10(0);
    func_02053be0(1);
    GX::SetGraphicsMode(1, 0, 0);

    REG_DISPCNT &= ~0x1f00;
    REG_DISPCNT &= ~0xe000;
    REG_DISPCNT &= ~0x38000000;
    REG_DISPCNT &= ~0x7000000;
    REG_DISPCNT &= 0xffcfffef;
    REG_DISPCNT &= 0xffbfff9f;
    REG_DISPCNT &= ~0x800000;
    REG_DISPCAPCNT = 0x80000000;

    REG_BG0CNT &= 0x43;
    REG_BG1CNT &= 0x43;
    REG_BG2CNT &= 0x43;
    REG_BG3CNT &= 0x43;
    REG_BG0CNT &= ~0x40;
    REG_BG1CNT &= ~0x40;
    REG_BG2CNT &= ~0x40;
    REG_BG3CNT &= ~0x40;
    REG_BG0CNT &= ~3;
    REG_BG1CNT &= ~3;
    REG_BG2CNT &= ~3;
    REG_BG3CNT &= ~3;

    REG_BG0OFS = 0;
    REG_BG1OFS = 0;
    REG_BG2OFS = 0;
    REG_BG3OFS = 0;

    G2x::SetBGyAffine(&REG_BG2PA, &m, 0, 0, 0, 0);
    G2x::SetBGyAffine(&REG_BG3PA, &m, 0, 0, 0, 0);

    REG_WININ &= ~0x3f;
    REG_WININ &= ~0x3f00;
    REG_WINOUT &= ~0x3f;
    REG_WINOUT &= ~0x3f00;

    REG_WIN0H = 0;
    REG_WIN0V = 0;
    REG_WIN1H = 0;
    REG_WIN1V = 0;
    REG_MOSAIC_BG = 0;
    REG_MOSAIC_OBJ = 0;

    func_02053a90((u16*)0x400006c, 0);
    GXS::SetGraphicsMode(0);

    REG_DISPCNT_SUB &= ~0x1f00;
    REG_DISPCNT_SUB &= ~0xe000;
    REG_DISPCNT_SUB &= 0xffcfffef;
    REG_DISPCNT_SUB &= 0xffbfff9f;
    REG_DISPCNT_SUB &= ~0x800000;

    REG_DB_BG0CNT &= 0x43;
    REG_DB_BG1CNT &= 0x43;
    REG_DB_BG2CNT &= 0x43;
    REG_DB_BG3CNT &= 0x43;
    REG_DB_BG0CNT &= ~0x40;
    REG_DB_BG1CNT &= ~0x40;
    REG_DB_BG2CNT &= ~0x40;
    REG_DB_BG3CNT &= ~0x40;
    REG_DB_BG0CNT &= ~3;
    REG_DB_BG1CNT &= ~3;
    REG_DB_BG2CNT &= ~3;
    REG_DB_BG3CNT &= ~3;

    REG_DB_BG0OFS = 0;
    REG_DB_BG1OFS = 0;
    REG_DB_BG2OFS = 0;
    REG_DB_BG3OFS = 0;

    G2x::SetBGyAffine(&REG_DB_BG2PA, &m, 0, 0, 0, 0);
    G2x::SetBGyAffine(&REG_DB_BG3PA, &m, 0, 0, 0, 0);

    REG_DB_WININ &= ~0x3f;
    REG_DB_WININ &= ~0x3f00;
    REG_DB_WINOUT &= ~0x3f;
    REG_DB_WINOUT &= ~0x3f00;

    REG_DB_WIN0H = 0;
    REG_DB_WIN0V = 0;
    REG_DB_WIN1H = 0;
    REG_DB_WIN1V = 0;
    REG_DB_MOSAIC_BG = 0;
    REG_DB_MOSAIC_OBJ = 0;

    func_02053a90((u16*)0x400106c, 0);
}

namespace GX {
// @symbol _ZN2GX15DisableAllBanksEv
/* Hand every VRAM user's banks back to the LCDC, in the SDK's own order. */
void DisableAllBanks() {
    DisableBankForLCDC();
    DisableBankForBG();
    DisableBankForOBJ();
    DisableBankForARM7();
    DisableBankForTex();
    DisableBankForTexPltt();
    DisableBankForClearImage();
    DisableBankForBGExtPltt();
    DisableBankForOBJExtPltt();
    DisableBankForSubBG();
    DisableBankForSubOBJ();
    DisableBankForSubBGExtPltt();
    DisableBankForSubOBJExtPltt();
}
}

// @symbol _ZN10dScStage_c12SetVramBanksEv
void dScStage_c::SetVramBanks() {
    GX::DisableAllBanks();
    GX::SetBankForTex(3);
    GX::SetBankForTexPltt(0x30);
    GX::SetBankForBG(8);
    GX::SetBankForOBJ(0x40);
    GX::SetBankForSubBG(4);
    GX::SetBankForSubBGExtPltt(0x80);
    GX::SetBankForSubOBJ(0x100);
}

// @symbol _ZN8dScene_c20Initialise3dGraphicsEv
void dScene_c::Initialise3dGraphics()
{
    ResetHardwareRegisters();
    func_0205583c();
    GX::SetGraphicsMode(1, 0, 1);
    REG_LIGHT_VECTOR = 0x296a5800;
    REG_LIGHT_COLOR = 0x7fff;
    REG_DIF_AMB = 0x7fff;
    REG_SPE_EMI = 0;
    ::Initialise3dGraphics(0);
}

// @symbol _ZN8dScene_c19ResetFadersAndSoundEv
bool dScene_c::ResetFadersAndSound()
{
    gCurrentScene = this;
    if (!fBase_c::BeforeInitResources())
        return 0;
    SetFaders(&gSceneColorFader);
    gSoftResetFader = 0;
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
    func_02011974(&data_0209b53c);
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

    if (gSoftResetPending != 0) {
        int noActor = (gCurrentScene->actorID == 0);
        if (noActor != 0) {
            func_02023544();
        } else {
            if (gSoftResetFader == 0) {
                gSoftResetBrightnessFader.currInterp = 0;
                gSoftResetBrightnessFader.dFdBrightness_c::SetForwardTime(0x10, 0);
                gSoftResetFader = &gSoftResetBrightnessFader;
            } else if (gSoftResetBrightnessFader.dFdBrightness_c::IsAtEnd()) {
                StartSceneFade(1, 0, 0);
                gSceneColorFader.SetForwardTime(0, 0);
                MarkForDestruction();
            }
            return 0;
        }
    }

    if (gNextSceneId != 0x187) {
        if (gSceneFader->IsAtStart() != 0) {
            gSceneFader->SetForwardTime(0x1e, 0);
        } else if (gSceneFader->IsAtEnd() != 0) {
            MarkForDestruction();
        }
        return 1;
    }

    if ((pauseFlags & 1) != 0) {
        if (func_020431c4() == 0) {
            pauseFlags &= ~1;
            pauseFlags &= ~4;
        }
        return 0;
    } else {
        if (gSceneFader->IsAtEnd() != 0) {
            gSceneFader->SetBackwardTime(0x1e, 0);
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
    if (sceneID != gNextSceneId) {
        gNextSceneId = sceneID;
        gNextSceneParam = param;
        return 1;
    }
    return 0;
}

// @symbol _ZN8dScene_c14StartSceneFadeEjjt
void dScene_c::StartSceneFade(u32 sceneID, u32 param, u16 fadeColor)
{
    if (SetSceneToSpawn(sceneID, param))
        gSceneColorFader.color = fadeColor;
}

// @symbol _ZN8dScene_c18PrepareToSpawnBootEv
void dScene_c::PrepareToSpawnBoot()
{
    if (func_0203d9b4())
        gNextSceneId = 0;
    else
        gNextSceneId = 0x168;
    data_02092660 = 0;
}

// @symbol _ZN8dScene_c16SpawnIfNecessaryEv
int dScene_c::SpawnIfNecessary()
{
    u16 sceneID;
    if (data_02092660 != 0 || (sceneID = gNextSceneId) == 0x187)
        return 0;
    {
        int spawned = func_02013edc(sceneID, gNextSceneParam, 1);
        if (spawned == 0)
            return 0;
        gNextSceneId = 0x187;
        data_02092660 = 1;
        return spawned;
    }
}

// @symbol _ZN8dScene_c9SetFadersEP15dFdBrightness_c
void dScene_c::SetFaders(dFdBrightness_c *fader)
{
    if (gSceneFader) {
        if (gSceneFader->IsAtStart()) {
            fader->SetToStart();
        } else if (gSceneFader->IsAtEnd()) {
            fader->SetToEnd();
        }
    }
    gSceneFader = fader;
    data_0209d4ac = fader;
}

// @symbol _ZN8dScene_c20SetAndStopColorFaderEv
void dScene_c::SetAndStopColorFader()
{
    SetFaders(&gSceneColorFader);
    gSceneColorFader.speed = 0;
}
