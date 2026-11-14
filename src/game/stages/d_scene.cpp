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
#include "nitro/gx/gx_bgcnt.h"

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
extern Matrix2x2 data_02092668;

extern void Initialise3dGraphics(int arg);

/* The running scene: ResetFadersAndSound stores `this`, and dActor_c::Spawn,
   LevelObjects and dScEntry_c spawn their children under it. */
extern fBase_c *gCurrentScene;

extern dFader_c *gSoftResetFader;

/* Sound.cpp's sound-object registry; only its address is needed here.
   local extern: SndRegTable is Sound.cpp's own type, no header declares it. */
struct SndRegTable;
extern SndRegTable data_0209b53c;

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


extern dFdBrightness_c *data_0209d4ac;
}
extern void func_02011b7c(void);
extern void func_02011974(void *object);
extern int func_02013edc(s32 sceneID, s32 param, int a);

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

    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    GX_SetPower(GX_POWER_ALL);
    REG_POWCNT1 |= 1;

    GX::DisableAllBanks();
    GX_HBlankIntr(0);
    GX_VBlankIntr(1);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);

    GX_SetVisiblePlane(0);
    GX_SetVisibleWnd(GX_WNDMASK_NONE);
    GX_SetBGScrOffset(0);
    GX_SetBGCharOffset(0);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GX_SetOBJVRamModeBmp(GX_OBJVRAMMODE_BMP_2D_W128);
    GX_HBlankOBJProc(0);
    REG_DISPCAPCNT = 0x80000000;

    G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                     GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                     GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                         GX_BG_CHARBASE_0x00000);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                         GX_BG_CHARBASE_0x00000);
    G2_BG0Mosaic(0);
    G2_BG1Mosaic(0);
    G2_BG2Mosaic(0);
    G2_BG3Mosaic(0);
    G2_SetBG0Priority(0);
    G2_SetBG1Priority(0);
    G2_SetBG2Priority(0);
    G2_SetBG3Priority(0);

    G2_SetBG0Offset(0, 0);
    G2_SetBG1Offset(0, 0);
    G2_SetBG2Offset(0, 0);
    G2_SetBG3Offset(0, 0);

    G2_SetBG2Affine(&m, 0, 0, 0, 0);
    G2_SetBG3Affine(&m, 0, 0, 0, 0);

    G2_SetWnd0InsidePlane(GX_WND_PLANEMASK_NONE, 0);
    G2_SetWnd1InsidePlane(GX_WND_PLANEMASK_NONE, 0);
    G2_SetWndOutsidePlane(GX_WND_PLANEMASK_NONE, 0);
    G2_SetWndOBJInsidePlane(GX_WND_PLANEMASK_NONE, 0);

    G2_SetWnd0Position(0, 0, 0, 0);
    G2_SetWnd1Position(0, 0, 0, 0);
    REG_MOSAIC_BG = 0;
    REG_MOSAIC_OBJ = 0;

    GX_SetMasterBrightness(0);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    GXS_SetVisiblePlane(0);
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    GXS_SetOBJVRamModeBmp(GX_OBJVRAMMODE_BMP_2D_W128);
    GXS_HBlankOBJProc(0);

    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                          GX_BG_CHARBASE_0x00000);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                          GX_BG_CHARBASE_0x00000);
    G2S_BG0Mosaic(0);
    G2S_BG1Mosaic(0);
    G2S_BG2Mosaic(0);
    G2S_BG3Mosaic(0);
    G2S_SetBG0Priority(0);
    G2S_SetBG1Priority(0);
    G2S_SetBG2Priority(0);
    G2S_SetBG3Priority(0);

    G2S_SetBG0Offset(0, 0);
    G2S_SetBG1Offset(0, 0);
    G2S_SetBG2Offset(0, 0);
    G2S_SetBG3Offset(0, 0);

    G2S_SetBG2Affine(&m, 0, 0, 0, 0);
    G2S_SetBG3Affine(&m, 0, 0, 0, 0);

    G2S_SetWnd0InsidePlane(GX_WND_PLANEMASK_NONE, 0);
    G2S_SetWnd1InsidePlane(GX_WND_PLANEMASK_NONE, 0);
    G2S_SetWndOutsidePlane(GX_WND_PLANEMASK_NONE, 0);
    G2S_SetWndOBJInsidePlane(GX_WND_PLANEMASK_NONE, 0);

    G2S_SetWnd0Position(0, 0, 0, 0);
    G2S_SetWnd1Position(0, 0, 0, 0);
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
    GX_SetBankForTex(GX_VRAM_TEX_01_AB);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_01234_EF);
    GX_SetBankForBG(GX_VRAM_BG_128_D);
    GX_SetBankForOBJ(GX_VRAM_OBJ_16_G);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubBGExtPltt(GX_VRAM_SUB_BGEXTPLTT_0123_H);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_16_I);
}

void dScene_c::Initialise3dGraphics()
{
    ResetHardwareRegisters();
    G3X_Init();
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
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
