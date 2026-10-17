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
// - OAM::Render, OAM::RenderSub, Sound::PlaySub, cstd::atan2, Fog::Init and
//   TextureTransformer::SetFile stay mangled (Fix12 by value); the OAM
//   sprite records (COIN, TIMES, VS_NEW_STAR_APPEARED, ...) keep their
//   mangled data names because OAM.h declares no static data. RenderNumber
//   keeps a local extern spelled over the member's mask-emitting parameter
//   conversions; G2x::SetBlendBrightness, G3X::SetFog and
//   dScStage_c::LoadGraphics2D stay mangled where the ROM passes a raw int
//   the header's u16/bool parameter would narrow. G2:: stays mangled
//   (decl_common.h's `extern int G2[]` hides the namespace).
// - LoadSkybox and LoadTextureTransformers spell `new` as _Znwj plus a
//   null-guarded constructor call, which a new-expression does not emit; the
//   fader array passes dFdWipe_c's constructor and destructor by address.
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
enum dScStage_c_AssetId {
    ASSET_D_2D_UPR_COURSE_CLEAR_MESSAGE_NCL_BIN = 0x025a
};

#include "types.h"
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
#include "Model.h"
#include "dCamera_c.h"
#include "Particle__SysTracker.h"
#include "Fog.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dFdWipe_c.h"
#include "SaveData.h"
#include "G3X.h"
#include "TextureTransformer.h"
#include "dExtFrameCtrl_c.h"
#include "dExtShadowModel_c.h"
#include "dCc_c.h"
#include "OAM.h"
#include "Message.h"
#include "Memory.h"
#include "Heap.h"

struct OamAttr;
struct Matrix2x2;

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
unsigned int func_02012790(unsigned int snd);
int func_02029408(void);
int LoadFile(int handle);
void SetControllerMode(unsigned char m);
unsigned int LoadCompressedFileAt(unsigned short id, void *dst);
int GetSoundMode(void);
void SetSoundMode(int m);
void TurnBacklightOn(void);
void TurnBacklightOff(void);
int IsStarCollected(int lv, int star);

// local extern: Message.h's s8 level parameter re-narrows the SublevelToLevel result (LC_Update differs).
void _ZN7Message21DisplayLevelClearTextEta(int m, unsigned char lvl);
// local extern: Message::Display is a non-static member; this call passes the message id in r0 with no receiver.
void _ZN7Message7DisplayEj(int m);
// local extern: Message.h's u16 parameter re-narrows the s16 table entry (PS_Update differs by one word).
void _ZN7Message11DisplayTextEt(int m);
// local extern: Message.h's u16 parameter changes the sign-extended (s16) argument codegen (PS_Update differs).
void _ZN7Message22DisplayOptionsMenuTextEt(int m);
bool _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int fix, int e);
unsigned int _ZN5Sound12PlayBank3_2DEj(unsigned int a);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int a);
// local extern: SaveData.h's (s8, u8) parameters re-narrow both arguments (LC_Update differs).
void _ZN8SaveData21SetCoinRecordIfHigherEah(int a, int b);
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short *p, int val, short amt);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, unsigned int a, unsigned int b);
void _ZN3OAM14BOUNCING_ARROWE(void);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, void *attr, int x, int y, int a, int cc, int fx, int fy, int rot, int mode);
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
extern void *_ZN3OAM21CONTROLLER_MODE_TEXTSE[];
extern OamAttr *_ZN3OAM7NUMBERSE[];
extern char _ZN3OAM8VS_PAUSEE;
extern char _ZN3OAM19ARROW_POINTING_LEFTE;
extern char _ZN3OAM20ARROW_POINTING_RIGHTE;
extern char _ZN3OAM13MM_SMALL_STARE;
extern char _ZN3OAM5PAUSEE;
extern char _ZN3OAM9TINY_STARE;
extern char _ZN3OAM25SMALL_ARROW_POINTING_LEFTE;
extern char _ZN3OAM26SMALL_ARROW_POINTING_RIGHTE;
extern char _ZN3OAM16SMALL_STAR_EMPTYE;
extern OamAttr _ZN3OAM4COINE;
extern OamAttr _ZN3OAM5TIMESE;
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
struct MinimapState {
    char _pad[0x14];
    volatile u32 f14, f18, f1c, f20;
};
/* SetNextLevel's return table: level, return level, entrance. */
struct LevelReturnEntry {
    s8 a;
    s8 b;
    u8 c;
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
/* The level record data_0209f340 points at, as far as the texture-animation
   passes read it: the animation table at +0x10 and its count at +0x14. */
struct LevelTexAnim {
    int x;
    void *bta;
    char pad[4];
};
struct LevelInfo {
    char pad[0x10];
    LevelTexAnim *entries;
    unsigned char count;
};
struct UnkVis {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
    virtual void v6();
    virtual int v7();
};

extern "C" {
extern int OVERLAY_60_ID;
extern int OVERLAY_98_ID;
extern int OVERLAY_102_ID;
extern char VS_STAR_SPAWN_ORDERS[][0xC];
extern int _ZN3OAM20VS_NEW_STAR_APPEAREDE[];
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
extern UnkVis *data_0209f5bc;
extern unsigned short data_0209f5e8;
extern int data_0209fc48;
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
// And to _ZN4cstd6strcmpEPKcS1_.
namespace cstd {
int strcmp(const char *a, const char *b);
int sqrt(unsigned long long v);
int fdiv(int a, int b);
}

// Outside extern "C" so the namespace mangles to _ZN3G2S12GetBG1ScrPtrEv.
namespace G2S {
u16 *GetBG1ScrPtr();
char *GetBG0CharPtr();
char *GetBG0ScrPtr();
char *GetBG2ScrPtr();
}

// The GX/GXS/Sound entry points InitResources and CleanupResources call that no
// header declares yet.
namespace GX {
void DisableAllBanks();
void SetGraphicsMode(int, int, int);
void DispOn();
}
namespace GXS {
void SetGraphicsMode(int);
}
namespace Sound {
void ResetPlayerVoiceGroup();
void LoadGroupAndSetBank(int group, int bank);
void SetMusic(u32 player, u32 music);
}

#define REG16(a) (*(volatile u16 *)(a))
#define REG32(a) (*(volatile u32 *)(a))
#define TOUCHP(off) ((u8 *)(gTouchHeld + (off)))

#pragma defer_codegen off

// @symbol _ZN10dScStage_cD1Ev, _ZN10dScStage_cD0Ev
/* Destroy through dScStage_c's own three fields -- dBgW_Kc at 0x91c, Model at
 * 0x86c, Particle::SysTracker at 0x50 -- in reverse declaration order, then
 * dScene_c (now inline-defined in dScene_c.h, so its vptr store and dBase_c's
 * inline in turn), then fBase_c's subobject destructor, called rather than
 * inlined because fBase_c's is not. The deleting variant destroys the same
 * chain and then hands the object back through dScene_c's inline operator
 * delete -- declared there rather than on dBase_c, because mwcc only inlines
 * it when found on the class itself or its IMMEDIATE base, which for
 * dScStage_c is dScene_c. One definition emits both. */
dScStage_c::~dScStage_c()
{
}

// @symbol _ZN10dScStage_c9VE_UpdateEv
void dScStage_c::VE_Update()
{
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
            if (gTouchEdge[off]) ok = 1;
        }
        if (ok == 0) {
            if (IsButtonInputValid() == 0) return;
        }
        idx2 = gActivePlayerSlot;
        a = gTouchX[idx2 * 4];
        off2 = idx2 * 4;
        if (((u8)(a - 8) < 0xf0 && (u8)(gTouchY[off2] - 0x38) < 0x20)
            || IsButtonInputValid() != 0) {
            if (data_0209f2e0 == 0) {
                data_0209f244 = data_0208ee44 << 2;
            }
            data_0209f2e0 = 0;
            data_0209f22c = data_0208ee44 << 3;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f290 = 1;
            data_ov002_02111150 = 1;
            func_02012790(0x9a);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0xb0) return;
            return;
        } else {
            u8 idx3 = gActivePlayerSlot;
            int off3 = idx3 * 4;
            if ((u8)(gTouchX[idx3 * 4] - 8) >= 0xf0) return;
            if ((u8)(gTouchY[off3] - 0x68) >= 0x20) return;
            if (data_0209f2e0 == 1) {
                data_0209f244 = data_0208ee44 << 2;
            }
            data_0209f2e0 = 1;
            dScStage_c::UpdateMenuButtons(0);
            data_0209f22c = data_0208ee44 << 3;
            data_0209f290 = 1;
            data_ov002_02111150 = 1;
            func_02012790(0x9b);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0) return;
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
void dScStage_c::VE_Init()
{
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
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4000050, a | 0x20, -7);
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short *)0x4001050, b | 0x20, -7);
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
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)_ZN3OAM14BOUNCING_ARROWE, 0x40, r4,
                                                    -1, -1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)_ZN3OAM14BOUNCING_ARROWE, 0x80, r4,
                                                    -1, -1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)_ZN3OAM14BOUNCING_ARROWE, 0xc0, r4,
                                                    -1, -1, 0x1000, 0x1000, 0, -1);
    } else {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)_ZN3OAM14BOUNCING_ARROWE, 0xc, r4,
                                                    -1, -1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)_ZN3OAM14BOUNCING_ARROWE, 0xf4, r4,
                                                    -1, -1, 0x1000, 0x1000, 0, -1);
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
        void* p = (void*)LoadFile(ASSET_D_2D_UPR_COURSE_CLEAR_MESSAGE_NCL_BIN);
        _ZN2GX11LoadOBJPlttEPKvjj((void*)((char*)p + (((unsigned int)data_0209f2a8 >> 2) << 5)), 0x1c0, 0x10);
        Deallocate(p);
    }

    if (SublevelToLevel(data_02092124) >= 0xf && SublevelToLevel(data_02092124) < 0x15) {
        if (GetOwnerLanguage() == 5)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cea8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 4)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cee8, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 3)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf28, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 2)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf78, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
        else
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cb14, 0x80, 0x38, -1, -1, 0x1000, 0x1000, 0, -1);
    }

    if (data_0209f2b0 != 0) {
        if (data_0209f20c != 0 && SublevelToLevel(data_02092124) >= 0xf) {
            if (GetOwnerLanguage() == 5)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ce90, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 4)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ced0, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 3)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf08, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 2)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cfa0, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210caf4, 0x80, 0x70, -1, -1, 0x1000, 0x1000, 0, -1);
        } else {
            if (GetOwnerLanguage() == 5)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ce90, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 4)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210ced0, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 3)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cf08, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else if (GetOwnerLanguage() == 2)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210cfa0, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void*)&data_ov002_0210caf4, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }

    if (data_0209f2d4 == 3)
        RenderBouncingArrows();
}
// @symbol _ZN10dScStage_c9LC_UpdateEv
void dScStage_c::LC_Update()
{
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
        _ZN7Message21DisplayLevelClearTextEta(0x27a, SublevelToLevel(data_02092124));
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
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
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
                _ZN5Sound12PlayBank3_2DEj(0x6e);
            }
            if (data_0209f358[0] == data_0209f260) {
                if (data_0209f2ac != 0) {
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
                    data_0209f280 = 1;
                }
                data_0209f22c = 0x1e;
                if (level < 0xf) {
                    u8 coins = data_0209f260;
                    if (SaveData::GetCoinRecord(level) >= coins)
                        return;
                    _ZN8SaveData21SetCoinRecordIfHigherEah(level, coins);
                    data_0209f2b0 = 1;
                    func_02012790(0x22);
                    return;
                }
                if (level < 0xf)
                    return;
                if (level >= 0x15)
                    return;
                func_02012790(0x22);
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
                data_0209f280 = 1;
                return;
            }
            func_02012790(0x15);
            return;
        }
        if (data_0209f260 == 0 && level >= 0xf && level < 0x15) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x14, 0x7f, 0x15666, 0);
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
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x4000050, t6 | 0x20, -7);
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x4001050, t5 | 0x20, -7);
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
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x23, 0x7f, 0, 0x15666, 0);
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
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0xa);
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
void dScStage_c::CheckCameraInput()
{
    CamInput* cam = (CamInput *)data_0209f498;
    if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0 && data_0209d660 == 0) {
        int i = 0;
        int n1 = data_0209f21c;
        if (n1 > 0) {
        u8* st = data_0209f350;
        do {
            cam->pressed = 0;
            cam->held = 0;
            if (((TouchInfo *)gTouchHeld)[i].held != 0) {
                if (*(int *)((u8 *)data_0209f318 + 0x154) & 0x1000) {
                    u8 state = *((u8*)data_0209f498 + i * 0x18 + 0x16);
                    if ((state == 0 && ((((TouchInfo *)gTouchHeld)[i].x <= 0x58 && ((TouchInfo *)gTouchHeld)[i].y >= 0x8a) || (((TouchInfo *)gTouchHeld)[i].x >= 0xa7 && ((TouchInfo *)gTouchHeld)[i].y >= 0x8a)))
                        || (state == 2 && ((TouchInfo *)gTouchHeld)[i].x >= 0xa7 && ((TouchInfo *)gTouchHeld)[i].y >= 0x9a)) {
                        if (((((TouchInfo *)gTouchHeld)[i].held && ((TouchInfo *)gTouchHeld)[i].edge) ? 1 : 0) || *st == 1) {
                            u16 mask;
                            if ((state == 0 && ((TouchInfo *)gTouchHeld)[i].x < 0x2b) || (((TouchInfo *)gTouchHeld)[i].x >= 0xa7 && ((TouchInfo *)gTouchHeld)[i].x < 0xd1))
                                mask = 0x200;
                            else if ((state == 0 && ((TouchInfo *)gTouchHeld)[i].x >= 0x30 && ((TouchInfo *)gTouchHeld)[i].x <= 0x58) || ((TouchInfo *)gTouchHeld)[i].x >= 0xd6)
                                mask = 0x100;
                            else {
                                mask = data_0209f368[i];
                                if (mask == 0) mask = 0x200;
                            }
                            if ((((TouchInfo *)gTouchHeld)[i].held && ((TouchInfo *)gTouchHeld)[i].edge) ? 1 : 0)
                                *(u16*)(((int)cam + 6)) |= mask;
                            else
                                *(u16*)(((int)cam + 6)) |= mask & (mask ^ data_0209f368[i]);
                            data_0209f368[i] = mask;
                            *(u16*)(((int)cam + 4)) |= mask;
                            *st = 1;
                        }
                    } else if (*st == 1) {
                        *st = 0;
                        data_0209f368[i] = 0;
                    }
                }
            } else {
                if (*st == 1) *st = 0;
                data_0209f368[i] = 0;
            }
            {
                u8 state2 = *((u8*)data_0209f498 + i * 0x18 + 0x16);
                if (state2 != 0) {
                    if (((*((u8*)gTouchHeld + i * 4) && ((TouchInfo *)gTouchHeld)[i].edge) ? 1 : 0)
                        && ((state2 == 1 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd7 && ((TouchInfo *)gTouchHeld)[i].y >= 0x8d)
                            || (state2 == 2 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd7 && ((TouchInfo *)gTouchHeld)[i].y >= 0x73 && ((TouchInfo *)gTouchHeld)[i].y < 0x97))) {
                        data_ov002_02111180 = 0x10;
                        *(u16*)(((int)cam + 6)) |= 0x8000;
                        *st = 2;
                        cam->touching = 0;
                    } else if (*st == 2) {
                        if ((state2 == 1 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd5 && ((TouchInfo *)gTouchHeld)[i].y >= 0x8d)
                            || (state2 == 2 && ((TouchInfo *)gTouchHeld)[i].x >= 0xd5 && ((TouchInfo *)gTouchHeld)[i].y >= 0x73 && ((TouchInfo *)gTouchHeld)[i].y < 0x97)) {
                            cam->touching = 0;
                        } else {
                            *st = 0;
                        }
                    }
                }
            }
            i++; cam++; st++;
        } while (i < data_0209f21c);
        }
    } else {
        int i = 0;
        u8* st;
        int n = data_0209f21c;
        if (n > 0) {
        st = data_0209f350;
        do {
            cam->pressed = 0;
            cam->held = 0;
            *st = 0;
            i++; cam++; st++;
        } while (i < n);
        }
    }
}
#pragma opt_strength_reduction on
// @symbol _ZN10dScStage_c12RenderNumberEhiibi
void dScStage_c::RenderNumber(unsigned char num, int x, int y, bool b, int p5)
{
    int leading = 0;
    Matrix2x2 *mtx = 0;
    int i;

    for (i = 0; i < 3; i++)
    {
        int digit = __aeabi_idiv(num, data_020755b8[i]);
        if (digit != 0 || leading != 0 || i == 2)
        {
            if (b == 0)
            {
                OAM::Render(true, _ZN3OAM7NUMBERSE[digit], x, y, p5, -1, mtx);
            }
            else
            {
                OAM::Render(false, _ZN3OAM7NUMBERSE[digit], x, y, p5, -1, 0);
            }
            leading = 1;
            x += 9;
        }
        num = num % data_020755b8[i];
    }
}
// @symbol _ZN10dScStage_c9PS_RenderEv
void dScStage_c::PS_Render()
{
    if (func_02029408() != 0 && data_02092144[data_0209f250] != 0 &&
        data_0209f248 != 0xb && data_0209f248 != 0xe && data_0209f248 != 0xf &&
        ((unsigned int)data_0209f248 < 7U || (unsigned int)data_0209f248 > 8U)) {
        dScStage_c::RenderBouncingArrows();
    }

    int r0v = (data_0209f2d8 == 1);
    if (r0v != 0 && data_0209fc68 == 0) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM8VS_PAUSEE, 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
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
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM19ARROW_POINTING_LEFTE, 0x12, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                OAM::RenderSub((OamAttr *)&_ZN3OAM19ARROW_POINTING_LEFTE, 0x12, 0x10);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM19ARROW_POINTING_LEFTE, 0x14, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                OAM::RenderSub((OamAttr *)&_ZN3OAM19ARROW_POINTING_LEFTE, 0x14, 0x10);
            }
            if (data_0209f238 == 2) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM20ARROW_POINTING_RIGHTE, 0xee, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                OAM::RenderSub((OamAttr *)&_ZN3OAM20ARROW_POINTING_RIGHTE, 0xee, 0x10);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM20ARROW_POINTING_RIGHTE, 0xec, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                OAM::RenderSub((OamAttr *)&_ZN3OAM20ARROW_POINTING_RIGHTE, 0xec, 0x10);
            }
        }
        if (data_0209f2c8 != 0xf) {
            var_sb = 0x50;
            var_sl = 1;
            if (var_r8 != 0) {
                do {
                    if (IsStarCollected(data_0209f2c8, var_sl) != 0) {
                        OAM::Render(false, (OamAttr *)&_ZN3OAM13MM_SMALL_STARE, var_sb, 0x60, -1,
                                    -1, 0);
                        var_r7 = (var_r7 + 1) & 0xff;
                    } else {
                        OAM::Render(false, (OamAttr *)&_ZN3OAM16SMALL_STAR_EMPTYE, var_sb, 0x60, -1,
                                    -1, 0);
                        sp18 = 1;
                    }
                    var_sb += 0x10;
                    var_sl += 1;
                } while (var_r7 != var_r8);
            }
            if (sp18 == 0 && var_r8 != 7) {
                OAM::Render(false, (OamAttr *)&_ZN3OAM16SMALL_STAR_EMPTYE, var_sb, 0x60, -1, -1, 0);
            }
            unsigned char r5v = SaveData::GetCoinRecord(data_0209f2c8);
            OAM::Render(false, &_ZN3OAM4COINE, 0x70, 0x70, -1, -1, 0);
            OAM::Render(false, &_ZN3OAM5TIMESE, 0x80, 0x78, -1, -1, 0);
            RenderNumber(r5v, 0x88, 0x70, 1, 8);
            if (IsStarCollected(data_0209f2c8, 0) != 0) {
                OAM::Render(false, (OamAttr *)&_ZN3OAM13MM_SMALL_STARE, 0xb0, 0x78, -1, -1, 0);
            }
        } else {
            OAM::Render(false, (OamAttr *)&_ZN3OAM13MM_SMALL_STARE, 0x70, 0x60, -1, -1, 0);
            OAM::Render(false, &_ZN3OAM5TIMESE, 0x80, 0x60, -1, -1, 0);
            RenderNumber(CountStarsCollectedInLevelToDisplay(0x1d), 0x88, 0x58, 1, 8);
        }
    } else if (data_0209d45c & 8) {
        if ((unsigned int)data_0209f2c8 < 0xfU) {
            unsigned char r4v = CountStarsCollectedInLevelToDisplay(data_0209f2c8);
            OAM::Render(false, (OamAttr *)&_ZN3OAM13MM_SMALL_STARE, 0x58, 0x8c, -1, -1, 0);
            OAM::Render(false, &_ZN3OAM5TIMESE, 0x68, 0x8c, -1, -1, 0);
            RenderNumber(r4v, 0x70, 0x84, 1, 1);
            unsigned char r4v2 = SaveData::GetCoinRecord(data_0209f2c8);
            OAM::Render(false, &_ZN3OAM4COINE, 0x88, 0x84, -1, -1, 0);
            OAM::Render(false, &_ZN3OAM5TIMESE, 0x98, 0x8c, -1, -1, 0);
            RenderNumber(r4v2, 0xa0, 0x84, 1, 1);
        }
        if (data_0209f248 == 0xb) {
            OAM::RenderSub((OamAttr *)&_ZN3OAM5PAUSEE, 0x80, 0x60);
        }
    }

    unsigned char f248b = data_0209f248;
    if ((unsigned char)(f248b + 0xf9) <= 1) {
        if (data_0209f2e0 == 3) {
            if (data_0209f29c == 0) {
                if (GetOwnerLanguage() == 5) {
                    OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x46,
                                ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
                } else if (GetOwnerLanguage() == 3) {
                    OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x4e,
                                ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
                } else {
                    OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x56,
                                ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
                }
            } else {
                OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0xce,
                            ((data_0209f2e0 * 5) + 6) * 8, -1, -1, 0);
            }
        } else {
            OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x26, ((data_0209f2e0 * 5) + 6) * 8,
                        -1, -1, 0);
        }
        if (GetOwnerLanguage() == 5) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov001_020abd98[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 4) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov001_020ac218[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 3) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov001_020ac64c[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov001_020acb68[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, _ZN3OAM21CONTROLLER_MODE_TEXTSE[data_0209f2dc], 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
        return;
    }

    if (f248b != 0xa) {
        return;
    }

    if (data_0209f2e0 == 2) {
        if (data_0209f29c == 0) {
            if (GetOwnerLanguage() == 5) {
                OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x46, 0xa8, -1, -1, 0);
            } else if (GetOwnerLanguage() == 3) {
                OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x4e, 0xa8, -1, -1, 0);
            } else {
                OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0x56, 0xa8, -1, -1, 0);
            }
        } else {
            OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0xce, 0xa8, -1, -1, 0);
        }
    } else {
        OAM::Render(true, (OamAttr *)&_ZN3OAM9TINY_STARE, 0xe, ((data_0209f2e0 * 6) + 7) * 8, -1,
                    -1, 0);
    }

    if (data_0209f238 == 2) {
        OAM::Render(true, (OamAttr *)&_ZN3OAM25SMALL_ARROW_POINTING_LEFTE, 0x62, 0x38, -1, -1, 0);
    } else {
        OAM::Render(true, (OamAttr *)&_ZN3OAM25SMALL_ARROW_POINTING_LEFTE, 0x64, 0x38, -1, -1, 0);
    }

    if (data_0209f238 == 1) {
        OAM::Render(true, (OamAttr *)&_ZN3OAM26SMALL_ARROW_POINTING_RIGHTE, 0xf6, 0x38, -1, -1, 0);
        return;
    }
    OAM::Render(true, (OamAttr *)&_ZN3OAM26SMALL_ARROW_POINTING_RIGHTE, 0xf4, 0x38, -1, -1, 0);
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
void dScStage_c::PS_UpdateOkAndBackButtons(bool b)
{
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
        u16 *scr = G2S::GetBG1ScrPtr() + data_020755cc[idx];
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
void dScStage_c::PS_UpdateSaveMenu(bool b)
{
  u16 v;
  int i;
  for (i = 0; i < 2; i++)
  {
    int j;
    u16 *p;
    if (b != 0)
    {
      v = 0x1000;
    }
    else
    {
      unsigned int t;
      if (data_0209f2e0 == i && data_0209f244 == 0) t = 0x2000;
      else t = 0x1000;
      v = (u16) t;
    }
    p = G2S::GetBG1ScrPtr() + data_020755c8[i];
    for (j = 0; j < 10; j++)
    {
      p[0] = (p[0] & 0x3ff) | v;
      p[0x20] = (p[0x20] & 0x3ff) | v;
      p[0x40] = (p[0x40] & 0x3ff) | v;
      p[0x60] = (p[0x60] & 0x3ff) | v;
      p++;
    }
  }
}
// @symbol _ZN10dScStage_c20PS_UpdateOptionsMenuEv
void dScStage_c::PS_UpdateOptionsMenu()
{
    int i;
    int j;
    int counter;
    u16 sel;
    u16 *scr;
    u16 v;

    sel = (data_0209f2e4 != 0) ? 0x5000 : 0x6000;
    scr = G2S::GetBG1ScrPtr() + 0xae;
    for (j = 0; j < 0xf; j++) {
        scr[0]    = (scr[0]    & 0x3ff) | sel;
        scr[0x20] = (scr[0x20] & 0x3ff) | sel;
        scr[0x40] = (scr[0x40] & 0x3ff) | sel;
        scr[0x60] = (scr[0x60] & 0x3ff) | sel;
        scr++;
    }

    for (j = 0; j < 2; j++) {
        scr = G2S::GetBG1ScrPtr() + data_020755c0[j];
        v = (data_0209f2e0 == j) ? 0x4000 : 0x3000;
        counter = 0;
        for (; counter < 0xa; counter++) {
            scr[0]    = (scr[0]    & 0x3ff) | v;
            scr[0x20] = (scr[0x20] & 0x3ff) | v;
            scr++;
        }
    }

    for (i = 0; i < 2; i++) {
        scr = G2S::GetBG1ScrPtr() + data_020755c4[i];
        v = (data_0209f2ec == i && data_0209f2cc == 0) ? 0x6000 : 0x5000;
        counter = 0;
        for (; counter < 7; counter++) {
            scr[0]    = (scr[0]    & 0x3ff) | v;
            scr[0x20] = (scr[0x20] & 0x3ff) | v;
            scr[0x40] = (scr[0x40] & 0x3ff) | v;
            scr[0x60] = (scr[0x60] & 0x3ff) | v;
            scr++;
        }
    }
}
// @symbol _ZN10dScStage_c17UpdateMenuButtonsEb
    void dScStage_c::UpdateMenuButtons(bool b)
    {
      int i;
      for (i = 0; i < data_0209f2b4; i++)
      {
        unsigned int v;
        int j;
        u16 *p;
        p = G2S::GetBG1ScrPtr() + data_0209f360[i];
        if (b == 0)
        {
          if (data_0209f2e0 == i && data_0209f244 == 0)
          {
            v = 0x2000;
          }
          else
          {
            v = 0x1000;
          }
          v = (u16) v;
        }
        else
        {
          v = 0x1000;
        }
        if (data_0209f2c4 != 0)
        {
          u8 t = (u8) (data_0209f248 + 0xf9);
          if (t <= 1)
          {
            v = (u16) (v + 0x4000);
          }
        }
        for (j = 0; j < 0x20; j++)
        {
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
void dScStage_c::PS_Cleanup(){
  if(data_0209f2a0[0]){
    data_0209d4c8.StartTimer();
    data_0209f2a0[0]=0;
  }
  data_0209d45c &= ~0xe;
  data_0209d454 &= ~3;
  if(data_0209f280==0) Sound::UnpauseMusic();
  else Sound::StopLoadedMusic_Layer1(0x3c);
  data_0209f300=0xf;
  *(short*)0x4000050=0;
  *(short*)0x4001050=0;
  data_0209f280=0;
  data_0209f2c4=0;
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
void dScStage_c::PS_Update()
{
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
        _ZN7Message7DisplayEj(data_020756d0[data_0209f2c8]);
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
                if ((a != 0) && (TOUCHP(de_off)[1] != 0)) {
                    var_r0 = 1;
                }
                if ((var_r0 != 0) && ((u32)(vx = TOUCHP(sl2 * 4)[2]) < 0x38U) && ((u32)TOUCHP(sl2 * 4)[3] < 0x20U)) {
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
                    if ((a != 0) && (TOUCHP(sl2 * 4)[1] != 0)) {
                        var_r0_2 = 1;
                    } else {
                        var_r0_2 = sp4;
                    }
                    if (var_r0_2 != 0) {
                        if (((u32)(u8)(TOUCHP(sl2 * 4)[2] - 0xC8) < 0x38U) && ((u32)TOUCHP(sl2 * 4)[3] < 0x20U)) {
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
                if ((data_0209f2c8 == 0xF) || (CountStarsCollectedInLevelToDisplay(data_0209f2c8) != 0)) {
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
        _ZN7Message11DisplayTextEt(data_020756d0[data_0209f2c8]);
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
        _ZN7Message22DisplayOptionsMenuTextEt((s16)(data_0209f234 + 0x284));
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
                _ZN7Message22DisplayOptionsMenuTextEt((s16)(data_0209f234 + 0x284));
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
                _ZN7Message22DisplayOptionsMenuTextEt((s16)(data_0209f234 + 0x284));
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
        _ZN5Sound22StopLoadedMusic_Layer1Ej(0xa);
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
 * Behavior calls it with nothing in r0. SetBlendBrightness is reached through
 * its mangled symbol `_ZN3G2x18SetBlendBrightnessEPVtts` (declared above). */
void dScStage_c::PS_Init() {
    int sl;
    u8 r7;
    u8 r8;
    u8 sb;
    int r5;
    int r4;

    sb = data_0209d454;
    if (sb & 4) {
        data_0209d454 = sb & ~4;
        sb = data_0209d454;
        *(volatile s32 *)0x04001000 = (*(volatile s32 *)0x04001000 & ~0x1f00) | (sb << 8);
    }

    r7 = data_0209f218;
    r8 = data_0209d45c;
    sl = 0;

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

    r5 = r8 ^ 0x10;
    r4 = sb ^ 0x10;

    if (r7 != 0) {
        data_0209f1ec = 7;
    } else {
        if (data_0209f2d8 == 1)
            sl = 1;
        if (sl != 0 && data_0209fc68 == 0) {
            data_0209f1ec = 4;
        } else {
            if (SublevelToLevel(data_0209f2f8) == 0x1d)
                data_0209f1ec = 0;
            else
                data_0209f1ec = 2;
        }
    }

    Sound::PauseMusic();
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x04000050, r5 | 0x20, -7);
    _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x04001050, r4 | 0x20, -7);
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
   affine words after it. */
struct SceneRelated {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkc;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1c;
    s32 unk20;
};

/* Nullary, and a member: vtable slot 2 of dScStage_c::graphCallback_c (RTTI
   si-child of dGraph_c::callback_c). Every dispatcher -- func_02018ec0,
   func_02018efc -- calls it as `ldr r1,[r0]; ldr r1,[r1,#8]; blx r1` with
   r0 = the callback object and r1 never set as an argument. Sets the BG3
   affine transform from the object's matrix. */
s32 dScStage_c::GraphCallback2() {
    SceneRelated *scene = (SceneRelated *)this;
    G2x::SetBGyAffine((volatile unsigned short *)REG_DB_BG3PA, (Matrix2x2 *)&scene->unk4,
                      scene->unk14, scene->unk18, scene->unk1c, scene->unk20);
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
    data_0209f3c4.f14 = a;
    data_0209f3c4.f18 = b;
    data_0209f3c4.f1c = c;
    data_0209f3c4.f20 = d;
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
    UnkVis *o = data_0209f5bc;
    if (o->v7() == 0) {
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
    if (data_0209f5bc->v7() != 0)
        return;
    WipeFader *o2 = (WipeFader *)((dFdWipe_c *)data_0209f324 + 6);
    data_0209d4b0 = (int)o2;
    o2->w8();
    o2->w3(0x1e, 0);
}

// @symbol FUN_020299f4
extern "C" void FUN_020299f4(void) {
    if (data_0209f5bc->v7() != 0)
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
            if (v == p->a) {
                data_0209211c = p->b;
                data_0209f200 = p->c;
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
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210cfd0, 0x80, 0x30, -1, -1,
                                                        0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 4)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210d050, 0x80, 0x30, -1, -1,
                                                        0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 3)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210d0e8, 0x80, 0x30, -1, -1,
                                                        0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 2)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210d188, 0x80, 0x30, -1, -1,
                                                        0x1000, 0x1000, 0, -1);
        else
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, _ZN3OAM20VS_NEW_STAR_APPEAREDE, 0x80,
                                                        0x30, -1, -1, 0x1000, 0x1000, 0, -1);
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
            int t = (((u8 *)data_0209f394[i])[0x711] != 0);
            if (t == 1)
                count++;
        }
    }
    if (data_0209f5bc->v5() == 0)
        return;
    if (count < data_0209fc50)
        return;
    if (data_0209f2bc != 0) {
        if (GetOwnerLanguage() == 5)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d008, 0x80, 0x30,
                                                        -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 4)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d098, 0x80, 0x30,
                                                        -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 3)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d130, 0x80, 0x30,
                                                        -1, -1, 0x1000, 0x1000, 0, -1);
        else if (GetOwnerLanguage() == 2)
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d1c8, 0x80, 0x30,
                                                        -1, -1, 0x1000, 0x1000, 0, -1);
        else
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d210, 0x80, 0x30,
                                                        -1, -1, 0x1000, 0x1000, 0, -1);
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov002_0210ca0c[3 - data_0209f2bc], 0x80,
                                                    0x50, -1, -1, 0x1000, 0x1000, 0, -1);
        return;
    }
    if (data_0209f304 == 0)
        return;
    if (GetOwnerLanguage() == 5)
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d378, 0x80, 0x50, -1,
                                                    -1, 0x1000, 0x1000, 0, -1);
    else if (GetOwnerLanguage() == 4)
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d3a0, 0x80, 0x50, -1,
                                                    -1, 0x1000, 0x1000, 0, -1);
    else if (GetOwnerLanguage() == 3)
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d3c0, 0x80, 0x50, -1,
                                                    -1, 0x1000, 0x1000, 0, -1);
    else if (GetOwnerLanguage() == 2)
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d3e0, 0x80, 0x50, -1,
                                                    -1, 0x1000, 0x1000, 0, -1);
    else
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (void *)&data_ov002_0210d420, 0x80, 0x50, -1,
                                                    -1, 0x1000, 0x1000, 0, -1);
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
    return ((short *)data_0209f340)[11];
}

// @symbol func_0202a96c
extern "C" int func_0202a96c(void) {
    return ((unsigned short *)data_0209f340)[7];
}

// @symbol func_0202a980
extern "C" int func_0202a980(void) {
    return ((unsigned short *)data_0209f340)[6];
}

// @symbol _Z23LoadMinimapChangeObjecti5Fix12IiEh
/* data_0209f314 points at mTexAnimSlots (InitResources stores it). Each
   0xc-byte slot doubles as the per-area record: HideArea/ShowArea toggle its
   mActive byte, and mBlockList heads a sorted list of these minimap-change
   records, threshold at +0, next at +0xc (the link CleanupResources walks
   when it frees them). */
struct MinimapChangeNode {
    int f0; /* 0x00 value */
    s8 f4;  /* 0x04 */
    char pad5[3];
    struct MinimapChangeNode *f8; /* 0x08 */
    struct MinimapChangeNode *fc; /* 0x0c */
};

/* The cartridge's global operator new is _Znwj, operator new(unsigned int);
   a new-expression here asks for operator new(unsigned long) (_Znwm), which
   the cartridge does not carry, so the allocation calls _Znwj by name. */
extern "C" void *_Znwj(u32 size);

extern "C" void _Z23LoadMinimapChangeObjecti5Fix12IiEh(int idx, Fix12i val, u8 h) {
    struct MinimapChangeNode *nn = (MinimapChangeNode *)_Znwj(sizeof(MinimapChangeNode));
    StageTexAnimSlot *e = (StageTexAnimSlot *)((char *)data_0209f314 + idx * 0xc);
    struct MinimapChangeNode *cur = (MinimapChangeNode *)e->mBlockList;
    struct MinimapChangeNode *prev = cur;
    if (cur != 0) {
        for (;;) {
            prev = cur;
            if (val > cur->f0)
                break;
            cur = cur->fc;
            if (cur == 0)
                break;
        }
    }
    nn->fc = prev;
    if (prev == 0) {
        nn->f8 = 0;
    } else {
        nn->f8 = prev->f8;
        prev->f8 = nn;
    }
    if (nn->f8 == 0)
        e->mBlockList = nn;
    {
        int hh = h;
        nn->f0 = val;
        if (hh == 0xf)
            hh = -1;
        nn->f4 = (u8)hh;
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
    return (((unsigned char *)data_0209f340)[0x15] & 1) != 0;
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
/* The object GetMinimapID places: its height at +0x60, its area at +0xcc. */
struct Obj {
    char pad0[0x60];
    int f60;
    char pad64[0xcc - 0x64];
    s8 fcc;
};

extern "C" int GetMinimapID(struct Obj *obj, int arg) {
    int idx = obj->fcc;
    char *g;
    int val;
    struct MinimapChangeNode *node;
    if (idx < 0)
        idx = data_02092120;
    g = (char *)data_0209f314;
    if (g != 0) {
        node = (MinimapChangeNode *)((StageTexAnimSlot *)(g + idx * 0xc))->mBlockList;
        if (node != 0) {
            val = obj->f60;
            while (node != 0) {
                int d = val - node->f0;
                if (d > 0) {
                    if (arg >= 0 && d < 0xc8000)
                        return arg;
                    break;
                }
                node = node->fc;
            }
            if (node != 0)
                return node->f4;
        }
    }
    return *(s8 *)(*(char **)(data_0209f340 + 0x10) + idx * 0xc + 8);
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
 * Model, construct it and load the file for that ID. mSkyboxModel is typed in
 * include/dScStage_c.h because of this store and the virtual destroy in
 * CleanupResources.
 *
 * The `new` is spelled as the raw _Znwj call rather than as `new Model` because
 * the ROM calls operator new and then guards the constructor on a null return,
 * which the language's own `new` expression does not emit.
 */
extern "C" {
extern void *_ZN5ModelC1Ev(void *thiz);
}

void dScStage_c::LoadSkybox() {
    u32 skyboxID = GetSkyboxID();
    if (!skyboxID)
        return;
    Model *model = (Model *)_Znwj(0x50);
    if (model != 0) {
        model = (Model *)_ZN5ModelC1Ev(model);
    }
    mSkyboxModel = model;
    u16 fileID = data_02075620[skyboxID - 1];
    mSkyboxModel->LoadAndSetFile(fileID, 0, 2);
}

// @symbol _ZN10dScStage_c22RenderModelTransparentEv
/* Model's own vtable shape as far as the two RenderModel passes need it (six
   slots: D1, D0, DoSetFile, UpdateVerts, Virtual10, Render), named to avoid
   colliding with the real ModelBase (which has only three). */
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

/* Local view of the real ModelComponents (include/ModelBase.h): `sub` is that
   type's `modelFile` (offset 0x0); `components` is its `materials` (offset
   0x4, 0x30-byte records). */
struct LocalModelComponents {
    void *sub;             // offset 0x0 -> *(sub+8) is Inner*
    Component *components; // offset 0x4
};

void dScStage_c::RenderModelTransparent() {
    LocalModelComponents *mc = (LocalModelComponents *)&mModel.data;
    Inner *arr = *(Inner **)((char *)mc->sub + 8);
    StageTexAnimSlot *q = mTexAnimSlots;
    int i;
    for (i = 0; i < ((LevelInfo *)data_0209f340)->count; i++, q++, arr++) {
        if (q->mActive != 0) {
            u8 *bp = arr->ids;
            u16 j;
            for (j = 0; j < arr->count; j++) {
                Component *e = (Component *)((char *)mc->components + *bp * 0x30);
                if ((e->flags & 0x1f0000) != 0x1f0000)
                    e->flags &= ~0x80000000;
                else
                    e->flags |= 0x80000000;
                bp++;
            }
        }
    }
    ((LocalModelVtableShape *)((char *)&mModel))->Render(&data_020755d4);
}

// @symbol _ZN10dScStage_c11RenderModelEv
void dScStage_c::RenderModel() {
    LocalModelComponents *mc = (LocalModelComponents *)((char *)&mModel.data);
    Inner *inner = *(Inner **)((char *)mc->sub + 8);
    StageTexAnimSlot *slot = mTexAnimSlots;
    int i;

    for (i = 0; i < ((LevelInfo *)data_0209f340)->count; i++) {
        u8 flag = slot->mActive;
        u8 *idx = inner->ids;

        if (flag != 0) {
            u16 j;
            for (j = 0; j < inner->count; j++) {
                u8 id = *idx;
                Component *comp = (Component *)((char *)mc->components + id * 0x30);
                idx++;
                u32 flagsTest = *(volatile u32 *)&comp->flags;
                if ((flagsTest & 0x1f0000) == 0x1f0000) {
                    u32 *p = &comp->flags;
                    *p &= ~0x80000000;
                } else {
                    u32 *p = &comp->flags;
                    *p |= 0x80000000;
                }
            }
            if (slot->mTransformer != 0)
                ((TextureTransformer *)slot->mTransformer)->Update(mModel.data);
        } else {
            u16 j;
            for (j = 0; j < inner->count; j++) {
                u8 id = *idx;
                Component *comp = (Component *)((char *)mc->components + id * 0x30);
                comp->flags |= 0x80000000;
                idx++;
            }
        }

        slot++;
        inner++;
    }

    ((LocalModelVtableShape *)((char *)&mModel))->Render(&data_020755d4);
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
extern "C" {
extern void *_ZN18TextureTransformerC1Ev(void *);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *self, void *bta, int a,
                                                                  int b, u32 c);
}

void dScStage_c::LoadTextureTransformers() {
    struct LevelTexAnim *e = ((LevelInfo *)data_0209f340)->entries;
    StageTexAnimSlot *slot = mTexAnimSlots;
    void *bmd = *(void **)((char *)&mModel.data);
    int i;
    for (i = 0; i < (int)((LevelInfo *)data_0209f340)->count; i++) {
        if (e->bta) {
            TextureTransformer::Prepare(*(BMD_File *)bmd, *(BTA_File *)e->bta);
            void *t = _Znwj(0x14);
            if (t)
                t = _ZN18TextureTransformerC1Ev(t);
            slot->mTransformer = t;
            _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(slot->mTransformer, e->bta, 0,
                                                                  0x1000, 0);
        }
        e++;
        slot++;
    }
}

// @symbol _ZN10dScStage_c9LoadModelEv
void dScStage_c::LoadModel() {
    mModel.LoadAndSetFile(*(unsigned short *)(*(char **)&data_0209f340 + 8), 1, -1);
    struct TexPalModel *list;
    unsigned int count;
    Component *node;
    unsigned int i;
    LocalModelComponents *p;

    p = (LocalModelComponents *)&mModel.data;
    list = (struct TexPalModel *)p->sub;
    node = p->components;
    count = list->count;
    i = 0;
    while (i < count) {
        unsigned int v = node->flags;
        i++;
        if ((v & 0x1f0000) != 0x1f0000) {
            node->flags = (v & ~0x3f000000) | 0x13000000;
        }
        node++;
    }
    data_0209f320 = (int)p;
}

// @symbol _ZN10dScStage_c9RenderFogEv
extern "C" {
void *func_ov002_020f1c20(void);
// local extern: G3X::SetFog takes a bool, and the ROM passes the raw byte with no conversion
void _ZN3G3X6SetFogEbiii(int b, int a1, int a2, int a3);
}

void dScStage_c::RenderFog() {
    Fog *fog = 0;
    if (data_0209f2f8 == 5)
        fog = (Fog *)func_ov002_020f1c20();
    if (fog == 0) {
        void *cam = data_0209f318;
        fog = mFog;
        if (((dCamera_c *)cam)->IsUnderwater())
            fog++;
    }
    _ZN3G3X6SetFogEbiii(fog->mEnabled, 0, fog->mShift, fog->mOffset);
    *(volatile unsigned int *)0x4000358 = fog->mColor | 0x1f0000;
    G3X::SetFogTable(fog);
}

// @symbol _ZN10dScStage_c7LoadFogEv
/* dScStage_c::LoadFog. Lays down two default ramps -- a shallow one in mFog[0] and
   a steeper one in mFog[1] -- then lets the level's own fog records, if it has
   any, overwrite them through Fog::Init. */
extern "C" {
extern void _ZN3Fog4InitEt5Fix12IiES1_(Fog *self, unsigned short color, int nearv, int farv);
}

void dScStage_c::LoadFog() {
    unsigned char a = 0;
    unsigned char b = 0x28;
    int i;
    for (i = 0; i < 0x20; i++) {
        mFog[0].mDensity[i] = a;
        mFog[1].mDensity[i] = b;
        a += 4;
        b += 2;
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
        unsigned char *src = (unsigned char *)data_0209f31c;
        Fog *dst = mFog;
        int j;
        for (j = 0; j < data_0209f258; j++) {
            if (src[0] != 1) {
                dst->mEnabled = 0;
            } else {
                unsigned short color =
                    ((src[1] >> 3) | ((src[2] >> 3) << 5)) | ((src[3] >> 3) << 10);
                _ZN3Fog4InitEt5Fix12IiES1_(dst, color, *((unsigned short *)(src + 4)),
                                           *((unsigned short *)(src + 6)));
            }
            src += 8;
            dst++;
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
        StageTexAnimSlot *s = mTexAnimSlots;
        int i;
        for (i = 0; i < ((LevelInfo *)data_0209f340)->count; i++, s++) {
            if (s->mActive != 0 && s->mTransformer != 0)
                ((TextureTransformer *)s->mTransformer)->Advance();
        }
        mSysTracker.Update();
    }
    func_020391f0();
    RenderFog();
    {
        Model *t = mSkyboxModel;
        if (t != 0) {
            int *dst = (int *)&t->mat4x3;
            int *src = (int *)((long long)(int)&((dCamera_c *)data_0209f318)->pos);
            dst[9] = src[0] >> 3;
            dst[10] = src[1] >> 3;
            dst[11] = src[2] >> 3;
            mSkyboxModel->Render(0);
        }
    }
    RenderModel();
    dExtShadowModel_c::RenderAll();
    RenderModelTransparent();
    {
        int b = (data_0209f2d8 == 1);
        if (b) {
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
        int x = 0;
        int y = 0;
        int j = 0;
        int n = data_0209f21c;
        for (; j < n; j++) {
            Player *p = (Player *)data_0209f394[j];
            if (p != 0) {
                u8 bit = p->mHasWings;
                int *q = (int *)((long long)(int)&p->mPosX);
                x ^= (u8)(q[2] ^ (q[0] ^ q[1]));
                y |= (u8)((bit & 1) << j);
            }
        }
        func_0203db3c(0x10, x);
        func_0203db3c(0x10, y);
    }
    if (data_0209f2c4 != 0) {
        PS_Render();
    } else if (data_0209f20c != 0 && data_0209f2d4 != 0) {
        int v = (SublevelToLevel(data_02092124) < 0xf) ? 0x80 : 0x70;
        if (data_0209f2b0 != 0)
            v += 0x10;
        OAM::Render(0, &_ZN3OAM4COINE, 0x68, v, -1, -1, 0);
        OAM::Render(0, &_ZN3OAM5TIMESE, 0x78, v + 8, -1, -1, 0);
        RenderNumber(data_0209f260, 0x80, v, 1, 8);
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
    if (data_0209f5bc->v5() == 0) {
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
                        int t = (*(u8 *)(p + 0x711) != 0) ? 1 : 0;
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
                    int b3 = (data_0209fc48 != 0);
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
s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
}

void dScStage_c::CheckInput() {
    u8 m = data_0209f2d8;
    int b1 = (m == 1);
    if (b1 == false) {
        int b2 = (m == 2);
        if (b2 != false || !(((SaveData *)data_0209caa0)->flags2 & 0x80) ||
            (data_0209d660 == 0 && (int)(data_0209fc48 != 0) != false)) {
            {
                CamInput *q = (CamInput *)data_0209f498;
                int k;
                for (k = 0; k < 4; k++) {
                    q->touching = 0;
                    q->delay = 0;
                    q->cnt = 0;
                    q->mag = 0;
                    q->nx = 0;
                    q->ny = 0;
                    q->ang = 0;
                    q->held = 0;
                    q->pressed = 0;
                    q++;
                }
            }
            return;
        }
    }
    CamInput *p = (CamInput *)data_0209f498;
    int i = 0;
    int cnt = data_0209f21c;
    int newx;
    int newy;
    u8 *st;
    if (cnt <= 0)
        return;
    st = data_0209f350;
    do {
        u32 held = data_020a0e58[i].held;
        u32 pressed = data_020a0e58[i].pressed;
        u8 touched = TOUCH_DATA[i].held;
        if ((touched != 0 && data_0209d660 != 0) || (*st != 1 && touched != 0)) {
            p->touching = 1;
            DecIfAbove0_Byte(&p->cnt);
            {
                int b = (TOUCH_DATA[i].held != 0 && TOUCH_DATA[i].edge != 0);
                if (b != false) {
                    if (p->cnt != 0) {
                        int ty2 = TOUCH_DATA[i].y - p->dy;
                        int tx2 = TOUCH_DATA[i].x - p->dx;
                        if (tx2 < 0)
                            tx2 = 0;
                        else if (tx2 > 0xff)
                            tx2 = 0xff;
                        if (ty2 < 0)
                            ty2 = 0;
                        else if (ty2 > 0xbf)
                            ty2 = 0xbf;
                        p->tx = tx2;
                        p->ty = ty2;
                    } else {
                        p->delay = 5;
                    }
                }
            }
            if (DecIfAbove0_Byte(&p->delay) != 0) {
                p->tx = TOUCH_DATA[i].x;
                p->ty = TOUCH_DATA[i].y;
                p->mag = 0;
                p->nx = 0;
                p->ny = 0;
                p->ang = 0;
            } else {
                int dx;
                int dy;
                int len;
                int max;
                newx = TOUCH_DATA[i].x;
                newy = TOUCH_DATA[i].y;
                dx = newx - p->tx;
                dy = newy - p->ty;
                p->dx = dx;
                p->dy = dy;
                {
                    s64 sq = (s64)dx * dx;
                    sq += (s64)dy * dy;
                    len = cstd::sqrt(sq);
                }
                if (p->mode == 2)
                    max = 0x30;
                else
                    max = 0x24;
                if (len > max) {
                    int quot = cstd::fdiv(max, len);
                    dx = FX_MUL(dx, quot);
                    p->tx = newx - dx;
                    dy = FX_MUL(dy, quot);
                    p->ty = newy - dy;
                    len = max;
                } else if (len < 4) {
                    len = 0;
                }
                p->mag = (len << 12) / max;
                p->nx = (dx << 12) / max;
                p->ny = (dy << 12) / max;
                p->ang = _ZN4cstd5atan2E5Fix12IiES1_(dx, dy);
            }
        } else {
            int b2;
            int dirs;
            b2 = (touched == 0 && TOUCH_DATA[i].edge != 0);
            if (b2 != false)
                p->cnt = 5;
            DecIfAbove0_Byte(&p->cnt);
            p->touching = 0;
            dirs = 0;
            if (p->mode == 0)
                dirs = data_020a0e58[i].held & 0xf0;
            p->ang = data_020756b0[dirs >> 4];
            if (dirs != 0) {
                p->mag = 0x1000;
                p->nx = data_02082214[((u16)p->ang >> 4) * 2];
                p->ny = data_02082214[((u16)p->ang >> 4) * 2 + 1];
            } else {
                p->mag = 0;
                p->nx = 0;
                p->ny = 0;
            }
        }
        {
            int b3 = (data_0209fc48 != 0);
            if (b3 == false) {
                u16 *map = data_0209214c[p->mode];
                u32 j = 0;
                do {
                    if (held & 1)
                        p->held |= *map;
                    held >>= 1;
                    if (pressed & 1)
                        p->pressed |= *map;
                    pressed >>= 1;
                    map++;
                    j++;
                } while (j < 0x10);
                if (p->mode != 2 && (p->held & 0x300) == 0x300) {
                    *(u16 *)((int)p + 4) |= 0x4000;
                    if (p->pressed & 0x300) {
                        *(u16 *)((int)p + 6) |= 0x4000;
                    }
                }
            }
        }
        st++;
        i++;
        p++;
    } while (i < data_0209f21c);
}

// @symbol _ZN3Fog4InitEt5Fix12IiES1_
/* Fills the density ramp from a near/far pair, then writes the fog registers.
   The ramp is walked with a running `p`, not `self->mDensity[i]`: MEASURED,
   the indexed spelling changes the function's size. */
extern "C" void _ZN3Fog4InitEt5Fix12IiES1_(struct Fog *self, unsigned short color, int nearv,
                                           int farv) {
    int dist;
    int step;
    int i;
    char *p; /* walks mDensity */

    self->mEnabled = 1;
    dist = 0x28000 - (nearv << 4);
    self->mColor = color;
    self->mShift = 6;
    self->mOffset = 0;
    if (dist >= 0x80000)
        dist = 0x7ffff;
    step = (0x100000 - (farv << 4) - dist) >> 5;
    i = 0;
    p = ((char *)self);
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
/* _ZN10dScStage_c16CleanupResourcesEv @ 0x0202c9a8 (arm9, size 0x264)
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
    u32 i;

    for (i = 0; i < 12; i++)
        ((SharedFilePtr *)(data_020756f0[i]))->Release();

    {
        int atEnd = (data_0209f2d8 == 2);
        if (!atEnd)
            Sound::ResetPlayerVoiceGroup();
    }
    EndKuppaScript();
    data_0209d4a8 = 0;

    {
        /* Was `*(void ***)((char *)&unk_9bc)` -- reading a pointer back out of
           a field the header declared as one byte. include/dScStage_c.h types it now;
           this read and LoadSkybox's store are the two ends of the evidence. */
        void **o = (void **)mSkyboxModel;
        if (o)
            DestroyVirt(o);
    }

    {
        StageTexAnimSlot *e = mTexAnimSlots;
        int j;
        for (j = 0; j < ((LevelInfo *)data_0209f340)->count; j++, e++) {
            void **o = (void **)e->mTransformer;
            if (o)
                DestroyVirt(o);
            {
                char *p = (char *)e->mBlockList;
                while (p) {
                    char *next = *(char **)(p + 0xc);
                    Memory::operator_delete2(p);
                    p = next;
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
    ((dBgW *)((char *)&mMeshCollider))->Disable();
    dScStage_c::ResetMeshColliders();
    Deallocate(func_01ffb0c8(&mMeshCollider));
    UnloadLevelOverlays(data_0209f2f8);
    if (data_0209f2f8 == 1)
        UnloadArchive(7);

    {
        int k;
        for (k = 0; k < 4; k++)
            data_0209f394[k] = 0;
    }
    data_0209f318 = 0;
    data_0209f2c4 = 0;
    Message::ResetAllGlobals();

    {
        u8 mode = data_0209f2d8;
        int music = 0;
        data_0209f2e0 = 0;
        data_0209f2d4 = 0;
        data_0209f244 = 0;
        data_0209f2b0 = 0;
        data_0209f20c = 0;
        if (mode == 1)
            music = 1;
        if (music) {
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
// local extern: member form converts the e3d argument to bool; the ROM call passes the raw int
void _ZN10dScStage_c14LoadGraphics2DEbi(int e3d, int level);
}

int dScStage_c::InitResources() {
    dScStage_c *thiz = this;

    if (thiz->mWaitingForLoad == 0) {
        u8 modeByte = data_0209f2d8;
        int v0 = (modeByte == 1) ? 1 : 0;
        if (v0) {
            Sound::Player::SetPlayableSeqCount(2, 5);
            int n = 3;
            Sound::Player::SetPlayableSeqCount(n, n);
        }
        dScene_c::Initialise3dGraphics();
        Enable3dEngines();
        data_0209f21c = data_0208a0e0;
        data_0209f250 = (u8)func_0203da9c();
        ResetInput();
        data_0209f2c4 = 0;

        int temp_r4 = SublevelToLevel(data_02092110);
        int temp_r0 = SublevelToLevel(data_0209f2f8);
        u32 bits = ((u8)data_02092110 + 0xDC) & 0xFF;
        int v1 = 0;
        if (bits <= 0xD) {
            if ((1 << bits) & 0x2A15)
                v1 = 1;
        }
        modeByte = data_0209f2d8;
        int b1 = (modeByte == 1) ? 1 : 0;

        if (b1 || data_0209f26c == 2 || v1 || temp_r4 == 0x1D || temp_r4 != temp_r0) {
            int r8 = 0;
            data_0209f34c = 0;
            do {
                int r7 = 0;
                do {
                    data_0209f4f8[r8][r7] = 0;
                    r7 += 1;
                } while (r7 < 0x10);
                r8 += 1;
            } while (r8 < 3);

            if ((s32)data_0209f21c > 0) {
                int count = data_0209f21c;
                int idx = 0;
                u8 *p1 = (u8 *)data_0209f30c;
                u8 *p2 = (u8 *)data_0209f310;
                int sl = (modeByte == 1) ? 1 : 0;
                u8 f26c = data_0209f26c;
                do {
                    if (sl || (v1 == 0 && f26c != 1)) {
                        data_0209f358[idx] = 0;
                    }
                    *p1 = 0;
                    *p2 = 0;
                    idx += 1;
                    p1 += 1;
                    p2 += 1;
                } while (idx < count);
            }
        }

        data_0209f2fc = data_0209f26c;
        if (data_0209f2fc == 1) {
            data_02092124 = data_0209f2f8;
            data_02092118 = -1;
        }

        s8 prevLevel = data_0209f2f8;
        data_0209f2f8 = data_02092110;
        data_0209f264 = data_0209f268;
        data_0209f220 = data_0209f1f0;
        data_02092120 = -1;
        data_0209f314 = (int)thiz->mTexAnimSlots;
        data_0209f32c = 0x80000000;
        data_0209f274 = 0;
        SublevelToLevel(data_0209f2f8);

        s32 archiveIdx = 0xBF;
        if (data_0209f2f8 == 0x33)
            archiveIdx = 2;
        else if (data_0209f2f8 == 0x2B)
            archiveIdx = 3;
        else if (data_0209f2f8 == 0x1D)
            archiveIdx = 4;
        else if (data_0209f2f8 == 0x2A)
            archiveIdx = 5;

        u32 r7_2 = 2;
        do {
            if (r7_2 != (u32)archiveIdx) {
                UnloadArchive(r7_2);
            }
            r7_2 += 1;
        } while (r7_2 <= 5);

        LoadTextNarcs();
        LoadArchive(0);
        if (data_0209f2f8 == 1) {
            mHeap::Heap_t *saved = ((mHeap::Heap_t *)data_020a0eac)->SetDefault();
            LoadArchive(7);
            saved->SetDefault();
        }
        if (archiveIdx != 0xBF) {
            LoadArchive(archiveIdx);
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

        volatile u16 *pVram = (volatile u16 *)0x0400000A;
        *pVram = *pVram & ~3;
        *pVram = (*pVram & 0x43) | 0x1F1C;
        *pVram = *pVram & ~0x40;
        data_0209d454 = 0x18;
        data_0209f204 = 0;
        data_0209d4a8 = (void *)&data_0209f3c4;
        data_0209b454 = 0;
        func_02039218();
        ResetKuppaScript();

        int v0_3 = (data_0209f2d8 == 1) ? 1 : 0;
        if (v0_3 != 0) {
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

        int v0_4 = (data_0209f2d8 == 2) ? 1 : 0;
        if (v0_4 == 0) {
            s8 level2 = *(s8 *)(unsigned)(((unsigned long long)(unsigned)&data_0209f2f8) & ~0ULL);
            int soundGroup = 0;
            u8 bank = 0x36;
            if (data_0209f220 == 2) {
                if (level2 == 1 || level2 == 0x33)
                    soundGroup = 0x2B;
                else if (level2 == 0x1D)
                    soundGroup = 0x2E;
                else if (level2 == 0x2A)
                    soundGroup = 0x2C;
                else if (level2 == 0x2B)
                    soundGroup = 0x2D;
            }
            if (soundGroup == 0) {
                int idx = level2 * 3;
                soundGroup = GetSoundGroupID(level2, data_0209f220);
                bank = data_02075769[idx];
            }
            Sound::LoadGroupAndSetBank(soundGroup, bank);
        }

        *(volatile u16 *)0x04000008 = (*(volatile u16 *)0x04000008 & ~3) | 2;
        int v0_6 = (data_0209f2d8 == 1) ? 1 : 0;
        _ZN10dScStage_c14LoadGraphics2DEbi(v0_6, data_0209f2f8);

        {
            u32 i;
            for (i = 0; i < 0xC; i++) {
                Model::LoadFile(*(SharedFilePtr *)data_020756f0[i]);
            }
        }

        int v0_7 = (data_0209f2d8 == 1) ? 1 : 0;
        u16 *faderTbl;
        if (v0_7 && archiveIdx != 0xBF) {
            faderTbl = data_02075600;
        } else if (data_0209f2f8 == 5) {
            faderTbl = data_020755f0;
        } else {
            faderTbl = data_020755e0;
        }

        data_0209f324 =
            func_02073470(7, 0x60, 8, (void *)&_ZN9dFdWipe_cC1Ev, (void *)&_ZN9dFdWipe_cD1Ev);
        int r8_4 = 0;
        int r7_3 = 0;
        do {
            ((dFdWipe_c *)((char *)data_0209f324 + r7_3))->LoadAndSetFile(faderTbl[r8_4]);
            r8_4 += 1;
            r7_3 += 0x60;
        } while (r8_4 < 7);

        data_0209f340 = (char *)data_02092208[data_0209f2f8];
        thiz->LoadModel();
        data_0208ee44 = 2;
        dScStage_c::LoadClsnAndObjects(*(LVL_Overlay *)data_0209f340, data_0209f264,
                                       thiz->mMeshCollider);

        int v0_8 = (data_0209f2d8 == 1) ? 1 : 0;
        if (v0_8 == 0) {
            int nofc = (data_0209fc48 != 0) ? 1 : 0;
            if (nofc == 0) {
                if (!IsLevelInsideCastle(data_0209f2f8) || !IsLevelInsideCastle(prevLevel)) {
                    if (!IsLevelTinyHugeIslandOutside(data_0209f2f8) ||
                        !IsLevelTinyHugeIslandOutside(prevLevel)) {
                        if (data_0209f2f8 != 2 || (((SaveData *)data_0209caa0)->flags2 & 0x200)) {
                            Sound::LoadAndSetMusic_Layer1(data_0207576a[data_0209f2f8 * 3]);
                            u8 sndIdx = data_0209f250;
                            if (((u8 *)data_0209f394[sndIdx])[0x6ff] != 0) {
                                Sound::SetMusic(sndIdx, 0x33);
                            }
                        }
                    }
                }
            }
        }

        if (func_0203da3c() != 2 && archiveIdx != 0xBF) {
            UnloadArchive(archiveIdx);
        }
        LoadDebugFont();
        thiz->LoadFog();
        thiz->LoadTextureTransformers();
        thiz->LoadSkybox();
        data_0209cef0 = 1;
        thiz->mSysTracker.Initialise();

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
        int v0_9 = (data_0209f2d8 == 2) ? 1 : 0;
        if (v0_9 == 0) {
            *p1 = (*p1 & ~0x1F00) | (data_0209d454 << 8);
        }
        data_0209cee8 = 0;
        dExtShadowModel_c::CleanAll();
        data_02092110 = -1;

        if (data_0209fc68 != 0) {
            thiz->mWaitingForLoad = 1;
        }
    }

    if (thiz->mWaitingForLoad != 0) {
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
char *_ZN2G213GetBG2CharPtrEv(void);
u32 LoadCompressedFileAt(u16 fileID, void *target);
int LoadFileAt(int handle, int dest);
char *_ZN2G212GetBG2ScrPtrEv(void);
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

void dScStage_c::LoadGraphics2D(bool b, int i) {
    char *r4;
    char *r7;

    BG3CNT &= ~3;
    BG3CNT = (BG3CNT & 0x43) | 0x1100;
    BG3CNT &= ~0x40;
    SetBg0Offset(0, 0);

    BG2CNT = (BG2CNT & ~3) | 1;
    BG2CNT &= ~0x40;
    BG2CNT = (BG2CNT & 0x43) | 0xd10;

    if (b == 0) {
        LoadFont(0);
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xb00f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xac0f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xa80f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa40f, _ZN2G213GetBG2CharPtrEv());
        } else {
            LoadCompressedFileAt(0xa00f, _ZN2G213GetBG2CharPtrEv());
        }
        LoadFileAt(0x980b, 0x5000000);
        r4 = _ZN2G212GetBG2ScrPtrEv();
        LoadCompressedFileAt(0x9808, r4);
        LoadCompressedFileAt(0x9809, r4 + 0x800);
        LoadCompressedFileAt(0x980a, r4 + 0x1000);
        LoadControllerModeText(0x280);
    } else {
        LoadFont3D();
    }

    if ((int)(data_0209f2d8 == 0) != 0 && (((SaveData *)data_0209caa0)->flags2 & 0x80) == 0 &&
        (int)(data_0209fc48 != 0) == 0) {
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
            char *scr2 = G2S::GetBG2ScrPtr();
            volatile u16 sp0 = 0x10d0;
            MultiStore16(sp0, scr2, 0x800);
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
    r4 = (char *)G2S::GetBG1ScrPtr();
    LoadCompressedFileAt(0x9804, r4 + 0x800);
    LoadCompressedFileAt(0x9803, r4 + 0x1000);

    if (GetOwnerLanguage() == 5) {
        LoadCompressedFileAt(0xb009, r4 + 0x1800);
        LoadCompressedFileAt(0xb00b, r4 + 0x2000);
    } else if (GetOwnerLanguage() == 4) {
        LoadCompressedFileAt(0xac09, r4 + 0x1800);
        LoadCompressedFileAt(0xac0b, r4 + 0x2000);
    } else if (GetOwnerLanguage() == 3) {
        LoadCompressedFileAt(0xa809, r4 + 0x1800);
        LoadCompressedFileAt(0xa80b, r4 + 0x2000);
    } else if (GetOwnerLanguage() == 2) {
        LoadCompressedFileAt(0xa409, r4 + 0x1800);
        LoadCompressedFileAt(0xa40b, r4 + 0x2000);
    } else {
        LoadCompressedFileAt(0xa009, r4 + 0x1800);
        LoadCompressedFileAt(0xa00b, r4 + 0x2000);
    }

    r7 = G2S::GetBG2ScrPtr();
    LoadCompressedFileAt(0x9805, r7);
    LoadCompressedFileAt(0x9806, r7 + 0x1000);

    if (b != 0) {
        if (data_0209fc68 != 0) {
            return;
        }
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xb00f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xac0f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xa80f, _ZN2G213GetBG2CharPtrEv());
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa40f, _ZN2G213GetBG2CharPtrEv());
        } else {
            LoadCompressedFileAt(0xa00f, _ZN2G213GetBG2CharPtrEv());
        }
        LoadFileAt(0x980b, 0x5000000);
        LoadCompressedFileAt(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN,
                             _ZN2G212GetBG2ScrPtrEv() + 0x1800);
        LoadCompressedFileAt(0x9801, r4 + 0x2800);
        return;
    }

    if (SublevelToLevel(i) == 0x1d) {
        volatile u16 sp2 = 0;
        MultiStore16(sp2, r4, 0x40);
        LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_SELECT_PAUSE_NSC_BIN, r4 + 0x40);
        LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_PAGE_NSC_BIN, _ZN2G212GetBG2ScrPtrEv() + 0x1800);
        LoadCompressedFileAt(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN,
                             _ZN2G212GetBG2ScrPtrEv() + 0x2800);
        LoadCompressedFileAt(0x9802, r4 + 0x2800);
        return;
    }

    LoadCompressedFileAt(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN, _ZN2G212GetBG2ScrPtrEv() + 0x1800);
    LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_SELECT_PAUSE_NSC_BIN, r4);
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
