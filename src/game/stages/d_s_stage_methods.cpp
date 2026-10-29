//cpp
/* dScStage_c -- the playable level scene, arm9.
 *
 * .text 0x02023688..0x0202e118, in ROM order: the destructor pair; the VE_
 * (VS exit), LC_ (level clear) and PS_ (pause screen) groups; the stage's
 * free-function layer (level loading and the player/minigame start paths,
 * the per-level tables and their setters, minimap areas, the Event bit
 * accessors, star/cannon queries, the wipe-fader helpers); the class's
 * rendering, resource and input members (graph callbacks, RenderModel*,
 * LoadModel/LoadFog/LoadSkybox, Render, Behavior, CheckInput,
 * CleanupResources, InitResources, LoadGraphics2D); the level-overlay loaders;
 * and the factory dScStage_c_classInit last, the way every scene TU ends.
 * UpdateMessage (0x0201cb7c) sits with the message code and is not part of
 * this range. The free functions keep their unmangled C names via
 * `extern "C"`, so no caller changes.
 *
 * _ZTS10dScStage_c 0x02092170. _ZTI10dScStage_c 0x02092158, base word
 * _ZTI8dScene_c. _ZTV10dScStage_c 0x020921c0. The out-of-line destructor is
 * the key function. Under `#pragma defer_codegen off` it emits D1
 * (0x02023688), D0 (0x020236f0), then a base-object D2 the cartridge does
 * not carry; the factory's inlined `new` adds dScene_c's and dBase_c's D2,
 * which the cartridge does not carry either (manifest compiler_only_output).
 */

// leftovers:
// - OAM::Render, OAM::RenderSub, Sound::PlaySub and cstd::atan2
//   stay mangled (Fix12 by value). Fog::Init is the scalar Fix12i method.
//   RenderNumber keeps a local extern spelled over the member's mask-emitting
//   parameter conversions. InitResources calls LoadGraphics2D with the comparison
//   itself as the bool, which is what the ROM passes.
// - LoadTextureTransformers spells `new` as an explicit operator new call
//   plus a null-guarded constructor call, a form from before the allocator
//   was named _Znwm, when a new-expression could not link; the fader array passes dFdWipe_c's constructor and destructor by address.
// - CheckCameraInput keeps `#pragma opt_strength_reduction off` and a local
//   CamInput overlay of the camera input array; PS_Update and Behavior keep
//   `#pragma opt_common_subs off` (switched back on after each), the
//   stale-tmpv levers in PS_Update's case 0xa and the
//   address-rematerialization pokes in its case-1 star scroll, and its
//   sp0..sp10/var_r0/var_sl/var_fp locals stay unrenamed until each slot's
//   role is recovered. The touch and controller globals
//   (gTouchHeld..gTouchY, gActivePlayerSlot, data_0209f498) have no header;
//   data_ov001_ and data_ov002_ handles keep linker names.
// - func_0202a850 keeps `#pragma opt_dead_assignments off` around its pair of
//   stores the ROM writes twice.
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScStage_c_AssetId { ASSET_D_2D_UPR_COURSE_CLEAR_MESSAGE_NCL_BIN = 0x025a };

#include "types.h"
#include "G2.h"
#include "dFdBrightness_c.h"
#include "dScStage_c.h"
#include "Timer.h"
#include "decl_common.h"
#include "Sound.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "Player.h"
#include "G2x.h"
#include "nitro/hw/registers.h"
#include "dScene_c.h"
#include "Event.h"
#include "dExtSimpleModel_c.h"
#include "dCamera_c.h"
#include "Particle__SysTracker.h"
#include "Fog.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dFdWipe_c.h"
#include "SaveData.h"
#include "G3X.h"
#include "dExtAnmTexSRT_c.h"
#include "dExtFrameCtrl_c.h"
#include "dExtShadowModel_c.h"
#include "dCc_c.h"
#include "OAM.h"
#include "Message.h"
#include "Memory.h"
#include "Heap.h"
#include "GX.h"
#include "cstd.h"
#include "daMugenBGM_c.h"

struct OamAttr;
struct Matrix2x2 {
    int m[4];
};

// Partial view of the graph callback object, not the playable scene.
struct StageGraphCallbackView {
    void *vtable;
    Matrix2x2 matrix;
    s32 originX;
    s32 originY;
    s32 targetX;
    s32 targetY;
};

/* TU-local overlay of the per-controller input records at data_0209f498,
 * 0x18 apart. CheckInput fills them from the pad and the touch screen each
 * frame; ResetInput clears them; the pause screen polls the mode byte at
 * +0x16 (SetControllerMode's data_0209f4ae). */
struct CamInput {
    s16 dx;      /* 0x00 */
    s16 dy;      /* 0x02 */
    u16 held;    /* 0x04 */
    u16 pressed; /* 0x06 */
    s16 mag;     /* 0x08 */
    s16 nx;      /* 0x0a */
    s16 ny;      /* 0x0c */
    s16 ang;     /* 0x0e */
    u8 tx;       /* 0x10 */
    u8 ty;       /* 0x11 */
    u8 delay;    /* 0x12 */
    u8 cnt;      /* 0x13 */
    u8 touching; /* 0x14 */
    u8 pad15;    /* 0x15 */
    u8 mode;     /* 0x16 */
    u8 pad17;    /* 0x17 */
};

/* TU-local overlay of the touch records the ROM keeps at gTouchHeld:
   held/edge are the gTouchHeld/gTouchEdge lanes, x/y the gTouchX/gTouchY lanes. */
struct TouchInfo {
    u8 held;
    u8 edge;
    u8 x;
    u8 y;
};

extern "C" {
int IsButtonInputValid(void);
int SublevelToLevel(int lv);
int GetOwnerLanguage(void);
void GiveLives(int n);
void Deallocate(void *ptr);
void SetBg2Offset(int x, int y);
void SetBg3Offset(int x, int y);
void SetBg1Offset(int x, int y);
void SetSubBg0Offset(int x, int y);
void SetSubBg1Offset(int x, int y);
int func_02029408(void);
int LoadFile(int handle);
void SetControllerMode(unsigned char m);
unsigned int LoadCompressedFileAt(unsigned short id, void *dst);
int GetSoundMode(void);
void SetSoundMode(int m);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
int IsStarCollected(int lv, int star);

bool d_s_stage_methodsPlaySubHold(unsigned int a, unsigned int b, unsigned int c, int fix, int e);

void OamRender10_d_s_stage_methods(int sub, void *attr, int x, int y, int a, int cc, int fx, int fy,
                                   int rot, int mode);
int __aeabi_idiv(int a, int b);

extern int data_0208ee44;
extern u8 data_0208ee3c;
extern u8 data_0208ee40;
extern s8 data_02092124;
extern s16 data_02092144[];
extern unsigned int data_0209b454;
extern unsigned int data_0209b464;
extern u8 data_020755b8[];
extern volatile int data_02075610[];
extern u16 data_020755cc[];
extern u16 data_020755c0[];
extern u16 data_020755c4[];
extern s16 data_020756d0[];
extern u8 data_0209d660;
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern Timer data_0209d4c8;
extern int data_0209fc68;
extern u8 data_0209f1ec;
extern u8 data_0209f1fc;
extern u8 data_0209f210;
extern u8 data_0209f218;
extern u8 data_0209f20c;
extern u8 data_0209f21c;
extern u8 data_0209f22c;
extern u8 data_0209f234;
extern u8 data_0209f238;
extern u8 data_0209f23c;
extern u8 data_0209f240;
extern u8 data_0209f244;
extern u8 data_0209f248;
extern u8 data_0209f250;
extern u8 data_0209f260;
extern u8 data_0209f280;
extern u8 data_0209f284;
extern u8 data_0209f290;
extern u8 data_0209f294;
extern u8 data_0209f29c;
extern u8 data_0209f2a0[];
extern u8 data_0209f2a4;
extern u8 data_0209f2a8;
extern u8 data_0209f2ac;
extern u8 data_0209f2b0;
extern u8 data_0209f2b4;
extern u8 data_0209f2b8;
extern u8 data_0209f2c4;
extern u8 data_0209f2c8;
extern u8 data_0209f2cc;
extern u8 data_0209f2d4;
extern u8 data_0209f2d8;
extern u8 data_0209f2dc;
extern u8 data_0209f2e0;
extern u8 data_0209f2e4;
extern u8 data_0209f2ec;
extern u8 data_0209f2f0;
extern s8 data_0209f2f8;
extern u16 data_0209f300;
extern u16 data_0209f360[];
extern u16 data_0209f368[];
extern u8 data_0209f350[];
extern void *data_0209f318;
extern unsigned char data_0209f498[];
extern u8 data_0209f4ae[];
extern int data_020a0db0;
extern u8 data_ov002_02111150;
extern u8 data_ov002_02111178;
extern u8 data_ov002_02111180;
extern int data_ov002_0210cea8;
extern int data_ov002_0210cee8;
extern int data_ov002_0210cf28;
extern int data_ov002_0210cf78;
extern int data_ov002_0210cb14;
extern int data_ov002_0210ce90;
extern int data_ov002_0210ced0;
extern int data_ov002_0210cf08;
extern int data_ov002_0210cfa0;
extern int data_ov002_0210caf4;
extern void *data_ov001_020abd98[];
extern void *data_ov001_020ac218[];
extern void *data_ov001_020ac64c[];
extern void *data_ov001_020acb68[];
}

/* The level-state globals the stage's free functions below read and write: the
   level/entrance/star selection, the per-player lives, coins and stars, the
   area, minimap and overlay tables. Byte flags sit in the stage's .bss block
   at 0x0209f1ec..0x0209f5e8 together with the pause-screen state above. */
/* One array member, not four words: C++ scalarizes a struct copy of four
   named fields, and the ROM's copy is a single ldm/stm block move. */
struct MinimapDesc {
    u32 w[4];
};
/* data_0209f3c4 is the stage's graph callback object (InitResources hands
   it to data_0209d4a8). Its last four words are StageGraphCallbackView's
   BG3 affine origin and target, which GraphCallback2 passes to SetBGyAffine
   and UpdateMinimap sets. */
struct MinimapState {
    char _pad[0x14];
    volatile u32 originX, originY, targetX, targetY;
};
/* SetNextLevel's return table: level, return level, entrance. */
struct LevelReturnEntry {
    s8 level;
    s8 returnLevel;
    u8 entrance;
};
struct OvTbl {
    unsigned int t[7];
};
struct PadData {
    u16 held;
    u16 pressed;
};
/* The object at data_0209f5bc; only its slot-5 and slot-7 queries are called
   from this file. */
/* The level record data_0209f340 points at, as far as this file and dMap_c
   read it: the per-area table at +0x10 and its count at +0x14. Each area
   entry carries its texture animation and the minimap GetMinimapID falls
   back to when no minimap-change record applies. */
struct LevelTexAnim {
    int x;
    void *bta;
    s8 minimapId; /* 0x08 */
    char pad[3];
};
struct LevelInfo {
    char pad[0xc];
    /* func_0202a980; dMap_c decompresses it into the minimap BG3 characters. */
    u16 minimapTilesFile; /* 0x0c */
    /* func_0202a96c; dMap_c loads it as the minimap BG extended palette. */
    u16 minimapPaletteFile; /* 0x0e */
    LevelTexAnim *entries;  /* 0x10 */
    unsigned char count;    /* 0x14 */
    /* Bit 0: StartWithFarCamera. */
    u8 cameraFlags; /* 0x15 */
    /* func_0202a958; dMap_c stores it as mScale. */
    s16 minimapScale; /* 0x16 */
};

extern "C" {
extern int OVERLAY_60_ID;
extern int OVERLAY_98_ID;
extern int OVERLAY_102_ID;
extern char VS_STAR_SPAWN_ORDERS[][0xC];
extern u8 data_020755bc[];
extern u16 data_020755e0[7];
extern u16 data_020755f0[7];
extern u16 data_02075600[7];
extern u16 data_02075620[];
extern struct LevelReturnEntry data_02075638[];
extern s16 data_020756b0[];
extern void *data_020756f0[12];
extern unsigned char data_02075768[];
extern u8 data_02075769[];
extern s8 data_0207576a[];
extern OvTbl data_02075804[];
extern int data_020758c8[];
extern unsigned char data_02075998[][7];
extern s16 data_02082214[];
extern u8 data_0208a0e0;
extern s8 data_02092110;
extern s8 data_02092118;
extern s8 data_0209211c;
extern s8 data_02092120;
extern u8 data_02092128[];
extern int data_02092130;
extern unsigned int data_02092134;
extern int data_02092138;
extern u16 *data_0209214c[];
extern s32 data_02092208[];
extern u8 data_0209caa0[];
extern s32 data_0209cef0;
extern void *data_0209d4a8;
extern s32 data_0209e650;
extern u8 data_0209f1f4;
extern u8 data_0209f1f8;
extern u8 data_0209f200;
extern u8 data_0209f204;
extern u8 data_0209f208;
extern u8 data_0209f214;
extern u8 data_0209f220;
extern u8 data_0209f228[];
extern u8 data_0209f230;
extern u8 data_0209f24c;
extern u8 data_0209f25c;
extern u8 data_0209f264;
extern u8 data_0209f268;
extern u8 data_0209f26c;
extern u8 data_0209f270;
extern u8 data_0209f274;
extern u8 data_0209f278;
extern u8 data_0209f27c;
extern u8 data_0209f298;
extern u8 data_0209f2bc;
extern u8 data_0209f2d0;
extern s8 data_0209f2f4;
extern u8 data_0209f2fc;
extern u16 data_0209f304;
extern u16 data_0209f308;
extern s8 data_0209f30c[];
extern s8 data_0209f30d;
extern s8 data_0209f310[];
extern s8 data_0209f311;
extern int *data_0209f328;
extern s32 data_0209f32c;
extern void *data_0209f330;
extern s32 data_0209f338;
extern char *data_0209f340;
extern s32 data_0209f344;
extern short *data_0209f348;
extern void *data_0209f354;
extern unsigned char data_0209f370[];
extern unsigned char data_0209f37c[];
extern void *data_0209f394[];
extern void *data_0209f3a4[];
extern MinimapState data_0209f3c4;
extern MinimapDesc data_0209f3c8;
extern int data_0209f3e8[];
extern int data_0209f40c[];
extern u32 data_0209f4f8[][16];
extern dFdBrightness_c *data_0209f5bc;
extern unsigned short data_0209f5e8;
extern int gRunningKuppaScript;
extern u8 data_0209fcc8;
extern PadData data_020a0e58[];
extern u16 data_020a0e5a[];
extern void *data_ov002_0210ca0c[];
extern int data_ov002_0210cfd0[];
extern int data_ov002_0210d008;
extern int data_ov002_0210d050[];
extern int data_ov002_0210d098;
extern int data_ov002_0210d0e8[];
extern int data_ov002_0210d130;
extern int data_ov002_0210d188[];
extern int data_ov002_0210d1c8;
extern int data_ov002_0210d210;
extern int data_ov002_0210d378;
extern int data_ov002_0210d3a0;
extern int data_ov002_0210d3c0;
extern int data_ov002_0210d3e0;
extern int data_ov002_0210d420;
extern u16 data_ov002_02111188;
}

/* The free functions this file defines, declared ahead of their first use
   (the ones decl_common.h and the block above already declare are not
   repeated). */
struct Obj;
struct ObjTeleport;
extern "C" {
int func_02029600(int x);
void UpdateMinimap(MinimapDesc *desc, u32 a, u32 b, u32 c, u32 d);
void StartEntranceFaderWipe(int index);
void StartExitCharacterWipe(void);
void FUN_020299f4(void);
void FUN_02029ab0(void);
void EnterBigBoosHaunt(void);
void HitDeathPlane(int arg);
void ExitLevel(void);
void SetNextLevel(int arg);
void SetNextStar(void);
void DeathTable_SetBit(s32 id);
u32 DeathTable_GetBit(s32 id);
void GiveVsStars(int idx, int delta);
void ClearSpikeBomb(int idx);
void SetStarMarker(int i, int v1, int v2);
void CollectStarInCurLevel(s32 starID);
s32 IsStarCollectedInCurLevel(s32 starID);
signed char NumRedCoins(void);
int GiveRedCoins(int i, int amt);
short NumCoins(void);
void GiveCoins(int idx, int amount);
void InitControllerMode(int idx, u8 val);
int func_0202a8e0(int a, unsigned char b);
int func_0202a958(void);
int func_0202a96c(void);
int func_0202a980(void);
void HideArea(int idx);
void ShowArea(int idx);
unsigned char IsAreaShowing(int idx);
void LoadLevel(int levelID, u32 entranceID, int starID, u32 arg3, s8 returnState);
void StartMinigameMenu(u8 returnToRecRoom);
void PrepareVsMode(void);
int StartWithFarCamera(void);
unsigned char GetStarCameraSetting(int idx);
int GetMinimapScale(int idx);
void func_0202af80(int a, int b);
int GetMinimapID(struct Obj *obj, int arg);
void func_0202b044(int a, int b);
void func_0202b060(int a, int b);
struct ObjTeleport *GetTeleportDestObj(int i);
void func_0202b090(int a, int b);
void func_0202b0c4(int a, int b);
void func_0202b0e0(int a, int b);
void SetTouchScreenDelay(void);
void ResetInput(void);
int IsLevelTinyHugeIslandOutside(int level);
int IsLevelInsideCastle(int level);
int GetSoundGroupID(int group, int sub);
void func_0202deac(int levelID);
s32 GetLevelPart(s32 level);

/* Defined elsewhere. */
void OpenCannonInLevel(s8 levelID);
int IsCannonOpenInLevel(s8 levelID);
void CollectStarInLevel(s8 levelID, s32 starID);
int IsStarCollectedInLevel(s8 levelID, s32 starID);
void func_ov002_020b18f0(int a);
}
void UnloadLevelOverlays(int levelID);
void LoadLevelOverlays(int level);
void LoadOrUnloadObjectOverlays(void (*fn)(int), int levelID);

// Outside extern "C" so the namespace mangles to _ZN8Particle9RenderAllEv.
namespace Particle {
void RenderAll();
}

// The Sound entry points InitResources and CleanupResources call that no
// header declares yet.
namespace Sound {
void ResetPlayerVoiceGroup();
void LoadGroupAndSetBank(int group, int bank);
void SetMusic(u32 player, u32 music);
}

#define REG16(a) (*(volatile u16 *)(a))
#define REG32(a) (*(volatile u32 *)(a))
/* The TouchInfo record at byte offset off (slot * 4) from gTouchHeld. */
#define TOUCHP(off) ((TouchInfo *)(gTouchHeld + (off)))

#pragma defer_codegen off

// @symbol _ZN10dScStage_cD1Ev, _ZN10dScStage_cD0Ev
/* Destroy through dScStage_c's own three fields -- dBgW_Kc at 0x91c, dExtSimpleModel_c at
 * 0x86c, Particle::SysTracker at 0x50 -- in reverse declaration order, then
 * dScene_c (now inline-defined in dScene_c.h, so its vptr store and dBase_c's
 * inline in turn), then fBase_c's subobject destructor, called rather than
 * inlined because fBase_c's is not. The deleting variant destroys the same
 * chain and then hands the object back through dScene_c's inline operator
 * delete -- declared there rather than on dBase_c, because mwcc only inlines
 * it when found on the class itself or its IMMEDIATE base, which for
 * dScStage_c is dScene_c. One definition emits both. */
dScStage_c::~dScStage_c() {}

// @symbol _ZN10dScStage_c9VE_UpdateEv
void dScStage_c::VE_Update() {
    u8 t = data_0209f244;
    u8 t2;
    u8 s;
    if (t != 0) {
        data_0209f244 = t - data_0208ee44;
        if (data_0209f244 == 0) {
            dScStage_c::UpdateMenuButtons(0);
        }
    }
    t2 = data_0209f22c;
    if (t2 != 0) {
        data_0209f22c = t2 - data_0208ee44;
        return;
    }
    s = data_0209f290;
    switch (s) {
    case 0: {
        u8 idx = gActivePlayerSlot;
        int ok = 0;
        int off = idx * 4;
        u8 idx2;
        int off2;
        u8 a;
        if (gTouchHeld[idx * 4]) {
            if (gTouchEdge[off])
                ok = 1;
        }
        if (ok == 0) {
            if (IsButtonInputValid() == 0)
                return;
        }
        idx2 = gActivePlayerSlot;
        a = gTouchX[idx2 * 4];
        off2 = idx2 * 4;
        if (((u8)(a - 8) < 0xf0 && (u8)(gTouchY[off2] - 0x38) < 0x20) ||
            IsButtonInputValid() != 0) {
            if (data_0209f2e0 == 0) {
                data_0209f244 = data_0208ee44 << 2;
            }
            data_0209f2e0 = 0;
            data_0209f22c = data_0208ee44 << 3;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f290 = 1;
            data_ov002_02111150 = 1;
            func_02012790(0x9a);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0xb0)
                return;
            return;
        } else {
            u8 idx3 = gActivePlayerSlot;
            int off3 = idx3 * 4;
            if ((u8)(gTouchX[idx3 * 4] - 8) >= 0xf0)
                return;
            if ((u8)(gTouchY[off3] - 0x68) >= 0x20)
                return;
            if (data_0209f2e0 == 1) {
                data_0209f244 = data_0208ee44 << 2;
            }
            data_0209f2e0 = 1;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f290 = 1;
            data_ov002_02111150 = 1;
            func_02012790(0x9b);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 1: {
        if (data_0209f2e0 == 0) {
            dScene_c::StartSceneFade(6, 1, 0);
        } else {
            dScene_c::StartSceneFade(1, 0, 0);
        }
        data_0209f290 = 2;
        data_0209d45c &= ~4;
        data_0209d454 &= ~3;
        *(u16 *)0x04000050 = 0;
        *(u16 *)0x04001050 = 0;
        break;
    }
    }
}
// @symbol _ZN10dScStage_c7VE_InitEv
void dScStage_c::VE_Init() {
    unsigned int a;
    unsigned int b;

    a = data_0209d45c ^ 0x10;
    b = data_0209d454 ^ 0x10;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x1100;
    *(volatile unsigned short *)0x400000c = (*(volatile unsigned short *)0x400000c & 0x43) | 0x1010;
    Message::DisplayVsExitText(0x1f);
    data_0209f360[0] = 0xe0;
    data_0209f360[1] = 0x1a0;
    data_0209f2b4 = 2;
    data_0209f2e0 = 0;
    dScStage_c::UpdateMenuButtons(true);
    SetBg2Offset(0, 0);
    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    data_0209d45c |= 4;
    data_0209d454 |= 3;
    data_0209b454 |= 0x40000000;
    data_0209b464 = data_0209b454;
    data_0209f294 = 1;
    data_0209f290 = 0;
    G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, a | 0x20, -7);
    G2x::SetBlendBrightness((volatile unsigned short *)0x4001050, b | 0x20, -7);
}
// @symbol _ZN10dScStage_c20RenderBouncingArrowsEv
void dScStage_c::RenderBouncingArrows() {
    int r4;
    unsigned char A;
    if (data_0208ee44 == 1) {
        r4 = (data_020a0db0 & 0x10) ? 0xae : 0xb0;
    } else {
        r4 = (data_020a0db0 & 8) ? 0xae : 0xb0;
    }
    A = data_0209f2c4;
    if ((A == 0 && data_0209f284 != 0 && (int)(data_0209f2d8 == 1) == 0) ||
        (A != 0 && (unsigned char)(data_0209f248 + 0xf7) <= 2u)) {
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0x40, r4, -1, -1, 0x1000, 0x1000, 0, -1);
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0x80, r4, -1, -1, 0x1000, 0x1000, 0, -1);
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0xc0, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    } else {
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0xc, r4, -1, -1, 0x1000, 0x1000, 0, -1);
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0xf4, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    }
}
// @symbol _ZN10dScStage_c9LC_RenderEv
void dScStage_c::LC_Render() {
    data_0209f2a8 = data_0209f2a8 + data_0208ee44;
    if (data_0209f2a8 >= 0xc)
        data_0209f2a8 = 0;

    if (data_0209f20c == 0)
        return;

    {
        void *p = (void *)LoadFile(ASSET_D_2D_UPR_COURSE_CLEAR_MESSAGE_NCL_BIN);
        GX::LoadOBJPltt((void *)((char *)p + (((unsigned int)data_0209f2a8 >> 2) << 5)), 0x1c0,
                        0x10);
        Deallocate(p);
    }

    if (SublevelToLevel(data_02092124) >= 0xf && SublevelToLevel(data_02092124) < 0x15) {
        if (GetOwnerLanguage() == 5)
            OAM::Render(0, (OamAttr *)&data_ov002_0210cea8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else if (GetOwnerLanguage() == 4)
            OAM::Render(0, (OamAttr *)&data_ov002_0210cee8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else if (GetOwnerLanguage() == 3)
            OAM::Render(0, (OamAttr *)&data_ov002_0210cf28, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else if (GetOwnerLanguage() == 2)
            OAM::Render(0, (OamAttr *)&data_ov002_0210cf78, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else
            OAM::Render(0, (OamAttr *)&data_ov002_0210cb14, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0,
                        -1);
    }

    if (data_0209f2b0 != 0) {
        if (data_0209f20c != 0 && SublevelToLevel(data_02092124) >= 0xf) {
            if (GetOwnerLanguage() == 5)
                OAM::Render(0, (OamAttr *)&data_ov002_0210ce90, 0x80, 0x70, -1, -1, 0x1000, 0x1000,
                            0, -1);
            else if (GetOwnerLanguage() == 4)
                OAM::Render(0, (OamAttr *)&data_ov002_0210ced0, 0x80, 0x70, -1, -1, 0x1000, 0x1000,
                            0, -1);
            else if (GetOwnerLanguage() == 3)
                OAM::Render(0, (OamAttr *)&data_ov002_0210cf08, 0x80, 0x70, -1, -1, 0x1000, 0x1000,
                            0, -1);
            else if (GetOwnerLanguage() == 2)
                OAM::Render(0, (OamAttr *)&data_ov002_0210cfa0, 0x80, 0x70, -1, -1, 0x1000, 0x1000,
                            0, -1);
            else
                OAM::Render(0, (OamAttr *)&data_ov002_0210caf4, 0x80, 0x70, -1, -1, 0x1000, 0x1000,
                            0, -1);
        } else {
            if (GetOwnerLanguage() == 5)
                OAM::Render(0, (OamAttr *)&data_ov002_0210ce90, 0x80, 0x80, -1, -1, 0x1000, 0x1000,
                            0, -1);
            else if (GetOwnerLanguage() == 4)
                OAM::Render(0, (OamAttr *)&data_ov002_0210ced0, 0x80, 0x80, -1, -1, 0x1000, 0x1000,
                            0, -1);
            else if (GetOwnerLanguage() == 3)
                OAM::Render(0, (OamAttr *)&data_ov002_0210cf08, 0x80, 0x80, -1, -1, 0x1000, 0x1000,
                            0, -1);
            else if (GetOwnerLanguage() == 2)
                OAM::Render(0, (OamAttr *)&data_ov002_0210cfa0, 0x80, 0x80, -1, -1, 0x1000, 0x1000,
                            0, -1);
            else
                OAM::Render(0, (OamAttr *)&data_ov002_0210caf4, 0x80, 0x80, -1, -1, 0x1000, 0x1000,
                            0, -1);
        }
    }

    if (data_0209f2d4 == 3)
        RenderBouncingArrows();
}
// @symbol _ZN10dScStage_c9LC_UpdateEv
void dScStage_c::LC_Update() {
    if (data_0209f244 != 0) {
        data_0209f244 = data_0209f244 - data_0208ee44;
        if (data_0209f244 == 0)
            dScStage_c::UpdateMenuButtons(0);
    }
    if (data_0209f22c != 0) {
        data_0209f22c = data_0209f22c - data_0208ee44;
        return;
    }

    switch (data_0209f2d4) {
    case 0:
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        Message::DisplayLevelClearText(0x27a, SublevelToLevel(data_02092124));
        data_0209f360[0] = 0xa0;
        data_0209f360[1] = 0x140;
        data_0209f360[2] = 0x1e0;
        data_0209f2b4 = 3;
        data_0209f2b0 = 0;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        SetBg3Offset(0, 0);
        data_0209d45c |= 8;
        data_0209f260 = 0;
        data_0209f2d4 = 1;
        data_0209f280 = 0;
        if (data_0209f358[0] != 0)
            return;
        if (data_0209f2ac != 0) {
            Sound::PlaySub(0x23, 0x14, 0x7f, 0x15666, 0);
            data_0209f280 = 1;
        }
        data_0209f22c = 0x1e;
        return;
    case 1: {
        int level = SublevelToLevel(data_02092124);
        if (data_0209f358[0] > data_0209f260) {
            data_0209f260 += 1;
            if (data_0209f260 % 50 == 0) {
                GiveLives(1);
                Sound::PlayBank3_2D(0x6e);
            }
            if (data_0209f358[0] == data_0209f260) {
                if (data_0209f2ac != 0) {
                    Sound::PlaySub(0x23, 0x14, 0x7f, 0x15666, 0);
                    data_0209f280 = 1;
                }
                data_0209f22c = 0x1e;
                if (level < 0xf) {
                    u8 coins = data_0209f260;
                    if (SaveData::GetCoinRecord(level) >= coins)
                        return;
                    SaveData::SetCoinRecordIfHigher(level, coins);
                    data_0209f2b0 = 1;
                    func_02012790(0x22);
                    return;
                }
                if (level < 0xf)
                    return;
                if (level >= 0x15)
                    return;
                func_02012790(0x22);
                Sound::PlaySub(0x23, 0x14, 0x7f, 0x15666, 0);
                data_0209f280 = 1;
                return;
            }
            func_02012790(0x15);
            return;
        }
        if (data_0209f260 == 0 && level >= 0xf && level < 0x15) {
            Sound::PlaySub(0x23, 0x14, 0x7f, 0x15666, 0);
            data_0209f280 = 1;
        }
        data_0209f22c = 0x3c;
        data_0209f358[0] = 0;
        data_0209f2d4 = 2;
        return;
    }
    case 2: {
        int t6;
        int t5;
        if (SublevelToLevel(data_0209f2f8) == 0x1d) {
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1210;
        } else {
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        }
        data_0209f358[0] = 0;
        t6 = data_0209d45c ^ 0x18;
        t5 = data_0209d454 ^ 0x10;
        SetBg2Offset(0, 0);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209d45c |= 4;
        data_0209d454 |= 3;
        G2x::SetBlendBrightness((volatile u16 *)0x4000050, t6 | 0x20, -7);
        G2x::SetBlendBrightness((volatile u16 *)0x4001050, t5 | 0x20, -7);
        data_0209f2d4 = 3;
        return;
    }
    case 3: {
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x28) < 0x20) ||
                (data_0209f2e0 == 0 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 0)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 0;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                func_02012790(0x57);
                data_0209f2d4 = 4;
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x50) < 0x20) ||
                (data_0209f2e0 == 1 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 1)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 1;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                func_02012790(0x58);
                data_0209f2d4 = 4;
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (!((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x78) < 0x20) &&
                (data_0209f2e0 != 2 || IsButtonInputValid() == 0))
                return;
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            func_02012790(0x59);
            data_0209f2d4 = 6;
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 4:
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xe00;
        data_0209f2d4 = 5;
        data_0209d45c &= ~4;
        Message::DisplaySaveStatusText(0x295);
        data_0209f2b8 = 0x78;
        return;
    case 5:
        if (data_0209f2b8 == 0)
            return;
        if (data_0209f2b8 == 0x78) {
            SaveData::SaveCurrentFile();
        } else if (data_0209f2b8 == 0x3c) {
            Message::DisplaySaveStatusText(0x296);
        }
        data_0209f2b8 = data_0209f2b8 - data_0208ee44;
        if (data_0209f2b8 == 0)
            data_0209f2d4 = 6;
        return;
    case 6:
        if (data_0209f280 != 0) {
            Sound::PlaySub(0x23, 0x7f, 0, 0x15666, 0);
            data_0209f280 = 0;
        }
        data_0209d45c &= ~0xc;
        data_0209d454 &= ~3;
        REG16(0x4000050) = 0;
        if (data_0209f2e0 != 1) {
            data_ov002_02111178 = 5;
        } else {
            dScene_c::StartSceneFade(1, 0, 0);
            data_ov002_02111178 = 0;
            Sound::StopLoadedMusic_Layer1(0xa);
            {
                int t = data_0209b454 | 0x40000000;
                data_0209b454 = t;
                data_0209b464 = t;
            }
        }
        data_0209f2e0 = 0;
        data_0209f2d4 = 0;
        data_0209f2b0 = 0;
        data_0209f20c = 0;
        return;
    default:
        return;
    }
}
// @symbol _ZN10dScStage_c16CheckCameraInputEv
#pragma opt_strength_reduction off
void dScStage_c::CheckCameraInput() {
    CamInput *cam = (CamInput *)data_0209f498;
    if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0 && data_0209d660 == 0) {
        int i = 0;
        int n1 = data_0209f21c;
        if (n1 > 0) {
            u8 *st = data_0209f350;
            do {
                cam->pressed = 0;
                cam->held = 0;
                if (((TouchInfo *)gTouchHeld)[i].held != 0) {
                    if (((dCamera_c *)data_0209f318)->mFlags & 0x1000) {
                        // cam->mode here builds 32 bytes shorter; the indexed read is the ROM shape.
                        u8 state = ((CamInput *)data_0209f498)[i].mode;
                        if ((state == 0 && ((((TouchInfo *)gTouchHeld)[i].x <= 0x58 &&
                                             ((TouchInfo *)gTouchHeld)[i].y >= 0x8a) ||
                                            (((TouchInfo *)gTouchHeld)[i].x >= 0xa7 &&
                                             ((TouchInfo *)gTouchHeld)[i].y >= 0x8a))) ||
                            (state == 2 && ((TouchInfo *)gTouchHeld)[i].x >= 0xa7 &&
                             ((TouchInfo *)gTouchHeld)[i].y >= 0x9a)) {
                            if (((((TouchInfo *)gTouchHeld)[i].held &&
                                  ((TouchInfo *)gTouchHeld)[i].edge)
                                     ? 1
                                     : 0) ||
                                *st == 1) {
                                u16 mask;
                                if ((state == 0 && ((TouchInfo *)gTouchHeld)[i].x < 0x2b) ||
                                    (((TouchInfo *)gTouchHeld)[i].x >= 0xa7 &&
                                     ((TouchInfo *)gTouchHeld)[i].x < 0xd1))
                                    mask = 0x200;
                                else if ((state == 0 && ((TouchInfo *)gTouchHeld)[i].x >= 0x30 &&
                                          ((TouchInfo *)gTouchHeld)[i].x <= 0x58) ||
                                         ((TouchInfo *)gTouchHeld)[i].x >= 0xd6)
                                    mask = 0x100;
                                else {
                                    mask = data_0209f368[i];
                                    if (mask == 0)
                                        mask = 0x200;
                                }
                                if ((((TouchInfo *)gTouchHeld)[i].held &&
                                     ((TouchInfo *)gTouchHeld)[i].edge)
                                        ? 1
                                        : 0)
                                    cam->pressed |= mask;
                                else
                                    cam->pressed |= mask & (mask ^ data_0209f368[i]);
                                data_0209f368[i] = mask;
                                cam->held |= mask;
                                *st = 1;
                            }
                        } else if (*st == 1) {
                            *st = 0;
                            data_0209f368[i] = 0;
                        }
                    }
                } else {
                    if (*st == 1)
                        *st = 0;
                    data_0209f368[i] = 0;
                }
                {
                    u8 state2 = ((CamInput *)data_0209f498)[i].mode;
                    if (state2 != 0) {
                        if (((((TouchInfo *)gTouchHeld)[i].held &&
                              ((TouchInfo *)gTouchHeld)[i].edge)
                                 ? 1
                                 : 0) &&
                            ((state2 == 1 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd7 &&
                              ((TouchInfo *)gTouchHeld)[i].y >= 0x8d) ||
                             (state2 == 2 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd7 &&
                              ((TouchInfo *)gTouchHeld)[i].y >= 0x73 &&
                              ((TouchInfo *)gTouchHeld)[i].y < 0x97))) {
                            data_ov002_02111180 = 0x10;
                            cam->pressed |= 0x8000;
                            *st = 2;
                            cam->touching = 0;
                        } else if (*st == 2) {
                            if ((state2 == 1 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd5 &&
                                 ((TouchInfo *)gTouchHeld)[i].y >= 0x8d) ||
                                (state2 == 2 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd5 &&
                                 ((TouchInfo *)gTouchHeld)[i].y >= 0x73 &&
                                 ((TouchInfo *)gTouchHeld)[i].y < 0x97)) {
                                cam->touching = 0;
                            } else {
                                *st = 0;
                            }
                        }
                    }
                }
                i++;
                cam++;
                st++;
            } while (i < data_0209f21c);
        }
    } else {
        int i = 0;
        u8 *st;
        int n = data_0209f21c;
        if (n > 0) {
            st = data_0209f350;
            do {
                cam->pressed = 0;
                cam->held = 0;
                *st = 0;
                i++;
                cam++;
                st++;
            } while (i < n);
        }
    }
}
#pragma opt_strength_reduction on
// @symbol _ZN10dScStage_c12RenderNumberEhiibi
void dScStage_c::RenderNumber(unsigned char num, int x, int y, bool b, int p5) {
    int leading = 0;
    Matrix2x2 *mtx = 0;
    int i;

    for (i = 0; i < 3; i++) {
        int digit = __aeabi_idiv(num, data_020755b8[i]);
        if (digit != 0 || leading != 0 || i == 2) {
            if (b == 0) {
                OAM::Render(true, OAM::NUMBERS[digit], x, y, p5, -1, mtx);
            } else {
                OAM::Render(false, OAM::NUMBERS[digit], x, y, p5, -1, 0);
            }
            leading = 1;
            x += 9;
        }
        num = num % data_020755b8[i];
    }
}
// @symbol _ZN10dScStage_c9PS_RenderEv
void dScStage_c::PS_Render() {
    if (func_02029408() != 0 && data_02092144[data_0209f250] != 0 && data_0209f248 != 0xb &&
        data_0209f248 != 0xe && data_0209f248 != 0xf &&
        ((unsigned int)data_0209f248 < 7U || (unsigned int)data_0209f248 > 8U)) {
        dScStage_c::RenderBouncingArrows();
    }

    int r0v = (data_0209f2d8 == 1);
    if (r0v != 0 && data_0209fc68 == 0) {
        OAM::Render(0, &OAM::VS_PAUSE, 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
    } else if (data_0209f248 == 1 ||
               (SublevelToLevel(data_0209f2f8) == 0x1d &&
                ((unsigned int)data_0209f248 < 7U || (unsigned int)data_0209f248 > 8U))) {
        int var_sl;
        unsigned char var_sb;
        unsigned char var_r8;
        int var_r7;
        int sp18;
        var_r7 = 0;
        sp18 = 0;
        var_r8 = CountStarsCollectedInLevelToDisplay(data_0209f2c8);
        if (IsStarCollected(data_0209f2c8, 0) != 0) {
            var_r8 -= 1;
        }
        if (data_0209f248 == 1 && data_0209f2f0 == 0) {
            if (data_0209f238 == 1) {
                OAM::Render(0, &OAM::ARROW_POINTING_LEFT, 0x12, 0x60, -1, -1, 0x1000, 0x1000, 0,
                            -1);
                OAM::RenderSub(&OAM::ARROW_POINTING_LEFT, 0x12, 0x10);
            } else {
                OAM::Render(0, &OAM::ARROW_POINTING_LEFT, 0x14, 0x60, -1, -1, 0x1000, 0x1000, 0,
                            -1);
                OAM::RenderSub(&OAM::ARROW_POINTING_LEFT, 0x14, 0x10);
            }
            if (data_0209f238 == 2) {
                OAM::Render(0, &OAM::ARROW_POINTING_RIGHT, 0xee, 0x60, -1, -1, 0x1000, 0x1000, 0,
                            -1);
                OAM::RenderSub(&OAM::ARROW_POINTING_RIGHT, 0xee, 0x10);
            } else {
                OAM::Render(0, &OAM::ARROW_POINTING_RIGHT, 0xec, 0x60, -1, -1, 0x1000, 0x1000, 0,
                            -1);
                OAM::RenderSub(&OAM::ARROW_POINTING_RIGHT, 0xec, 0x10);
            }
        }
        if (data_0209f2c8 != 0xf) {
            var_sb = 0x50;
            var_sl = 1;
            if (var_r8 != 0) {
                do {
                    if (IsStarCollected(data_0209f2c8, var_sl) != 0) {
                        OAM::Render(false, &OAM::MM_SMALL_STAR, var_sb, 0x60, -1, -1, 0);
                        var_r7 = (var_r7 + 1) & 0xff;
                    } else {
                        OAM::Render(false, &OAM::SMALL_STAR_EMPTY, var_sb, 0x60, -1, -1, 0);
                        sp18 = 1;
                    }
                    var_sb += 0x10;
                    var_sl += 1;
                } while (var_r7 != var_r8);
            }
            if (sp18 == 0 && var_r8 != 7) {
                OAM::Render(false, &OAM::SMALL_STAR_EMPTY, var_sb, 0x60, -1, -1, 0);
            }
            unsigned char r5v = SaveData::GetCoinRecord(data_0209f2c8);
            OAM::Render(false, &OAM::COIN, 0x70, 0x70, -1, -1, 0);
            OAM::Render(false, &OAM::TIMES, 0x80, 0x78, -1, -1, 0);
            RenderNumber(r5v, 0x88, 0x70, 1, 8);
            if (IsStarCollected(data_0209f2c8, 0) != 0) {
                OAM::Render(false, &OAM::MM_SMALL_STAR, 0xb0, 0x78, -1, -1, 0);
            }
        } else {
            OAM::Render(false, &OAM::MM_SMALL_STAR, 0x70, 0x60, -1, -1, 0);
            OAM::Render(false, &OAM::TIMES, 0x80, 0x60, -1, -1, 0);
            RenderNumber(CountStarsCollectedInLevelToDisplay(0x1d), 0x88, 0x58, 1, 8);
        }
    } else if (data_0209d45c & 8) {
        if ((unsigned int)data_0209f2c8 < 0xfU) {
            unsigned char r4v = CountStarsCollectedInLevelToDisplay(data_0209f2c8);
            OAM::Render(false, &OAM::MM_SMALL_STAR, 0x58, 0x8c, -1, -1, 0);
            OAM::Render(false, &OAM::TIMES, 0x68, 0x8c, -1, -1, 0);
            RenderNumber(r4v, 0x70, 0x84, 1, 1);
            unsigned char r4v2 = SaveData::GetCoinRecord(data_0209f2c8);
            OAM::Render(false, &OAM::COIN, 0x88, 0x84, -1, -1, 0);
            OAM::Render(false, &OAM::TIMES, 0x98, 0x8c, -1, -1, 0);
            RenderNumber(r4v2, 0xa0, 0x84, 1, 1);
        }
        if (data_0209f248 == 0xb) {
            OAM::RenderSub(&OAM::PAUSE, 0x80, 0x60);
        }
    }

    unsigned char f248b = data_0209f248;
    if ((unsigned char)(f248b + 0xf9) <= 1) {
        if (data_0209f2e0 == 3) {
            if (data_0209f29c == 0) {
                if (GetOwnerLanguage() == 5) {
                    OAM::Render(true, &OAM::TINY_STAR, 0x46, ((data_0209f2e0 * 5) + 6) * 8, -1, -1,
                                0);
                } else if (GetOwnerLanguage() == 3) {
                    OAM::Render(true, &OAM::TINY_STAR, 0x4e, ((data_0209f2e0 * 5) + 6) * 8, -1, -1,
                                0);
                } else {
                    OAM::Render(true, &OAM::TINY_STAR, 0x56, ((data_0209f2e0 * 5) + 6) * 8, -1, -1,
                                0);
                }
            } else {
                OAM::Render(true, &OAM::TINY_STAR, 0xce, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
            }
        } else {
            OAM::Render(true, &OAM::TINY_STAR, 0x26, ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
        }
        if (GetOwnerLanguage() == 5) {
            OAM::Render(0, (OamAttr *)data_ov001_020abd98[data_0209f2dc], 0x80, 0x60, -1, -1,
                        0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 4) {
            OAM::Render(0, (OamAttr *)data_ov001_020ac218[data_0209f2dc], 0x80, 0x60, -1, -1,
                        0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 3) {
            OAM::Render(0, (OamAttr *)data_ov001_020ac64c[data_0209f2dc], 0x80, 0x60, -1, -1,
                        0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 2) {
            OAM::Render(0, (OamAttr *)data_ov001_020acb68[data_0209f2dc], 0x80, 0x60, -1, -1,
                        0x1000, 0x1000, 0, -1);
            return;
        }
        OAM::Render(0, OAM::CONTROLLER_MODE_TEXTS[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000,
                    0x1000, 0, -1);
        return;
    }

    if (f248b != 0xa) {
        return;
    }

    if (data_0209f2e0 == 2) {
        if (data_0209f29c == 0) {
            if (GetOwnerLanguage() == 5) {
                OAM::Render(true, &OAM::TINY_STAR, 0x46, 0xa8, -1, -1, 0);
            } else if (GetOwnerLanguage() == 3) {
                OAM::Render(true, &OAM::TINY_STAR, 0x4e, 0xa8, -1, -1, 0);
            } else {
                OAM::Render(true, &OAM::TINY_STAR, 0x56, 0xa8, -1, -1, 0);
            }
        } else {
            OAM::Render(true, &OAM::TINY_STAR, 0xce, 0xa8, -1, -1, 0);
        }
    } else {
        OAM::Render(true, &OAM::TINY_STAR, 0xe, ((data_0209f2e0 * 6) + 7) * 8, -1, -1, 0);
    }

    if (data_0209f238 == 2) {
        OAM::Render(true, &OAM::SMALL_ARROW_POINTING_LEFT, 0x62, 0x38, -1, -1, 0);
    } else {
        OAM::Render(true, &OAM::SMALL_ARROW_POINTING_LEFT, 0x64, 0x38, -1, -1, 0);
    }

    if (data_0209f238 == 1) {
        OAM::Render(true, &OAM::SMALL_ARROW_POINTING_RIGHT, 0xf6, 0x38, -1, -1, 0);
        return;
    }
    OAM::Render(true, &OAM::SMALL_ARROW_POINTING_RIGHT, 0xf4, 0x38, -1, -1, 0);
}
// @symbol _ZN10dScStage_c25PS_UpdateOkAndBackButtonsEb
/* Lever (notes 6au): TWO coupled changes that only work together.
 *   1. the array object is declared `extern volatile int data_02075610[];`
 *   2. there is NO named pointer local -- the array is indexed INLINE in the loop bound.
 * A named pointer local puts the base in mwcc's address-constant class, which is colored
 * after every normal web and lands it in r7; that alone was the whole 9-word miss, and it
 * dominates even if the pointer is itself volatile. A plain inline access rematerializes
 * the base in normal birth order but into a scratch register and caches the bound, which
 * drops the tail reload and comes out three words SHORT. The volatile forces the bound to
 * be re-read every iteration (the ROM reloads at +0xc0 and +0x114), which keeps the base
 * live across the inner loop so it must take a callee-saved register -- but via the
 * normal-birth path, not the address-constant class. Result ptr r5 / 0x1000 r6 / idx r7,
 * exactly the ROM. Do NOT reintroduce a named pointer and do NOT drop the volatile;
 * either one alone regresses. Link-verified: VERIFIED, 0 diffs, 0 blind.
 */
void dScStage_c::PS_UpdateOkAndBackButtons(bool b) {
    int sl;
    int i;
    int base;
    int data_0x1000;
    int idx;
    int data_0x2000;
    int j;

    if (GetOwnerLanguage() == 5 || GetOwnerLanguage() == 3)
        base = 2;
    else
        base = 0;

    for (i = 0; i < 2; i++) {
        data_0x2000 = 0x2000;
        data_0x1000 = 0x1000;

        if (b) {
            sl = data_0x1000;
        } else {
            int v;
            if (data_0209f2b4 == data_0209f2e0 && data_0209f29c == i && data_0209f244 == 0)
                v = data_0x2000;
            else
                v = data_0x1000;
            sl = (u16)v;
        }

        idx = i + base;
        u16 *scr = (u16 *)G2S::GetBG1ScrPtr() + data_020755cc[idx];
        for (j = 0; j < data_02075610[idx]; j++) {
            scr[0] = (scr[0] & 0x3ff) | sl;
            scr[0x20] = (scr[0x20] & 0x3ff) | sl;
            scr[0x40] = (scr[0x40] & 0x3ff) | sl;
            scr[0x60] = (scr[0x60] & 0x3ff) | sl;
            scr++;
        }
    }
}
// @symbol _ZN10dScStage_c17PS_UpdateSaveMenuEb
void dScStage_c::PS_UpdateSaveMenu(bool b) {
    u16 v;
    int i;
    for (i = 0; i < 2; i++) {
        int j;
        u16 *p;
        if (b != 0) {
            v = 0x1000;
        } else {
            unsigned int t;
            if (data_0209f2e0 == i && data_0209f244 == 0)
                t = 0x2000;
            else
                t = 0x1000;
            v = (u16)t;
        }
        p = (u16 *)G2S::GetBG1ScrPtr() + data_020755c8[i];
        for (j = 0; j < 10; j++) {
            p[0] = (p[0] & 0x3ff) | v;
            p[0x20] = (p[0x20] & 0x3ff) | v;
            p[0x40] = (p[0x40] & 0x3ff) | v;
            p[0x60] = (p[0x60] & 0x3ff) | v;
            p++;
        }
    }
}
// @symbol _ZN10dScStage_c20PS_UpdateOptionsMenuEv
void dScStage_c::PS_UpdateOptionsMenu() {
    int i;
    int j;
    int counter;
    u16 sel;
    u16 *scr;
    u16 v;

    sel = (data_0209f2e4 != 0) ? 0x5000 : 0x6000;
    scr = (u16 *)G2S::GetBG1ScrPtr() + 0xae;
    for (j = 0; j < 0xf; j++) {
        scr[0] = (scr[0] & 0x3ff) | sel;
        scr[0x20] = (scr[0x20] & 0x3ff) | sel;
        scr[0x40] = (scr[0x40] & 0x3ff) | sel;
        scr[0x60] = (scr[0x60] & 0x3ff) | sel;
        scr++;
    }

    for (j = 0; j < 2; j++) {
        scr = (u16 *)G2S::GetBG1ScrPtr() + data_020755c0[j];
        v = (data_0209f2e0 == j) ? 0x4000 : 0x3000;
        counter = 0;
        for (; counter < 0xa; counter++) {
            scr[0] = (scr[0] & 0x3ff) | v;
            scr[0x20] = (scr[0x20] & 0x3ff) | v;
            scr++;
        }
    }

    for (i = 0; i < 2; i++) {
        scr = (u16 *)G2S::GetBG1ScrPtr() + data_020755c4[i];
        v = (data_0209f2ec == i && data_0209f2cc == 0) ? 0x6000 : 0x5000;
        counter = 0;
        for (; counter < 7; counter++) {
            scr[0] = (scr[0] & 0x3ff) | v;
            scr[0x20] = (scr[0x20] & 0x3ff) | v;
            scr[0x40] = (scr[0x40] & 0x3ff) | v;
            scr[0x60] = (scr[0x60] & 0x3ff) | v;
            scr++;
        }
    }
}
// @symbol _ZN10dScStage_c17UpdateMenuButtonsEb
void dScStage_c::UpdateMenuButtons(bool b) {
    int i;
    for (i = 0; i < data_0209f2b4; i++) {
        unsigned int v;
        int j;
        u16 *p;
        p = (u16 *)G2S::GetBG1ScrPtr() + data_0209f360[i];
        if (b == 0) {
            if (data_0209f2e0 == i && data_0209f244 == 0) {
                v = 0x2000;
            } else {
                v = 0x1000;
            }
            v = (u16)v;
        } else {
            v = 0x1000;
        }
        if (data_0209f2c4 != 0) {
            u8 t = (u8)(data_0209f248 + 0xf9);
            if (t <= 1) {
                v = (u16)(v + 0x4000);
            }
        }
        for (j = 0; j < 0x20; j++) {
            p[0] = (p[0] & 0x3ff) | v;
            p[0x20] = (p[0x20] & 0x3ff) | v;
            p[0x40] = (p[0x40] & 0x3ff) | v;
            p[0x60] = (p[0x60] & 0x3ff) | v;
            p++;
        }
    }
}
// @symbol _ZN10dScStage_c10PS_CleanupEv
/* Static, like the rest of the PS_ group: restarts the timer, unpauses or
 * stops the music and clears the blend registers, all through globals. */
void dScStage_c::PS_Cleanup() {
    if (data_0209f2a0[0]) {
        data_0209d4c8.StartTimer();
        data_0209f2a0[0] = 0;
    }
    data_0209d45c &= ~0xe;
    data_0209d454 &= ~3;
    if (data_0209f280 == 0)
        Sound::UnpauseMusic();
    else
        Sound::StopLoadedMusic_Layer1(0x3c);
    data_0209f300 = 0xf;
    *(short *)0x4000050 = 0;
    *(short *)0x4001050 = 0;
    data_0209f280 = 0;
    data_0209f2c4 = 0;
}
// Pen-down edge on a held touch. An int-returning inline because the ROM
// materializes this 0/1 before testing it (the controller-mode rows, case 8).
static inline int IsTouchPressed(u8 held, u8 slot) {
    return held != 0 && gTouchEdge[slot * 4] != 0;
}
// @symbol _ZN10dScStage_c9PS_UpdateEv
/* Pause-screen update. Four spellings below are load-bearing for the
 * emitted code: case 3's first row stores data_0209f2c4 (not data_0209f1ec);
 * the backlight-on guard reuses the stale tmpv left by the sound-mode
 * hit-test chain (tmpv = tx - 0x6e, assigned inside that chain's condition,
 * as are ty2 and t6: with opt_common_subs off, hoisting them reorders the
 * code and reloading gTouchY adds four instructions); the first gTouchY check
 * indexes with a fresh gActivePlayerSlot load; and the f238 store sits in the unconditional scope
 * after the displayed-level normalization so the case-1 literal loads keep
 * ROM order. */
#pragma opt_common_subs off
void dScStage_c::PS_Update() {
    register u8 var_sl;
    register u8 var_fp;
    s32 sp0;
    s32 sp4;
    s32 sp8;
    s32 spC;
    s32 sp10;
    s32 var_r0;

    if (data_0209f210 != 0) {
        data_0209f210 = data_0209f210 - data_0208ee44;
        if (data_0209f210 == 0)
            data_0209f238 = 0;
    }
    if (data_0209f2e4 != 0) {
        data_0209f2e4 = data_0209f2e4 - data_0208ee44;
        if (data_0209f2e4 == 0)
            dScStage_c::PS_UpdateOptionsMenu();
    }
    if (data_0209f244 != 0) {
        data_0209f244 = data_0209f244 - data_0208ee44;
        if (data_0209f244 == 0) {
            u8 st = data_0209f248;
            if (st == 8 || st == 0xa) {
                if (data_0209f240 != 0) {
                    data_0209f240 = 0;
                    dScStage_c::PS_UpdateOkAndBackButtons(0);
                } else if (st == 8) {
                    dScStage_c::UpdateMenuButtons(0);
                }
            } else if ((u8)(st + 0xef) <= 1) {
                dScStage_c::PS_UpdateSaveMenu(0);
            } else {
                dScStage_c::UpdateMenuButtons(0);
            }
        }
    }
    if (data_0209f2cc != 0) {
        data_0209f2cc = data_0209f2cc - data_0208ee44;
        if (data_0209f2cc == 0)
            dScStage_c::PS_UpdateOptionsMenu();
    }
    if (data_0209f23c != 0) {
        data_0209f23c = data_0209f23c - data_0208ee44;
        if (data_0209f23c == 0)
            dScStage_c::PS_UpdateOptionsMenu();
    }
    if (data_0209f22c != 0) {
        data_0209f22c = data_0209f22c - data_0208ee44;
        return;
    }

    {
        u8 nxt = data_0209f1ec;
        u8 cur = data_0209f248;
        if (cur != nxt)
            data_0209f248 = nxt;
    }

    switch (data_0209f248) {
    case 0: {
        u8 i;
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xc00;
        REG16(0x400000a) = (REG16(0x400000a) & 0x43) | 0x1210;
        REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        i = 0;
        do {
            if (CountStarsCollectedInLevelToDisplay(i) != 0)
                break;
            i = (u8)(i + 1);
        } while (i < 0xf);
        if (i == 0xf) {
            data_0209f2c8 = i;
            data_0209f2f0 = 1;
        } else {
            if (SublevelToLevel(data_02092124) >= 0xf) {
                data_0209f2c8 = 0xf;
            } else {
                data_0209f2c8 = SublevelToLevel(data_02092124);
            }
            data_0209f2f0 = 0;
        }
        Message::Display(data_020756d0[data_0209f2c8]);
        data_0209f360[0] = 0x80;
        data_0209f360[1] = 0x120;
        data_0209f360[2] = 0x1c0;
        data_0209f360[3] = 0x260;
        data_0209f2b4 = 4;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        data_0209f240 = 0;
        dScStage_c::PS_UpdateOkAndBackButtons(1);
        SetBg1Offset(0, 0);
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209d45c |= 0xe;
        data_0209d454 |= 3;
        data_0209f1ec = 1;
        return;
    }
    case 1: {
        if (data_0209f300 != 0)
            return;
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x20) < 0x20) ||
                (data_0209f2e0 == 0 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 0)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 0;
                data_0209f22c = data_0208ee44 << 3;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f2c4 = 2;
                func_02012790(3);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0xb0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x48) < 0x20) ||
                (data_0209f2e0 == 1 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 1)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 1;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                data_0209f1ec = 7;
                func_02012790(0x53);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x70) < 0x20) ||
                (data_0209f2e0 == 2 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 2)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 2;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                data_0209f1ec = 9;
                func_02012790(0x54);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x98) < 0x20) ||
                (data_0209f2e0 == 3 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 3)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 3;
                data_0209f22c = data_0208ee44 << 3;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f1ec = 0xc;
                func_02012790(0x55);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0xb0)
                    return;
                return;
            }
        }
        if (data_0209f2f0 != 0)
            return;
        {
            register u8 *de8_sb;
            var_sl = 0;
            var_fp = var_sl;
            (void)(u32)&data_0209f238;
            (void)(u32)&gTouchHeld;
            (void)(u32)&data_0209f2c8;
            sp4 = var_sl;
            sp8 = var_sl;
            sp0 = var_sl;
            sp10 = 0x52;
            spC = 0x51;
            do {
                s32 de_off;
                u8 sl2;
                u8 a;
                u8 vx;
                {
                    volatile u8 *pSlot = &gActivePlayerSlot;
                    volatile s32 *psp = &sp0;
                    var_r0 = *psp;
                    sl2 = *pSlot;
                    {
                        register u8 aa = gTouchHeld[sl2 * 4];
                        a = aa;
                    }
                    de_off = sl2 * 4;
                }
                if ((a != 0) && (TOUCHP(de_off)->edge != 0)) {
                    var_r0 = 1;
                }
                if ((var_r0 != 0) && ((u32)(vx = TOUCHP(sl2 * 4)->x) < 0x38U) &&
                    ((u32)TOUCHP(sl2 * 4)->y < 0x20U)) {
                    u8 t;
                    data_0209f2c8 = (u8)(data_0209f2c8 - 1);
                    var_fp = 1;
                    t = data_0209f2c8;
                    if (t == 0xFF)
                        data_0209f2c8 = 0xF;
                    {
                        u8 *p0 = &data_0209f210;
                        u8 *p1 = &data_0209f210;
                        u8 *p = ((u8)(vx & 0xff) < 0x38) ? p0 : p1;
                        data_0209f238 = 1;
                        *p = (u8)(data_0208ee44 * 3);
                    }
                } else {
                    s32 var_r0_2;
                    if ((a != 0) && (TOUCHP(sl2 * 4)->edge != 0)) {
                        var_r0_2 = 1;
                    } else {
                        var_r0_2 = sp4;
                    }
                    if (var_r0_2 != 0) {
                        if (((u32)(u8)(TOUCHP(sl2 * 4)->x - 0xC8) < 0x38U) &&
                            ((u32)TOUCHP(sl2 * 4)->y < 0x20U)) {
                            data_0209f2c8 = (u8)(data_0209f2c8 + 1);
                            data_0209f238 = 2;
                            var_fp = 1;
                            if (data_0209f2c8 == 0x10) {
                                data_0209f2c8 = (u8)sp8;
                            }
                            data_0209f210 = (u8)(data_0208ee44 * 3);
                        }
                    }
                }
                if ((data_0209f2c8 == 0xF) ||
                    (CountStarsCollectedInLevelToDisplay(data_0209f2c8) != 0)) {
                    var_sl = 1;
                    if (data_0209f238 == 1) {
                        func_02012790(spC);
                    } else if (data_0209f238 == 2) {
                        func_02012790(sp10);
                    }
                }
            } while (var_sl == 0);
        }
        if (var_fp == 0)
            return;
        Message::DisplayText(data_020756d0[data_0209f2c8]);
        return;
    }
    case 2: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xc00;
        REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        data_0209f1ec = 3;
        data_0209f2c8 = SublevelToLevel(data_0209f2f8);
        Message::DisplayPauseText(0x277, data_0209f2c8);
        data_0209f360[0] = 0x60;
        data_0209f360[1] = 0x100;
        data_0209f360[2] = 0x1a0;
        data_0209f360[3] = 0x240;
        data_0209f2b4 = 4;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        SetBg3Offset(0, 0);
        data_0209d45c |= 8;
        if (func_02029408() != 0 && data_02092144[data_0209f250] != 0) {
            SetBg2Offset(0, 0);
            SetSubBg0Offset(0, 0);
            SetSubBg1Offset(0, 0);
            data_0209d45c |= 4;
            data_0209d454 |= 3;
            return;
        }
        data_0209f1ec = 0xb;
        return;
    }
    case 3: {
        if (data_0209f300 != 0)
            return;
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x18) < 0x20) ||
                (data_0209f2e0 == 0 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 0)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 0;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                data_0209f2c4 = 2;
                func_02012790(3);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x40) < 0x20) ||
                (data_0209f2e0 == 1 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 1)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 1;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                data_0209f1ec = 7;
                func_02012790(0x53);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x68) < 0x20) ||
                (data_0209f2e0 == 2 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 2)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 2;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                data_0209f1ec = 9;
                func_02012790(0x54);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (!((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x90) < 0x20) &&
                (data_0209f2e0 != 3 || IsButtonInputValid() == 0))
                return;
            if (data_0209f2e0 == 3)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 3;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0x13;
            func_02012790(0x56);
            return;
        }
    }
    case 4: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0x1010;
        data_0209f1ec = 5;
        Message::DisplayPauseTextVS(0);
        data_0209f360[0] = 0xa0;
        data_0209f360[1] = 0x140;
        data_0209f360[2] = 0x1e0;
        data_0209f2b4 = 3;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        if (func_02029408() == 0) {
            data_0209f1ec = 0xb;
            return;
        }
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        data_0209d45c |= 4;
        data_0209d45c &= ~8;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209d454 |= 3;
        return;
    }
    case 5: {
        if (data_0209f300 != 0)
            return;
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x28) < 0x20) ||
                (data_0209f2e0 == 0 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 0)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 0;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                data_0209f2c4 = 2;
                func_02012790(3);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x50) < 0x20) ||
                (data_0209f2e0 == 1 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 1)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 1;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                data_0209f1ec = 0x13;
                data_ov002_02111150 = 1;
                func_02012790(0x56);
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (!((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x78) < 0x20) &&
                (data_0209f2e0 != 2 || IsButtonInputValid() == 0))
                return;
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 6;
            data_ov002_02111150 = 1;
            func_02012790(0x55);
            return;
        }
    }
    case 6: {
        dScene_c::StartSceneFade(1, 0, 0);
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        data_0209f280 = 1;
        dScStage_c::PS_Cleanup();
        return;
    }
    case 7: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xf00;
        if (data_0209f218 != 0) {
            if (GetOwnerLanguage() == 5) {
                LoadCompressedFileAt(0xb001, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 4) {
                LoadCompressedFileAt(0xac01, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 3) {
                LoadCompressedFileAt(0xa801, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 2) {
                LoadCompressedFileAt(0xa401, G2S::GetBG1ScrPtr());
            } else {
                LoadCompressedFileAt(0xa001, G2S::GetBG1ScrPtr());
            }
        } else {
            if (GetOwnerLanguage() == 5) {
                LoadCompressedFileAt(0xb009, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 4) {
                LoadCompressedFileAt(0xac09, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 3) {
                LoadCompressedFileAt(0xa809, G2S::GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 2) {
                LoadCompressedFileAt(0xa409, G2S::GetBG1ScrPtr());
            } else {
                LoadCompressedFileAt(0xa009, G2S::GetBG1ScrPtr());
            }
        }
        data_0209f360[0] = 0x80;
        data_0209f360[1] = 0x120;
        data_0209f360[2] = 0x1c0;
        data_0209f2b4 = 3;
        data_0209f1ec = 8;
        {
            u8 cur = data_0209f4ae[gActivePlayerSlot * 0x18];
            data_0209f2dc = cur;
            data_0209f2e0 = cur;
        }
        dScStage_c::UpdateMenuButtons(0);
        Message::DisplayControllerModeText(0x280);
        REG16(0x400000c) = (((data_0209f2dc + 0xd) << 8) | (REG16(0x400000c) & 0x43)) | 0x10;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        REG32(0x4001010) = 0;
        REG32(0x4001014) = 0;
        data_0209f29c = 0;
        data_0209f240 = 0;
        dScStage_c::PS_UpdateOkAndBackButtons(1);
        SetBg2Offset(0, 0);
        data_0209d45c &= ~2;
        data_0209d45c |= 4;
        data_0209d45c &= ~8;
        data_0209d454 |= 3;
        return;
    }
    case 8: {
        u8 slot;
        u8 ty;
        u8 tx;
        u8 relx;
        u8 firstw;
        int idxa;
        if (data_0209f300 != 0)
            return;
        if (gTouchHeld[gActivePlayerSlot * 4] == 0) {
            if (IsButtonInputValid() == 0)
                return;
        }
        if (GetOwnerLanguage() == 5) {
            slot = gActivePlayerSlot;
            firstw = 0x64;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x44);
        } else if (GetOwnerLanguage() == 3) {
            slot = gActivePlayerSlot;
            firstw = 0x74;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x3c);
        } else {
            slot = gActivePlayerSlot;
            firstw = 0x54;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x4c);
        }
        idxa = slot * 4;
        ty = gTouchY[slot * 4];
        if ((u8)(ty - 0x1e) < 0x24) {
            if ((u8)(ty - 0x20) >= 0x20)
                return;
            if (data_0209f2a4 == 1) {
                int t = 0;
                if (gTouchHeld[idxa] != 0) {
                    if (gTouchEdge[idxa] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 1;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xd10;
            data_0209f244 = data_0208ee44 << 2;
            {
                u8 a = gTouchHeld[slot * 4];
                if (a == 0 || (IsTouchPressed(a, slot) && data_0209f2dc == 0)) {
                    data_0209f2dc = 0;
                    SetControllerMode(0);
                    data_0209f22c = data_0208ee44 << 3;
                    func_02012790(0x116);
                    data_0209f2c4 = 2;
                } else {
                    data_0209f2dc = 0;
                    data_0209f240 = 0;
                    dScStage_c::PS_UpdateOkAndBackButtons(0);
                    func_02012790(0x5b);
                }
                data_0209f2e0 = 0;
                dScStage_c::UpdateMenuButtons(0);
                return;
            }
        } else if ((u8)(ty - 0x46) < 0x24) {
            if ((u8)(ty - 0x48) >= 0x20)
                return;
            if (data_0209f2a4 == 2) {
                int t = 0;
                if (gTouchHeld[idxa] != 0) {
                    if (gTouchEdge[idxa] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 2;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xe10;
            data_0209f244 = data_0208ee44 << 2;
            {
                u8 a = gTouchHeld[slot * 4];
                if (a == 0 || (IsTouchPressed(a, slot) && data_0209f2dc == 1)) {
                    data_0209f2dc = 1;
                    SetControllerMode(1);
                    data_0209f22c = data_0208ee44 << 3;
                    func_02012790(0x116);
                    data_0209f2c4 = 2;
                } else {
                    data_0209f2dc = 1;
                    data_0209f240 = 0;
                    dScStage_c::PS_UpdateOkAndBackButtons(0);
                    func_02012790(0x5b);
                }
                data_0209f2e0 = 1;
                dScStage_c::UpdateMenuButtons(0);
                return;
            }
        } else if ((u8)(ty - 0x6e) < 0x24) {
            if ((u8)(ty - 0x70) >= 0x20)
                return;
            if (data_0209f2a4 == 3) {
                int t = 0;
                if (gTouchHeld[idxa] != 0) {
                    if (gTouchEdge[idxa] != 0)
                        t = 1;
                }
                if (t == 0)
                    return;
            }
            data_0209f2a4 = 3;
            REG16(0x400000c) = (REG16(0x400000c) & 0x43) | 0xf10;
            data_0209f244 = data_0208ee44 << 2;
            {
                u8 a = gTouchHeld[slot * 4];
                if (a == 0 || (IsTouchPressed(a, slot) && data_0209f2dc == 2)) {
                    data_0209f2dc = 2;
                    SetControllerMode(2);
                    data_0209f22c = data_0208ee44 << 3;
                    func_02012790(0x116);
                    data_0209f2c4 = 2;
                } else {
                    data_0209f2dc = 2;
                    data_0209f240 = 0;
                    dScStage_c::PS_UpdateOkAndBackButtons(0);
                    func_02012790(0x5b);
                }
                data_0209f2e0 = 2;
                dScStage_c::UpdateMenuButtons(0);
                return;
            }
        } else {
            int t = 0;
            u8 a0 = gTouchHeld[idxa];
            if (a0 != 0) {
                if (gTouchEdge[idxa] != 0)
                    t = 1;
            }
            if ((t != 0 && relx < firstw && (u8)(ty - 0x98) < 0x20) ||
                (data_0209f218 == 0 && IsTouchPressed(a0, slot) && (u8)(tx - 0xd8) < 0x20 &&
                 (u8)(ty - 0x98) < 0x20) ||
                IsButtonInputValid() != 0) {
                if (relx < firstw && (u8)(gTouchY[gActivePlayerSlot * 4] - 0x98) < 0x20 &&
                    IsButtonInputValid() != 0) {
                    data_0209f29c = 0;
                } else {
                    u8 slot2 = gActivePlayerSlot;
                    if ((u8)(gTouchX[slot2 * 4] - 0xd8) < 0x20 &&
                        (u8)(gTouchY[slot2 * 4] - 0x98) < 0x20) {
                        data_0209f29c = 1;
                    }
                }
                data_0209f2e0 = 3;
                SetControllerMode(data_0209f2dc);
                data_0209f240 = 1;
                data_0209f244 = data_0208ee44 << 2;
                dScStage_c::PS_UpdateOkAndBackButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                if (data_0209f29c == 0) {
                    data_0209f2c4 = 2;
                    func_02012790(0x116);
                    return;
                }
                func_02012790(0x5c);
                if (SublevelToLevel(data_0209f2f8) == 0x1d) {
                    data_0209f1ec = 0;
                } else {
                    data_0209f1ec = 2;
                }
                return;
            }
            data_0209f2a4 = 0;
            return;
        }
    }
    case 9: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0x1000;
        data_0209f1ec = 0xa;
        data_0209f234 = GetSoundMode();
        data_0209f2ec = data_0208ee3c ^ 1;
        data_0209f1fc = data_0208ee40;
        data_0209f238 = 0;
        Message::DisplayOptionsMenuText(data_0209f234 + 0x284);
        data_0209f2b4 = 2;
        data_0209f2e0 = 0;
        dScStage_c::PS_UpdateOptionsMenu();
        data_0209f29c = 0;
        data_0209f240 = 0;
        dScStage_c::PS_UpdateOkAndBackButtons(1);
        if (SublevelToLevel(data_0209f2f8) == 0x1d) {
            data_0209d45c &= ~2;
        } else {
            data_0209d45c &= ~4;
        }
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        return;
    }
    case 0xa: {
        u8 slot;
        u8 ty;
        u8 tx;
        u8 relx;
        u8 firstw;
        if (gTouchHeld[gActivePlayerSlot * 4] == 0) {
            if (IsButtonInputValid() == 0)
                return;
        }
        if (GetOwnerLanguage() == 5) {
            slot = gActivePlayerSlot;
            firstw = 0x70;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x40);
        } else if (GetOwnerLanguage() == 3) {
            slot = gActivePlayerSlot;
            firstw = 0x60;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x48);
        } else {
            slot = gActivePlayerSlot;
            firstw = 0x50;
            tx = gTouchX[slot * 4];
            relx = (u8)(tx - 0x50);
        }
        if ((u8)(tx - 0x5a) < 0x14) {
            ty = gTouchY[slot * 4];
            if ((u8)(ty - 0x2e) < 0x14) {
                if ((u8)(tx - 0x5c) >= 0x10)
                    return;
                if ((u8)(ty - 0x30) >= 0x10)
                    return;
                if (data_0209f2a4 == 1) {
                    int t = 0;
                    if (gTouchHeld[slot * 4] != 0) {
                        if (gTouchEdge[slot * 4] != 0)
                            t = 1;
                    }
                    if (t == 0)
                        return;
                }
                data_0209f2a4 = 1;
                data_0209f234 = data_0209f234 - 1;
                if (data_0209f234 == 0xff)
                    data_0209f234 = 2;
                data_0209f238 = 2;
                data_0209f210 = data_0208ee44 * 3;
                data_0209f2e0 = 0;
                Message::DisplayOptionsMenuText(data_0209f234 + 0x284);
                data_0209f2e4 = 8;
                dScStage_c::PS_UpdateOptionsMenu();
                SetSoundMode(data_0209f234);
                func_02012790(0x64);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x5c) < 0x10)
                    return;
                return;
            }
        }
        {
            u8 ty2;
            int t6;
            int tmpv;
            if (((u8)(tx - 0xea) < 0x14 && (u8)((ty2 = gTouchY[slot * 4]) - 0x2e) < 0x14) ||
                ((u8)(tmpv = tx - 0x6e) < 0x7c && (u8)((ty2 = gTouchY[slot * 4]) - 0x26) < 0x24) ||
                ((u8)(t6 = tx - 6) < 0x3c && (u8)((ty2 = gTouchY[slot * 4]) - 0x26) < 0x24)) {
                if (!((u8)(tx - 0xec) < 0x10 && (u8)(ty2 - 0x30) < 0x10) &&
                    !((u8)(tx - 0x70) < 0x78 && (u8)(ty2 - 0x28) < 0x20) &&
                    ((u8)(tx - 8) >= 0x38 || (u8)(ty2 - 0x28) >= 0x20))
                    return;
                if (data_0209f2a4 == 2) {
                    int t = 0;
                    if (gTouchHeld[slot * 4] != 0) {
                        if (gTouchEdge[slot * 4] != 0)
                            t = 1;
                    }
                    if (t == 0)
                        return;
                }
                data_0209f2a4 = 2;
                data_0209f234 = data_0209f234 + 1;
                if (data_0209f234 == 3)
                    data_0209f234 = 0;
                data_0209f238 = 1;
                data_0209f210 = data_0208ee44 * 3;
                data_0209f2e0 = 0;
                SetSoundMode(data_0209f234);
                func_02012790(0x64);
                data_0209f2e4 = 8;
                dScStage_c::PS_UpdateOptionsMenu();
                Message::DisplayOptionsMenuText(data_0209f234 + 0x284);
                {
                    u8 s3 = gActivePlayerSlot;
                    u8 x3 = gTouchX[s3 * 4];
                    if ((u8)(x3 - 0xec) < 0x10) {
                        if ((u8)(gTouchY[s3 * 4] - 0x28) < 0x10)
                            return;
                    }
                    if ((u8)(x3 - 0x70) < 0x78) {
                        if ((u8)(gTouchY[s3 * 4] - 0x20) < 0x20)
                            return;
                    }
                    if ((u8)(x3 - 8) < 0x38)
                        return;
                    return;
                }
            }
            if ((u8)tmpv < 0x3c) {
                u8 ty2b = gTouchY[slot * 4];
                if ((u8)(ty2b - 0x56) < 0x24) {
                    tmpv = tx - 0x70;
                    if ((u8)tmpv >= 0x38)
                        return;
                    tmpv = ty2b - 0x58;
                    if ((u8)tmpv >= 0x20)
                        return;
                    if (data_0209f2a4 == 3) {
                        int t = 0;
                        if (gTouchHeld[slot * 4] != 0) {
                            if (gTouchEdge[slot * 4] != 0)
                                t = 1;
                        }
                        if (t == 0)
                            return;
                    }
                    data_0209f2a4 = 3;
                    if (data_0209f2ec != 0) {
                        func_02012790(0x66);
                        TurnBacklightOn();
                    } else {
                        data_0209f2cc = data_0208ee44 << 2;
                        func_02012790(0x67);
                    }
                    data_0209f2ec = 0;
                    data_0209f2e0 = 1;
                    dScStage_c::PS_UpdateOptionsMenu();
                    if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x70) < 0x38)
                        return;
                    return;
                }
            }
            if ((u8)(tx - 0xae) < 0x3c) {
                u8 ty3 = gTouchY[slot * 4];
                if ((u8)(ty3 - 0x56) < 0x24) {
                    tmpv = tx - 0xb0;
                    if ((u8)tmpv >= 0x38)
                        return;
                    tmpv = ty3 - 0x58;
                    if ((u8)tmpv >= 0x20)
                        return;
                    if (data_0209f2a4 == 4) {
                        int t = 0;
                        if (gTouchHeld[slot * 4] != 0) {
                            if (gTouchEdge[slot * 4] != 0)
                                t = 1;
                        }
                        if (t == 0)
                            return;
                    }
                    data_0209f2a4 = 4;
                    if (data_0209f2ec == 0) {
                        func_02012790(0x66);
                        TurnBacklightOff();
                    } else {
                        data_0209f2cc = data_0208ee44 << 2;
                        func_02012790(0x67);
                    }
                    data_0209f2ec = 1;
                    data_0209f2e0 = 1;
                    dScStage_c::PS_UpdateOptionsMenu();
                    if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0xb0) < 0x38)
                        return;
                    return;
                }
            }
            if ((u8)t6 < 0x4c) {
                u8 ty4 = gTouchY[slot * 4];
                if ((u8)(ty4 - 0x56) < 0x24) {
                    tmpv = tx - 8;
                    if ((u8)tmpv >= 0x48)
                        return;
                    tmpv = ty4 - 0x58;
                    if ((u8)tmpv >= 0x20)
                        return;
                    if (data_0209f2a4 == 5) {
                        int t = 0;
                        if (gTouchHeld[slot * 4] != 0) {
                            if (gTouchEdge[slot * 4] != 0)
                                t = 1;
                        }
                        if (t == 0)
                            return;
                    }
                    data_0209f2a4 = 5;
                    if (data_0209f2e0 != 1) {
                        data_0209f2e0 = 1;
                        dScStage_c::PS_UpdateOptionsMenu();
                    }
                    data_0209f2ec = data_0209f2ec ^ 1;
                    if (data_0209f2ec == 0) {
                        TurnBacklightOn();
                    } else {
                        TurnBacklightOff();
                    }
                    data_0209f2cc = data_0208ee44 << 2;
                    dScStage_c::PS_UpdateOptionsMenu();
                    func_02012790(0x66);
                    return;
                }
            }
            {
                int t = 0;
                if (gTouchHeld[slot * 4] != 0) {
                    if (gTouchEdge[slot * 4] != 0)
                        t = 1;
                }
                if (t == 0 ||
                    !((relx < firstw && (u8)(gTouchY[slot * 4] - 0x98) < 0x20) ||
                      ((u8)(tx - 0xd8) < 0x20 && (u8)(gTouchY[slot * 4] - 0x98) < 0x20))) {
                    if (IsButtonInputValid() == 0)
                        return;
                }
                if ((relx < firstw && (u8)(gTouchY[gActivePlayerSlot * 4] - 0x98) < 0x20) ||
                    IsButtonInputValid() != 0) {
                    data_0209f29c = 0;
                } else {
                    u8 s4 = gActivePlayerSlot;
                    if ((u8)(gTouchX[s4 * 4] - 0xd8) < 0x20 &&
                        (u8)(gTouchY[s4 * 4] - 0x98) < 0x20) {
                        data_0209f29c = 1;
                    }
                }
                data_0209f2e0 = 2;
                data_0209f22c = data_0208ee44 << 3;
                dScStage_c::PS_UpdateOptionsMenu();
                data_0209f240 = 1;
                data_0209f244 = data_0208ee44 << 2;
                dScStage_c::PS_UpdateOkAndBackButtons(0);
                if (data_0209f29c == 0) {
                    SetSoundMode(data_0209f234);
                    if (data_0209f1fc != 0) {
                        data_0208ee40 = 1;
                    } else {
                        data_0208ee40 = 0;
                    }
                    data_0209f2c4 = 2;
                    func_02012790(0x116);
                    return;
                }
                func_02012790(0x5d);
                if (SublevelToLevel(data_0209f2f8) == 0x1d) {
                    data_0209f1ec = 0;
                } else {
                    data_0209f1ec = 2;
                }
                return;
            }
        }
    }
    case 0xb: {
        if (IsButtonInputValid() == 0)
            return;
        data_0209f244 = data_0208ee44 << 2;
        data_0209f2c4 = 2;
        func_02012790(3);
        return;
    }
    case 0xc: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xd00;
        Message::DisplaySaveMenuText(0x290);
        data_0209f360[0] = 0xa0;
        data_0209f360[1] = 0x140;
        data_0209f360[2] = 0x1e0;
        data_0209f2b4 = 3;
        data_0209f2e0 = 0;
        dScStage_c::UpdateMenuButtons(1);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        data_0209f1ec = 0xd;
        return;
    }
    case 0xd: {
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x28) < 0x20) ||
                (data_0209f2e0 == 0 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 0)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 0;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                data_0209f2c4 = 2;
                func_02012790(3);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x50) < 0x20) ||
                (data_0209f2e0 == 1 && IsButtonInputValid() != 0)) {
                if (data_0209f2e0 == 1)
                    data_0209f244 = data_0208ee44 << 2;
                data_0209f2e0 = 1;
                dScStage_c::UpdateMenuButtons(0);
                data_0209f22c = data_0208ee44 << 3;
                data_0209f1ec = 0xe;
                func_02012790(0x5e);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (!((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x78) < 0x20) &&
                (data_0209f2e0 != 2 || IsButtonInputValid() == 0))
                return;
            if (data_0209f2e0 == 2)
                data_0209f244 = data_0208ee44 << 2;
            data_0209f2e0 = 2;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f1ec = 0x10;
            func_02012790(0x5f);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 0xe: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0xe00;
        data_0209f1ec = 0xf;
        Message::DisplaySaveStatusText(0x295);
        data_0209d45c &= ~2;
        data_0209f2b8 = 0x78;
        return;
    }
    case 0xf: {
        if (data_0209f2b8 == 0)
            return;
        if (data_0209f2b8 == 0x78)
            SaveData::SaveCurrentFile();
        if (data_0209f2b8 == 0x3c)
            Message::DisplaySaveStatusText(0x296);
        data_0209f2b8 = data_0209f2b8 - data_0208ee44;
        if (data_0209f2b8 != 0)
            return;
        if (data_0209f2e0 != 1) {
            data_0209f2c4 = 2;
            return;
        }
        dScene_c::StartSceneFade(1, 0, 0);
        data_0209f280 = 1;
        dScStage_c::PS_Cleanup();
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        Sound::StopLoadedMusic_Layer1(0xa);
        return;
    }
    case 0x10: {
        REG16(0x400100a) = (REG16(0x400100a) & 0x43) | 0x1100;
        data_0209f1ec = 0x11;
        Message::DisplayDontSaveText(0x297);
        data_0209f2e0 = 1;
        dScStage_c::PS_UpdateSaveMenu(1);
        return;
    }
    case 0x11: {
        {
            int touched = 0;
            u8 slot = gActivePlayerSlot;
            if (gTouchHeld[slot * 4] != 0) {
                if (gTouchEdge[slot * 4] != 0)
                    touched = 1;
            }
            if (!touched) {
                if (IsButtonInputValid() == 0)
                    return;
            }
        }
        {
            register u8 slot;
            register u8 tx;
            int rel;
            int rely;
            slot = gActivePlayerSlot;
            tx = gTouchX[slot * 4];
            rel = tx - 0x28;
            if ((u8)rel < 0x50) {
                rely = gTouchY[slot * 4] - 0x98;
                if ((u8)rely < 0x20) {
                    if (data_0209f2e0 == 0)
                        data_0209f244 = data_0208ee44 << 2;
                    if ((u8)rel < 0x50 && (u8)rely < 0x20) {
                        data_0209f22c = data_0208ee44 << 3;
                        data_0209f1ec = 0x12;
                        func_02012790(0x60);
                    } else {
                        func_02012790(0x51);
                    }
                    data_0209f2e0 = 0;
                    dScStage_c::PS_UpdateSaveMenu(0);
                    return;
                }
            }
            {
                rel = tx - 0x88;
                if ((u8)rel >= 0x50)
                    return;
                {
                    rely = gTouchY[slot * 4] - 0x98;
                    if ((u8)rely >= 0x20)
                        return;
                    if (data_0209f2e0 == 1)
                        data_0209f244 = data_0208ee44 << 2;
                    if ((u8)rel < 0x50 && (u8)rely < 0x20) {
                        data_0209f22c = data_0208ee44 << 3;
                        data_0209f1ec = 0x12;
                        func_02012790(3);
                    } else {
                        func_02012790(0x52);
                    }
                    data_0209f2e0 = 1;
                    dScStage_c::PS_UpdateSaveMenu(0);
                    return;
                }
            }
        }
    }
    case 0x12: {
        if (data_0209f2e0 != 0) {
            data_0209f2c4 = 2;
            return;
        }
        dScene_c::StartSceneFade(1, 0, 0);
        data_0209f280 = 1;
        dScStage_c::PS_Cleanup();
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        return;
    }
    case 0x13: {
        int t = (data_0209f2d8 == 1);
        if (t != false && data_0209fc68 == 0) {
            dScene_c::StartSceneFade(6, 1, 0);
            data_0209f280 = 1;
            dScStage_c::PS_Cleanup();
            data_0209b454 |= 0x40000000;
            data_0209b464 = data_0209b454;
            return;
        }
        LoadLevelNoReturn(2, 0, -1, 0);
        data_0209b454 |= 0x40000000;
        data_0209b464 = data_0209b454;
        data_0209f280 = 1;
        data_0209f2c4 = 2;
        return;
    }
    default:
        return;
    }
}
#pragma opt_common_subs on

// @symbol func_02029408
/* Can the current player pause? Forwards to Player::CanPause on the active
 * entry of the live player list (data_0209f394, indexed by data_0209f250).
 * dScStage_c polls it before opening the pause menu. */

extern "C" int func_02029408(void) {
    return ((Player *)data_0209f394[data_0209f250])->CanPause();
}

// @symbol _ZN10dScStage_c7PS_InitEv
/* Open the pause screen: clear the pause-menu state, pick the first page from
 * the level-clear/VS/castle situation, pause the music and darken both
 * screens. Static, like the rest of the pause-screen family -- dScStage_c::
 * Behavior calls it with nothing in r0. */
void dScStage_c::PS_Init() {
    int isVsMode;
    u8 specialPauseState;
    u8 mainLayers;
    u8 subLayers;
    int mainBlendTargets;
    int subBlendTargets;

    subLayers = data_0209d454;
    if (subLayers & 4) {
        data_0209d454 = subLayers & ~4;
        subLayers = data_0209d454;
        *(volatile s32 *)0x04001000 = (*(volatile s32 *)0x04001000 & ~0x1f00) | (subLayers << 8);
    }

    specialPauseState = data_0209f218;
    mainLayers = data_0209d45c;
    isVsMode = 0;

    data_0209f248 = 0;
    data_0209f2e0 = 0;
    data_0209f2c8 = 0;
    data_0209f230 = 0;
    data_0209f2f0 = 0;
    data_0209f210 = 0;
    data_0209f22c = 0;
    data_0209f240 = 0;
    data_0209f238 = 0;
    data_0209f2cc = 0;
    data_0209f300 = 0xf;
    data_0209f23c = 0;
    data_0209f244 = 0;
    data_0209f29c = 0;
    data_0209f280 = 0;

    mainBlendTargets = mainLayers ^ 0x10;
    subBlendTargets = subLayers ^ 0x10;

    if (specialPauseState != 0) {
        data_0209f1ec = 7;
    } else {
        if (data_0209f2d8 == 1)
            isVsMode = 1;
        if (isVsMode != 0 && data_0209fc68 == 0) {
            data_0209f1ec = 4;
        } else {
            if (SublevelToLevel(data_0209f2f8) == 0x1d)
                data_0209f1ec = 0;
            else
                data_0209f1ec = 2;
        }
    }

    Sound::PauseMusic();
    G2x::SetBlendBrightness((volatile u16 *)0x04000050, mainBlendTargets | 0x20, -7);
    G2x::SetBlendBrightness((volatile u16 *)0x04001050, subBlendTargets | 0x20, -7);
}

// @symbol func_02029600
extern "C" {
int func_02029600(int x) {
    int b = (int)(data_0209f2d8 == 2);
    if (b == 0) {
        unsigned char saved = *(u8 *)&data_ov002_02111148;
        int r = GetMinimapID((Obj *)x, -1);
        if (r == saved)
            return 1;
    }
    return 0;
}
}

// @symbol IsButtonInputValid
extern "C" {
int IsButtonInputValid(void) {
    int mode;
    int val;

    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) == 0 && data_0209f204 == 0 &&
        data_0209f294 == 0) {
        mode = data_0209caa0[0x42];
        if (mode == 0) {
            val = *(unsigned short *)((char *)&data_020a0e5a + (gActivePlayerSlot << 2));
            if (val & 0xf)
                return 1;
        }
        if (mode == 1) {
            val = *(unsigned short *)((char *)&data_020a0e5a + (gActivePlayerSlot << 2));
            if (val & 0xc0f)
                return 1;
        }
        if (mode == 2) {
            val = *(unsigned short *)((char *)&data_020a0e5a + (gActivePlayerSlot << 2));
            if (val & 0xcff)
                return 1;
        }
        return 0;
    }
    val = *(unsigned short *)((char *)&data_020a0e5a + (gActivePlayerSlot << 2));
    if (val & 0xc) {
        data_0209f27c = 0;
        return 1;
    }
    if (val & ~0xc) {
        if (data_0209f27c == 0) {
            func_02012790(0xe);
            data_0209f27c = 1;
        }
    } else {
        data_0209f27c = 0;
    }
    return 0;
}
}

// @symbol _ZN10dScStage_c14GraphCallback2Ev
/* The callback object GraphCallback2 reads through `this`: vptr at +0 (never
   touched as data), a fixed-point 2x2 matrix at +0x4 that the object's
   static-init constructor at 0x02074edc seeds with 0x1000 (1.0), and the four
   affine words after it (StageGraphCallbackView). */

/* Nullary, and a member: vtable slot 2 of dScStage_c::graphCallback_c (RTTI
   si-child of dGraph_c::callback_c). Every dispatcher -- func_02018ec0,
   func_02018efc -- calls it as `ldr r1,[r0]; ldr r1,[r1,#8]; blx r1` with
   r0 = the callback object and r1 never set as an argument. Sets the BG3
   affine transform from the object's matrix. */
s32 dScStage_c::GraphCallback2() {
    StageGraphCallbackView *callback = (StageGraphCallbackView *)this;
    G2x::SetBGyAffine((volatile unsigned short *)REG_DB_BG3PA, &callback->matrix, callback->originX,
                      callback->originY, callback->targetX, callback->targetY);
    return 1;
}

// @symbol _ZN10dScStage_c14GraphCallback1Ev
/* Vtable slot 1 of dScStage_c::graphCallback_c: render every particle. */
int dScStage_c::GraphCallback1() {
    Particle::RenderAll();
    return 1;
}

// @symbol UpdateMinimap
/* Copy a 16-byte descriptor into data_0209f3c8 and store the four scalar
   params into data_0209f3c4 (+0x14..+0x20). */
extern "C" void UpdateMinimap(MinimapDesc *desc, u32 a, u32 b, u32 c, u32 d) {
    *(MinimapDesc *)(((long long)(int)&data_0209f3c8)) = *desc;
    data_0209f3c4.originX = a;
    data_0209f3c4.originY = b;
    data_0209f3c4.targetX = c;
    data_0209f3c4.targetY = d;
}

// @symbol StartEntranceFaderWipe
/* The fader wipe array. dScStage_c::InitResources fills data_0209f324 with
   func_02073470(7, 0x60, 8, &dFdWipe_cC1, &dFdWipe_cD1): seven objects of 0x60,
   which is sizeof(dFdWipe_c). dScStage_c::CleanupResources tears the array
   down and zeroes it. */
extern "C" void StartEntranceFaderWipe(int index) {
    dFdWipe_c *f = &((dFdWipe_c *)data_0209f324)[index];
    dScene_c::SetFaders(f);
    f->SetToEnd();
}

// @symbol StartExitCharacterWipe
/* Start the exit wipe for the current character (per-character wipe table). */
extern "C" void StartExitCharacterWipe(void) {
    StartExitFaderWipe(data_020755bc[data_0209caa0[0x41]]);
}

// @symbol StartExitFaderWipe
extern "C" void StartExitFaderWipe(int index) {
    dFdWipe_c *f = &((dFdWipe_c *)data_0209f324)[index];
    dScene_c::SetFaders(f);
    f->SetToStart();
}

// @symbol FUN_02029934
extern "C" void FUN_02029934(void) {
    dFdBrightness_c *o = data_0209f5bc;
    if (o->IsBetweenStartAndEnd() == 0) {
        data_0209d4b0 = 0;
        *(volatile unsigned short *)0x4000050 = 0;
    }
}

// @symbol FUN_02029980
/* Wipe slot 6 (data_0209f324 + 0x240) seen through its fader vtable: slots
   3/4 (SetBackwardTime/SetForwardTime) are called here with two arguments,
   slots 8/9 are SetToEnd/SetToStart. */
struct WipeFader {
    virtual void w0();
    virtual void w1();
    virtual void w2();
    virtual void w3(int, int);
    virtual void w4(int, int);
    virtual void w5();
    virtual void w6();
    virtual void w7();
    virtual void w8();
    virtual void w9();
};
extern "C" void FUN_02029980(void) {
    if (data_0209f5bc->IsBetweenStartAndEnd() != 0)
        return;
    WipeFader *o2 = (WipeFader *)((dFdWipe_c *)data_0209f324 + 6);
    data_0209d4b0 = (int)o2;
    o2->w8();
    o2->w3(0x1e, 0);
}

// @symbol FUN_020299f4
extern "C" void FUN_020299f4(void) {
    if (data_0209f5bc->IsBetweenStartAndEnd() != 0)
        return;
    WipeFader *o2 = (WipeFader *)((dFdWipe_c *)data_0209f324 + 6);
    data_0209d4b0 = (int)o2;
    o2->w9();
    o2->w4(0x1e, 0);
}

// @symbol FUN_02029a68
extern "C" void FUN_02029a68(void) {
    if (data_0209d4b0 == (int)((dFdWipe_c *)data_0209f324 + 6)) {
        data_0209d4b0 = 0;
        *(volatile unsigned short *)0x4000050 = 0;
    }
    data_0209f274 = 0;
}

// @symbol FUN_02029ab0
/* One entry past the six wipes' 0x240 bytes: two words at +0x4/+0x8. */
typedef struct WipeEntry {
    u32 pad0;
    u32 field4;
    u32 field8;
} WipeEntry;

extern "C" void FUN_02029ab0(void) {
    WipeEntry *entry;
    dScene_c::SetAndStopColorFader();
    entry = (WipeEntry *)((dFdWipe_c *)data_0209f324 + 6);
    data_0209d4b0 = (int)entry;
    entry->field4 = 0xc00;
    entry->field8 = 0;
    data_0209f274 = 1;
}

// @symbol EnterBigBoosHaunt
extern "C" void EnterBigBoosHaunt(void) {
    LoadLevel(0xc, 0, 0, 0, 0);
}

// @symbol KillPlayer
/* Kill the player: if lives remain, go to the next level with reason 2,
   otherwise trigger a game-over scene fade. Then play the death sound. */
extern "C" void KillPlayer(void) {
    if (data_0209f2f4 != 0)
        SetNextLevel(2);
    else
        dScene_c::StartSceneFade(8, 0, 0);
    StartExitFaderWipe(4);
    func_02012790(0x18);
}

// @symbol HitDeathPlane
/* The player hit a death plane: either restart (SetNextLevel) or trigger a
   scene fade, then always exit with StartExitFaderWipe. */
extern "C" void HitDeathPlane(int arg) {
    if (data_0209f2f4 != 0 || arg == 0)
        SetNextLevel(arg);
    else
        dScene_c::StartSceneFade(8, 0, 0);
    StartExitFaderWipe(6);
}

// @symbol ExitLevel
/* Exit the current level: next level 1 (the castle grounds), then the exit
   wipe. */
extern "C" void ExitLevel(void) {
    SetNextLevel(1);
    StartExitCharacterWipe();
}

// @symbol SetNextLevel
extern "C" void SetNextLevel(int arg) {
    int v = data_0209f2f8;
    unsigned int i;
    struct LevelReturnEntry *p;

    if (v == 0x22) {
        if (arg != 1) {
            data_0209211c = 1;
            data_0209f200 = 0xa;
        }
    } else {
        p = data_02075638;
        for (i = 0; i < 8; i++) {
            if (v == p->level) {
                data_0209211c = p->returnLevel;
                data_0209f200 = p->entrance;
                break;
            }
            p++;
        }
    }

    if (data_0209211c < 0 || SublevelToLevel(v) == 0x1d) {
        data_02092110 = 1;
        data_0209f268 = 0xd;
    } else {
        data_02092110 = data_0209211c;
        data_0209f268 = data_0209f200;
    }

    data_0209211c = -1;
    data_0209f1f0 = data_0209f220;
    SetNextStar();
    data_0209f26c = arg;
}

// @symbol SetNextStar
extern "C" void SetNextStar(void) {
    int level = SublevelToLevel(data_02092110);
    int b;
    if (data_0209f1f0 == 1)
        return;
    b = (int)(data_0209f2d8 == 1);
    if (b != 0)
        return;
    if (level != -1) {
        if (level <= 0xe)
            return;
        if (level == 0x13)
            return;
    }
    data_0209f1f0 = 1;
}

// @symbol DeathTable_ClearBit
/* The actor death table: one 16-word row of bits per level part, indexed by
   an actor's deathTableID. */
extern "C" void DeathTable_ClearBit(s32 id) {
    if (id < 0)
        return;
    s32 level = GetLevelPart((s32)data_0209f2f8);
    u32 *table = data_0209f4f8[level];
    u32 bitMask = ~(1u << (id & 0x1f));
    s32 word = id >> 5;
    table[word] = table[word] & bitMask;
}

// @symbol DeathTable_SetBit
extern "C" void DeathTable_SetBit(s32 id) {
    if (id < 0)
        return;
    s32 level = GetLevelPart((s32)data_0209f2f8);
    u32 *table = data_0209f4f8[level];
    s32 word = id >> 5;
    u32 val = table[word];
    u32 bitMask = 1u << (id & 0x1f);
    table[word] = val | bitMask;
}

// @symbol DeathTable_GetBit
extern "C" u32 DeathTable_GetBit(s32 id) {
    if (id < 0)
        return 0;
    s32 level = GetLevelPart((s32)data_0209f2f8);
    u32 *table = data_0209f4f8[level];
    s32 word = id >> 5;
    s32 bit = id & 0x1f;
    u32 bitMask = 1u << bit;
    return table[word] & bitMask;
}

// @symbol _ZN5Event8ClearBitEj
/* The event-flag bitfield accessors: Clear/Set/Get over the one-word event
   table data_0209f34c. */
namespace Event {

s32 ClearBit(u32 bit) {
    return data_0209f34c &= ~(1 << bit);
}

// @symbol _ZN5Event6SetBitEj
void SetBit(u32 bit) {
    data_0209f34c |= 1 << bit;
}

// @symbol _ZN5Event6GetBitEj
s32 GetBit(u32 bit) {
    return data_0209f34c & (1 << bit);
}

}

// @symbol _ZN10dScStage_c19RenderVsModeNewStarEv
void dScStage_c::RenderVsModeNewStar() {
    u16 timer = data_0209f308;
    if (timer == 0)
        return;
    if (data_0209f204 != 0)
        return;
    data_0209f308 = timer - data_0208ee44;
    if (data_0209f308 % 60 >= 10) {
        if (GetOwnerLanguage() == 5)
            OAM::Render(0, (OamAttr *)data_ov002_0210cfd0, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else if (GetOwnerLanguage() == 4)
            OAM::Render(0, (OamAttr *)data_ov002_0210d050, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else if (GetOwnerLanguage() == 3)
            OAM::Render(0, (OamAttr *)data_ov002_0210d0e8, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else if (GetOwnerLanguage() == 2)
            OAM::Render(0, (OamAttr *)data_ov002_0210d188, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else
            OAM::Render(0, &OAM::VS_NEW_STAR_APPEARED, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0, -1);
        if (data_0209f1f4 != 0)
            return;
        func_02012790(0x2c);
        data_0209f1f4 = 1;
        return;
    }
    data_0209f1f4 = 0;
}

// @symbol FUN_0202a130
extern "C" void FUN_0202a130(void) {
    if (data_0209f2bc != 0)
        return;
    data_0209f308 = 0xb4;
    data_0209f1f4 = 0;
}

// @symbol _ZN10dScStage_c21RenderVsModeCountdownEv
void dScStage_c::RenderVsModeCountdown() {
    int count = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (data_0209f394[i]) {
            int t = (((Player *)data_0209f394[i])->mLevelEntryDone != 0);
            if (t == 1)
                count++;
        }
    }
    if (((dFdBrightness_c *)data_0209f5bc)->IsAtStart() == 0)
        return;
    if (count < data_0209fc50)
        return;
    if (data_0209f2bc != 0) {
        if (GetOwnerLanguage() == 5)
            OAM::Render(0, (OamAttr *)&data_ov002_0210d008, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else if (GetOwnerLanguage() == 4)
            OAM::Render(0, (OamAttr *)&data_ov002_0210d098, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else if (GetOwnerLanguage() == 3)
            OAM::Render(0, (OamAttr *)&data_ov002_0210d130, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else if (GetOwnerLanguage() == 2)
            OAM::Render(0, (OamAttr *)&data_ov002_0210d1c8, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        else
            OAM::Render(0, (OamAttr *)&data_ov002_0210d210, 0x80, 0x30, -1, -1, 0x1000, 0x1000, 0,
                        -1);
        OAM::Render(0, (OamAttr *)data_ov002_0210ca0c[3 - data_0209f2bc], 0x80, 0x50, -1, -1,
                    0x1000, 0x1000, 0, -1);
        return;
    }
    if (data_0209f304 == 0)
        return;
    if (GetOwnerLanguage() == 5)
        OAM::Render(0, (OamAttr *)&data_ov002_0210d378, 0x80, 0x50, -1, -1, 0x1000, 0x1000, 0, -1);
    else if (GetOwnerLanguage() == 4)
        OAM::Render(0, (OamAttr *)&data_ov002_0210d3a0, 0x80, 0x50, -1, -1, 0x1000, 0x1000, 0, -1);
    else if (GetOwnerLanguage() == 3)
        OAM::Render(0, (OamAttr *)&data_ov002_0210d3c0, 0x80, 0x50, -1, -1, 0x1000, 0x1000, 0, -1);
    else if (GetOwnerLanguage() == 2)
        OAM::Render(0, (OamAttr *)&data_ov002_0210d3e0, 0x80, 0x50, -1, -1, 0x1000, 0x1000, 0, -1);
    else
        OAM::Render(0, (OamAttr *)&data_ov002_0210d420, 0x80, 0x50, -1, -1, 0x1000, 0x1000, 0, -1);
}

// @symbol NumVsStarsObtained
extern "C" s32 NumVsStarsObtained(void) {
    int i = 1;
    signed char sum = data_0209f310[0];
    signed char *p;
    if (i >= (int)data_0209f21c)
        return sum;
    p = &data_0209f311;
    do {
        sum = (signed char)(sum + *p);
        i++;
        p++;
    } while (i < (int)data_0209f21c);
    return sum;
}

// @symbol GiveVsStars
extern "C" void GiveVsStars(int idx, int delta) {
    data_0209f310[idx] += delta;
}

// @symbol _ZN10dScStage_c15IsPauseDisabledEv
/* dScStage_c::IsPauseDisabled() at 0x0202c8f8. Static: reads only globals, and no
 * caller sets up an object. */

extern "C" {
extern int func_ov002_020bd8ac(unsigned char *p);
}

int dScStage_c::IsPauseDisabled() {
    unsigned char *p = *(unsigned char **)data_0209f394;
    if (p != 0 && func_ov002_020bd8ac(p) == 0)
        return 1;
    return 0;
}

// @symbol ClearSpikeBomb
/* The level's eight spike-bomb slots (daKirai_c's mSlotIndex). */
extern "C" void ClearSpikeBomb(int idx) {
    if (idx < 0)
        return;
    if (idx < 8)
        data_0209f3a4[idx] = 0;
}

// @symbol AddSpikeBomb
extern "C" int AddSpikeBomb(void *p) {
    int i;
    for (i = 0; i < 8; i++) {
        if (data_0209f3a4[i] == 0) {
            data_0209f3a4[i] = p;
            return i;
        }
    }
    return -1;
}

// @symbol SetStarMarker
extern "C" void SetStarMarker(int i, int v1, int v2) {
    data_0209f40c[i] = v1;
    data_0209f37c[i] = v2;
}

// @symbol OpenCannonInCurLevel
extern "C" void OpenCannonInCurLevel(void) {
    OpenCannonInLevel(data_0209f2f8);
}

// @symbol IsCannonOpenInCurLevel
extern "C" int IsCannonOpenInCurLevel(void) {
    return IsCannonOpenInLevel(data_0209f2f8);
}

// @symbol CollectStarInCurLevel
extern "C" void CollectStarInCurLevel(s32 starID) {
    CollectStarInLevel(data_0209f2f8, starID);
}

// @symbol IsStarCollectedInCurLevel
extern "C" s32 IsStarCollectedInCurLevel(s32 starID) {
    return IsStarCollectedInLevel(data_0209f2f8, starID);
}

// @symbol NumRedCoins
extern "C" signed char NumRedCoins(void) {
    int i = 1;
    signed char sum = data_0209f30c[0];
    signed char *p;
    if (i >= (int)data_0209f21c)
        return sum;
    p = &data_0209f30d;
    do {
        sum = (signed char)(sum + *p);
        i++;
        p++;
    } while (i < (int)data_0209f21c);
    return sum;
}

// @symbol GiveRedCoins
extern "C" int GiveRedCoins(int i, int amt) {
    signed char *p = &data_0209f30c[i];
    data_0209f30c[i] = (signed char)(data_0209f30c[i] + amt);
    int v = data_0209f30c[i];
    if (v > 0x63) {
        v = 0x63;
        *p = (signed char)v;
        return v;
    }
    if (v < 0) {
        v = 0;
        *p = (signed char)v;
        return v;
    }
    return v;
}

// @symbol NumCoins
extern "C" short NumCoins(void) {
    int i = 1;
    int n = data_0209f21c;
    short *p = data_0209f358;
    short sum = p[0];
    if (n <= 1)
        return sum;
    do {
        sum = sum + p[i];
        i++;
    } while (i < n);
    return sum;
}

// @symbol GiveCoins
extern "C" void GiveCoins(int idx, int amount) {
    short *p = data_0209f358;
    short cur = p[idx];
    short v = cur + amount;
    p[idx] = v;
    v = p[idx];
    if (v > 0xff) {
        p[idx] = 0xff;
    } else if (v < 0) {
        p[idx] = 0;
    }
    func_ov002_020b18f0((int)data_0209f394[idx]);
}

// @symbol GiveHealth
/* Returns the clamped health, which Player::GiveHealth hands straight back. */
extern "C" int GiveHealth(int idx, int amt) {
    int v;
    data_02092144[idx] += amt;
    v = data_02092144[idx];
    if (v > 0x880) {
        v = 0x880;
        data_02092144[idx] = v;
        return v;
    }
    if (v < 0) {
        v = 0;
        data_02092144[idx] = v;
    }
    return v;
}

// @symbol func_0202a850
#pragma opt_dead_assignments off
extern "C" void func_0202a850(int a, int b) {
    data_02092138 = a;
    data_0209212c = b;
    data_02092138 = a - 0x64000;
    data_0209212c = b + 0x64000;
}
#pragma opt_dead_assignments reset

// @symbol InitControllerMode
/* The per-controller records at data_0209f498 are 0x18 apart; the mode byte
   is their +0x16, data_0209f4ae. The save file keeps the active player's
   copy at data_0209caa0+0x42. */
extern "C" void InitControllerMode(int idx, u8 val) {
    data_0209f4ae[idx * 0x18] = val;
    if (idx == gActivePlayerSlot)
        data_0209caa0[0x42] = val;
}

// @symbol SetControllerMode
extern "C" void SetControllerMode(unsigned char mode) {
    data_0209f4ae[gActivePlayerSlot * 0x18] = mode;
    data_0209caa0[0x42] = mode;
}

// @symbol func_0202a8e0
extern "C" int func_0202a8e0(int a, unsigned char b) {
    int i;
    for (i = 0; i < 9; i++) {
        if (data_0209f3e8[i] == 0) {
            data_0209f3e8[i] = a;
            data_0209f370[i] = b;
            return i;
        }
    }
    return -1;
}

// @symbol _ZN10dScStage_c11GetSkyboxIDEv
/* Member, not static: LoadSkybox reaches it with r0 still holding `this` and
 * no argument set up. The body reads the level record through the global at
 * 0x0209f340 rather than through a field, so the bytes cannot show the
 * `this` -- the call site is the evidence. */
int dScStage_c::GetSkyboxID() {
    unsigned int *p = *(unsigned int **)&data_0209f340;
    return (p[6] >> 4) & 0x1f;
}

// @symbol func_0202a958
extern "C" int func_0202a958(void) {
    return ((LevelInfo *)data_0209f340)->minimapScale;
}

// @symbol func_0202a96c
extern "C" int func_0202a96c(void) {
    return ((LevelInfo *)data_0209f340)->minimapPaletteFile;
}

// @symbol func_0202a980
extern "C" int func_0202a980(void) {
    return ((LevelInfo *)data_0209f340)->minimapTilesFile;
}

// @symbol _Z23LoadMinimapChangeObjectiia
/* data_0209f314 points at mTexAnimSlots (InitResources stores it). Each
   0xc-byte slot doubles as the per-area record: HideArea/ShowArea toggle its
   mActive byte, and mBlockList heads a sorted list of these minimap-change
   records, threshold at +0, next at +0xc (the link CleanupResources walks
   when it frees them). GetMinimapID walks next from the head and answers
   minimapId for the first record whose height an actor is above. */
struct MinimapChangeNode {
    int height; /* 0x00 */
    /* 0xf from the loader becomes -1. */
    s8 minimapId; /* 0x04 */
    char pad5[3];
    /* Only LoadMinimapChangeObject reads it, to find the new list head. */
    struct MinimapChangeNode *prev; /* 0x08 */
    struct MinimapChangeNode *next; /* 0x0c */
};

void LoadMinimapChangeObject(int idx, Fix12i val, s8 h) {
    struct MinimapChangeNode *nn = (MinimapChangeNode *)::operator new(sizeof(MinimapChangeNode));
    StageTexAnimSlot *e = &((StageTexAnimSlot *)data_0209f314)[idx];
    struct MinimapChangeNode *cur = (MinimapChangeNode *)e->mBlockList;
    struct MinimapChangeNode *prev = cur;
    if (cur != 0) {
        for (;;) {
            prev = cur;
            if (val > cur->height)
                break;
            cur = cur->next;
            if (cur == 0)
                break;
        }
    }
    nn->next = prev;
    if (prev == 0) {
        nn->prev = 0;
    } else {
        nn->prev = prev->prev;
        prev->prev = nn;
    }
    if (nn->prev == 0)
        e->mBlockList = nn;
    {
        int hh = h;
        nn->height = val;
        if (hh == 0xf)
            hh = -1;
        nn->minimapId = (u8)hh;
    }
}

// @symbol HideArea
extern "C" void HideArea(int idx) {
    ((StageTexAnimSlot *)data_0209f314)[idx].mActive = 0;
}

// @symbol ShowArea
extern "C" void ShowArea(int idx) {
    ((StageTexAnimSlot *)data_0209f314)[idx].mActive = 1;
}

// @symbol IsAreaShowing
extern "C" unsigned char IsAreaShowing(int idx) {
    return ((StageTexAnimSlot *)data_0209f314)[idx].mActive;
}

// @symbol ChangeArea
extern "C" void ChangeArea(int areaID) {
    if (data_02092120 >= 0)
        HideArea(data_02092120);
    data_02092120 = (s8)areaID;
    ShowArea(areaID);
}

// @symbol GiveLives
extern "C" void GiveLives(int delta) {
    int v = data_0209f2f4 + delta;
    if (v < 0)
        v = 0;
    else if (v > 0x64)
        v = 0x64;
    data_0209f2f4 = (signed char)v;
}

// @symbol LoadLevel
/* The four register parameters are spelled wide because LoadLevelNoReturn
   (int, u32, u32, u32 in decl_common.h) forwards its own four with no
   narrowing; the stack-passed returnState is read back with ldrsb, so it is
   a signed byte. */
extern "C" void LoadLevel(int levelID, u32 entranceID, int starID, u32 arg3, s8 returnState) {
    int cur;
    int chk;

    if (data_02092110 < 0) {
        dScene_c::SetAndStopColorFader();
        *(unsigned short *)((char *)&data_0209f5e8 + 0xc) = 0;
    }
    data_02092110 = levelID;
    data_0209f268 = entranceID;
    if (starID < 0)
        data_0209f1f0 = data_0209f220;
    else
        data_0209f1f0 = (u8)starID;
    SetNextStar();
    data_0209f26c = arg3;
    if (returnState >= 0) {
        data_0209211c = data_0209f2f8;
        data_0209f200 = (u8)returnState;
        chk = data_02092118;
        if (chk < 0)
            return;
        cur = SublevelToLevel(data_02092110);
        chk = SublevelToLevel(chk);
        if (cur != chk) {
            data_02092118 = -1;
            return;
        }
        data_02092110 = data_02092118;
        data_0209f268 = data_0209f24c;
        data_0209f1f0 = data_0209f270;
    } else {
        cur = SublevelToLevel(data_0209f2f8);
        if (data_0209211c >= 0 && (cur == 6 || cur == 7 || cur == 0xb)) {
            data_02092118 = data_02092110;
            data_0209f24c = data_0209f268;
            data_0209f270 = data_0209f1f0;
        } else {
            data_02092118 = -1;
        }
    }
}

// @symbol LoadLevelNoReturn
extern "C" void LoadLevelNoReturn(int levelID, u32 arg1, u32 arg2, u32 arg3) {
    LoadLevel(levelID, arg1, arg2, arg3, -1);
    data_0209211c = -1;
    data_02092118 = -1;
}

// @symbol SetPlayerGlobals
/* Initialise the player globals: lives, health, controller mode. */
extern "C" void SetPlayerGlobals(void) {
    data_0209f2f4 = 4;
    s32 i = 0;
    do {
        u8 mode = data_0209caa0[0x42];
        data_02092144[i] = 0x880;
        InitControllerMode(i, mode);
        i++;
    } while (i < 4);
    /* A signed byte here: decl_common.h types it unsigned for its other readers. */
    *(s8 *)&data_02092114 = -1;
    data_0209f254 = 0;
}

// @symbol ExitMinigameMenu
/* Leave the minigame menu, either back to the rec room or fading to the main
   menu. */
extern "C" void ExitMinigameMenu(void) {
    if (data_0209f298) {
        LoadLevelNoReturn(0x32, 4, 1, 0);
        data_0209f2f8 = 6;
        dScene_c::StartSceneFade(3, 0, 0x7fff);
    } else {
        dScene_c::StartSceneFade(1, 0, 0);
    }
}

// @symbol StartMinigameMenu
/* Start the minigame menu scene; the argument says whether to return to the
   rec room afterwards. */
extern "C" void StartMinigameMenu(u8 returnToRecRoom) {
    data_0209f298 = returnToRecRoom;
    dScene_c::SetAndStopColorFader();
    dScene_c::StartSceneFade(5, 0, 0x7fff);
}

// @symbol PrepareVsMode
/* Set up VS mode: mark the game mode, initialise the player globals, start
   the scene fade, then reset the save data to its defaults. */
extern "C" void PrepareVsMode(void) {
    data_0209f2d8 = 1;
    SetPlayerGlobals();
    dScene_c::StartSceneFade(3, 0, 0x7fff);
    SaveData::SetDefaultValues((FileSaveData *)data_0209caa0);
}

// @symbol func_0202ae74
extern "C" void func_0202ae74(void) {
    StartFile(1, 0);
}

// @symbol StartFile
/* Start a single-player file: clear the game mode, load the level, set the
   player globals, copy the current character to data_02092128, one player,
   clear the star/level flags, then start the scene fade. */
extern "C" void StartFile(s8 levelID, u8 entranceID) {
    data_0209f2d8 = 0;
    LoadLevelNoReturn(levelID, entranceID, 1, 0);
    SetPlayerGlobals();
    data_02092128[0] = data_0209caa0[0x41];
    SetNumPlayers(1);
    data_02092124 = 6;
    data_0209f228[0] = 0;
    dScene_c::StartSceneFade(3, 0, 0x7fff);
}

// @symbol StartWithFarCamera
extern "C" int StartWithFarCamera(void) {
    return (((LevelInfo *)data_0209f340)->cameraFlags & 1) != 0;
}

// @symbol GetStarCameraSetting
extern "C" unsigned char GetStarCameraSetting(int idx) {
    return (data_02092134 >> (idx * 4)) & 0xf;
}

// @symbol GetMinimapScale
extern "C" int GetMinimapScale(int idx) {
    if (idx >= 0) {
        if (idx < (int)data_0209f25c)
            return data_0209f348[idx];
    }
    return 0xa00;
}

// @symbol func_0202af80
/* The six table setters below each store a pointer and its count. */
extern "C" void func_0202af80(int a, int b) {
    data_0209f348 = (short *)a;
    data_0209f25c = b;
}

// @symbol GetMinimapID
/* The actor GetMinimapID places, with dActor_c's names: its height at
   +0x60, its area at +0xcc. */
struct Obj {
    char pad0[0x60];
    int mPosY; /* 0x60 */
    char pad64[0xcc - 0x64];
    s8 mAreaId; /* 0xcc */
};

extern "C" int GetMinimapID(struct Obj *obj, int arg) {
    int idx = obj->mAreaId;
    char *g;
    int val;
    struct MinimapChangeNode *node;
    if (idx < 0)
        idx = data_02092120;
    g = (char *)data_0209f314;
    if (g != 0) {
        node = (MinimapChangeNode *)((StageTexAnimSlot *)g)[idx].mBlockList;
        if (node != 0) {
            val = obj->mPosY;
            while (node != 0) {
                int d = val - node->height;
                if (d > 0) {
                    if (arg >= 0 && d < 0xc8000)
                        return arg;
                    break;
                }
                node = node->next;
            }
            if (node != 0)
                return node->minimapId;
        }
    }
    return ((LevelInfo *)data_0209f340)->entries[idx].minimapId;
}

// @symbol func_0202b044
extern "C" void func_0202b044(int a, int b) {
    data_0209f334 = (u16 *)a;
    data_0209f2e8 = b;
}

// @symbol func_0202b060
extern "C" void func_0202b060(int a, int b) {
    data_0209f31c = (char *)a;
    data_0209f258 = b;
}

// @symbol GetTeleportDestObj
/* The level's teleport destinations, eight bytes apart. */
struct ObjTeleport {
    int a;
    int b;
};
extern "C" struct ObjTeleport *GetTeleportDestObj(int i) {
    return (ObjTeleport *)data_0209f330 + i;
}

// @symbol func_0202b090
extern "C" void func_0202b090(int a, int b) {
    data_0209f330 = (void *)a;
    data_0209f2d0 = b;
}

// @symbol GetViewObj
/* The level's camera view records, 0xe bytes apart. */
struct ViewObj {
    unsigned char d[0xe];
};
extern "C" struct ViewObj *GetViewObj(int idx) {
    return &((ViewObj *)data_0209f354)[idx];
}

// @symbol func_0202b0c4
extern "C" void func_0202b0c4(int a, int b) {
    data_0209f354 = (void *)a;
    data_0209f1f8 = b;
}

// @symbol func_0202b0e0
extern "C" void func_0202b0e0(int a, int b) {
    data_0209f328 = (int *)a;
    data_0209f214 = b;
}

// @symbol _ZN10dScStage_c10LoadSkyboxEv
/* Ask the level record which skybox to use and, if there is one, allocate a
 * dExtSimpleModel_c, construct it and load the file for that ID. mSkyboxModel is typed in
 * include/dScStage_c.h because of this store and the virtual destroy in
 * CleanupResources.
 */
void dScStage_c::LoadSkybox() {
    u32 skyboxID = GetSkyboxID();
    if (!skyboxID)
        return;
    mSkyboxModel = new dExtSimpleModel_c;
    u16 fileID = data_02075620[skyboxID - 1];
    mSkyboxModel->LoadAndSetFile(fileID, 0, 2);
}

// @symbol _ZN10dScStage_c22RenderModelTransparentEv
/* dExtSimpleModel_c's own vtable shape as far as the two RenderModel passes need it (six
   slots: D1, D0, DoSetFile, UpdateVerts, Virtual10, Render), named to avoid
   colliding with the real dExtModel_c (which has only three). */
struct LocalModelVtableShape {
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void Render(void *ctx);
};

struct Inner {
    char pad[0x30];
    u16 count;
    u8 *ids;
    char pad38[0x8];
};

struct Component {
    char pad[0x24];
    u32 flags;
    char pad28[0x8];
};

/* Local view of the real ModelComponents (include/dExtModel_c.h): `sub` is that
   type's `modelFile` (offset 0x0); `components` is its `materials` (offset
   0x4, 0x30-byte records). */
struct LocalModelComponents {
    void *sub;             // offset 0x0 -> *(sub+8) is Inner*
    Component *components; // offset 0x4
};

void dScStage_c::RenderModelTransparent() {
    ModelComponents *components = &mModel.data;
    BMD_Bone *bone = components->modelFile->bones;
    StageTexAnimSlot *slot = mTexAnimSlots;
    int slotIndex;
    for (slotIndex = 0; slotIndex < ((LevelInfo *)data_0209f340)->count;
         slotIndex++, slot++, bone++) {
        if (slot->mActive != 0) {
            u8 *materialIds = bone->materialIds;
            u16 materialIndex;
            for (materialIndex = 0; materialIndex < bone->numMaterials; materialIndex++) {
                BMD_Material *material = &components->materials[*materialIds];
                if ((material->flags & 0x1f0000) != 0x1f0000)
                    material->flags &= ~0x80000000;
                else
                    material->flags |= 0x80000000;
                materialIds++;
            }
        }
    }
    dExtSimpleModel_c *model = &mModel;
    model->Render((const Vector3 *)&data_020755d4);
}

// @symbol _ZN10dScStage_c11RenderModelEv
void dScStage_c::RenderModel() {
    ModelComponents *components = &mModel.data;
    BMD_Bone *bone = components->modelFile->bones;
    StageTexAnimSlot *slot = mTexAnimSlots;
    int slotIndex;

    for (slotIndex = 0; slotIndex < ((LevelInfo *)data_0209f340)->count; slotIndex++) {
        u8 active = slot->mActive;
        u8 *materialIds = bone->materialIds;

        if (active != 0) {
            u16 materialIndex;
            for (materialIndex = 0; materialIndex < bone->numMaterials; materialIndex++) {
                u8 materialId = *materialIds;
                BMD_Material *material = &components->materials[materialId];
                materialIds++;
                u32 flagsTest = *(volatile u32 *)&material->flags;
                if ((flagsTest & 0x1f0000) == 0x1f0000) {
                    u32 *flags = &material->flags;
                    *flags &= ~0x80000000;
                } else {
                    u32 *flags = &material->flags;
                    *flags |= 0x80000000;
                }
            }
            if (slot->mTransformer != 0)
                slot->mTransformer->Update(mModel.data);
        } else {
            u16 materialIndex;
            for (materialIndex = 0; materialIndex < bone->numMaterials; materialIndex++) {
                u8 materialId = *materialIds;
                BMD_Material *material = &components->materials[materialId];
                material->flags |= 0x80000000;
                materialIds++;
            }
        }

        slot++;
        bone++;
    }

    dExtSimpleModel_c *model = &mModel;
    model->Render((const Vector3 *)&data_020755d4);
}

// @symbol CopyTexPalFromLevelModel
/* Copy each material's texture/palette parameters from the level model's
   material of the same texture and palette names; returns 0 if any material
   found no partner. The level model's pair is the one LoadModel leaves in
   data_0209f320. */
struct Tex {
    char *name;
    char pad[0x10];
}; /* stride 0x14 */
struct Pal {
    char *name;
    char pad[0xc];
}; /* stride 0x10 */

struct TexPalEntry {
    u32 texIdx;      /* 0x00 */
    u32 palIdx;      /* 0x04 */
    char pad1[0x14]; /* 0x08 */
    u32 f1c;         /* 0x1c */
    u32 f20;         /* 0x20 */
    char pad2[0xc];  /* 0x24 */
}; /* 0x30 */

struct TexPalModel {
    char pad0[0x18];
    struct Tex *texArr; /* 0x18 */
    char pad1[4];       /* 0x1c */
    struct Pal *palArr; /* 0x20 */
    u32 count;          /* 0x24 */
};

struct Arg {
    struct TexPalModel *model;   /* 0x00 */
    struct TexPalEntry *entries; /* 0x04 */
};

extern "C" int CopyTexPalFromLevelModel(void *p) {
    struct Arg *arg = (struct Arg *)p;
    struct Arg *level = (struct Arg *)data_0209f320;
    int ret = 1;
    struct TexPalEntry *sbase = level->entries;
    struct TexPalModel *a = arg->model;
    struct Tex *atex = a->texArr;
    struct TexPalModel *sm = level->model;
    struct Tex *mtex = sm->texArr;
    struct Pal *mpal = sm->palArr;
    struct Pal *apal = a->palArr;
    struct TexPalEntry *e = arg->entries;
    u32 i;

    for (i = 0; i < a->count; i++) {
        char *texName = atex[e->texIdx].name;
        char *palName = apal[e->palIdx].name;
        struct TexPalEntry *s = sbase;
        int found = 0;
        u32 j;
        for (j = 0; j < sm->count; j++) {
            if (cstd::strcmp(texName, mtex[s->texIdx].name) == 0 &&
                cstd::strcmp(palName, mpal[s->palIdx].name) == 0) {
                e->f1c = s->f1c;
                e->f20 = s->f20;
                found = 1;
            }
            s++;
        }
        if (!found)
            ret = 0;
        e++;
    }
    return ret;
}

// @symbol _ZN10dScStage_c23LoadTextureTransformersEv
void dScStage_c::LoadTextureTransformers() {
    struct LevelTexAnim *entry = ((LevelInfo *)data_0209f340)->entries;
    StageTexAnimSlot *slot = mTexAnimSlots;
    BMD_File *modelFile = mModel.data.modelFile;
    int slotIndex;
    for (slotIndex = 0; slotIndex < (int)((LevelInfo *)data_0209f340)->count; slotIndex++) {
        if (entry->bta) {
            dExtAnmTexSRT_c::Prepare(*modelFile, *(BTA_File *)entry->bta);
            slot->mTransformer = new dExtAnmTexSRT_c;
            slot->mTransformer->SetFile(*(BTA_File *)entry->bta, 0, 0x1000, 0);
        }
        entry++;
        slot++;
    }
}

// @symbol _ZN10dScStage_c9LoadModelEv
void dScStage_c::LoadModel() {
    mModel.LoadAndSetFile(*(unsigned short *)(*(char **)&data_0209f340 + 8), 1, -1);
    BMD_File *modelFile;
    unsigned int materialCount;
    BMD_Material *material;
    unsigned int materialIndex;
    ModelComponents *components;

    components = &mModel.data;
    modelFile = components->modelFile;
    material = components->materials;
    materialCount = modelFile->numMaterials;
    materialIndex = 0;
    while (materialIndex < materialCount) {
        unsigned int flags = material->flags;
        materialIndex++;
        if ((flags & 0x1f0000) != 0x1f0000) {
            material->flags = (flags & ~0x3f000000) | 0x13000000;
        }
        material++;
    }
    data_0209f320 = (int)components;
}

// @symbol _ZN10dScStage_c9RenderFogEv

void dScStage_c::RenderFog() {
    Fog *fog = 0;
    if (data_0209f2f8 == 5)
        fog = daMugenBGM_c::GetActiveFog();
    if (fog == 0) {
        dCamera_c *camera = (dCamera_c *)data_0209f318;
        fog = mFog;
        if (camera->IsUnderwater())
            fog++;
    }
    G3X::SetFog(fog->mEnabled, 0, fog->mShift, fog->mOffset);
    *(volatile unsigned int *)0x4000358 = fog->mColor | 0x1f0000;
    G3X::SetFogTable(fog);
}

// @symbol _ZN10dScStage_c7LoadFogEv
/* dScStage_c::LoadFog. Lays down two default ramps -- a shallow one in mFog[0] and
   a steeper one in mFog[1] -- then lets the level's own fog records, if it has
   any, overwrite them through Fog::Init. */
void dScStage_c::LoadFog() {
    unsigned char normalDensity = 0;
    unsigned char underwaterDensity = 0x28;
    int densityIndex;
    for (densityIndex = 0; densityIndex < 0x20; densityIndex++) {
        mFog[0].mDensity[densityIndex] = normalDensity;
        mFog[1].mDensity[densityIndex] = underwaterDensity;
        normalDensity += 4;
        underwaterDensity += 2;
    }
    mFog[0].mEnabled = 0;
    mFog[1].mEnabled = 1;
    mFog[1].mShift = 6;
    mFog[1].mOffset = 0;
    mFog[1].mColor = 0x6000;
    if (data_0209f258 == 0) {
        return;
    }
    {
        unsigned char *fogRecord = (unsigned char *)data_0209f31c;
        Fog *fog = mFog;
        int fogIndex;
        for (fogIndex = 0; fogIndex < data_0209f258; fogIndex++) {
            if (fogRecord[0] != 1) {
                fog->mEnabled = 0;
            } else {
                unsigned short color = ((fogRecord[1] >> 3) | ((fogRecord[2] >> 3) << 5)) |
                                       ((fogRecord[3] >> 3) << 10);
                fog->Init(color, *((unsigned short *)(fogRecord + 4)),
                          *((unsigned short *)(fogRecord + 6)));
            }
            fogRecord += 8;
            fog++;
        }
    }
}

// @symbol _ZN10dScStage_c8CanPauseEv
/* Static: five globals, no object. */
int dScStage_c::CanPause() {
    if (((unsigned)(data_0209f2d8 == 1)) == 0)
        return 1;
    if (data_0209fc68 != 0)
        return 0;
    if (data_0209f2bc != 0 || (data_ov002_02111188 == 0 && data_0209f204 != 0))
        return 0;
    return 1;
}

// @symbol _ZN10dScStage_c16OnPendingDestroyEv
/* dScStage_c::OnPendingDestroy() at 0x0202b8a0
 * fBase_c vtable slot 12 (0x30), called when the dScStage_c scene is marked for
 * destruction, before CleanupResources(). dScStage_c overrides it with an empty body
 * -- no teardown work needed at this point.
 * `this` is dScStage_c* (dScStage_c derives from dScene_c <- dBase_c <- fBase_c).
 */

void dScStage_c::OnPendingDestroy() {
    (void)this;
}

// @symbol _ZN10dScStage_c6RenderEv
extern "C" {
void func_020391f0(void);
void func_0203083c(void);
void func_ov001_020aaf40(void);
void func_0203db3c(int i, int v);
}

int dScStage_c::Render() {
    if (((data_0209f294 | (data_0209f2c4 | data_0209f20c)) & 0xff) == 0) {
        StageTexAnimSlot *slot = mTexAnimSlots;
        int slotIndex;
        for (slotIndex = 0; slotIndex < ((LevelInfo *)data_0209f340)->count; slotIndex++, slot++) {
            if (slot->mActive != 0 && slot->mTransformer != 0)
                slot->mTransformer->Advance();
        }
        mSysTracker.Update();
    }
    func_020391f0();
    RenderFog();
    {
        dExtSimpleModel_c *skybox = mSkyboxModel;
        if (skybox != 0) {
            Matrix4x3 *transform = &skybox->mat4x3;
            Vector3 *cameraPosition = &((dCamera_c *)data_0209f318)->pos;
            transform->t.x = cameraPosition->x >> 3;
            transform->t.y = cameraPosition->y >> 3;
            transform->t.z = cameraPosition->z >> 3;
            mSkyboxModel->Render(0);
        }
    }
    RenderModel();
    dExtShadowModel_c::RenderAll();
    RenderModelTransparent();
    {
        int isVsMode = (data_0209f2d8 == 1);
        if (isVsMode) {
            if (data_0209fc9c == 0 && data_0209f2c4 == 0) {
                RenderVsModeCountdown();
                RenderVsModeNewStar();
                if (data_0209f294 != 0 && data_0209f290 != 2)
                    RenderBouncingArrows();
            }
            func_0203083c();
        }
    }
    dCc_c::Process();
    func_ov001_020aaf40();
    {
        int positionXor = 0;
        int playerFlagBits = 0;
        int playerIndex = 0;
        int playerCount = data_0209f21c;
        for (; playerIndex < playerCount; playerIndex++) {
            Player *player = (Player *)data_0209f394[playerIndex];
            if (player != 0) {
                u8 playerFlags = player->mHasWings;
                int *position = (int *)((long long)(int)&player->mPosX);
                positionXor ^= (u8)(position[2] ^ (position[0] ^ position[1]));
                playerFlagBits |= (u8)((playerFlags & 1) << playerIndex);
            }
        }
        func_0203db3c(0x10, positionXor);
        func_0203db3c(0x10, playerFlagBits);
    }
    if (data_0209f2c4 != 0) {
        PS_Render();
    } else if (data_0209f20c != 0 && data_0209f2d4 != 0) {
        int coinCounterY = (SublevelToLevel(data_02092124) < 0xf) ? 0x80 : 0x70;
        if (data_0209f2b0 != 0)
            coinCounterY += 0x10;
        OAM::Render(0, &OAM::COIN, 0x68, coinCounterY, -1, -1, 0);
        OAM::Render(0, &OAM::TIMES, 0x78, coinCounterY + 8, -1, -1, 0);
        RenderNumber(data_0209f260, 0x80, coinCounterY, 1, 8);
    }
    LC_Render();
    return 1;
}

// @symbol _ZN10dScStage_c8BehaviorEv
extern "C" {
void ProcessKuppaScript();
void func_02032f54();
void func_020199a4();
void func_02019a58();
void func_02011c24();
void func_02030a78();
void func_020345b0(int a);
}

static inline u32 MaskOff(u32 x, u32 m) {
    return x & ~m;
}

#pragma opt_common_subs off

int dScStage_c::Behavior() {
    data_0209b464 = data_0209b454;
    CheckCameraInput();
    CheckInput();
    if (data_0209f300 != 0)
        data_0209f300 -= 1;
    if (data_0209f2c4 == 2 && data_0209f22c == 0)
        PS_Cleanup();
    ProcessKuppaScript();
    if (data_0209f5bc->IsAtStart() == 0) {
        dExtShadowModel_c::CleanAll();
        return 1;
    }
    {
        int b = (data_0209f2d8 == 1);
        if (b == 0) {
            UpdateMessage();
        } else {
            u8 c9c = data_0209fc9c;
            if (c9c != 0) {
                func_02032f54();
                if (data_0209fcc8 >= 7u)
                    func_020199a4();
                return 1;
            }
            if (data_0209fc68 != 0) {
                int b2 = (data_0209fc68 == 6);
                if (b2) {
                    if (c9c == 0) {
                        func_02019a58();
                        func_02012790(0x123);
                        func_02011c24();
                        func_02030a78();
                        func_020345b0(0x13);
                    }
                    return 1;
                }
            }
            {
                int cnt = 0;
                int i;
                for (i = 0; i < 4; i++) {
                    char *p = (char *)data_0209f394[i];
                    if (p != 0) {
                        int t = (((Player *)p)->mLevelEntryDone != 0) ? 1 : 0;
                        if (t == 1)
                            cnt++;
                    }
                }
                if (cnt >= data_0209fc50) {
                    if (data_0209f304 == 0x28 && data_0209f2bc == 3)
                        func_02012790(0x2b);
                    if (data_0209f2c4 == 0) {
                        if (data_0209f304 != 0 || data_0209f2bc != 0) {
                            data_0209f304 = data_0209f304 - data_0208ee44;
                            if (data_0209f304 == 0 && data_0209f2bc != 0) {
                                data_0209f2bc -= 1;
                                data_0209f304 = 0x28;
                                if (data_0209f2bc != 0) {
                                    func_02012790(0x2b);
                                } else {
                                    func_02012790(0x2a);
                                    Sound::LoadAndSetMusic_Layer1(0x4d);
                                }
                            }
                        }
                    }
                }
            }
            if (data_ov002_02111188 == 0 && data_0209f204 != 0) {
                if (data_0209fc68 == 0) {
                    if (data_02092110 < 0 && data_0209d4b0 == 0) {
                        if (data_0209f294 == 0 && data_0209f290 == 0)
                            VE_Init();
                        else
                            VE_Update();
                    }
                } else {
                    dScene_c::StartSceneFade(7, 0, 0);
                    data_02092778 = 1;
                    data_0209d4b0 = 0;
                }
                Sound::StopLoadedMusic_Layer1(0x3c);
            }
        }
    }
    if (data_02092110 >= 0) {
        int lvl = SublevelToLevel(data_02092110);
        int lvl2 = SublevelToLevel(data_0209f2f8);
        int bb = (data_0209f2d8 == 2);
        if (bb == 0 && lvl <= 0xe && lvl != lvl2 && data_02092118 < 0 &&
            (data_02092110 != 0xc || data_0209f268 != 4)) {
            dScene_c::SetSceneToSpawn(4, 0);
        } else {
            dScene_c::SetSceneToSpawn(3, 0);
        }
        if ((IsLevelInsideCastle(data_0209f2f8) == 0 || IsLevelInsideCastle(data_02092110) == 0) &&
            (IsLevelTinyHugeIslandOutside(data_0209f2f8) == 0 ||
             IsLevelTinyHugeIslandOutside(data_02092110) == 0)) {
            int bb2 = (data_0209f2d8 == 2);
            if (bb2 == 0)
                Sound::StopLoadedMusic_Layer1(0x3c);
        }
    } else if (data_0209f20c != 0) {
        LC_Update();
    } else if (data_0209f2c4 != 0) {
        PS_Update();
    } else {
        u32 pi = gActivePlayerSlot;
        u16 h1 = *(u16 *)((char *)data_020a0e58 + pi * 4);
        if ((((h1 & 0x200) == 0 && (h1 & 0x100) == 0) ||
             (*(u16 *)((char *)data_020a0e5a + pi * 4) & 8) == 0) &&
            data_0209d660 == 0 && data_0209d4b0 == 0 && CanPause() != 0 && IsPauseDisabled() == 0 &&
            (u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0 &&
            MaskOff(data_0209b454, 0) == 0) {
            u8 st = data_0209f2d8;
            int b1 = (st == 1);
            if ((b1 && data_0209fc68 == 0) || (((SaveData *)data_0209caa0)->flags2 & 0x80)) {
                int b2 = (st == 2);
                if (b2 == 0) {
                    int b3 = (gRunningKuppaScript != 0);
                    if (b3 == 0) {
                        if (data_0209f300 == 0) {
                            u32 pj = gActivePlayerSlot;
                            u16 h2 = *(u16 *)((char *)data_020a0e5a + pj * 4);
                            int t8 = h2 & 8;
                            if ((t8 != 0 && (*(u16 *)((char *)data_020a0e58 + pj * 4) & 4) == 0) ||
                                (b1 == 0 && t8 == 0 && (h2 & 4) != 0)) {
                                data_0209f2c4 = 1;
                                func_02012790(2);
                                if (data_0209d4c8.mIsRunning != 0) {
                                    data_0209d4c8.StopTimer();
                                    data_0209f2a0[0] = 1;
                                }
                                if (*(u16 *)((char *)data_020a0e58 + gActivePlayerSlot * 4) & 4)
                                    data_0209f218 = 1;
                                else
                                    data_0209f218 = 0;
                                PS_Init();
                                PS_Update();
                            }
                        }
                    }
                }
            }
        }
    }
    if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0) {
        if ((data_0209b454 & ~0x20000000) == 0)
            dExtShadowModel_c::CleanAll();
    }
    return 1;
}
#pragma opt_common_subs on

// @symbol _ZN10dScStage_c10CheckInputEv
// The ROM reads the touch records through one base pointer (gTouchHeld) plus
// field offsets, not through the per-lane gTouchX/gTouchY/gTouchEdge symbols.
#define TOUCH_DATA ((TouchInfo *)gTouchHeld)

extern "C" {
u8 DecIfAbove0_Byte(u8 *p);
s16 atan2_hold_d_s_stage_methods_1(int y, int x);
}

void dScStage_c::CheckInput() {
    u8 mode = data_0209f2d8;
    int isVsMode = (mode == 1);
    if (isVsMode == false) {
        int modeIsTwo = (mode == 2);
        if (modeIsTwo != false || !(((SaveData *)data_0209caa0)->flags2 & 0x80) ||
            (data_0209d660 == 0 && (int)(gRunningKuppaScript != 0) != false)) {
            {
                CamInput *control = (CamInput *)data_0209f498;
                int playerIndex;
                for (playerIndex = 0; playerIndex < 4; playerIndex++) {
                    control->touching = 0;
                    control->delay = 0;
                    control->cnt = 0;
                    control->mag = 0;
                    control->nx = 0;
                    control->ny = 0;
                    control->ang = 0;
                    control->held = 0;
                    control->pressed = 0;
                    control++;
                }
            }
            return;
        }
    }
    CamInput *control = (CamInput *)data_0209f498;
    int playerIndex = 0;
    int playerCount = data_0209f21c;
    int touchX;
    int touchY;
    u8 *playerInputState;
    if (playerCount <= 0)
        return;
    playerInputState = data_0209f350;
    do {
        u32 held = data_020a0e58[playerIndex].held;
        u32 pressed = data_020a0e58[playerIndex].pressed;
        u8 touched = TOUCH_DATA[playerIndex].held;
        if ((touched != 0 && data_0209d660 != 0) || (*playerInputState != 1 && touched != 0)) {
            control->touching = 1;
            DecIfAbove0_Byte(&control->cnt);
            {
                int touchStarted =
                    (TOUCH_DATA[playerIndex].held != 0 && TOUCH_DATA[playerIndex].edge != 0);
                if (touchStarted != false) {
                    if (control->cnt != 0) {
                        int originY = TOUCH_DATA[playerIndex].y - control->dy;
                        int originX = TOUCH_DATA[playerIndex].x - control->dx;
                        if (originX < 0)
                            originX = 0;
                        else if (originX > 0xff)
                            originX = 0xff;
                        if (originY < 0)
                            originY = 0;
                        else if (originY > 0xbf)
                            originY = 0xbf;
                        control->tx = originX;
                        control->ty = originY;
                    } else {
                        control->delay = 5;
                    }
                }
            }
            if (DecIfAbove0_Byte(&control->delay) != 0) {
                control->tx = TOUCH_DATA[playerIndex].x;
                control->ty = TOUCH_DATA[playerIndex].y;
                control->mag = 0;
                control->nx = 0;
                control->ny = 0;
                control->ang = 0;
            } else {
                int deltaX;
                int deltaY;
                int distance;
                int maxDistance;
                touchX = TOUCH_DATA[playerIndex].x;
                touchY = TOUCH_DATA[playerIndex].y;
                deltaX = touchX - control->tx;
                deltaY = touchY - control->ty;
                control->dx = deltaX;
                control->dy = deltaY;
                {
                    s64 distanceSquared = (s64)deltaX * deltaX;
                    distanceSquared += (s64)deltaY * deltaY;
                    distance = cstd::sqrt(distanceSquared);
                }
                if (control->mode == 2)
                    maxDistance = 0x30;
                else
                    maxDistance = 0x24;
                if (distance > maxDistance) {
                    int scale = cstd::fdiv(maxDistance, distance);
                    deltaX = FX_MUL(deltaX, scale);
                    control->tx = touchX - deltaX;
                    deltaY = FX_MUL(deltaY, scale);
                    control->ty = touchY - deltaY;
                    distance = maxDistance;
                } else if (distance < 4) {
                    distance = 0;
                }
                control->mag = (distance << 12) / maxDistance;
                control->nx = (deltaX << 12) / maxDistance;
                control->ny = (deltaY << 12) / maxDistance;
                control->ang = cstd::atan2(deltaX, deltaY);
            }
        } else {
            int touchEnded;
            int directionButtons;
            touchEnded = (touched == 0 && TOUCH_DATA[playerIndex].edge != 0);
            if (touchEnded != false)
                control->cnt = 5;
            DecIfAbove0_Byte(&control->cnt);
            control->touching = 0;
            directionButtons = 0;
            if (control->mode == 0)
                directionButtons = data_020a0e58[playerIndex].held & 0xf0;
            control->ang = data_020756b0[directionButtons >> 4];
            if (directionButtons != 0) {
                control->mag = 0x1000;
                control->nx = data_02082214[((u16)control->ang >> 4) * 2];
                control->ny = data_02082214[((u16)control->ang >> 4) * 2 + 1];
            } else {
                control->mag = 0;
                control->nx = 0;
                control->ny = 0;
            }
        }
        {
            int skipButtonMapping = (gRunningKuppaScript != 0);
            if (skipButtonMapping == false) {
                u16 *buttonMap = data_0209214c[control->mode];
                u32 buttonIndex = 0;
                do {
                    if (held & 1)
                        control->held |= *buttonMap;
                    held >>= 1;
                    if (pressed & 1)
                        control->pressed |= *buttonMap;
                    pressed >>= 1;
                    buttonMap++;
                    buttonIndex++;
                } while (buttonIndex < 0x10);
                if (control->mode != 2 && (control->held & 0x300) == 0x300) {
                    control->held |= 0x4000;
                    if (control->pressed & 0x300) {
                        control->pressed |= 0x4000;
                    }
                }
            }
        }
        playerInputState++;
        playerIndex++;
        control++;
    } while (playerIndex < data_0209f21c);
}

// @symbol _ZN3Fog4InitEtii
/* Fills the density ramp from a near/far pair, then writes the fog registers.
   The ramp is walked with a running `p`, not `mDensity[i]`: MEASURED,
   the indexed spelling changes the function's size. */
void Fog::Init(unsigned short color, Fix12i nearv, Fix12i farv) {
    int dist;
    int step;
    int i;
    char *p; /* walks mDensity */

    this->mEnabled = 1;
    dist = 0x28000 - (nearv << 4);
    this->mColor = color;
    this->mShift = 6;
    this->mOffset = 0;
    if (dist >= 0x80000)
        dist = 0x7ffff;
    step = (0x100000 - (farv << 4) - dist) >> 5;
    i = 0;
    p = ((char *)this);
    while (i < 0x20) {
        if (dist <= 0) {
            *p = 0;
        } else if (dist > 0x80000) {
            *p = 0x7f;
        } else {
            *p = (unsigned char)(dist >> 12);
        }
        i++;
        p++;
        dist += step;
    }
}

// @symbol SetTouchScreenDelay
extern "C" void SetTouchScreenDelay(void) {
    CamInput *p = (CamInput *)data_0209f498;
    int i;
    for (i = 0; i < data_0209f21c; i++) {
        p->delay = 5;
        p++;
    }
}

// @symbol ResetInput
extern "C" u8 GetControllerMode(int a0);

extern "C" void ResetInput(void) {
    CamInput *p = (CamInput *)data_0209f498;
    int i;
    for (i = 0; i < data_0209f21c; i++) {
        u8 mode;
        int m = (int)(data_0209f2d8 == 1);
        if (m == 0)
            mode = p->mode;
        else
            mode = GetControllerMode(i);
        func_0205a588(p, 0, 0x18);
        p->mode = mode;
        p->delay = 5;
        p++;
    }
}

// @symbol _ZN10dScStage_c16CleanupResourcesEv
/* dScStage_c::CleanupResources() @ 0x0202c9a8 (arm9, size 0x264)
 * Releases the level file handles, tears down fader/mesh-collider/message
 * state and unloads the level overlays/archive. Returns 1.
 */
extern "C" {
void EndKuppaScript(void);
void UnloadArchive(int);
// local extern: the fader array's destructor is passed by address to
// func_02073244 here and to func_02073470 in InitResources, which member
// syntax cannot express.
void _ZN9dFdWipe_cD1Ev(void *thiz);
}

/* Destroy through vtable slot 1 (D0). */
static inline void DestroyVirt(void **o) {
    if (o)
        ((void (*)(void *))((void **)o[0])[1])((void *)o);
}

int dScStage_c::CleanupResources() {
    u32 fileIndex;

    for (fileIndex = 0; fileIndex < 12; fileIndex++)
        ((SharedFilePtr *)(data_020756f0[fileIndex]))->Release();

    {
        int atEnd = (data_0209f2d8 == 2);
        if (!atEnd)
            Sound::ResetPlayerVoiceGroup();
    }
    EndKuppaScript();
    data_0209d4a8 = 0;

    {
        dExtSimpleModel_c *skybox = mSkyboxModel;
        if (skybox)
            delete skybox;
    }

    {
        StageTexAnimSlot *slot = mTexAnimSlots;
        int slotIndex;
        for (slotIndex = 0; slotIndex < ((LevelInfo *)data_0209f340)->count; slotIndex++, slot++) {
            dExtAnmTexSRT_c *transformer = slot->mTransformer;
            if (transformer)
                delete transformer;
            {
                MinimapChangeNode *block = (MinimapChangeNode *)slot->mBlockList;
                while (block) {
                    MinimapChangeNode *next = block->next;
                    Memory::operator_delete2(block);
                    block = next;
                }
            }
        }
    }

    data_0209f320 = 0;
    data_0209f314 = 0;
    dScene_c::SetAndStopColorFader();
    data_0209d4b0 = 0;
    func_02073244(data_0209f324, 0x60, 8, _ZN9dFdWipe_cD1Ev);
    data_0209f324 = 0;
    CleanCommonModelDataArr();
    mMeshCollider.Disable();
    dScStage_c::ResetMeshColliders();
    Deallocate(func_01ffb0c8(&mMeshCollider));
    UnloadLevelOverlays(data_0209f2f8);
    if (data_0209f2f8 == 1)
        UnloadArchive(7);

    {
        int playerIndex;
        for (playerIndex = 0; playerIndex < 4; playerIndex++)
            data_0209f394[playerIndex] = 0;
    }
    data_0209f318 = 0;
    data_0209f2c4 = 0;
    Message::ResetAllGlobals();

    {
        u8 mode = data_0209f2d8;
        int resetSequenceCounts = 0;
        data_0209f2e0 = 0;
        data_0209f2d4 = 0;
        data_0209f244 = 0;
        data_0209f2b0 = 0;
        data_0209f20c = 0;
        if (mode == 1)
            resetSequenceCounts = 1;
        if (resetSequenceCounts) {
            Sound::Player::SetPlayableSeqCount(2, 2);
            Sound::Player::SetPlayableSeqCount(3, 1);
        }
    }
    return 1;
}

// @symbol _ZN10dScStage_c13InitResourcesEv
// MATCHING. dScStage_c::InitResources @ 0x0202cc0c (arm9, size 0xa84), mwccarm 2004/b56.
//
// This function sat at a declared floor of 3-4 words for two campaigns. The residue was
// never an allocator preference: GetSoundGroupID takes TWO arguments and this file
// declared it with one. The callee (0x0202de24) reads r1 before it ever writes it --
// `cmp r1,#1`, `cmp r1,#4`, `cmp r1,#2` -- so the second argument is real, and
// GetSoundGroupID (later in this file) is defined as `(int group, int sub)`. A `bl` is
// byte-identical whether or not an argument is passed, so no byte gate, linkcheck or
// fdiff run could see the error.
//
// Passing data_0209f220 (already loaded and compared at +0x514/+0x520) keeps that value
// live in r1 all the way to the call at +0x570. That blocks r1 for the `bank` web, so
// bank is coloured r2 (`mov r2,#0x36` at +0x524, `ldrb r2,[r1,r2]` at +0x580) and the
// join copy `mov r1,r2` at +0x584 becomes necessary -- all three residual words at once.
//
// The levers are coupled: the old draft's widening cast on the LoadGroupAndSetBank
// argument existed only to buy back the size the missing copy cost. With the copy
// restored it overshoots, so the plain spelling is the matching one.
//   (2-arg, plain) = MATCH   (2-arg, widened) = 1   (1-arg, plain) = 0xa80   (1-arg, widened) = 3
//
// The two pairs of stores below (f284/f294 and f2d4/f20c) are in ROM statement order.
// Swapping either pair is byte-identical -- the literal-pool words are relocations and
// therefore wildcards to the byte gate -- but sends each address to the wrong pool slot.
// match.py --strict-relocs is what distinguishes them; keep this order.
extern "C" {

void Enable3dEngines(void);
void ResetInput(void);
int SublevelToLevel(int i);
void UnloadArchive(int i);
void LoadTextNarcs(void);
int LoadArchive(int idx);
void Initialise3dGraphics(int arg);
void InitialiseVramGlobals(void);
void func_02039218(void);
void ResetKuppaScript(void);
int GetSoundGroupID(int level, int mode);
void LoadDebugFont(void);
int IsLevelInsideCastle(int level);
int IsLevelTinyHugeIslandOutside(int level);
int func_020308a8(void);
int func_0203da9c(void);
int func_0203da3c(void);
u32 func_0203dad4(void);
void func_0203b9b4(int *p, int v);
void *func_02073470(int a, int b, int c, void *ctor, void *dtor);
void func_ov001_020ab2e4(void);
// local extern: the fader array's constructor is passed by address to
// func_02073470, which member syntax cannot express.
void *_ZN9dFdWipe_cC1Ev(void *thiz);
}

int dScStage_c::InitResources() {
    dScStage_c *scene = this;

    if (scene->mWaitingForLoad == 0) {
        u8 mode = data_0209f2d8;
        int isVsMode = (mode == 1) ? 1 : 0;
        if (isVsMode) {
            Sound::Player::SetPlayableSeqCount(2, 5);
            int sequenceCount = 3;
            Sound::Player::SetPlayableSeqCount(sequenceCount, sequenceCount);
        }
        dScene_c::Initialise3dGraphics();
        Enable3dEngines();
        data_0209f21c = data_0208a0e0;
        data_0209f250 = (u8)func_0203da9c();
        ResetInput();
        data_0209f2c4 = 0;

        int nextLevel = SublevelToLevel(data_02092110);
        int previousLevel = SublevelToLevel(data_0209f2f8);
        u32 sublevelBit = ((u8)data_02092110 + 0xDC) & 0xFF;
        int specialSublevel = 0;
        if (sublevelBit <= 0xD) {
            if ((1 << sublevelBit) & 0x2A15)
                specialSublevel = 1;
        }
        mode = data_0209f2d8;
        int vsModeReset = (mode == 1) ? 1 : 0;

        if (vsModeReset || data_0209f26c == 2 || specialSublevel || nextLevel == 0x1D ||
            nextLevel != previousLevel) {
            int row = 0;
            data_0209f34c = 0;
            do {
                int column = 0;
                do {
                    data_0209f4f8[row][column] = 0;
                    column += 1;
                } while (column < 0x10);
                row += 1;
            } while (row < 3);

            if ((s32)data_0209f21c > 0) {
                int count = data_0209f21c;
                int idx = 0;
                u8 *p1 = (u8 *)data_0209f30c;
                u8 *vsStars = (u8 *)data_0209f310;
                int vsModeCounters = (mode == 1) ? 1 : 0;
                u8 loadMode = data_0209f26c;
                do {
                    if (vsModeCounters || (specialSublevel == 0 && loadMode != 1)) {
                        data_0209f358[idx] = 0;
                    }
                    *p1 = 0;
                    *vsStars = 0;
                    idx += 1;
                    p1 += 1;
                    vsStars += 1;
                } while (idx < count);
            }
        }

        data_0209f2fc = data_0209f26c;
        if (data_0209f2fc == 1) {
            data_02092124 = data_0209f2f8;
            data_02092118 = -1;
        }

        s8 previousSublevel = data_0209f2f8;
        data_0209f2f8 = data_02092110;
        data_0209f264 = data_0209f268;
        data_0209f220 = data_0209f1f0;
        data_02092120 = -1;
        data_0209f314 = (int)scene->mTexAnimSlots;
        data_0209f32c = 0x80000000;
        data_0209f274 = 0;
        SublevelToLevel(data_0209f2f8);

        s32 levelArchive = 0xBF;
        if (data_0209f2f8 == 0x33)
            levelArchive = 2;
        else if (data_0209f2f8 == 0x2B)
            levelArchive = 3;
        else if (data_0209f2f8 == 0x1D)
            levelArchive = 4;
        else if (data_0209f2f8 == 0x2A)
            levelArchive = 5;

        u32 archiveToUnload = 2;
        do {
            if (archiveToUnload != (u32)levelArchive) {
                UnloadArchive(archiveToUnload);
            }
            archiveToUnload += 1;
        } while (archiveToUnload <= 5);

        LoadTextNarcs();
        LoadArchive(0);
        if (data_0209f2f8 == 1) {
            mHeap::Heap_t *previousHeap = ((mHeap::Heap_t *)data_020a0eac)->SetDefault();
            LoadArchive(7);
            previousHeap->SetDefault();
        }
        if (levelArchive != 0xBF) {
            LoadArchive(levelArchive);
        }

        ::Initialise3dGraphics(0x1F);
        GX::DisableAllBanks();
        dScStage_c::SetVramBanks();

        volatile u32 *p0 = (volatile u32 *)0x04000000;
        volatile u32 *p1 = (volatile u32 *)0x04001000;
        *p0 &= 0xFFCFFFEF;
        *p1 &= 0xFFCFFFEF;
        int one = 1;
        GX::SetGraphicsMode(one, 0, one);
        GXS::SetGraphicsMode(3);
        *p0 = (*p0 & ~0x38000000) | 0x08000000;
        *(volatile u16 *)0x04000304 = (*(volatile u16 *)0x04000304 & 0xFFFFFDF1) | 0x20E;
        GX::DispOn();
        *p1 |= 0x10000;
        InitialiseVramGlobals();

        volatile u16 *bg1Control = (volatile u16 *)0x0400000A;
        *bg1Control = *bg1Control & ~3;
        *bg1Control = (*bg1Control & 0x43) | 0x1F1C;
        *bg1Control = *bg1Control & ~0x40;
        data_0209d454 = 0x18;
        data_0209f204 = 0;
        data_0209d4a8 = (void *)&data_0209f3c4;
        data_0209b454 = 0;
        func_02039218();
        ResetKuppaScript();

        int vsModeTimers = (data_0209f2d8 == 1) ? 1 : 0;
        if (vsModeTimers != 0) {
            data_0209f2bc = 3;
            data_0209f304 = 0x28;
            data_0209f308 = 0;
        } else {
            data_0209f2bc = 0;
            data_0209f304 = 0;
        }

        {
            int i;
            for (i = 0; i < 0xC; i++) {
                data_0209f40c[i] = 0;
            }
        }
        {
            int i;
            for (i = 0; i < 9; i++) {
                data_0209f3e8[i] = 0;
            }
        }
        func_ov001_020ab2e4();
        {
            int i;
            for (i = 0; i < 8; i++) {
                data_0209f3a4[i] = 0;
            }
        }

        data_0209f1f8 = 0;
        data_0209f2d0 = 0;
        data_0209f258 = 0;
        data_0209f2e8 = 0;
        data_0209f25c = 0;
        data_0209f338 = 0;
        data_02092134 = 0x44444444;
        LoadLevelOverlays(data_0209f2f8);

        int skipSoundGroup = (data_0209f2d8 == 2) ? 1 : 0;
        if (skipSoundGroup == 0) {
            s8 sublevel = *(s8 *)(unsigned)(((unsigned long long)(unsigned)&data_0209f2f8) & ~0ULL);
            int soundGroup = 0;
            u8 bank = 0x36;
            if (data_0209f220 == 2) {
                if (sublevel == 1 || sublevel == 0x33)
                    soundGroup = 0x2B;
                else if (sublevel == 0x1D)
                    soundGroup = 0x2E;
                else if (sublevel == 0x2A)
                    soundGroup = 0x2C;
                else if (sublevel == 0x2B)
                    soundGroup = 0x2D;
            }
            if (soundGroup == 0) {
                int idx = sublevel * 3;
                soundGroup = GetSoundGroupID(sublevel, data_0209f220);
                bank = data_02075769[idx];
            }
            Sound::LoadGroupAndSetBank(soundGroup, bank);
        }

        *(volatile u16 *)0x04000008 = (*(volatile u16 *)0x04000008 & ~3) | 2;
        LoadGraphics2D(data_0209f2d8 == 1, data_0209f2f8);

        {
            u32 i;
            for (i = 0; i < 0xC; i++) {
                dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)data_020756f0[i]);
            }
        }

        int vsModeFaders = (data_0209f2d8 == 1) ? 1 : 0;
        u16 *faderFileIds;
        if (vsModeFaders && levelArchive != 0xBF) {
            faderFileIds = data_02075600;
        } else if (data_0209f2f8 == 5) {
            faderFileIds = data_020755f0;
        } else {
            faderFileIds = data_020755e0;
        }

        data_0209f324 =
            func_02073470(7, 0x60, 8, (void *)&_ZN9dFdWipe_cC1Ev, (void *)&_ZN9dFdWipe_cD1Ev);
        int faderIndex = 0;
        int faderOffset = 0;
        do {
            ((dFdWipe_c *)((char *)data_0209f324 + faderOffset))
                ->LoadAndSetFile(faderFileIds[faderIndex]);
            faderIndex += 1;
            faderOffset += 0x60;
        } while (faderIndex < 7);

        data_0209f340 = (char *)data_02092208[data_0209f2f8];
        scene->LoadModel();
        data_0208ee44 = 2;
        dScStage_c::LoadClsnAndObjects(*(LVL_Overlay *)data_0209f340, data_0209f264,
                                       scene->mMeshCollider);

        int vsModeMusic = (data_0209f2d8 == 1) ? 1 : 0;
        if (vsModeMusic == 0) {
            int skipMusicSetup = (gRunningKuppaScript != 0) ? 1 : 0;
            if (skipMusicSetup == 0) {
                if (!IsLevelInsideCastle(data_0209f2f8) || !IsLevelInsideCastle(previousSublevel)) {
                    if (!IsLevelTinyHugeIslandOutside(data_0209f2f8) ||
                        !IsLevelTinyHugeIslandOutside(previousSublevel)) {
                        if (data_0209f2f8 != 2 || (((SaveData *)data_0209caa0)->flags2 & 0x200)) {
                            Sound::LoadAndSetMusic_Layer1(data_0207576a[data_0209f2f8 * 3]);
                            u8 localPlayer = data_0209f250;
                            if (((u8 *)data_0209f394[localPlayer])[0x6ff] != 0) {
                                Sound::SetMusic(localPlayer, 0x33);
                            }
                        }
                    }
                }
            }
        }

        if (func_0203da3c() != 2 && levelArchive != 0xBF) {
            UnloadArchive(levelArchive);
        }
        LoadDebugFont();
        scene->LoadFog();
        scene->LoadTextureTransformers();
        scene->LoadSkybox();
        data_0209cef0 = 1;
        scene->mSysTracker.Initialise();

        u8 f2fc = data_0209f2fc;
        data_0209f294 = 0;
        data_0209f290 = 0;
        data_0209f284 = 0;
        data_0209f300 = 0;
        if (f2fc != 1) {
            data_0209f20c = 0;
            data_0209f2d4 = 0;
        }
        data_0209d45c = 0x11;

        *p0 = (*p0 & ~0x1F00) | 0x1100;
        int skipSubDisplayLayers = (data_0209f2d8 == 2) ? 1 : 0;
        if (skipSubDisplayLayers == 0) {
            *p1 = (*p1 & ~0x1F00) | (data_0209d454 << 8);
        }
        data_0209cee8 = 0;
        dExtShadowModel_c::CleanAll();
        data_02092110 = -1;

        if (data_0209fc68 != 0) {
            scene->mWaitingForLoad = 1;
        }
    }

    if (scene->mWaitingForLoad != 0) {
        if (func_020308a8() == 0)
            return -1;
    }

    data_02092778 = 0;
    func_0203b9b4(&data_0209e650, func_0203dad4());
    data_0209f208 = 0;
    u32 idx = func_0203dad4() % 6;
    data_0209f344 = (s32)&VS_STAR_SPAWN_ORDERS[idx];

    return 1;
}

// @symbol _ZN10dScStage_c14LoadGraphics2DEbi
/* Runtime asset handles resolved through the overlay 0 file table. */
enum ZN10dScStage_c14LoadGraphics2DEbi_AssetId {
    ASSET_D_2D_FRAMEWORK_PAGE_NSC_BIN = 0x022f,
    ASSET_D_2D_FRAMEWORK_SELECT_PAUSE_NSC_BIN = 0x0230,
    ASSET_D_2D_OP_BG_CL01_NCG_BIN = 0x0239,
    ASSET_D_2D_OP_BG_CL01_NSC_BIN = 0x023a,
    ASSET_D_2D_OP_BG_CL02_NSC_BIN = 0x023b,
    ASSET_D_2D_OP_BG_CL_NCL_BIN = 0x023c,
    ASSET_D_2D_OP_OBJ_CL_NCG_BIN = 0x023d,
    ASSET_D_2D_OP_OBJ_CL_NCL_BIN = 0x023e,
    ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN = 0x025c
};

extern "C" {
void SetBg0Offset(int a, int b);
void LoadFont(u8 arg);
int GetOwnerLanguage(void);
u32 LoadCompressedFileAt(u16 fileID, void *target);
int LoadFileAt(int handle, int dest);
void LoadControllerModeText(int a);
void LoadFont3D(void);
void MultiStore16(u16 val, char *dst, int nbytes);
void SetSubBg0Offset(int a, int b);
void SetSubBg1Offset(int a, int b);
void SetSubBg2Offset(int a, int b);
}

#define BG2CNT     (*(volatile u16 *)0x400000c)
#define BG3CNT     (*(volatile u16 *)0x400000e)
#define BG0CNT_SUB (*(volatile u16 *)0x4001008)
#define BG1CNT_SUB (*(volatile u16 *)0x400100a)
#define BG2CNT_SUB (*(volatile u16 *)0x400100c)

void dScStage_c::LoadGraphics2D(bool vsMode, int sublevel) {
    char *screenMap;
    char *subBg2Screen;

    BG3CNT &= ~3;
    BG3CNT = (BG3CNT & 0x43) | 0x1100;
    BG3CNT &= ~0x40;
    SetBg0Offset(0, 0);

    BG2CNT = (BG2CNT & ~3) | 1;
    BG2CNT &= ~0x40;
    BG2CNT = (BG2CNT & 0x43) | 0xd10;

    if (vsMode == 0) {
        LoadFont(0);
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xb00f, G2::GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xac0f, G2::GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xa80f, G2::GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa40f, G2::GetBG2CharPtr());
        } else {
            LoadCompressedFileAt(0xa00f, G2::GetBG2CharPtr());
        }
        LoadFileAt(0x980b, 0x5000000);
        screenMap = (char *)G2::GetBG2ScrPtr();
        LoadCompressedFileAt(0x9808, screenMap);
        LoadCompressedFileAt(0x9809, screenMap + 0x800);
        LoadCompressedFileAt(0x980a, screenMap + 0x1000);
        LoadControllerModeText(0x280);
    } else {
        LoadFont3D();
    }

    if ((int)(data_0209f2d8 == 0) != 0 && (((SaveData *)data_0209caa0)->flags2 & 0x80) == 0 &&
        (int)(gRunningKuppaScript != 0) == 0) {
        BG0CNT_SUB &= ~3;
        BG0CNT_SUB &= ~0x40;
        BG0CNT_SUB = (BG0CNT_SUB & 0x43) | 0xd000;
        BG1CNT_SUB &= ~3;
        BG1CNT_SUB &= ~0x40;
        BG1CNT_SUB = (BG1CNT_SUB & 0x43) | 0xd400;
        BG1CNT_SUB &= ~3;
        BG1CNT_SUB &= ~0x40;
        BG2CNT_SUB = (BG2CNT_SUB & 0x43) | 0x1800;

        LoadCompressedFileAt(ASSET_D_2D_OP_BG_CL01_NCG_BIN, G2S::GetBG0CharPtr());
        LoadFileAt(ASSET_D_2D_OP_BG_CL_NCL_BIN, 0x5000400);
        LoadCompressedFileAt(ASSET_D_2D_OP_BG_CL02_NSC_BIN, G2S::GetBG0ScrPtr());
        LoadCompressedFileAt(ASSET_D_2D_OP_BG_CL01_NSC_BIN, (char *)G2S::GetBG1ScrPtr());
        LoadCompressedFileAt(ASSET_D_2D_OP_OBJ_CL_NCG_BIN, (void *)0x6600000);
        LoadFileAt(ASSET_D_2D_OP_OBJ_CL_NCL_BIN, 0x5000600);
        {
            char *clearScreenMap = (char *)G2S::GetBG2ScrPtr();
            volatile u16 fillTile = 0x10d0;
            MultiStore16(fillTile, clearScreenMap, 0x800);
        }
        data_0209d454 = 0x17;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        SetSubBg2Offset(0, 0);
        return;
    }

    BG0CNT_SUB &= ~3;
    BG0CNT_SUB &= ~0x40;
    BG0CNT_SUB = (BG0CNT_SUB & 0x43) | 0x1200;
    BG1CNT_SUB &= ~3;
    BG1CNT_SUB &= ~0x40;
    BG1CNT_SUB = (BG1CNT_SUB & 0x43) | 0xc00;
    BG2CNT_SUB = (BG2CNT_SUB & ~3) | 2;
    BG2CNT_SUB &= ~0x40;
    BG2CNT_SUB = (BG2CNT_SUB & 0x43) | 0x1300;

    if (GetOwnerLanguage() == 5) {
        LoadCompressedFileAt(0xb00d, G2S::GetBG0CharPtr());
    } else if (GetOwnerLanguage() == 4) {
        LoadCompressedFileAt(0xac0d, G2S::GetBG0CharPtr());
    } else if (GetOwnerLanguage() == 3) {
        LoadCompressedFileAt(0xa80d, G2S::GetBG0CharPtr());
    } else if (GetOwnerLanguage() == 2) {
        LoadCompressedFileAt(0xa40d, G2S::GetBG0CharPtr());
    } else {
        LoadCompressedFileAt(0xa00d, G2S::GetBG0CharPtr());
    }

    LoadFileAt(0x9807, 0x5000400);
    screenMap = (char *)G2S::GetBG1ScrPtr();
    LoadCompressedFileAt(0x9804, screenMap + 0x800);
    LoadCompressedFileAt(0x9803, screenMap + 0x1000);

    if (GetOwnerLanguage() == 5) {
        LoadCompressedFileAt(0xb009, screenMap + 0x1800);
        LoadCompressedFileAt(0xb00b, screenMap + 0x2000);
    } else if (GetOwnerLanguage() == 4) {
        LoadCompressedFileAt(0xac09, screenMap + 0x1800);
        LoadCompressedFileAt(0xac0b, screenMap + 0x2000);
    } else if (GetOwnerLanguage() == 3) {
        LoadCompressedFileAt(0xa809, screenMap + 0x1800);
        LoadCompressedFileAt(0xa80b, screenMap + 0x2000);
    } else if (GetOwnerLanguage() == 2) {
        LoadCompressedFileAt(0xa409, screenMap + 0x1800);
        LoadCompressedFileAt(0xa40b, screenMap + 0x2000);
    } else {
        LoadCompressedFileAt(0xa009, screenMap + 0x1800);
        LoadCompressedFileAt(0xa00b, screenMap + 0x2000);
    }

    subBg2Screen = (char *)G2S::GetBG2ScrPtr();
    LoadCompressedFileAt(0x9805, subBg2Screen);
    LoadCompressedFileAt(0x9806, subBg2Screen + 0x1000);

    if (vsMode != 0) {
        if (data_0209fc68 != 0) {
            return;
        }
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xb00f, G2::GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xac0f, G2::GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xa80f, G2::GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa40f, G2::GetBG2CharPtr());
        } else {
            LoadCompressedFileAt(0xa00f, G2::GetBG2CharPtr());
        }
        LoadFileAt(0x980b, 0x5000000);
        LoadCompressedFileAt(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN,
                             (char *)G2::GetBG2ScrPtr() + 0x1800);
        LoadCompressedFileAt(0x9801, screenMap + 0x2800);
        return;
    }

    if (SublevelToLevel(sublevel) == 0x1d) {
        volatile u16 blankTile = 0;
        MultiStore16(blankTile, screenMap, 0x40);
        LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_SELECT_PAUSE_NSC_BIN, screenMap + 0x40);
        LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_PAGE_NSC_BIN,
                             (char *)G2::GetBG2ScrPtr() + 0x1800);
        LoadCompressedFileAt(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN,
                             (char *)G2::GetBG2ScrPtr() + 0x2800);
        LoadCompressedFileAt(0x9802, screenMap + 0x2800);
        return;
    }

    LoadCompressedFileAt(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN,
                         (char *)G2::GetBG2ScrPtr() + 0x1800);
    LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_SELECT_PAUSE_NSC_BIN, screenMap);
}

// @symbol _ZN10dScStage_c19BeforeInitResourcesEv
/* Slot 1 keeps the fader/sound reset and skips Scene's graphics setup.
 * The boolean result and this pointer pass through the ROM's tail call. */
bool dScStage_c::BeforeInitResources() {
    return ResetFadersAndSound();
}

// @symbol IsLevelTinyHugeIslandOutside
extern "C" int IsLevelTinyHugeIslandOutside(int level) {
    return level == 0x18 || level == 0x19;
}

// @symbol IsLevelInsideCastle
extern "C" int IsLevelInsideCastle(int level) {
    if (level == 2 || level == 4 || level == 5 || level == 0x32)
        return 1;
    return 0;
}

// @symbol GetSoundGroupID
extern "C" int GetSoundGroupID(int group, int sub) {
    unsigned char v = data_02075768[group * 3];
    if (group != 6)
        return v;
    if (sub == 1 || sub == 4)
        return 4;
    if (sub == 2)
        return 5;
    return v;
}

// @symbol _Z19UnloadLevelOverlaysi
/* Unloads the per-level overlay (if any) then the object overlays. A free
 * C++ function: the ROM symbol is the compiler's mangling of
 * UnloadLevelOverlays(int). */
extern "C" void UnloadOverlay(int ovID);

void UnloadLevelOverlays(int levelID) {
    if (data_02092130 != -1) {
        UnloadOverlay(data_02092130);
        data_02092130 = -1;
    }
    LoadOrUnloadObjectOverlays(UnloadOverlay, levelID);
}

// @symbol func_0202deac
extern "C" void func_0202deac(int levelID) {
    LoadLevelOverlays(levelID);
    data_0209f278 = 1;
}

// @symbol _Z17LoadLevelOverlaysi
void LoadLevelOverlays(int level) {
    if (data_0209f278) {
        if (data_02092130 != data_020758c8[level])
            Crash();
        data_0209f278 = 0;
        return;
    }
    int ov = data_020758c8[level];
    if (ov != -1) {
        LoadOverlay(ov);
        data_02092130 = ov;
    }
    LoadOrUnloadObjectOverlays(LoadOverlay, level);
}

// @symbol _Z26LoadOrUnloadObjectOverlaysPFviEi
void LoadOrUnloadObjectOverlays(void (*fn)(int), int idx) {
    unsigned int i;
    int b = (data_0209f2d8 == 1);
    if (b != 0)
        return;
    for (i = 0; i < 7; i++) {
        b = (data_0209f2d8 == 1);
        if (b == 0 || i == 6) {
            unsigned char v = data_02075998[idx][i];
            if (v != 0) {
                fn(data_02075804[i].t[v]);
            }
        }
    }
    if (idx == 0x24 || idx == 0x26 || idx == 0x28) {
        fn((int)&OVERLAY_60_ID);
        return;
    }
    b = (data_0209f2d8 == 1);
    if (b == 0) {
        fn((int)&OVERLAY_98_ID);
    }
    fn((int)&OVERLAY_102_ID);
}

/* The STAGE registry factory. Every instruction falls out of the one `new`:
 * fBase_c::operator new(sizeof(dScStage_c)), fBase_c's C2, the inlined
 * dBase_c and dScene_c vptr stores and pauseFlags ORs, this class's vptr,
 * then the out-of-line constructors of mSysTracker, mModel and
 * mMeshCollider in member order. The null check is `new`'s own.
 * Reconstructed source-style name (later EAD lineage supplies classInit);
 * historical alias Stage_Spawn. */
// @symbol dScStage_c_classInit
extern "C" dScStage_c *dScStage_c_classInit() {
    return new dScStage_c();
}
