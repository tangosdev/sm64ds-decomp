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
// - func_02053fxx and data_02092660/68 keep linker names: no header home
//   proven for them yet.
#include "dScene_c.h"
#include "dScStage_c.h"
#include "dFdColor_c.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/g3x.h"
#include "GX.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_light.h"

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
/* local extern: nitro/gx/gx.h declares these two; the parameter names here
   hold this TU's @NNN numbers. */
extern int GX_HBlankIntr(int enable);
extern int GX_VBlankIntr(int enable);

extern Matrix2x2 data_02092668;

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

void dScene_c::ResetHardwareRegisters()
{
    Matrix2x2 m;

    *(Matrix2x2Words *)&m = *(Matrix2x2Words *)&data_02092668;

    REG_POWCNT1 |= 0x8000;
    REG_POWCNT1 = (REG_POWCNT1 & 0xfffffdf1) | 0x20e;
    REG_POWCNT1 |= 1;

    GX::DisableAllBanks();
    GX_HBlankIntr(0);
    GX_VBlankIntr(1);
    GX_SetGraphicsMode(1, 0, 0);

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

    G2_SetBG2Affine(&m, 0, 0, 0, 0);
    G2_SetBG3Affine(&m, 0, 0, 0, 0);

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

    GX_SetMasterBrightness(0);
    GXS_SetGraphicsMode(0);

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

    G2S_SetBG2Affine(&m, 0, 0, 0, 0);
    G2S_SetBG3Affine(&m, 0, 0, 0, 0);

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

    GXS_SetMasterBrightness(0);
}

namespace GX {
/* Hand every VRAM user's banks back to the LCDC, in the SDK's own order. */
void DisableAllBanks() {
    GX_DisableBankForLCDC();
    GX_DisableBankForBG();
    GX_DisableBankForOBJ();
    GX_DisableBankForARM7();
    GX_DisableBankForTex();
    GX_DisableBankForTexPltt();
    GX_DisableBankForClearImage();
    GX_DisableBankForBGExtPltt();
    GX_DisableBankForOBJExtPltt();
    GX_DisableBankForSubBG();
    GX_DisableBankForSubOBJ();
    GX_DisableBankForSubBGExtPltt();
    GX_DisableBankForSubOBJExtPltt();
}
}

void dScStage_c::SetVramBanks() {
    GX::DisableAllBanks();
    GX_SetBankForTex(3);
    GX_SetBankForTexPltt(0x30);
    GX_SetBankForBG(8);
    GX_SetBankForOBJ(0x40);
    GX_SetBankForSubBG(4);
    GX_SetBankForSubBGExtPltt(0x80);
    GX_SetBankForSubOBJ(0x100);
}

void dScene_c::Initialise3dGraphics()
{
    ResetHardwareRegisters();
    G3X_Init();
    GX_SetGraphicsMode(1, 0, 1);
    G3_LightVector(GX_LIGHTID_0, 0, -0xb50, -0xb50); /* (0, -1/sqrt2, -1/sqrt2) */
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
    REG_DIF_AMB = GX_RGB(31, 31, 31);
    REG_SPE_EMI = 0;
    ::Initialise3dGraphics(0);
}

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

bool dScene_c::BeforeInitResources()
{
    if (!ResetFadersAndSound())
        return false;
    Initialise3dGraphics();
    return true;
}

void dScene_c::AfterInitResources(u32 vfSuccess)
{
    dBase_c::AfterInitResources(vfSuccess);
}

int dScene_c::BeforeCleanupResources()
{
    if (!fBase_c::BeforeCleanupResources())
        return 0;
    func_02011974(&data_0209b53c);
    return 1;
}

void dScene_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2)
        data_02092660 = 0;
    fBase_c::AfterCleanupResources(vfSuccess);
}

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
        if (checkChildProcessCreateState() == 0) {
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

void dScene_c::AfterBehavior(u32 vfSuccess)
{
    fBase_c::AfterBehavior(vfSuccess);
}

int dScene_c::BeforeRender()
{
    return fBase_c::BeforeRender() != 0;
}

void dScene_c::AfterRender(u32 vfSuccess)
{
    fBase_c::AfterRender(vfSuccess);
}

int dScene_c::SetSceneToSpawn(u32 sceneID, u32 param)
{
    if (sceneID != gNextSceneId) {
        gNextSceneId = sceneID;
        gNextSceneParam = param;
        return 1;
    }
    return 0;
}

void dScene_c::StartSceneFade(u32 sceneID, u32 param, u16 fadeColor)
{
    if (SetSceneToSpawn(sceneID, param))
        gSceneColorFader.color = fadeColor;
}

void dScene_c::PrepareToSpawnBoot()
{
    if (func_0203d9b4())
        gNextSceneId = 0;
    else
        gNextSceneId = 0x168;
    data_02092660 = 0;
}

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

void dScene_c::SetAndStopColorFader()
{
    SetFaders(&gSceneColorFader);
    gSceneColorFader.speed = 0;
}
