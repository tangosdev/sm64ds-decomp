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
 * this range. The free functions are C++ linkage; symbols.txt records
 * the Itanium names.
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
// - CheckCameraInput keeps `#pragma opt_strength_reduction off`; PS_Update and Behavior keep
//   `#pragma opt_common_subs off` (switched back on after each), the
//   stale-tmpv levers in PS_Update's case 0xa and the `aa` staging in its
//   case-1 star scroll, and its
//   sp0..sp10/var_r0/var_sl/var_fp locals stay unrenamed until each slot's
//   role is recovered. The touch globals (gTouchHeld..gTouchY,
//   gActivePlayerSlot) have no header;
//   data_ov001_ and data_ov002_ handles keep linker names.
/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScStage_c_AssetId { ASSET_D_2D_UPR_COURSE_CLEAR_MESSAGE_NCL_BIN = 0x025a };

#include "KuppaScript.h"
#include "types.h"
#include "nitro/gx/gx_bgcnt.h"
#include "private/controller_input.h"
#include "dFdBrightness_c.h"
#include "dFdColor_c.h"
#include "dScStage_c.h"
#include "LVL_Overlay.h"
#include "Timer.h"
#include "decl_common.h"
#include "Sound.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "daPly_c.h"
#include "nitro/gx/g2.h"
#include "nitro/hw/registers.h"
#include "dScene_c.h"
#include "Event.h"
#include "dExtSimpleModel_c.h"
#include "dCamera_c.h"
#include "dPa_c.h"
#include "Fog.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dFdWipe_c.h"
#include "SaveData.h"
#include "nitro/gx/g3x.h"
#include "dExtAnmTexSRT_c.h"
#include "dExtFrameCtrl_c.h"
#include "dExtShadowModel_c.h"
#include "dCc_c.h"
#include "OAM.h"
#include "Message.h"
#include "Memory.h"
#include "Heap.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_load.h"
#include "GX.h"
#include "cstd.h"
#include "daMugenBGM_c.h"
#include "Matrix2x2.h"
#include "math/CalcLib.h"
#include "dGraph_c.h"
#include "dClipper.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
#include "msl/string.h"
#include "nitro/mi/memory.h"

struct OamAttr;

/* Definition order is the static-init order. The callback's implicit
   constructor stores the base vptr, then this vtable, and leaves the matrix
   and affine words zero. dClipper's virtual destructor is what makes mwcc
   register it; the callback has none. */
dScStage_c::graphCallback_c sStageGraphCallback;
dClipper data_0209f43c;

int IsButtonInputValid(void);
int SublevelToLevel(int lv);
int GetOwnerLanguage(void);
void GiveLives(int n);
void Deallocate(void *ptr);
int func_02029408(void);
void SetControllerMode(unsigned char m);
int IsStarCollected(int lv, int star);
extern "C" {
void SetBg2Offset(int x, int y);
void SetBg3Offset(int x, int y);
void SetBg1Offset(int x, int y);
void SetSubBg0Offset(int x, int y);
void SetSubBg1Offset(int x, int y);
void *LoadFile(int handle);
unsigned int LoadCompressedFileAt(int id, void *dst);
int GetSoundMode(void);
void SetSoundMode(int m);
void TurnBacklightOn(void);
void TurnBacklightOff(void);

bool d_s_stage_methodsPlaySubHold(unsigned int a, unsigned int b, unsigned int c, int fix, int e);

void OamRender10_d_s_stage_methods(int sub, void *attr, int x, int y, int a, int cc, int fx, int fy,
                                   int rot, int mode);

extern int data_0208ee44;
extern u8 data_0208ee3c;
extern u8 data_0208ee40;
extern s8 gLastCourseSublevel;
extern s16 gPlayerHealth[];
/* The actor update mask for the next frame. While it is non-zero, only actors
   whose mFlags share a bit with it run their behaviour (dActor_c::
   BeforeBehavior tests gActorUpdateMask, which Behavior copies from this word
   at the top of every frame; writers that need it at once copy it themselves).
   Cutscenes set their own bit, and pausing is refused while any bit is set.
   ACTOR_UPDATE_MASK_STAGE_ENDING (dActor_c.h) is set with the fade out of
   every exit: VE_Init, LC_Update's exit choice, and the pause screen's exit
   and save-and-quit paths. */
extern unsigned int gNextActorUpdateMask;
extern unsigned int gActorUpdateMask;
extern u8 data_020755b8[];
extern volatile int data_02075610[];
extern u16 data_020755cc[];
extern u16 data_020755c0[];
extern u16 data_020755c4[];
extern s16 data_020756d0[];
extern u8 data_0209d660;
/* The main engine's enabled layers, the same byte for DISPCNT bits 8..12:
   func_02019144 commits it every VBlank. BG0 is the 3D scene, BG3 the message
   text and OBJ the top-screen sprites. */
extern u8 gMainScreenLayers;
/* The sub engine's enabled layers, DISPCNT_SUB bits 8..12 shifted down to bit
   0 (DISPLAY_LAYER_*, nitro/hw/registers.h); every writer copies it in with
   `<< 8`. On the stage, BG3 is the map picture and OBJ every bottom-screen
   sprite. */
extern u8 gSubScreenLayers;
extern Timer gRaceTimer;
extern int data_0209fc68;
extern int data_020a0db0;
extern u8 data_ov002_02111150;
extern u8 gMeterCounterSlideState;
extern u8 gZoomButtonActiveTimer;
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
/* SetNextLevel's return table: level, return level, entrance. */
struct LevelReturnEntry {
    s8 level;
    s8 returnLevel;
    u8 entrance;
};
struct OvTbl {
    unsigned int t[7];
};
/* The object at gSceneFader; only its slot-5 and slot-7 queries are called
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
extern u8 data_0208a0e0;
/* Where the stage goes next. Each destination is a sublevel, an entrance
   and a star; the crash screen prints the current three as "StageNo",
   "PlayerID" (the entrance) and "LayerNo" (the star), and gCurrentAreaId as
   "RoomNo". LoadLevel and SetNextLevel fill the gNext* set; InitResources
   moves it into the current set and resets gNextSublevel to -1, so a
   non-negative gNextSublevel means a level change is pending. LoadLevel with
   a return entrance records the sublevel being left as gReturnSublevel, and
   SetNextLevel (the exit path) sends the player back there. The checkpoint
   set keeps a destination reached from level 6, 7 or 0xb while a return is
   set; a later LoadLevel with a return entrance into the same level
   resumes at it. */
extern s8 gNextSublevel;
extern s8 gCheckpointSublevel;
extern s8 gReturnSublevel;
extern s8 gCurrentAreaId;
extern u8 data_02092128[];
extern int data_02092130;
extern unsigned int data_02092134;
extern int data_02092138;
extern u16 *data_0209214c[];
extern s32 data_02092208[];
extern s32 data_0209cef0;
/* The scene's graph callback: the scene registers its own object here and
   clears it on the way out. The frame calls its four slots: GraphCallback0 at
   the frame start (func_02019390), GraphCallback1 before the frame wait
   (func_02019404), GraphCallback2 and GraphCallback3 in the VBlank IRQ
   (func_02019144, func_02019100). dGraph_c::callback_c is the ROM-proven base
   of every scene's nested graphCallback_c (include/dGraph_c.h). */
extern dGraph_c::callback_c *gGraphCallback;
/* The stage's .bss, 0x0209f1ec..0x0209f5b8: the pause, VS-exit and
   level-clear screens' state, the level and player globals the free
   functions keep, and the per-level tables. mwcc sorts a TU's objects by
   size, so the run ascends from bytes to the death table. The graph callback
   and dClipper (defined above) and the compiler's destructor-registration
   node for dClipper sit between the tables. */
/* The pause page to show next: PS_Update moves it into sPauseState at the
   top of each frame. */
static u8 sNextPauseState;
u8 gNextStarId;
static u8 data_0209f1f4;
u8 gNumViewObjs;
static u8 data_0209f1fc;
u8 gReturnEntranceId;
u8 data_0209f204;
u8 data_0209f208;
u8 data_0209f20c;
/* Counts down from 3 frames after a pause arrow is touched; PS_Update clears
   sPressedPauseArrow when it reaches 0. */
static u8 sPauseArrowPressTimer;
static u8 sNumEntranceObjs;
/* Set when the pause was opened with SELECT rather than START. */
static u8 sPausedWithSelect;
/* How many players this stage runs with: InitResources copies it from
   data_0208a0e0 (1 at boot; dScEntry_c::EnterMatchLoading can set 4), and
   the player loops run to it. */
u8 gNumPlayers;
u8 gCurrentStarId;
u8 data_0209f224;
u8 gCollectedStarId;
static u8 sMenuWaitTimer;
static u8 data_0209f230;
static u8 data_0209f234;
/* The pause arrow drawn pressed in, 0 for none. On the course page 1 is the
   left arrow and 2 the right (PS_Render moves that arrow two pixels out); the
   sound-options page sets 2 for its left arrow and 1 for its right. */
static u8 sPressedPauseArrow;
/* Another options-page countdown: PS_Update repaints the page when it
   reaches 0, like sSoundBoxFlashTimer and sBacklightFlashTimer. Nothing in the
   ROM sets it non-zero (its only other access is PS_Init's clear), and the
   runtime pass did not confirm a role, so it keeps its placeholder name. */
static u8 data_0209f23c;
static u8 data_0209f240;
/* Frames until the selected big button's highlight is redrawn. Touching or
   confirming the button that is already selected sets it to 4 frames, and
   UpdateMenuButtons, PS_UpdateOkAndBackButtons and PS_UpdateSaveMenu draw the
   highlight palette only once it is 0, so the press blinks. Each update loop
   repaints the buttons when it reaches 0. */
static u8 sMenuHighlightDelay;
/* The pause page shown, the switch PS_Update runs. */
static u8 sPauseState;
static u8 sCheckpointEntranceId;
/* This console's player: InitResources stores func_0203da9c(), the local
   player number, so the HUD, the pause screen and the camera index the
   per-player arrays (gPlayers, gPlayerHealth, gNumStageStars) by it. */
u8 gLocalPlayerId;
u8 data_0209f254;
static u8 sNumFogObjs;
static u8 sNumMinimapScaleObjs;
static u8 sLevelClearCoinCount;
u8 gCurrentEntranceId;
static u8 sNextEntranceId;
static u8 sReturnState;
static u8 sCheckpointStarId;
u8 data_0209f274;
static u8 data_0209f278;
static u8 data_0209f27c;
static u8 data_0209f280;
/* Non-zero while a talk or event is pointing the player at the Touch Screen:
   dMeter_c::Render then calls RenderBouncingArrows, which draws the three
   down-arrows along the foot of the top screen. Every writer that sets it
   also plays sound 0x24 -- the intro Lakitu partway through message 0x182,
   the 0x74a sign, the red Bob-omb buddy, a cap icon flashing on the map, the
   end of a teleport -- and clears it once that is over. InitResources clears
   it on stage load. */
u8 gShowTouchScreenArrows;
u8 gShowRedCoinMarkers;
u8 data_0209f28c;
static u8 data_0209f290;
/* Non-zero from VE_Init (the VS time-up exit screen: exit text, both screens
   dimmed) until the next InitResources; data_0209f290 is that screen's step.
   Every "is play suspended" test ORs it with gGamePaused (pause) and
   data_0209f20c, so while it is set the HUD, camera, actor behaviour and
   button input all stand still. */
u8 gVsExitScreenActive;
static u8 data_0209f298;
/* Which of the two buttons along the foot of the controls and options pages
   is selected while sSelectedMenuButton is on that row: 0 the left (OK) and 1
   the right (Back). PS_UpdateOkAndBackButtons highlights button i when this is
   i; choosing 1 returns to the previous pause page. It is a selection, not a
   "pressed" flag. */
static u8 sBackButtonSelected;
static u8 data_0209f2a0;
static u8 data_0209f2a4;
static u8 sLevelClearPaletteTimer;
u8 gNewStarCollected;
static u8 sLevelClearNewRecord;
/* How many big buttons the current menu has (2 to 4); UpdateMenuButtons
   repaints that many from sMenuButtonTileOffsets. On the controls page a
   sSelectedMenuButton equal to it means the bottom OK/Back row. */
static u8 sNumMenuButtons;
static u8 data_0209f2b8;
u8 data_0209f2bc;
u8 data_0209f2c0;
/* Non-zero while the pause screen is up. dActor_c::BeforeBehavior, the
   camera, the meter and the map stand still while it is set. */
u8 gGamePaused;
/* The course the pause screen shows stars and the coin record for, stepped by
   the course arrows. 0xf is the castle: the total star count, with no
   star/coin line. */
static u8 sPauseDisplayedCourse;
/* Set to 4 frames when a backlight button is touched without changing the
   backlight, or when the toggle flips it. PS_UpdateOptionsMenu draws the
   selected backlight button with the highlight palette (0x6000) only while it
   is 0, so the press blinks; PS_Update repaints the page when it reaches 0. */
static u8 sBacklightFlashTimer;
static u8 sNumTeleportDestObjs;
u8 gLevelClearStep;
/* 0 in the adventure (StartFile), 1 in VS (PrepareVsMode), 2 while the
   ending's fall-in-front-of-the-castle cutscene runs: KuppaScript sets it when
   it resumes sCutsceneEndingMarioFall and clears it when that script ends. */
u8 gGameMode;
/* The controller mode chosen on the controls page (0..2), seeded from the
   player's record, saved through SetControllerMode, and the index of the
   callout-text sprite PS_Render draws. */
static u8 sSelectedControllerMode;
/* The selected big button of the pause, VS-exit and course-clear menus (the
   cursor row on the controls page). The menus read it when the selection is
   confirmed. */
static u8 sSelectedMenuButton;
/* Set to 8 when an arrow steps the sound mode. While it is non-zero
   PS_UpdateOptionsMenu draws the SOUND box with palette 0x5000 instead of the
   highlight 0x6000, and PS_Update repaints the page when it reaches 0. */
static u8 sSoundBoxFlashTimer;
u8 gNumMinimapTileObjs;
/* The backlight button drawn selected on the options page: 0 the left (on)
   and 1 the right (off). Seeded from data_0208ee3c ^ 1 when the page opens;
   touching a button sets it, and the toggle flips it. The buttons show it only
   when PS_UpdateOptionsMenu repaints the page. */
static u8 sSelectedBacklightButton;
/* Set when no course 0..14 has a star yet: the pause screen then shows only
   the castle total (sPauseDisplayedCourse 0xf) and neither draws nor reads
   the course arrows. */
static u8 sCourseArrowsHidden;
/* The lives count: SetPlayerGlobals starts it at 4, GiveLives clamps it to
   0..100, KillPlayer and HitDeathPlane go to game over at 0, and
   dMeter_c::RenderLifeCount draws it. */
s8 gNumLives;
s8 gCurrentSublevel;
u8 gPrevReturnState;
/* Frames the pause screen ignores input for: PS_Init and PS_Cleanup set it
   to 0xf and Behavior counts it down; the pages return while it is
   non-zero, and the pause cannot be opened again until it is 0. */
static u16 sPauseInputLockout;
static u16 data_0209f304;
static u16 data_0209f308;
s8 gNumRedCoins[4];
/* gNumRedCoins + 1 and gNumStageStars + 1: NumRedCoins and NumVsStarsObtained
   load each sum's second element through its own literal. */
extern s8 data_0209f30d;
/* Stars collected in this stage, one count per player: the silver stars of a
   course (shown as that many star icons on the HUD; the next silver star's
   sound is 0x19 + it) and each player's stars in a VS match. */
s8 gNumStageStars[4];
extern s8 data_0209f311;
static int data_0209f314;
/* The stage's camera: LoadEntranceObjects spawns it (profile 0x14c) once the
   players are placed, and CleanupResources clears it. */
dCamera_c *gCamera;
static char *sFogObjs;
static int data_0209f320;
static void *data_0209f324;
static LVL_Overlay::StandardEntry *sEntranceObjs;
s32 data_0209f32c;
static void *sTeleportDestObjs;
u16 *gMinimapTileObjs;
void *gUnusedType13Objs;
void *data_0209f33c;
static char *data_0209f340;
u8 *data_0209f344;
static short *sMinimapScaleObjs;
static s32 sEventFlags;
u8 data_0209f350[4];
static void *sViewObjs;
short gNumCoins[4];
/* Each big button's offset, in screen entries, into the sub engine's BG1
   map; the menu that opens sets them with sNumMenuButtons. */
static u16 sMenuButtonTileOffsets[4];
static u16 data_0209f368[4];
/* Per cap slot (data_0209f3e8, nine of them): the character whose cap it is,
   0 Mario, 1 Luigi, 2 Wario. func_0202a8e0 stores its caller's byte when it
   claims the slot, and dMap_c::Render picks the map icon by it. */
u8 gMapCapIconCharacters[9];
/* Per STAR_MARKERS slot: the marker type SetStarMarker stores. dMap_c::Render
   draws types 0..3 from OAM::MM_STAR_MARKERS (type 3 does not blink); type 4
   draws no star marker, only a red-coin icon while gShowRedCoinMarkers is
   set. */
u8 gStarMarkerTypes[12];
/* The live players, one per gNumPlayers slot; LevelObjects fills it. */
daPly_c *gPlayers[4];
void *data_0209f3a4[8];
/* The graph callback's minimap descriptor, its +4. */
extern MinimapDesc data_0209f3c8;
/* The cap slots: the actor whose cap each of the nine holds, 0 when free.
   func_0202a8e0 claims one, dCapIcon_c::Unlink frees it. */
int data_0209f3e8[9];
/* The map's star markers, twelve slots: SetStarMarker stores the actor and
   gStarMarkerTypes its type; InitResources clears them. */
dActor_c *STAR_MARKERS[12];
/* The per-player input records. CheckInput fills them from the pad and the
   touch screen each frame and ResetInput clears them; the player, camera,
   meter and map read them. */
ControllerInput data_0209f498[4];
/* The mode byte of record 0 (+0x16); SetControllerMode, InitControllerMode
   and PS_Update index it by player * 0x18. */
extern u8 data_0209f4ae[];
/* The actor death table: a 16-word bit row per level part, indexed by an
   actor's deathTableID. InitResources clears it only when the course
   changes, so it survives a reload of the same course. */
static u32 sActorDeathTable[3][16];
extern dFdBrightness_c *gSceneFader;
extern dFdColor_c gSceneColorFader;
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
struct ObjTeleport;
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
void SetStarMarker(int i, dActor_c *actor, int v2);
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
void func_0202af80(short *table, int count);
int GetMinimapID(dActor_c *actor, int arg);
void func_0202b044(u16 *table, int count);
void func_0202b060(char *table, int count);
struct ObjTeleport *GetTeleportDestObj(int i);
void func_0202b090(void *table, int count);
void func_0202b0c4(void *table, int count);
void func_0202b0e0(LVL_Overlay::StandardEntry *entries, int count);
void SetTouchScreenDelay(void);
void ResetInput(void);
int IsLevelTinyHugeIslandOutside(int level);
int IsLevelInsideCastle(int level);
int GetSoundGroupID(int group, int sub);
void func_0202deac(int levelID);
s8 GetLevelPart(s32 level);
void OpenCannonInLevel(s8 levelID);
int IsCannonOpenInLevel(s8 levelID);
void CollectStarInLevel(s8 levelID, s32 starID);
int IsStarCollectedInLevel(s8 levelID, s32 starID);
void func_ov002_020b18f0(char *a);
extern "C" {}
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

/* The TouchInfo record at byte offset off (slot * 4) from gTouchHeld. */
#define TOUCHP(off) ((TouchInfo *)(gTouchHeld + (off)))

#pragma defer_codegen off

/* Destroy through dScStage_c's own three fields -- dBgW_Kc at 0x91c, dExtSimpleModel_c at
 * 0x86c, dPa_c at 0x50 -- in reverse declaration order, then
 * dScene_c (now inline-defined in dScene_c.h, so its vptr store and dBase_c's
 * inline in turn), then fBase_c's subobject destructor, called rather than
 * inlined because fBase_c's is not. The deleting variant destroys the same
 * chain and then hands the object back through dScene_c's inline operator
 * delete -- declared there rather than on dBase_c, because mwcc only inlines
 * it when found on the class itself or its IMMEDIATE base, which for
 * dScStage_c is dScene_c. One definition emits both. */
dScStage_c::~dScStage_c() {}

void dScStage_c::VE_Update() {
    u8 t = sMenuHighlightDelay;
    u8 t2;
    u8 s;
    if (t != 0) {
        sMenuHighlightDelay = t - data_0208ee44;
        if (sMenuHighlightDelay == 0) {
            dScStage_c::UpdateMenuButtons(0);
        }
    }
    t2 = sMenuWaitTimer;
    if (t2 != 0) {
        sMenuWaitTimer = t2 - data_0208ee44;
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
            if (sSelectedMenuButton == 0) {
                sMenuHighlightDelay = data_0208ee44 << 2;
            }
            sSelectedMenuButton = 0;
            sMenuWaitTimer = data_0208ee44 << 3;
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
            if (sSelectedMenuButton == 1) {
                sMenuHighlightDelay = data_0208ee44 << 2;
            }
            sSelectedMenuButton = 1;
            dScStage_c::UpdateMenuButtons(0);
            sMenuWaitTimer = data_0208ee44 << 3;
            data_0209f290 = 1;
            data_ov002_02111150 = 1;
            func_02012790(0x9b);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 1: {
        if (sSelectedMenuButton == 0) {
            dScene_c::StartSceneFade(6, 1, 0);
        } else {
            dScene_c::StartSceneFade(1, 0, 0);
        }
        data_0209f290 = 2;
        gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
        gSubScreenLayers &= ~3;
        G2_BlendNone();
        G2S_BlendNone();
        break;
    }
    }
}
void dScStage_c::VE_Init() {
    unsigned int a;
    unsigned int b;

    a = gMainScreenLayers ^ DISPLAY_LAYER_OBJ;
    b = gSubScreenLayers ^ DISPLAY_LAYER_OBJ;
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                      GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8000,
                         GX_BG_CHARBASE_0x10000);
    Message::DisplayVsExitText(0x1f);
    sMenuButtonTileOffsets[0] = 0xe0;
    sMenuButtonTileOffsets[1] = 0x1a0;
    sNumMenuButtons = 2;
    sSelectedMenuButton = 0;
    dScStage_c::UpdateMenuButtons(true);
    SetBg2Offset(0, 0);
    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    gMainScreenLayers |= DISPLAY_LAYER_BG2;
    gSubScreenLayers |= 3;
    gNextActorUpdateMask |= ACTOR_UPDATE_MASK_STAGE_ENDING;
    gActorUpdateMask = gNextActorUpdateMask;
    gVsExitScreenActive = 1;
    data_0209f290 = 0;
    G2_SetBlendBrightness(a | 0x20, -7);
    G2S_SetBlendBrightness(b | 0x20, -7);
}
void dScStage_c::RenderBouncingArrows() {
    int r4;
    unsigned char A;
    if (data_0208ee44 == 1) {
        r4 = (data_020a0db0 & 0x10) ? 0xae : 0xb0;
    } else {
        r4 = (data_020a0db0 & 8) ? 0xae : 0xb0;
    }
    A = gGamePaused;
    if ((A == 0 && gShowTouchScreenArrows != 0 && (int)(gGameMode == 1) == 0) ||
        (A != 0 && (unsigned char)(sPauseState + 0xf7) <= 2u)) {
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0x40, r4, -1, -1, 0x1000, 0x1000, 0, -1);
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0x80, r4, -1, -1, 0x1000, 0x1000, 0, -1);
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0xc0, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    } else {
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0xc, r4, -1, -1, 0x1000, 0x1000, 0, -1);
        OAM::Render(0, &OAM::BOUNCING_ARROW, 0xf4, r4, -1, -1, 0x1000, 0x1000, 0, -1);
    }
}
void dScStage_c::LC_Render() {
    sLevelClearPaletteTimer = sLevelClearPaletteTimer + data_0208ee44;
    if (sLevelClearPaletteTimer >= 0xc)
        sLevelClearPaletteTimer = 0;

    if (data_0209f20c == 0)
        return;

    {
        void *p = LoadFile(ASSET_D_2D_UPR_COURSE_CLEAR_MESSAGE_NCL_BIN);
        GX_LoadOBJPltt((void *)((char *)p + (((unsigned int)sLevelClearPaletteTimer >> 2) << 5)),
                       0x1c0, 0x10);
        Deallocate(p);
    }

    if (SublevelToLevel(gLastCourseSublevel) >= 0xf &&
        SublevelToLevel(gLastCourseSublevel) < 0x15) {
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

    if (sLevelClearNewRecord != 0) {
        if (data_0209f20c != 0 && SublevelToLevel(gLastCourseSublevel) >= 0xf) {
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

    if (gLevelClearStep == 3)
        RenderBouncingArrows();
}
void dScStage_c::LC_Update() {
    if (sMenuHighlightDelay != 0) {
        sMenuHighlightDelay = sMenuHighlightDelay - data_0208ee44;
        if (sMenuHighlightDelay == 0)
            dScStage_c::UpdateMenuButtons(0);
    }
    if (sMenuWaitTimer != 0) {
        sMenuWaitTimer = sMenuWaitTimer - data_0208ee44;
        return;
    }

    switch (gLevelClearStep) {
    case 0:
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6800,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        Message::DisplayLevelClearText(0x27a, SublevelToLevel(gLastCourseSublevel));
        sMenuButtonTileOffsets[0] = 0xa0;
        sMenuButtonTileOffsets[1] = 0x140;
        sMenuButtonTileOffsets[2] = 0x1e0;
        sNumMenuButtons = 3;
        sLevelClearNewRecord = 0;
        sSelectedMenuButton = 0;
        dScStage_c::UpdateMenuButtons(1);
        SetBg3Offset(0, 0);
        gMainScreenLayers |= DISPLAY_LAYER_BG3;
        sLevelClearCoinCount = 0;
        gLevelClearStep = 1;
        data_0209f280 = 0;
        if (gNumCoins[0] != 0)
            return;
        if (gNewStarCollected != 0) {
            Sound::PlaySub(0x23, 0x14, 0x7f, 0x15666, 0);
            data_0209f280 = 1;
        }
        sMenuWaitTimer = 0x1e;
        return;
    case 1: {
        int level = SublevelToLevel(gLastCourseSublevel);
        if (gNumCoins[0] > sLevelClearCoinCount) {
            sLevelClearCoinCount += 1;
            if (sLevelClearCoinCount % 50 == 0) {
                GiveLives(1);
                Sound::PlayBank3_2D(0x6e);
            }
            if (gNumCoins[0] == sLevelClearCoinCount) {
                if (gNewStarCollected != 0) {
                    Sound::PlaySub(0x23, 0x14, 0x7f, 0x15666, 0);
                    data_0209f280 = 1;
                }
                sMenuWaitTimer = 0x1e;
                if (level < 0xf) {
                    u8 coins = sLevelClearCoinCount;
                    if (SaveData::GetCoinRecord(level) >= coins)
                        return;
                    SaveData::SetCoinRecordIfHigher(level, coins);
                    sLevelClearNewRecord = 1;
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
        if (sLevelClearCoinCount == 0 && level >= 0xf && level < 0x15) {
            Sound::PlaySub(0x23, 0x14, 0x7f, 0x15666, 0);
            data_0209f280 = 1;
        }
        sMenuWaitTimer = 0x3c;
        gNumCoins[0] = 0;
        gLevelClearStep = 2;
        return;
    }
    case 2: {
        int t6;
        int t5;
        if (SublevelToLevel(gCurrentSublevel) == 0x1d) {
            G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                                 GX_BG_SCRBASE_0x9000, GX_BG_CHARBASE_0x10000);
        } else {
            G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                                 GX_BG_SCRBASE_0x8000, GX_BG_CHARBASE_0x10000);
        }
        gNumCoins[0] = 0;
        t6 = gMainScreenLayers ^ (DISPLAY_LAYER_BG3 | DISPLAY_LAYER_OBJ);
        t5 = gSubScreenLayers ^ DISPLAY_LAYER_OBJ;
        SetBg2Offset(0, 0);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        gMainScreenLayers |= DISPLAY_LAYER_BG2;
        gSubScreenLayers |= 3;
        G2_SetBlendBrightness(t6 | 0x20, -7);
        G2S_SetBlendBrightness(t5 | 0x20, -7);
        gLevelClearStep = 3;
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
                (sSelectedMenuButton == 0 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 0)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 0;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                func_02012790(0x57);
                gLevelClearStep = 4;
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x50) < 0x20) ||
                (sSelectedMenuButton == 1 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 1)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 1;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                func_02012790(0x58);
                gLevelClearStep = 4;
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (!((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x78) < 0x20) &&
                (sSelectedMenuButton != 2 || IsButtonInputValid() == 0))
                return;
            if (sSelectedMenuButton == 2)
                sMenuHighlightDelay = data_0208ee44 << 2;
            sSelectedMenuButton = 2;
            dScStage_c::UpdateMenuButtons(0);
            sMenuWaitTimer = data_0208ee44 << 3;
            func_02012790(0x59);
            gLevelClearStep = 6;
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 4:
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x7000,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        gLevelClearStep = 5;
        gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
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
            gLevelClearStep = 6;
        return;
    case 6:
        if (data_0209f280 != 0) {
            Sound::PlaySub(0x23, 0x7f, 0, 0x15666, 0);
            data_0209f280 = 0;
        }
        gMainScreenLayers &= ~(DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3);
        gSubScreenLayers &= ~3;
        G2_BlendNone();
        if (sSelectedMenuButton != 1) {
            gMeterCounterSlideState = 5;
        } else {
            dScene_c::StartSceneFade(1, 0, 0);
            gMeterCounterSlideState = 0;
            Sound::StopLoadedMusic_Layer1(0xa);
            {
                int t = gNextActorUpdateMask | ACTOR_UPDATE_MASK_STAGE_ENDING;
                gNextActorUpdateMask = t;
                gActorUpdateMask = t;
            }
        }
        sSelectedMenuButton = 0;
        gLevelClearStep = 0;
        sLevelClearNewRecord = 0;
        data_0209f20c = 0;
        return;
    default:
        return;
    }
}
#pragma opt_strength_reduction off
void dScStage_c::CheckCameraInput() {
    ControllerInput *cam = data_0209f498;
    if ((u8)(gVsExitScreenActive | (gGamePaused | data_0209f20c)) == 0 && data_0209d660 == 0) {
        int i = 0;
        int n1 = gNumPlayers;
        if (n1 > 0) {
            u8 *st = data_0209f350;
            do {
                cam->pressed = 0;
                cam->held = 0;
                if (((TouchInfo *)gTouchHeld)[i].held != 0) {
                    if (gCamera->mFlags & 0x1000) {
                        // cam->mode here builds 32 bytes shorter; the indexed read is the ROM shape.
                        u8 state = data_0209f498[i].mode;
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
                    u8 state2 = data_0209f498[i].mode;
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
                            gZoomButtonActiveTimer = 0x10;
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
            } while (i < gNumPlayers);
        }
    } else {
        int i = 0;
        u8 *st;
        int n = gNumPlayers;
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
void dScStage_c::RenderNumber(unsigned char num, int x, int y, bool b, int p5) {
    int leading = 0;
    Matrix2x2 *mtx = 0;
    int i;

    for (i = 0; i < 3; i++) {
        int digit = num / data_020755b8[i];
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
void dScStage_c::PS_Render() {
    if (func_02029408() != 0 && gPlayerHealth[gLocalPlayerId] != 0 && sPauseState != 0xb &&
        sPauseState != 0xe && sPauseState != 0xf &&
        ((unsigned int)sPauseState < 7U || (unsigned int)sPauseState > 8U)) {
        dScStage_c::RenderBouncingArrows();
    }

    int r0v = (gGameMode == 1);
    if (r0v != 0 && data_0209fc68 == 0) {
        OAM::Render(0, &OAM::VS_PAUSE, 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
    } else if (sPauseState == 1 ||
               (SublevelToLevel(gCurrentSublevel) == 0x1d &&
                ((unsigned int)sPauseState < 7U || (unsigned int)sPauseState > 8U))) {
        int var_sl;
        unsigned char var_sb;
        unsigned char var_r8;
        int var_r7;
        int sp18;
        var_r7 = 0;
        sp18 = 0;
        var_r8 = CountStarsCollectedInLevelToDisplay(sPauseDisplayedCourse);
        if (IsStarCollected(sPauseDisplayedCourse, 0) != 0) {
            var_r8 -= 1;
        }
        if (sPauseState == 1 && sCourseArrowsHidden == 0) {
            if (sPressedPauseArrow == 1) {
                OAM::Render(0, &OAM::ARROW_POINTING_LEFT, 0x12, 0x60, -1, -1, 0x1000, 0x1000, 0,
                            -1);
                OAM::RenderSub(&OAM::ARROW_POINTING_LEFT, 0x12, 0x10);
            } else {
                OAM::Render(0, &OAM::ARROW_POINTING_LEFT, 0x14, 0x60, -1, -1, 0x1000, 0x1000, 0,
                            -1);
                OAM::RenderSub(&OAM::ARROW_POINTING_LEFT, 0x14, 0x10);
            }
            if (sPressedPauseArrow == 2) {
                OAM::Render(0, &OAM::ARROW_POINTING_RIGHT, 0xee, 0x60, -1, -1, 0x1000, 0x1000, 0,
                            -1);
                OAM::RenderSub(&OAM::ARROW_POINTING_RIGHT, 0xee, 0x10);
            } else {
                OAM::Render(0, &OAM::ARROW_POINTING_RIGHT, 0xec, 0x60, -1, -1, 0x1000, 0x1000, 0,
                            -1);
                OAM::RenderSub(&OAM::ARROW_POINTING_RIGHT, 0xec, 0x10);
            }
        }
        if (sPauseDisplayedCourse != 0xf) {
            var_sb = 0x50;
            var_sl = 1;
            if (var_r8 != 0) {
                do {
                    if (IsStarCollected(sPauseDisplayedCourse, var_sl) != 0) {
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
            unsigned char r5v = SaveData::GetCoinRecord(sPauseDisplayedCourse);
            OAM::Render(false, &OAM::COIN, 0x70, 0x70, -1, -1, 0);
            OAM::Render(false, &OAM::TIMES, 0x80, 0x78, -1, -1, 0);
            RenderNumber(r5v, 0x88, 0x70, 1, 8);
            if (IsStarCollected(sPauseDisplayedCourse, 0) != 0) {
                OAM::Render(false, &OAM::MM_SMALL_STAR, 0xb0, 0x78, -1, -1, 0);
            }
        } else {
            OAM::Render(false, &OAM::MM_SMALL_STAR, 0x70, 0x60, -1, -1, 0);
            OAM::Render(false, &OAM::TIMES, 0x80, 0x60, -1, -1, 0);
            RenderNumber(CountStarsCollectedInLevelToDisplay(0x1d), 0x88, 0x58, 1, 8);
        }
    } else if (gMainScreenLayers & DISPLAY_LAYER_BG3) {
        if ((unsigned int)sPauseDisplayedCourse < 0xfU) {
            unsigned char r4v = CountStarsCollectedInLevelToDisplay(sPauseDisplayedCourse);
            OAM::Render(false, &OAM::MM_SMALL_STAR, 0x58, 0x8c, -1, -1, 0);
            OAM::Render(false, &OAM::TIMES, 0x68, 0x8c, -1, -1, 0);
            RenderNumber(r4v, 0x70, 0x84, 1, 1);
            unsigned char r4v2 = SaveData::GetCoinRecord(sPauseDisplayedCourse);
            OAM::Render(false, &OAM::COIN, 0x88, 0x84, -1, -1, 0);
            OAM::Render(false, &OAM::TIMES, 0x98, 0x8c, -1, -1, 0);
            RenderNumber(r4v2, 0xa0, 0x84, 1, 1);
        }
        if (sPauseState == 0xb) {
            OAM::RenderSub(&OAM::PAUSE, 0x80, 0x60);
        }
    }

    unsigned char f248b = sPauseState;
    if ((unsigned char)(f248b + 0xf9) <= 1) {
        if (sSelectedMenuButton == 3) {
            if (sBackButtonSelected == 0) {
                if (GetOwnerLanguage() == 5) {
                    OAM::Render(true, &OAM::TINY_STAR, 0x46, ((sSelectedMenuButton * 5) + 6) * 8,
                                -1, -1, 0);
                } else if (GetOwnerLanguage() == 3) {
                    OAM::Render(true, &OAM::TINY_STAR, 0x4e, ((sSelectedMenuButton * 5) + 6) * 8,
                                -1, -1, 0);
                } else {
                    OAM::Render(true, &OAM::TINY_STAR, 0x56, ((sSelectedMenuButton * 5) + 6) * 8,
                                -1, -1, 0);
                }
            } else {
                OAM::Render(true, &OAM::TINY_STAR, 0xce, ((sSelectedMenuButton * 5) + 6) * 8, -1,
                            -1, 0);
            }
        } else {
            OAM::Render(true, &OAM::TINY_STAR, 0x26, ((sSelectedMenuButton * 5) + 6) * 8, -1, -1,
                        0);
        }
        if (GetOwnerLanguage() == 5) {
            OAM::Render(0, (OamAttr *)data_ov001_020abd98[sSelectedControllerMode], 0x80, 0x60, -1,
                        -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 4) {
            OAM::Render(0, (OamAttr *)data_ov001_020ac218[sSelectedControllerMode], 0x80, 0x60, -1,
                        -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 3) {
            OAM::Render(0, (OamAttr *)data_ov001_020ac64c[sSelectedControllerMode], 0x80, 0x60, -1,
                        -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        if (GetOwnerLanguage() == 2) {
            OAM::Render(0, (OamAttr *)data_ov001_020acb68[sSelectedControllerMode], 0x80, 0x60, -1,
                        -1, 0x1000, 0x1000, 0, -1);
            return;
        }
        OAM::Render(0, OAM::CONTROLLER_MODE_TEXTS[sSelectedControllerMode], 0x80, 0x60, -1, -1,
                    0x1000, 0x1000, 0, -1);
        return;
    }

    if (f248b != 0xa) {
        return;
    }

    if (sSelectedMenuButton == 2) {
        if (sBackButtonSelected == 0) {
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
        OAM::Render(true, &OAM::TINY_STAR, 0xe, ((sSelectedMenuButton * 6) + 7) * 8, -1, -1, 0);
    }

    if (sPressedPauseArrow == 2) {
        OAM::Render(true, &OAM::SMALL_ARROW_POINTING_LEFT, 0x62, 0x38, -1, -1, 0);
    } else {
        OAM::Render(true, &OAM::SMALL_ARROW_POINTING_LEFT, 0x64, 0x38, -1, -1, 0);
    }

    if (sPressedPauseArrow == 1) {
        OAM::Render(true, &OAM::SMALL_ARROW_POINTING_RIGHT, 0xf6, 0x38, -1, -1, 0);
        return;
    }
    OAM::Render(true, &OAM::SMALL_ARROW_POINTING_RIGHT, 0xf4, 0x38, -1, -1, 0);
}
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
            if (sNumMenuButtons == sSelectedMenuButton && sBackButtonSelected == i &&
                sMenuHighlightDelay == 0)
                v = data_0x2000;
            else
                v = data_0x1000;
            sl = (u16)v;
        }

        idx = i + base;
        u16 *scr = (u16 *)G2S_GetBG1ScrPtr() + data_020755cc[idx];
        for (j = 0; j < data_02075610[idx]; j++) {
            scr[0] = (scr[0] & 0x3ff) | sl;
            scr[0x20] = (scr[0x20] & 0x3ff) | sl;
            scr[0x40] = (scr[0x40] & 0x3ff) | sl;
            scr[0x60] = (scr[0x60] & 0x3ff) | sl;
            scr++;
        }
    }
}
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
            if (sSelectedMenuButton == i && sMenuHighlightDelay == 0)
                t = 0x2000;
            else
                t = 0x1000;
            v = (u16)t;
        }
        p = (u16 *)G2S_GetBG1ScrPtr() + data_020755c8[i];
        for (j = 0; j < 10; j++) {
            p[0] = (p[0] & 0x3ff) | v;
            p[0x20] = (p[0x20] & 0x3ff) | v;
            p[0x40] = (p[0x40] & 0x3ff) | v;
            p[0x60] = (p[0x60] & 0x3ff) | v;
            p++;
        }
    }
}
void dScStage_c::PS_UpdateOptionsMenu() {
    int i;
    int j;
    int counter;
    u16 sel;
    u16 *scr;
    u16 v;

    sel = (sSoundBoxFlashTimer != 0) ? 0x5000 : 0x6000;
    scr = (u16 *)G2S_GetBG1ScrPtr() + 0xae;
    for (j = 0; j < 0xf; j++) {
        scr[0] = (scr[0] & 0x3ff) | sel;
        scr[0x20] = (scr[0x20] & 0x3ff) | sel;
        scr[0x40] = (scr[0x40] & 0x3ff) | sel;
        scr[0x60] = (scr[0x60] & 0x3ff) | sel;
        scr++;
    }

    for (j = 0; j < 2; j++) {
        scr = (u16 *)G2S_GetBG1ScrPtr() + data_020755c0[j];
        v = (sSelectedMenuButton == j) ? 0x4000 : 0x3000;
        counter = 0;
        for (; counter < 0xa; counter++) {
            scr[0] = (scr[0] & 0x3ff) | v;
            scr[0x20] = (scr[0x20] & 0x3ff) | v;
            scr++;
        }
    }

    for (i = 0; i < 2; i++) {
        scr = (u16 *)G2S_GetBG1ScrPtr() + data_020755c4[i];
        v = (sSelectedBacklightButton == i && sBacklightFlashTimer == 0) ? 0x6000 : 0x5000;
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
void dScStage_c::UpdateMenuButtons(bool b) {
    int i;
    for (i = 0; i < sNumMenuButtons; i++) {
        unsigned int v;
        int j;
        u16 *p;
        p = (u16 *)G2S_GetBG1ScrPtr() + sMenuButtonTileOffsets[i];
        if (b == 0) {
            if (sSelectedMenuButton == i && sMenuHighlightDelay == 0) {
                v = 0x2000;
            } else {
                v = 0x1000;
            }
            v = (u16)v;
        } else {
            v = 0x1000;
        }
        if (gGamePaused != 0) {
            u8 t = (u8)(sPauseState + 0xf9);
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
/* Static, like the rest of the PS_ group: restarts the timer, unpauses or
 * stops the music and clears the blend registers, all through globals. */
void dScStage_c::PS_Cleanup() {
    if (data_0209f2a0) {
        gRaceTimer.StartTimer();
        data_0209f2a0 = 0;
    }
    gMainScreenLayers &= ~(DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3);
    gSubScreenLayers &= ~3;
    if (data_0209f280 == 0)
        Sound::UnpauseMusic();
    else
        Sound::StopLoadedMusic_Layer1(0x3c);
    sPauseInputLockout = 0xf;
    G2_BlendNone();
    G2S_BlendNone();
    data_0209f280 = 0;
    gGamePaused = 0;
}
// Pen-down edge on a held touch.
static inline int IsTouchPressed(u8 held, u8 slot) {
    return held != 0 && gTouchEdge[slot * 4] != 0;
}
/* Pause-screen update. */
#pragma opt_common_subs off
void dScStage_c::PS_Update() {
    u8 var_sl;
    u8 var_fp;
    s32 sp0;
    s32 sp4;
    s32 sp8;
    s32 spC;
    s32 sp10;
    s32 var_r0;

    if (sPauseArrowPressTimer != 0) {
        sPauseArrowPressTimer = sPauseArrowPressTimer - data_0208ee44;
        if (sPauseArrowPressTimer == 0)
            sPressedPauseArrow = 0;
    }
    if (sSoundBoxFlashTimer != 0) {
        sSoundBoxFlashTimer = sSoundBoxFlashTimer - data_0208ee44;
        if (sSoundBoxFlashTimer == 0)
            dScStage_c::PS_UpdateOptionsMenu();
    }
    if (sMenuHighlightDelay != 0) {
        sMenuHighlightDelay = sMenuHighlightDelay - data_0208ee44;
        if (sMenuHighlightDelay == 0) {
            u8 st = sPauseState;
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
    if (sBacklightFlashTimer != 0) {
        sBacklightFlashTimer = sBacklightFlashTimer - data_0208ee44;
        if (sBacklightFlashTimer == 0)
            dScStage_c::PS_UpdateOptionsMenu();
    }
    if (data_0209f23c != 0) {
        data_0209f23c = data_0209f23c - data_0208ee44;
        if (data_0209f23c == 0)
            dScStage_c::PS_UpdateOptionsMenu();
    }
    if (sMenuWaitTimer != 0) {
        sMenuWaitTimer = sMenuWaitTimer - data_0208ee44;
        return;
    }

    {
        u8 nxt = sNextPauseState;
        u8 cur = sPauseState;
        if (cur != nxt)
            sPauseState = nxt;
    }

    switch (sPauseState) {
    case 0: {
        u8 i;
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6000,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                         GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01);
        G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8000,
                             GX_BG_CHARBASE_0x10000);
        i = 0;
        do {
            if (CountStarsCollectedInLevelToDisplay(i) != 0)
                break;
            i = (u8)(i + 1);
        } while (i < 0xf);
        if (i == 0xf) {
            sPauseDisplayedCourse = i;
            sCourseArrowsHidden = 1;
        } else {
            if (SublevelToLevel(gLastCourseSublevel) >= 0xf) {
                sPauseDisplayedCourse = 0xf;
            } else {
                sPauseDisplayedCourse = SublevelToLevel(gLastCourseSublevel);
            }
            sCourseArrowsHidden = 0;
        }
        Message::Display(data_020756d0[sPauseDisplayedCourse]);
        sMenuButtonTileOffsets[0] = 0x80;
        sMenuButtonTileOffsets[1] = 0x120;
        sMenuButtonTileOffsets[2] = 0x1c0;
        sMenuButtonTileOffsets[3] = 0x260;
        sNumMenuButtons = 4;
        sSelectedMenuButton = 0;
        dScStage_c::UpdateMenuButtons(1);
        data_0209f240 = 0;
        dScStage_c::PS_UpdateOkAndBackButtons(1);
        SetBg1Offset(0, 0);
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        gMainScreenLayers |= DISPLAY_LAYER_BG1 | DISPLAY_LAYER_BG2 | DISPLAY_LAYER_BG3;
        gSubScreenLayers |= 3;
        sNextPauseState = 1;
        return;
    }
    case 1: {
        if (sPauseInputLockout != 0)
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
                (sSelectedMenuButton == 0 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 0)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 0;
                sMenuWaitTimer = data_0208ee44 << 3;
                dScStage_c::UpdateMenuButtons(0);
                gGamePaused = 2;
                func_02012790(3);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0xb0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x48) < 0x20) ||
                (sSelectedMenuButton == 1 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 1)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 1;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                sNextPauseState = 7;
                func_02012790(0x53);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x70) < 0x20) ||
                (sSelectedMenuButton == 2 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 2)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 2;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                sNextPauseState = 9;
                func_02012790(0x54);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x98) < 0x20) ||
                (sSelectedMenuButton == 3 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 3)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 3;
                sMenuWaitTimer = data_0208ee44 << 3;
                dScStage_c::UpdateMenuButtons(0);
                sNextPauseState = 0xc;
                func_02012790(0x55);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 0x28) < 0xb0)
                    return;
                return;
            }
        }
        if (sCourseArrowsHidden != 0)
            return;
        {
            u8 *de8_sb;
            var_sl = 0;
            var_fp = var_sl;
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
                var_r0 = sp0;
                sl2 = gActivePlayerSlot;
                {
                    u8 aa = gTouchHeld[sl2 * 4];
                    a = aa;
                }
                de_off = sl2 * 4;
                if ((a != 0) && (TOUCHP(de_off)->edge != 0)) {
                    var_r0 = 1;
                }
                if ((var_r0 != 0) && ((u32)(vx = TOUCHP(sl2 * 4)->x) < 0x38U) &&
                    ((u32)TOUCHP(sl2 * 4)->y < 0x20U)) {
                    u8 t;
                    sPauseDisplayedCourse = (u8)(sPauseDisplayedCourse - 1);
                    var_fp = 1;
                    t = sPauseDisplayedCourse;
                    if (t == 0xFF)
                        sPauseDisplayedCourse = 0xF;
                    {
                        u8 *p0 = &sPauseArrowPressTimer;
                        u8 *p1 = &sPauseArrowPressTimer;
                        u8 *p = ((u8)(vx & 0xff) < 0x38) ? p0 : p1;
                        sPressedPauseArrow = 1;
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
                            sPauseDisplayedCourse = (u8)(sPauseDisplayedCourse + 1);
                            sPressedPauseArrow = 2;
                            var_fp = 1;
                            if (sPauseDisplayedCourse == 0x10) {
                                sPauseDisplayedCourse = (u8)sp8;
                            }
                            sPauseArrowPressTimer = (u8)(data_0208ee44 * 3);
                        }
                    }
                }
                if ((sPauseDisplayedCourse == 0xF) ||
                    (CountStarsCollectedInLevelToDisplay(sPauseDisplayedCourse) != 0)) {
                    var_sl = 1;
                    if (sPressedPauseArrow == 1) {
                        func_02012790(spC);
                    } else if (sPressedPauseArrow == 2) {
                        func_02012790(sp10);
                    }
                }
            } while (var_sl == 0);
        }
        if (var_fp == 0)
            return;
        Message::DisplayText(data_020756d0[sPauseDisplayedCourse]);
        return;
    }
    case 2: {
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6000,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8000,
                             GX_BG_CHARBASE_0x10000);
        sNextPauseState = 3;
        sPauseDisplayedCourse = SublevelToLevel(gCurrentSublevel);
        Message::DisplayPauseText(0x277, sPauseDisplayedCourse);
        sMenuButtonTileOffsets[0] = 0x60;
        sMenuButtonTileOffsets[1] = 0x100;
        sMenuButtonTileOffsets[2] = 0x1a0;
        sMenuButtonTileOffsets[3] = 0x240;
        sNumMenuButtons = 4;
        sSelectedMenuButton = 0;
        dScStage_c::UpdateMenuButtons(1);
        SetBg3Offset(0, 0);
        gMainScreenLayers |= DISPLAY_LAYER_BG3;
        if (func_02029408() != 0 && gPlayerHealth[gLocalPlayerId] != 0) {
            SetBg2Offset(0, 0);
            SetSubBg0Offset(0, 0);
            SetSubBg1Offset(0, 0);
            gMainScreenLayers |= DISPLAY_LAYER_BG2;
            gSubScreenLayers |= 3;
            return;
        }
        sNextPauseState = 0xb;
        return;
    }
    case 3: {
        if (sPauseInputLockout != 0)
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
                (sSelectedMenuButton == 0 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 0)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 0;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                gGamePaused = 2;
                func_02012790(3);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x40) < 0x20) ||
                (sSelectedMenuButton == 1 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 1)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 1;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                sNextPauseState = 7;
                func_02012790(0x53);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x68) < 0x20) ||
                (sSelectedMenuButton == 2 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 2)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 2;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                sNextPauseState = 9;
                func_02012790(0x54);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (!((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x90) < 0x20) &&
                (sSelectedMenuButton != 3 || IsButtonInputValid() == 0))
                return;
            if (sSelectedMenuButton == 3)
                sMenuHighlightDelay = data_0208ee44 << 2;
            sSelectedMenuButton = 3;
            dScStage_c::UpdateMenuButtons(0);
            sMenuWaitTimer = data_0208ee44 << 3;
            sNextPauseState = 0x13;
            func_02012790(0x56);
            return;
        }
    }
    case 4: {
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6800,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8000,
                             GX_BG_CHARBASE_0x10000);
        sNextPauseState = 5;
        Message::DisplayPauseTextVS(0);
        sMenuButtonTileOffsets[0] = 0xa0;
        sMenuButtonTileOffsets[1] = 0x140;
        sMenuButtonTileOffsets[2] = 0x1e0;
        sNumMenuButtons = 3;
        sSelectedMenuButton = 0;
        dScStage_c::UpdateMenuButtons(1);
        if (func_02029408() == 0) {
            sNextPauseState = 0xb;
            return;
        }
        SetBg2Offset(0, 0);
        SetBg3Offset(0, 0);
        gMainScreenLayers |= DISPLAY_LAYER_BG2;
        gMainScreenLayers &= ~DISPLAY_LAYER_BG3;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        gSubScreenLayers |= 3;
        return;
    }
    case 5: {
        if (sPauseInputLockout != 0)
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
                (sSelectedMenuButton == 0 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 0)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 0;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                gGamePaused = 2;
                func_02012790(3);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x50) < 0x20) ||
                (sSelectedMenuButton == 1 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 1)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 1;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                sNextPauseState = 0x13;
                data_ov002_02111150 = 1;
                func_02012790(0x56);
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (!((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x78) < 0x20) &&
                (sSelectedMenuButton != 2 || IsButtonInputValid() == 0))
                return;
            if (sSelectedMenuButton == 2)
                sMenuHighlightDelay = data_0208ee44 << 2;
            sSelectedMenuButton = 2;
            dScStage_c::UpdateMenuButtons(0);
            sMenuWaitTimer = data_0208ee44 << 3;
            sNextPauseState = 6;
            data_ov002_02111150 = 1;
            func_02012790(0x55);
            return;
        }
    }
    case 6: {
        dScene_c::StartSceneFade(1, 0, 0);
        gNextActorUpdateMask |= ACTOR_UPDATE_MASK_STAGE_ENDING;
        gActorUpdateMask = gNextActorUpdateMask;
        data_0209f280 = 1;
        dScStage_c::PS_Cleanup();
        return;
    }
    case 7: {
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x7800,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        if (sPausedWithSelect != 0) {
            if (GetOwnerLanguage() == 5) {
                LoadCompressedFileAt(0xb001, G2S_GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 4) {
                LoadCompressedFileAt(0xac01, G2S_GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 3) {
                LoadCompressedFileAt(0xa801, G2S_GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 2) {
                LoadCompressedFileAt(0xa401, G2S_GetBG1ScrPtr());
            } else {
                LoadCompressedFileAt(0xa001, G2S_GetBG1ScrPtr());
            }
        } else {
            if (GetOwnerLanguage() == 5) {
                LoadCompressedFileAt(0xb009, G2S_GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 4) {
                LoadCompressedFileAt(0xac09, G2S_GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 3) {
                LoadCompressedFileAt(0xa809, G2S_GetBG1ScrPtr());
            } else if (GetOwnerLanguage() == 2) {
                LoadCompressedFileAt(0xa409, G2S_GetBG1ScrPtr());
            } else {
                LoadCompressedFileAt(0xa009, G2S_GetBG1ScrPtr());
            }
        }
        sMenuButtonTileOffsets[0] = 0x80;
        sMenuButtonTileOffsets[1] = 0x120;
        sMenuButtonTileOffsets[2] = 0x1c0;
        sNumMenuButtons = 3;
        sNextPauseState = 8;
        {
            u8 cur = data_0209f4ae[gActivePlayerSlot * 0x18];
            sSelectedControllerMode = cur;
            sSelectedMenuButton = cur;
        }
        dScStage_c::UpdateMenuButtons(0);
        Message::DisplayControllerModeText(0x280);
        REG_BG2CNT = (((sSelectedControllerMode + 0xd) << 8) | (REG_BG2CNT & 0x43)) | 0x10;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        G2S_SetBG0Offset(0, 0);
        G2S_SetBG1Offset(0, 0);
        sBackButtonSelected = 0;
        data_0209f240 = 0;
        dScStage_c::PS_UpdateOkAndBackButtons(1);
        SetBg2Offset(0, 0);
        gMainScreenLayers &= ~DISPLAY_LAYER_BG1;
        gMainScreenLayers |= DISPLAY_LAYER_BG2;
        gMainScreenLayers &= ~DISPLAY_LAYER_BG3;
        gSubScreenLayers |= 3;
        return;
    }
    case 8: {
        u8 slot;
        u8 ty;
        u8 tx;
        u8 relx;
        u8 firstw;
        int idxa;
        if (sPauseInputLockout != 0)
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
            G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                                 GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x10000);
            sMenuHighlightDelay = data_0208ee44 << 2;
            {
                u8 a = gTouchHeld[slot * 4];
                if (a == 0 || (IsTouchPressed(a, slot) && sSelectedControllerMode == 0)) {
                    sSelectedControllerMode = 0;
                    SetControllerMode(0);
                    sMenuWaitTimer = data_0208ee44 << 3;
                    func_02012790(0x116);
                    gGamePaused = 2;
                } else {
                    sSelectedControllerMode = 0;
                    data_0209f240 = 0;
                    dScStage_c::PS_UpdateOkAndBackButtons(0);
                    func_02012790(0x5b);
                }
                sSelectedMenuButton = 0;
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
            G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                                 GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x10000);
            sMenuHighlightDelay = data_0208ee44 << 2;
            {
                u8 a = gTouchHeld[slot * 4];
                if (a == 0 || (IsTouchPressed(a, slot) && sSelectedControllerMode == 1)) {
                    sSelectedControllerMode = 1;
                    SetControllerMode(1);
                    sMenuWaitTimer = data_0208ee44 << 3;
                    func_02012790(0x116);
                    gGamePaused = 2;
                } else {
                    sSelectedControllerMode = 1;
                    data_0209f240 = 0;
                    dScStage_c::PS_UpdateOkAndBackButtons(0);
                    func_02012790(0x5b);
                }
                sSelectedMenuButton = 1;
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
            G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16,
                                 GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x10000);
            sMenuHighlightDelay = data_0208ee44 << 2;
            {
                u8 a = gTouchHeld[slot * 4];
                if (a == 0 || (IsTouchPressed(a, slot) && sSelectedControllerMode == 2)) {
                    sSelectedControllerMode = 2;
                    SetControllerMode(2);
                    sMenuWaitTimer = data_0208ee44 << 3;
                    func_02012790(0x116);
                    gGamePaused = 2;
                } else {
                    sSelectedControllerMode = 2;
                    data_0209f240 = 0;
                    dScStage_c::PS_UpdateOkAndBackButtons(0);
                    func_02012790(0x5b);
                }
                sSelectedMenuButton = 2;
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
                (sPausedWithSelect == 0 && IsTouchPressed(a0, slot) && (u8)(tx - 0xd8) < 0x20 &&
                 (u8)(ty - 0x98) < 0x20) ||
                IsButtonInputValid() != 0) {
                if (relx < firstw && (u8)(gTouchY[gActivePlayerSlot * 4] - 0x98) < 0x20 &&
                    IsButtonInputValid() != 0) {
                    sBackButtonSelected = 0;
                } else {
                    u8 slot2 = gActivePlayerSlot;
                    if ((u8)(gTouchX[slot2 * 4] - 0xd8) < 0x20 &&
                        (u8)(gTouchY[slot2 * 4] - 0x98) < 0x20) {
                        sBackButtonSelected = 1;
                    }
                }
                sSelectedMenuButton = 3;
                SetControllerMode(sSelectedControllerMode);
                data_0209f240 = 1;
                sMenuHighlightDelay = data_0208ee44 << 2;
                dScStage_c::PS_UpdateOkAndBackButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                if (sBackButtonSelected == 0) {
                    gGamePaused = 2;
                    func_02012790(0x116);
                    return;
                }
                func_02012790(0x5c);
                if (SublevelToLevel(gCurrentSublevel) == 0x1d) {
                    sNextPauseState = 0;
                } else {
                    sNextPauseState = 2;
                }
                return;
            }
            data_0209f2a4 = 0;
            return;
        }
    }
    case 9: {
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8000,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        sNextPauseState = 0xa;
        data_0209f234 = GetSoundMode();
        sSelectedBacklightButton = data_0208ee3c ^ 1;
        data_0209f1fc = data_0208ee40;
        sPressedPauseArrow = 0;
        Message::DisplayOptionsMenuText(data_0209f234 + 0x284);
        sNumMenuButtons = 2;
        sSelectedMenuButton = 0;
        dScStage_c::PS_UpdateOptionsMenu();
        sBackButtonSelected = 0;
        data_0209f240 = 0;
        dScStage_c::PS_UpdateOkAndBackButtons(1);
        if (SublevelToLevel(gCurrentSublevel) == 0x1d) {
            gMainScreenLayers &= ~DISPLAY_LAYER_BG1;
        } else {
            gMainScreenLayers &= ~DISPLAY_LAYER_BG2;
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
                sPressedPauseArrow = 2;
                sPauseArrowPressTimer = data_0208ee44 * 3;
                sSelectedMenuButton = 0;
                Message::DisplayOptionsMenuText(data_0209f234 + 0x284);
                sSoundBoxFlashTimer = 8;
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
                sPressedPauseArrow = 1;
                sPauseArrowPressTimer = data_0208ee44 * 3;
                sSelectedMenuButton = 0;
                SetSoundMode(data_0209f234);
                func_02012790(0x64);
                sSoundBoxFlashTimer = 8;
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
                    if (sSelectedBacklightButton != 0) {
                        func_02012790(0x66);
                        TurnBacklightOn();
                    } else {
                        sBacklightFlashTimer = data_0208ee44 << 2;
                        func_02012790(0x67);
                    }
                    sSelectedBacklightButton = 0;
                    sSelectedMenuButton = 1;
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
                    if (sSelectedBacklightButton == 0) {
                        func_02012790(0x66);
                        TurnBacklightOff();
                    } else {
                        sBacklightFlashTimer = data_0208ee44 << 2;
                        func_02012790(0x67);
                    }
                    sSelectedBacklightButton = 1;
                    sSelectedMenuButton = 1;
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
                    if (sSelectedMenuButton != 1) {
                        sSelectedMenuButton = 1;
                        dScStage_c::PS_UpdateOptionsMenu();
                    }
                    sSelectedBacklightButton = sSelectedBacklightButton ^ 1;
                    if (sSelectedBacklightButton == 0) {
                        TurnBacklightOn();
                    } else {
                        TurnBacklightOff();
                    }
                    sBacklightFlashTimer = data_0208ee44 << 2;
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
                    sBackButtonSelected = 0;
                } else {
                    u8 s4 = gActivePlayerSlot;
                    if ((u8)(gTouchX[s4 * 4] - 0xd8) < 0x20 &&
                        (u8)(gTouchY[s4 * 4] - 0x98) < 0x20) {
                        sBackButtonSelected = 1;
                    }
                }
                sSelectedMenuButton = 2;
                sMenuWaitTimer = data_0208ee44 << 3;
                dScStage_c::PS_UpdateOptionsMenu();
                data_0209f240 = 1;
                sMenuHighlightDelay = data_0208ee44 << 2;
                dScStage_c::PS_UpdateOkAndBackButtons(0);
                if (sBackButtonSelected == 0) {
                    SetSoundMode(data_0209f234);
                    if (data_0209f1fc != 0) {
                        data_0208ee40 = 1;
                    } else {
                        data_0208ee40 = 0;
                    }
                    gGamePaused = 2;
                    func_02012790(0x116);
                    return;
                }
                func_02012790(0x5d);
                if (SublevelToLevel(gCurrentSublevel) == 0x1d) {
                    sNextPauseState = 0;
                } else {
                    sNextPauseState = 2;
                }
                return;
            }
        }
    }
    case 0xb: {
        if (IsButtonInputValid() == 0)
            return;
        sMenuHighlightDelay = data_0208ee44 << 2;
        gGamePaused = 2;
        func_02012790(3);
        return;
    }
    case 0xc: {
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6800,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        Message::DisplaySaveMenuText(0x290);
        sMenuButtonTileOffsets[0] = 0xa0;
        sMenuButtonTileOffsets[1] = 0x140;
        sMenuButtonTileOffsets[2] = 0x1e0;
        sNumMenuButtons = 3;
        sSelectedMenuButton = 0;
        dScStage_c::UpdateMenuButtons(1);
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        sNextPauseState = 0xd;
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
                (sSelectedMenuButton == 0 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 0)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 0;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                gGamePaused = 2;
                func_02012790(3);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x50) < 0x20) ||
                (sSelectedMenuButton == 1 && IsButtonInputValid() != 0)) {
                if (sSelectedMenuButton == 1)
                    sMenuHighlightDelay = data_0208ee44 << 2;
                sSelectedMenuButton = 1;
                dScStage_c::UpdateMenuButtons(0);
                sMenuWaitTimer = data_0208ee44 << 3;
                sNextPauseState = 0xe;
                func_02012790(0x5e);
                if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                    return;
                return;
            }
        }
        {
            u8 slot = gActivePlayerSlot;
            if (!((u8)(gTouchX[slot * 4] - 8) < 0xf0 && (u8)(gTouchY[slot * 4] - 0x78) < 0x20) &&
                (sSelectedMenuButton != 2 || IsButtonInputValid() == 0))
                return;
            if (sSelectedMenuButton == 2)
                sMenuHighlightDelay = data_0208ee44 << 2;
            sSelectedMenuButton = 2;
            dScStage_c::UpdateMenuButtons(0);
            sMenuWaitTimer = data_0208ee44 << 3;
            sNextPauseState = 0x10;
            func_02012790(0x5f);
            if ((u8)(gTouchX[gActivePlayerSlot * 4] - 8) < 0xf0)
                return;
            return;
        }
    }
    case 0xe: {
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x7000,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        sNextPauseState = 0xf;
        Message::DisplaySaveStatusText(0x295);
        gMainScreenLayers &= ~DISPLAY_LAYER_BG1;
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
        if (sSelectedMenuButton != 1) {
            gGamePaused = 2;
            return;
        }
        dScene_c::StartSceneFade(1, 0, 0);
        data_0209f280 = 1;
        dScStage_c::PS_Cleanup();
        gNextActorUpdateMask |= ACTOR_UPDATE_MASK_STAGE_ENDING;
        gActorUpdateMask = gNextActorUpdateMask;
        Sound::StopLoadedMusic_Layer1(0xa);
        return;
    }
    case 0x10: {
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        sNextPauseState = 0x11;
        Message::DisplayDontSaveText(0x297);
        sSelectedMenuButton = 1;
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
            u8 slot;
            u8 tx;
            int rel;
            int rely;
            slot = gActivePlayerSlot;
            tx = gTouchX[slot * 4];
            rel = tx - 0x28;
            if ((u8)rel < 0x50) {
                rely = gTouchY[slot * 4] - 0x98;
                if ((u8)rely < 0x20) {
                    if (sSelectedMenuButton == 0)
                        sMenuHighlightDelay = data_0208ee44 << 2;
                    if ((u8)rel < 0x50 && (u8)rely < 0x20) {
                        sMenuWaitTimer = data_0208ee44 << 3;
                        sNextPauseState = 0x12;
                        func_02012790(0x60);
                    } else {
                        func_02012790(0x51);
                    }
                    sSelectedMenuButton = 0;
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
                    if (sSelectedMenuButton == 1)
                        sMenuHighlightDelay = data_0208ee44 << 2;
                    if ((u8)rel < 0x50 && (u8)rely < 0x20) {
                        sMenuWaitTimer = data_0208ee44 << 3;
                        sNextPauseState = 0x12;
                        func_02012790(3);
                    } else {
                        func_02012790(0x52);
                    }
                    sSelectedMenuButton = 1;
                    dScStage_c::PS_UpdateSaveMenu(0);
                    return;
                }
            }
        }
    }
    case 0x12: {
        if (sSelectedMenuButton != 0) {
            gGamePaused = 2;
            return;
        }
        dScene_c::StartSceneFade(1, 0, 0);
        data_0209f280 = 1;
        dScStage_c::PS_Cleanup();
        gNextActorUpdateMask |= ACTOR_UPDATE_MASK_STAGE_ENDING;
        gActorUpdateMask = gNextActorUpdateMask;
        return;
    }
    case 0x13: {
        int t = (gGameMode == 1);
        if (t != false && data_0209fc68 == 0) {
            dScene_c::StartSceneFade(6, 1, 0);
            data_0209f280 = 1;
            dScStage_c::PS_Cleanup();
            gNextActorUpdateMask |= ACTOR_UPDATE_MASK_STAGE_ENDING;
            gActorUpdateMask = gNextActorUpdateMask;
            return;
        }
        LoadLevelNoReturn(2, 0, -1, 0);
        gNextActorUpdateMask |= ACTOR_UPDATE_MASK_STAGE_ENDING;
        gActorUpdateMask = gNextActorUpdateMask;
        data_0209f280 = 1;
        gGamePaused = 2;
        return;
    }
    default:
        return;
    }
}

/* Can the current player pause? Forwards to daPly_c::CanPause on the active
 * entry of the live player list (gPlayers, indexed by gLocalPlayerId).
 * dScStage_c polls it before opening the pause menu. */

int func_02029408(void) {
    return gPlayers[gLocalPlayerId]->CanPause();
}

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

    subLayers = gSubScreenLayers;
    if (subLayers & 4) {
        gSubScreenLayers = subLayers & ~4;
        subLayers = gSubScreenLayers;
        GXS_SetVisiblePlane(subLayers);
    }

    specialPauseState = sPausedWithSelect;
    mainLayers = gMainScreenLayers;
    isVsMode = 0;

    sPauseState = 0;
    sSelectedMenuButton = 0;
    sPauseDisplayedCourse = 0;
    data_0209f230 = 0;
    sCourseArrowsHidden = 0;
    sPauseArrowPressTimer = 0;
    sMenuWaitTimer = 0;
    data_0209f240 = 0;
    sPressedPauseArrow = 0;
    sBacklightFlashTimer = 0;
    sPauseInputLockout = 0xf;
    data_0209f23c = 0;
    sMenuHighlightDelay = 0;
    sBackButtonSelected = 0;
    data_0209f280 = 0;

    mainBlendTargets = mainLayers ^ 0x10;
    subBlendTargets = subLayers ^ DISPLAY_LAYER_OBJ;

    if (specialPauseState != 0) {
        sNextPauseState = 7;
    } else {
        if (gGameMode == 1)
            isVsMode = 1;
        if (isVsMode != 0 && data_0209fc68 == 0) {
            sNextPauseState = 4;
        } else {
            if (SublevelToLevel(gCurrentSublevel) == 0x1d)
                sNextPauseState = 0;
            else
                sNextPauseState = 2;
        }
    }

    Sound::PauseMusic();
    G2_SetBlendBrightness(mainBlendTargets | 0x20, -7);
    G2S_SetBlendBrightness(subBlendTargets | 0x20, -7);
}

int func_02029600(int x) {
    int b = (int)(gGameMode == 2);
    if (b == 0) {
        unsigned char saved = *(u8 *)&data_ov002_02111148;
        int r = GetMinimapID((dActor_c *)x, -1);
        if (r == saved)
            return 1;
    }
    return 0;
}

int IsButtonInputValid(void) {
    int mode;
    int val;

    if (((gGamePaused | data_0209f20c | gVsExitScreenActive) & 0xff) == 0 && data_0209f204 == 0 &&
        gVsExitScreenActive == 0) {
        mode = data_0209caa0.file.controllerMode;
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

/* Vtable slot 2. Every dispatcher calls it as `ldr r1,[r0]; ldr r1,[r1,#8];
   blx r1` with r0 = the callback object. The matrix at +0x4 stays zero until
   UpdateMinimap; the static initializer does not seed it. */
int dScStage_c::graphCallback_c::GraphCallback2() {
    G2S_SetBG3Affine(&matrix, originX, originY, targetX, targetY);
    return 1;
}

/* Vtable slot 1 of dScStage_c::graphCallback_c: render every particle. */
int dScStage_c::graphCallback_c::GraphCallback1() {
    Particle::RenderAll();
    return 1;
}

/* Copy a 16-byte descriptor into the callback object's matrix (+0x4) and store
   the four affine params after it. The object is one 0x24-byte object; the
   symbols.txt row data_0209f3c8 at the matrix stays only because dsd needs a
   symbol at every relocs.txt target, and the copy's pool word points there.
   The MinimapDesc cast stays because the ROM's copy is
   a single ldm/stm block move and a struct copy of four named fields
   scalarizes. */
void UpdateMinimap(MinimapDesc *desc, u32 a, u32 b, u32 c, u32 d) {
    *(MinimapDesc *)(&sStageGraphCallback.matrix) = *desc;
    sStageGraphCallback.originX = a;
    sStageGraphCallback.originY = b;
    sStageGraphCallback.targetX = c;
    sStageGraphCallback.targetY = d;
}

/* The fader wipe array. dScStage_c::InitResources fills data_0209f324 with
   __cxa_vec_new(7, 0x60, 8, &dFdWipe_cC1, &dFdWipe_cD1): seven objects of 0x60,
   which is sizeof(dFdWipe_c). dScStage_c::CleanupResources tears the array
   down and zeroes it. */
void StartEntranceFaderWipe(int index) {
    dFdWipe_c *f = &((dFdWipe_c *)data_0209f324)[index];
    dScene_c::SetFaders(f);
    f->SetToEnd();
}

/* Start the exit wipe for the current character (per-character wipe table). */
void StartExitCharacterWipe(void) {
    StartExitFaderWipe(data_020755bc[data_0209caa0.file.mCharacter]);
}

void StartExitFaderWipe(int index) {
    dFdWipe_c *f = &((dFdWipe_c *)data_0209f324)[index];
    dScene_c::SetFaders(f);
    f->SetToStart();
}

void FUN_02029934(void) {
    dFdBrightness_c *o = gSceneFader;
    if (o->IsBetweenStartAndEnd() == 0) {
        data_0209d4b0 = 0;
        G2_BlendNone();
    }
}

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
void FUN_02029980(void) {
    if (gSceneFader->IsBetweenStartAndEnd() != 0)
        return;
    WipeFader *o2 = (WipeFader *)((dFdWipe_c *)data_0209f324 + 6);
    data_0209d4b0 = (int)o2;
    o2->w8();
    o2->w3(0x1e, 0);
}

void FUN_020299f4(void) {
    if (gSceneFader->IsBetweenStartAndEnd() != 0)
        return;
    WipeFader *o2 = (WipeFader *)((dFdWipe_c *)data_0209f324 + 6);
    data_0209d4b0 = (int)o2;
    o2->w9();
    o2->w4(0x1e, 0);
}

void FUN_02029a68(void) {
    if (data_0209d4b0 == (int)((dFdWipe_c *)data_0209f324 + 6)) {
        data_0209d4b0 = 0;
        G2_BlendNone();
    }
    data_0209f274 = 0;
}

/* One entry past the six wipes' 0x240 bytes: two words at +0x4/+0x8. */
typedef struct WipeEntry {
    u32 pad0;
    u32 field4;
    u32 field8;
} WipeEntry;

void FUN_02029ab0(void) {
    WipeEntry *entry;
    dScene_c::SetAndStopColorFader();
    entry = (WipeEntry *)((dFdWipe_c *)data_0209f324 + 6);
    data_0209d4b0 = (int)entry;
    entry->field4 = 0xc00;
    entry->field8 = 0;
    data_0209f274 = 1;
}

void EnterBigBoosHaunt(void) {
    LoadLevel(0xc, 0, 0, 0, 0);
}

/* Kill the player: if lives remain, go to the next level with reason 2,
   otherwise trigger a game-over scene fade. Then play the death sound. */
void KillPlayer(void) {
    if (gNumLives != 0)
        SetNextLevel(2);
    else
        dScene_c::StartSceneFade(8, 0, 0);
    StartExitFaderWipe(4);
    func_02012790(0x18);
}

/* The player hit a death plane: either restart (SetNextLevel) or trigger a
   scene fade, then always exit with StartExitFaderWipe. */
void HitDeathPlane(int arg) {
    if (gNumLives != 0 || arg == 0)
        SetNextLevel(arg);
    else
        dScene_c::StartSceneFade(8, 0, 0);
    StartExitFaderWipe(6);
}

/* Exit the current level: next level 1 (the castle grounds), then the exit
   wipe. */
void ExitLevel(void) {
    SetNextLevel(1);
    StartExitCharacterWipe();
}

void SetNextLevel(int arg) {
    int v = gCurrentSublevel;
    unsigned int i;
    struct LevelReturnEntry *p;

    if (v == 0x22) {
        if (arg != 1) {
            gReturnSublevel = 1;
            gReturnEntranceId = 0xa;
        }
    } else {
        p = data_02075638;
        for (i = 0; i < 8; i++) {
            if (v == p->level) {
                gReturnSublevel = p->returnLevel;
                gReturnEntranceId = p->entrance;
                break;
            }
            p++;
        }
    }

    if (gReturnSublevel < 0 || SublevelToLevel(v) == 0x1d) {
        gNextSublevel = 1;
        sNextEntranceId = 0xd;
    } else {
        gNextSublevel = gReturnSublevel;
        sNextEntranceId = gReturnEntranceId;
    }

    gReturnSublevel = -1;
    gNextStarId = gCurrentStarId;
    SetNextStar();
    sReturnState = arg;
}

void SetNextStar(void) {
    int level = SublevelToLevel(gNextSublevel);
    int b;
    if (gNextStarId == 1)
        return;
    b = (int)(gGameMode == 1);
    if (b != 0)
        return;
    if (level != -1) {
        if (level <= 0xe)
            return;
        if (level == 0x13)
            return;
    }
    gNextStarId = 1;
}

/* The actor death table: one 16-word row of bits per level part, indexed by
   an actor's deathTableID. */
void DeathTable_ClearBit(s32 id) {
    if (id < 0)
        return;
    s32 level = GetLevelPart((s32)gCurrentSublevel);
    u32 *table = sActorDeathTable[level];
    u32 bitMask = ~(1u << (id & 0x1f));
    s32 word = id >> 5;
    table[word] = table[word] & bitMask;
}

void DeathTable_SetBit(s32 id) {
    if (id < 0)
        return;
    s32 level = GetLevelPart((s32)gCurrentSublevel);
    u32 *table = sActorDeathTable[level];
    s32 word = id >> 5;
    u32 val = table[word];
    u32 bitMask = 1u << (id & 0x1f);
    table[word] = val | bitMask;
}

u32 DeathTable_GetBit(s32 id) {
    if (id < 0)
        return 0;
    s32 level = GetLevelPart((s32)gCurrentSublevel);
    u32 *table = sActorDeathTable[level];
    s32 word = id >> 5;
    s32 bit = id & 0x1f;
    u32 bitMask = 1u << bit;
    return table[word] & bitMask;
}

/* The event-flag bitfield accessors: Clear/Set/Get over the one-word event
   table sEventFlags. */
namespace Event {

s32 ClearBit(u32 bit) {
    return sEventFlags &= ~(1 << bit);
}

void SetBit(u32 bit) {
    sEventFlags |= 1 << bit;
}

s32 GetBit(u32 bit) {
    return sEventFlags & (1 << bit);
}

}

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

void FUN_0202a130(void) {
    if (data_0209f2bc != 0)
        return;
    data_0209f308 = 0xb4;
    data_0209f1f4 = 0;
}

void dScStage_c::RenderVsModeCountdown() {
    int count = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (gPlayers[i]) {
            int t = (gPlayers[i]->mLevelEntryDone != 0);
            if (t == 1)
                count++;
        }
    }
    if (((dFdBrightness_c *)gSceneFader)->IsAtStart() == 0)
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

s32 NumVsStarsObtained(void) {
    int i = 1;
    signed char sum = gNumStageStars[0];
    signed char *p;
    if (i >= (int)gNumPlayers)
        return sum;
    p = &data_0209f311;
    do {
        sum = (signed char)(sum + *p);
        i++;
        p++;
    } while (i < (int)gNumPlayers);
    return sum;
}

void GiveVsStars(int idx, int delta) {
    gNumStageStars[idx] += delta;
}

/* dScStage_c::IsPauseDisabled() at 0x0202c8f8. Static: reads only globals, and no
 * caller sets up an object. */

int dScStage_c::IsPauseDisabled() {
    daPly_c *p = gPlayers[0];
    if (p != 0 && p->func_ov002_020bd8ac() == 0)
        return 1;
    return 0;
}

/* The level's eight spike-bomb slots (daKirai_c's mSlotIndex). */
void ClearSpikeBomb(int idx) {
    if (idx < 0)
        return;
    if (idx < 8)
        data_0209f3a4[idx] = 0;
}

int AddSpikeBomb(void *p) {
    int i;
    for (i = 0; i < 8; i++) {
        if (data_0209f3a4[i] == 0) {
            data_0209f3a4[i] = p;
            return i;
        }
    }
    return -1;
}

void SetStarMarker(int i, dActor_c *actor, int v2) {
    STAR_MARKERS[i] = actor;
    gStarMarkerTypes[i] = v2;
}

void OpenCannonInCurLevel(void) {
    OpenCannonInLevel(gCurrentSublevel);
}

int IsCannonOpenInCurLevel(void) {
    return IsCannonOpenInLevel(gCurrentSublevel);
}

void CollectStarInCurLevel(s32 starID) {
    CollectStarInLevel(gCurrentSublevel, starID);
}

s32 IsStarCollectedInCurLevel(s32 starID) {
    return IsStarCollectedInLevel(gCurrentSublevel, starID);
}

signed char NumRedCoins(void) {
    int i = 1;
    signed char sum = gNumRedCoins[0];
    signed char *p;
    if (i >= (int)gNumPlayers)
        return sum;
    p = &data_0209f30d;
    do {
        sum = (signed char)(sum + *p);
        i++;
        p++;
    } while (i < (int)gNumPlayers);
    return sum;
}

int GiveRedCoins(int i, int amt) {
    signed char *p = &gNumRedCoins[i];
    gNumRedCoins[i] = (signed char)(gNumRedCoins[i] + amt);
    int v = gNumRedCoins[i];
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

short NumCoins(void) {
    int i = 1;
    int n = gNumPlayers;
    short *p = gNumCoins;
    short sum = p[0];
    if (n <= 1)
        return sum;
    do {
        sum = sum + p[i];
        i++;
    } while (i < n);
    return sum;
}

void GiveCoins(int idx, int amount) {
    short *p = gNumCoins;
    short cur = p[idx];
    short v = cur + amount;
    p[idx] = v;
    v = p[idx];
    if (v > 0xff) {
        p[idx] = 0xff;
    } else if (v < 0) {
        p[idx] = 0;
    }
    func_ov002_020b18f0((char *)gPlayers[idx]);
}

/* Returns the clamped health, which daPly_c::GiveHealth hands straight back. */
int GiveHealth(int idx, int amt) {
    int v;
    gPlayerHealth[idx] += amt;
    v = gPlayerHealth[idx];
    if (v > 0x880) {
        v = 0x880;
        gPlayerHealth[idx] = v;
        return v;
    }
    if (v < 0) {
        v = 0;
        gPlayerHealth[idx] = v;
    }
    return v;
}

void func_0202a850(int a, int b) {
    data_02092138 = a;
    data_0209212c = b;
    data_02092138 -= 0x64000;
    data_0209212c += 0x64000;
}

/* The per-controller records at data_0209f498 are 0x18 apart; the mode byte
   is their +0x16, data_0209f4ae. The save file keeps the active player's
   copy at data_0209caa0+0x42 (FileSaveData::controllerMode, include/SaveBlock.h). */
void InitControllerMode(int idx, u8 val) {
    data_0209f4ae[idx * 0x18] = val;
    if (idx == gActivePlayerSlot)
        data_0209caa0.file.controllerMode = val;
}

void SetControllerMode(unsigned char mode) {
    data_0209f4ae[gActivePlayerSlot * 0x18] = mode;
    data_0209caa0.file.controllerMode = mode;
}

int func_0202a8e0(int a, unsigned char b) {
    int i;
    for (i = 0; i < 9; i++) {
        if (data_0209f3e8[i] == 0) {
            data_0209f3e8[i] = a;
            gMapCapIconCharacters[i] = b;
            return i;
        }
    }
    return -1;
}

/* Member, not static: LoadSkybox reaches it with r0 still holding `this` and
 * no argument set up. The body reads the level record through the global at
 * 0x0209f340 rather than through a field, so the bytes cannot show the
 * `this` -- the call site is the evidence. */
int dScStage_c::GetSkyboxID() {
    unsigned int *p = *(unsigned int **)&data_0209f340;
    return (p[6] >> 4) & 0x1f;
}

int func_0202a958(void) {
    return ((LevelInfo *)data_0209f340)->minimapScale;
}

int func_0202a96c(void) {
    return ((LevelInfo *)data_0209f340)->minimapPaletteFile;
}

int func_0202a980(void) {
    return ((LevelInfo *)data_0209f340)->minimapTilesFile;
}

/* data_0209f314 points at mAreas (InitResources stores it). In each
   0xc-byte area record, HideArea/ShowArea toggle mShowing, and
   mBlockList heads a sorted list of these minimap-change
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
    StageArea *e = &((StageArea *)data_0209f314)[idx];
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

void HideArea(int idx) {
    ((StageArea *)data_0209f314)[idx].mShowing = 0;
}

void ShowArea(int idx) {
    ((StageArea *)data_0209f314)[idx].mShowing = 1;
}

unsigned char IsAreaShowing(int idx) {
    return ((StageArea *)data_0209f314)[idx].mShowing;
}

void ChangeArea(int areaID) {
    if (gCurrentAreaId >= 0)
        HideArea(gCurrentAreaId);
    gCurrentAreaId = (s8)areaID;
    ShowArea(areaID);
}

void GiveLives(int delta) {
    int v = gNumLives + delta;
    if (v < 0)
        v = 0;
    else if (v > 0x64)
        v = 0x64;
    gNumLives = (signed char)v;
}

/* The four register parameters are spelled wide because LoadLevelNoReturn
   (int, u32, u32, u32 in decl_common.h) forwards its own four with no
   narrowing; the stack-passed returnState is read back with ldrsb, so it is
   a signed byte. */
void LoadLevel(int levelID, u32 entranceID, int starID, u32 arg3, s8 returnState) {
    int cur;
    int chk;

    if (gNextSublevel < 0) {
        dScene_c::SetAndStopColorFader();
        gSceneColorFader.color = 0;
    }
    gNextSublevel = levelID;
    sNextEntranceId = entranceID;
    if (starID < 0)
        gNextStarId = gCurrentStarId;
    else
        gNextStarId = (u8)starID;
    SetNextStar();
    sReturnState = arg3;
    if (returnState >= 0) {
        gReturnSublevel = gCurrentSublevel;
        gReturnEntranceId = (u8)returnState;
        chk = gCheckpointSublevel;
        if (chk < 0)
            return;
        cur = SublevelToLevel(gNextSublevel);
        chk = SublevelToLevel(chk);
        if (cur != chk) {
            gCheckpointSublevel = -1;
            return;
        }
        gNextSublevel = gCheckpointSublevel;
        sNextEntranceId = sCheckpointEntranceId;
        gNextStarId = sCheckpointStarId;
    } else {
        cur = SublevelToLevel(gCurrentSublevel);
        if (gReturnSublevel >= 0 && (cur == 6 || cur == 7 || cur == 0xb)) {
            gCheckpointSublevel = gNextSublevel;
            sCheckpointEntranceId = sNextEntranceId;
            sCheckpointStarId = gNextStarId;
        } else {
            gCheckpointSublevel = -1;
        }
    }
}

void LoadLevelNoReturn(int levelID, u32 arg1, u32 arg2, u32 arg3) {
    LoadLevel(levelID, arg1, arg2, arg3, -1);
    gReturnSublevel = -1;
    gCheckpointSublevel = -1;
}

/* Initialise the player globals: lives, health, controller mode. */
void SetPlayerGlobals(void) {
    gNumLives = 4;
    s32 i = 0;
    do {
        u8 mode = data_0209caa0.file.controllerMode;
        gPlayerHealth[i] = 0x880;
        InitControllerMode(i, mode);
        i++;
    } while (i < 4);
    /* A signed byte here: decl_common.h types it unsigned for its other readers. */
    *(s8 *)&data_02092114 = -1;
    data_0209f254 = 0;
}

/* Leave the minigame menu, either back to the rec room or fading to the main
   menu. */
void ExitMinigameMenu(void) {
    if (data_0209f298) {
        LoadLevelNoReturn(0x32, 4, 1, 0);
        gCurrentSublevel = 6;
        dScene_c::StartSceneFade(3, 0, 0x7fff);
    } else {
        dScene_c::StartSceneFade(1, 0, 0);
    }
}

/* Start the minigame menu scene; the argument says whether to return to the
   rec room afterwards. */
void StartMinigameMenu(u8 returnToRecRoom) {
    data_0209f298 = returnToRecRoom;
    dScene_c::SetAndStopColorFader();
    dScene_c::StartSceneFade(5, 0, 0x7fff);
}

/* Set up VS mode: mark the game mode, initialise the player globals, start
   the scene fade, then reset the save data to its defaults. */
void PrepareVsMode(void) {
    gGameMode = 1;
    SetPlayerGlobals();
    dScene_c::StartSceneFade(3, 0, 0x7fff);
    SaveData::SetDefaultValues(&data_0209caa0.file);
}

void func_0202ae74(void) {
    StartFile(1, 0);
}

/* Start a single-player file: clear the game mode, load the level, set the
   player globals, copy the current character to data_02092128, one player,
   clear the star/level flags, then start the scene fade. */
void StartFile(s8 levelID, u8 entranceID) {
    gGameMode = 0;
    LoadLevelNoReturn(levelID, entranceID, 1, 0);
    SetPlayerGlobals();
    data_02092128[0] = data_0209caa0.file.mCharacter;
    SetNumPlayers(1);
    gLastCourseSublevel = 6;
    gCollectedStarId = 0;
    dScene_c::StartSceneFade(3, 0, 0x7fff);
}

int StartWithFarCamera(void) {
    return (((LevelInfo *)data_0209f340)->cameraFlags & 1) != 0;
}

unsigned char GetStarCameraSetting(int idx) {
    return (data_02092134 >> (idx * 4)) & 0xf;
}

int GetMinimapScale(int idx) {
    if (idx >= 0) {
        if (idx < (int)sNumMinimapScaleObjs)
            return sMinimapScaleObjs[idx];
    }
    return 0xa00;
}

/* The six table setters below each store a pointer and its count. Each is
   called by one LevelObjects loader with that object type's table:
   LoadMinimapScaleObjects, LoadMinimapTileObjects, LoadFogObjects,
   LoadTeleportDestObjects, LoadViewObjects and LoadEntranceObjects.
   InitResources zeroes every count but the entrances' before the level's
   objects load. */
void func_0202af80(short *table, int count) {
    sMinimapScaleObjs = table;
    sNumMinimapScaleObjs = count;
}

/* The minimap for an actor: its area's height bands, read at the actor's
   height, or the level's own map when the area has none. */
int GetMinimapID(dActor_c *actor, int arg) {
    int idx = actor->mAreaId;
    char *g;
    int val;
    struct MinimapChangeNode *node;
    if (idx < 0)
        idx = gCurrentAreaId;
    g = (char *)data_0209f314;
    if (g != 0) {
        node = (MinimapChangeNode *)((StageArea *)g)[idx].mBlockList;
        if (node != 0) {
            val = actor->mPosY;
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

void func_0202b044(u16 *table, int count) {
    gMinimapTileObjs = table;
    gNumMinimapTileObjs = count;
}

void func_0202b060(char *table, int count) {
    sFogObjs = table;
    sNumFogObjs = count;
}

/* The level's teleport destinations, eight bytes apart. */
struct ObjTeleport {
    int a;
    int b;
};
extern "C" struct ObjTeleport *GetTeleportDestObj(int i) {
    return (ObjTeleport *)sTeleportDestObjs + i;
}

void func_0202b090(void *table, int count) {
    sTeleportDestObjs = table;
    sNumTeleportDestObjs = count;
}

/* The level's camera view records, 0xe bytes apart. */
struct ViewObj {
    unsigned char d[0xe];
};
extern "C" struct ViewObj *GetViewObj(int idx) {
    return &((ViewObj *)sViewObjs)[idx];
}

void func_0202b0c4(void *table, int count) {
    sViewObjs = table;
    gNumViewObjs = count;
}

void func_0202b0e0(LVL_Overlay::StandardEntry *entries, int count) {
    sEntranceObjs = entries;
    sNumEntranceObjs = count;
}

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
    StageArea *area = mAreas;
    int areaIndex;
    for (areaIndex = 0; areaIndex < ((LevelInfo *)data_0209f340)->count;
         areaIndex++, area++, bone++) {
        if (area->mShowing != 0) {
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

void dScStage_c::RenderModel() {
    ModelComponents *components = &mModel.data;
    BMD_Bone *bone = components->modelFile->bones;
    StageArea *area = mAreas;
    int areaIndex;

    for (areaIndex = 0; areaIndex < ((LevelInfo *)data_0209f340)->count; areaIndex++) {
        u8 showing = area->mShowing;
        u8 *materialIds = bone->materialIds;

        if (showing != 0) {
            u16 materialIndex;
            for (materialIndex = 0; materialIndex < bone->numMaterials; materialIndex++) {
                u8 materialId = *materialIds;
                BMD_Material *material = &components->materials[materialId];
                materialIds++;
                u32 flagsTest = material->flags;
                if ((flagsTest & 0x1f0000) == 0x1f0000) {
                    u32 *flags = &material->flags;
                    *flags &= ~0x80000000;
                } else {
                    u32 *flags = &material->flags;
                    *flags |= 0x80000000;
                }
            }
            if (area->mTransformer != 0)
                area->mTransformer->Update(mModel.data);
        } else {
            u16 materialIndex;
            for (materialIndex = 0; materialIndex < bone->numMaterials; materialIndex++) {
                u8 materialId = *materialIds;
                BMD_Material *material = &components->materials[materialId];
                material->flags |= 0x80000000;
                materialIds++;
            }
        }

        area++;
        bone++;
    }

    dExtSimpleModel_c *model = &mModel;
    model->Render((const Vector3 *)&data_020755d4);
}

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

int CopyTexPalFromLevelModel(void *p) {
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
            if (strcmp(texName, mtex[s->texIdx].name) == 0 &&
                strcmp(palName, mpal[s->palIdx].name) == 0) {
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

void dScStage_c::LoadTextureTransformers() {
    struct LevelTexAnim *entry = ((LevelInfo *)data_0209f340)->entries;
    StageArea *area = mAreas;
    BMD_File *modelFile = mModel.data.modelFile;
    int areaIndex;
    for (areaIndex = 0; areaIndex < (int)((LevelInfo *)data_0209f340)->count; areaIndex++) {
        if (entry->bta) {
            dExtAnmTexSRT_c::Prepare(*modelFile, *(BTA_File *)entry->bta);
            area->mTransformer = new dExtAnmTexSRT_c;
            area->mTransformer->SetFile(*(BTA_File *)entry->bta, 0, 0x1000, 0);
        }
        entry++;
        area++;
    }
}

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

void dScStage_c::RenderFog() {
    Fog *fog = 0;
    if (gCurrentSublevel == 5)
        fog = daMugenBGM_c::GetActiveFog();
    if (fog == 0) {
        dCamera_c *camera = gCamera;
        fog = mFog;
        if (camera->IsUnderwater())
            fog++;
    }
    G3X_SetFog(fog->mEnabled, 0, fog->mShift, fog->mOffset);
    G3X_SetFogColor(fog->mColor, 31);
    G3X_SetFogTable(fog);
}

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
    if (sNumFogObjs == 0) {
        return;
    }
    {
        unsigned char *fogRecord = (unsigned char *)sFogObjs;
        Fog *fog = mFog;
        int fogIndex;
        for (fogIndex = 0; fogIndex < sNumFogObjs; fogIndex++) {
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

/* Static: five globals, no object. */
int dScStage_c::CanPause() {
    if (((unsigned)(gGameMode == 1)) == 0)
        return 1;
    if (data_0209fc68 != 0)
        return 0;
    if (data_0209f2bc != 0 || (data_ov002_02111188 == 0 && data_0209f204 != 0))
        return 0;
    return 1;
}

/* dScStage_c::OnPendingDestroy() at 0x0202b8a0
 * fBase_c vtable slot 12 (0x30), called when the dScStage_c scene is marked for
 * destruction, before CleanupResources(). dScStage_c overrides it with an empty body
 * -- no teardown work needed at this point.
 * `this` is dScStage_c* (dScStage_c derives from dScene_c <- dBase_c <- fBase_c).
 */

void dScStage_c::OnPendingDestroy() {
    (void)this;
}

void func_020391f0(void);
void func_0203083c(void);
extern "C" {}
void func_ov001_020aaf40(void);
void func_0203db3c(int i, int v);
extern "C" {}

int dScStage_c::Render() {
    if (((gVsExitScreenActive | (gGamePaused | data_0209f20c)) & 0xff) == 0) {
        StageArea *area = mAreas;
        int areaIndex;
        for (areaIndex = 0; areaIndex < ((LevelInfo *)data_0209f340)->count; areaIndex++, area++) {
            if (area->mShowing != 0 && area->mTransformer != 0)
                area->mTransformer->Advance();
        }
        mParticle.Update();
    }
    func_020391f0();
    RenderFog();
    {
        dExtSimpleModel_c *skybox = mSkyboxModel;
        if (skybox != 0) {
            Matrix4x3 *transform = &skybox->mat4x3;
            Vector3 *cameraPosition = &gCamera->pos;
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
        int isVsMode = (gGameMode == 1);
        if (isVsMode) {
            if (data_0209fc9c == 0 && gGamePaused == 0) {
                RenderVsModeCountdown();
                RenderVsModeNewStar();
                if (gVsExitScreenActive != 0 && data_0209f290 != 2)
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
        int playerCount = gNumPlayers;
        for (; playerIndex < playerCount; playerIndex++) {
            daPly_c *player = gPlayers[playerIndex];
            if (player != 0) {
                u8 playerFlags = player->mHasWings;
                int *position = (int *)(&player->mPosX);
                positionXor ^= (u8)(position[2] ^ (position[0] ^ position[1]));
                playerFlagBits |= (u8)((playerFlags & 1) << playerIndex);
            }
        }
        func_0203db3c(0x10, positionXor);
        func_0203db3c(0x10, playerFlagBits);
    }
    if (gGamePaused != 0) {
        PS_Render();
    } else if (data_0209f20c != 0 && gLevelClearStep != 0) {
        int coinCounterY = (SublevelToLevel(gLastCourseSublevel) < 0xf) ? 0x80 : 0x70;
        if (sLevelClearNewRecord != 0)
            coinCounterY += 0x10;
        OAM::Render(0, &OAM::COIN, 0x68, coinCounterY, -1, -1, 0);
        OAM::Render(0, &OAM::TIMES, 0x78, coinCounterY + 8, -1, -1, 0);
        RenderNumber(sLevelClearCoinCount, 0x80, coinCounterY, 1, 8);
    }
    LC_Render();
    return 1;
}

void func_02032f54();
void func_02011c24();
void func_020345b0(int a);
extern "C" {
void func_020199a4();
void func_02019a58();
void func_02030a78();
}

static inline u32 MaskOff(u32 x, u32 m) {
    return x & ~m;
}

#pragma opt_common_subs off

int dScStage_c::Behavior() {
    gActorUpdateMask = gNextActorUpdateMask;
    CheckCameraInput();
    CheckInput();
    if (sPauseInputLockout != 0)
        sPauseInputLockout -= 1;
    if (gGamePaused == 2 && sMenuWaitTimer == 0)
        PS_Cleanup();
    KuppaScript::Process();
    if (gSceneFader->IsAtStart() == 0) {
        dExtShadowModel_c::CleanAll();
        return 1;
    }
    {
        int b = (gGameMode == 1);
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
                    daPly_c *p = gPlayers[i];
                    if (p != 0) {
                        int t = (p->mLevelEntryDone != 0) ? 1 : 0;
                        if (t == 1)
                            cnt++;
                    }
                }
                if (cnt >= data_0209fc50) {
                    if (data_0209f304 == 0x28 && data_0209f2bc == 3)
                        func_02012790(0x2b);
                    if (gGamePaused == 0) {
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
                    if (gNextSublevel < 0 && data_0209d4b0 == 0) {
                        if (gVsExitScreenActive == 0 && data_0209f290 == 0)
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
    if (gNextSublevel >= 0) {
        int lvl = SublevelToLevel(gNextSublevel);
        int lvl2 = SublevelToLevel(gCurrentSublevel);
        int bb = (gGameMode == 2);
        if (bb == 0 && lvl <= 0xe && lvl != lvl2 && gCheckpointSublevel < 0 &&
            (gNextSublevel != 0xc || sNextEntranceId != 4)) {
            dScene_c::SetSceneToSpawn(4, 0);
        } else {
            dScene_c::SetSceneToSpawn(3, 0);
        }
        if ((IsLevelInsideCastle(gCurrentSublevel) == 0 ||
             IsLevelInsideCastle(gNextSublevel) == 0) &&
            (IsLevelTinyHugeIslandOutside(gCurrentSublevel) == 0 ||
             IsLevelTinyHugeIslandOutside(gNextSublevel) == 0)) {
            int bb2 = (gGameMode == 2);
            if (bb2 == 0)
                Sound::StopLoadedMusic_Layer1(0x3c);
        }
    } else if (data_0209f20c != 0) {
        LC_Update();
    } else if (gGamePaused != 0) {
        PS_Update();
    } else {
        u32 pi = gActivePlayerSlot;
        u16 h1 = data_020a0e58[pi].held;
        if ((((h1 & 0x200) == 0 && (h1 & 0x100) == 0) || (data_020a0e58[pi].pressed & 8) == 0) &&
            data_0209d660 == 0 && data_0209d4b0 == 0 && CanPause() != 0 && IsPauseDisabled() == 0 &&
            (u8)(gVsExitScreenActive | (gGamePaused | data_0209f20c)) == 0 &&
            MaskOff(gNextActorUpdateMask, 0) == 0) {
            u8 st = gGameMode;
            int b1 = (st == 1);
            if ((b1 && data_0209fc68 == 0) ||
                (data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE)) {
                int b2 = (st == 2);
                if (b2 == 0) {
                    int b3 = (gRunningKuppaScript != 0);
                    if (b3 == 0) {
                        if (sPauseInputLockout == 0) {
                            u32 pj = gActivePlayerSlot;
                            u16 h2 = data_020a0e58[pj].pressed;
                            int t8 = h2 & 8;
                            if ((t8 != 0 && (data_020a0e58[pj].held & 4) == 0) ||
                                (b1 == 0 && t8 == 0 && (h2 & 4) != 0)) {
                                gGamePaused = 1;
                                func_02012790(2);
                                if (gRaceTimer.mIsRunning != 0) {
                                    gRaceTimer.StopTimer();
                                    data_0209f2a0 = 1;
                                }
                                if (data_020a0e58[gActivePlayerSlot].held & 4)
                                    sPausedWithSelect = 1;
                                else
                                    sPausedWithSelect = 0;
                                PS_Init();
                                PS_Update();
                            }
                        }
                    }
                }
            }
        }
    }
    if ((u8)(gVsExitScreenActive | (gGamePaused | data_0209f20c)) == 0) {
        if ((gNextActorUpdateMask & ~0x20000000) == 0)
            dExtShadowModel_c::CleanAll();
    }
    return 1;
}
#pragma opt_common_subs on

// The ROM reads the touch records through one base pointer (gTouchHeld) plus
// field offsets, not through the per-lane gTouchX/gTouchY/gTouchEdge symbols.
#define TOUCH_DATA ((TouchInfo *)gTouchHeld)

extern "C" {
s16 atan2_hold_d_s_stage_methods_1(int y, int x);
}

void dScStage_c::CheckInput() {
    u8 mode = gGameMode;
    int isVsMode = (mode == 1);
    if (isVsMode == false) {
        int modeIsTwo = (mode == 2);
        if (modeIsTwo != false || !(data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) ||
            (data_0209d660 == 0 && (int)(gRunningKuppaScript != 0) != false)) {
            {
                ControllerInput *control = data_0209f498;
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
    ControllerInput *control = data_0209f498;
    int playerIndex = 0;
    int playerCount = gNumPlayers;
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
                    int scale = FX_Div(maxDistance, distance);
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
                control->nx = FX_SinIdx((u16)control->ang >> 4);
                control->ny = FX_CosIdx((u16)control->ang >> 4);
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
    } while (playerIndex < gNumPlayers);
}

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
    p = (char *)this->mDensity;
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

void SetTouchScreenDelay(void) {
    ControllerInput *p = data_0209f498;
    int i;
    for (i = 0; i < gNumPlayers; i++) {
        p->delay = 5;
        p++;
    }
}

extern "C" u8 GetControllerMode(int a0);

void ResetInput(void) {
    ControllerInput *p = data_0209f498;
    int i;
    for (i = 0; i < gNumPlayers; i++) {
        u8 mode;
        int m = (int)(gGameMode == 1);
        if (m == 0)
            mode = p->mode;
        else
            mode = GetControllerMode(i);
        MI_CpuClear8(p, 0x18);
        p->mode = mode;
        p->delay = 5;
        p++;
    }
}

/* dScStage_c::CleanupResources() @ 0x0202c9a8 (arm9, size 0x264)
 * Releases the level file handles, tears down fader/mesh-collider/message
 * state and unloads the level overlays/archive. Returns 1.
 */
extern "C" {
void UnloadArchive(int);
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
        int atEnd = (gGameMode == 2);
        if (!atEnd)
            Sound::ResetPlayerVoiceGroup();
    }
    KuppaScript::End();
    gGraphCallback = 0;

    {
        dExtSimpleModel_c *skybox = mSkyboxModel;
        if (skybox)
            delete skybox;
    }

    {
        StageArea *area = mAreas;
        int areaIndex;
        for (areaIndex = 0; areaIndex < ((LevelInfo *)data_0209f340)->count; areaIndex++, area++) {
            dExtAnmTexSRT_c *transformer = area->mTransformer;
            if (transformer)
                delete transformer;
            {
                MinimapChangeNode *block = (MinimapChangeNode *)area->mBlockList;
                while (block) {
                    MinimapChangeNode *next = block->next;
                    ::operator delete(block);
                    block = next;
                }
            }
        }
    }

    data_0209f320 = 0;
    data_0209f314 = 0;
    dScene_c::SetAndStopColorFader();
    data_0209d4b0 = 0;
    delete[] (dFdWipe_c *)data_0209f324;
    data_0209f324 = 0;
    CleanCommonModelDataArr();
    mMeshCollider.Disable();
    dScStage_c::ResetMeshColliders();
    Deallocate(mMeshCollider.GetFile());
    UnloadLevelOverlays(gCurrentSublevel);
    if (gCurrentSublevel == 1)
        UnloadArchive(7);

    {
        int playerIndex;
        for (playerIndex = 0; playerIndex < 4; playerIndex++)
            gPlayers[playerIndex] = 0;
    }
    gCamera = 0;
    gGamePaused = 0;
    Message::ResetAllGlobals();

    {
        u8 mode = gGameMode;
        int resetSequenceCounts = 0;
        sSelectedMenuButton = 0;
        gLevelClearStep = 0;
        sMenuHighlightDelay = 0;
        sLevelClearNewRecord = 0;
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
// Passing gCurrentStarId (already loaded and compared at +0x514/+0x520) keeps that value
// live in r1 all the way to the call at +0x570. That blocks r1 for the `bank` web, so
// bank is coloured r2 (`mov r2,#0x36` at +0x524, `ldrb r2,[r1,r2]` at +0x580) and the
// join copy `mov r1,r2` at +0x584 becomes necessary -- all three residual words at once.
//
// The levers are coupled: the old draft's widening cast on the LoadGroupAndSetBank
// argument existed only to buy back the size the missing copy cost. With the copy
// restored it overshoots, so the plain spelling is the matching one.
//   (2-arg, plain) = MATCH   (2-arg, widened) = 1   (1-arg, plain) = 0xa80   (1-arg, widened) = 3
//
// The two pairs of stores below (f284/f294 and gLevelClearStep/data_0209f20c) are in ROM statement order.
// Swapping either pair is byte-identical -- the literal-pool words are relocations and
// therefore wildcards to the byte gate -- but sends each address to the wrong pool slot.
// match.py --strict-relocs is what distinguishes them; keep this order.
void ResetInput(void);
int SublevelToLevel(int i);
void Initialise3dGraphics(int arg);
void InitialiseVramGlobals(void);
void func_02039218(void);
int GetSoundGroupID(int level, int mode);
void LoadDebugFont(void);
int IsLevelInsideCastle(int level);
int IsLevelTinyHugeIslandOutside(int level);
int func_0203da9c(void);
int func_0203da3c(void);
u32 func_0203dad4(void);
extern "C" {

void Enable3dEngines(void);
void UnloadArchive(int i);
void LoadTextNarcs(void);
int LoadArchive(int idx);
int func_020308a8(void);
void func_0203b9b4(int *p, int v);
}
void func_ov001_020ab2e4(void);

int dScStage_c::InitResources() {
    dScStage_c *scene = this;

    if (scene->mWaitingForLoad == 0) {
        u8 mode = gGameMode;
        int isVsMode = (mode == 1) ? 1 : 0;
        if (isVsMode) {
            Sound::Player::SetPlayableSeqCount(2, 5);
            int sequenceCount = 3;
            Sound::Player::SetPlayableSeqCount(sequenceCount, sequenceCount);
        }
        dScene_c::Initialise3dGraphics();
        Enable3dEngines();
        gNumPlayers = data_0208a0e0;
        gLocalPlayerId = (u8)func_0203da9c();
        ResetInput();
        gGamePaused = 0;

        int nextLevel = SublevelToLevel(gNextSublevel);
        int previousLevel = SublevelToLevel(gCurrentSublevel);
        u32 sublevelBit = ((u8)gNextSublevel + 0xDC) & 0xFF;
        int specialSublevel = 0;
        if (sublevelBit <= 0xD) {
            if ((1 << sublevelBit) & 0x2A15)
                specialSublevel = 1;
        }
        mode = gGameMode;
        int vsModeReset = (mode == 1) ? 1 : 0;

        if (vsModeReset || sReturnState == 2 || specialSublevel || nextLevel == 0x1D ||
            nextLevel != previousLevel) {
            int row = 0;
            sEventFlags = 0;
            do {
                int column = 0;
                do {
                    sActorDeathTable[row][column] = 0;
                    column += 1;
                } while (column < 0x10);
                row += 1;
            } while (row < 3);

            if ((s32)gNumPlayers > 0) {
                int count = gNumPlayers;
                int idx = 0;
                u8 *p1 = (u8 *)gNumRedCoins;
                u8 *vsStars = (u8 *)gNumStageStars;
                int vsModeCounters = (mode == 1) ? 1 : 0;
                u8 loadMode = sReturnState;
                do {
                    if (vsModeCounters || (specialSublevel == 0 && loadMode != 1)) {
                        gNumCoins[idx] = 0;
                    }
                    *p1 = 0;
                    *vsStars = 0;
                    idx += 1;
                    p1 += 1;
                    vsStars += 1;
                } while (idx < count);
            }
        }

        gPrevReturnState = sReturnState;
        if (gPrevReturnState == 1) {
            gLastCourseSublevel = gCurrentSublevel;
            gCheckpointSublevel = -1;
        }

        s8 previousSublevel = gCurrentSublevel;
        gCurrentSublevel = gNextSublevel;
        gCurrentEntranceId = sNextEntranceId;
        gCurrentStarId = gNextStarId;
        gCurrentAreaId = -1;
        data_0209f314 = (int)scene->mAreas;
        data_0209f32c = 0x80000000;
        data_0209f274 = 0;
        SublevelToLevel(gCurrentSublevel);

        s32 levelArchive = 0xBF;
        if (gCurrentSublevel == 0x33)
            levelArchive = 2;
        else if (gCurrentSublevel == 0x2B)
            levelArchive = 3;
        else if (gCurrentSublevel == 0x1D)
            levelArchive = 4;
        else if (gCurrentSublevel == 0x2A)
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
        if (gCurrentSublevel == 1) {
            mHeap::Heap_t *previousHeap = data_020a0eac->SetDefault();
            LoadArchive(7);
            previousHeap->SetDefault();
        }
        if (levelArchive != 0xBF) {
            LoadArchive(levelArchive);
        }

        ::Initialise3dGraphics(0x1F);
        GX::DisableAllBanks();
        dScStage_c::SetVramBanks();

        GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
        GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
        GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
        GXS_SetGraphicsMode(GX_BGMODE_3);
        GX_SetBGScrOffset(1);
        GX_SetPower(GX_POWER_ALL);
        GX_DispOn();
        GXS_DispOn();
        InitialiseVramGlobals();

        G2_SetBG1Priority(0);
        G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xf800,
                         GX_BG_CHARBASE_0x1c000, GX_BG_EXTPLTT_01);
        G2_BG1Mosaic(false);
        gSubScreenLayers = 0x18;
        data_0209f204 = 0;
        gGraphCallback = (dGraph_c::callback_c *)&sStageGraphCallback;
        gNextActorUpdateMask = 0;
        func_02039218();
        KuppaScript::Reset();

        int vsModeTimers = (gGameMode == 1) ? 1 : 0;
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
                STAR_MARKERS[i] = 0;
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

        gNumViewObjs = 0;
        sNumTeleportDestObjs = 0;
        sNumFogObjs = 0;
        gNumMinimapTileObjs = 0;
        sNumMinimapScaleObjs = 0;
        gUnusedType13Objs = 0;
        data_02092134 = 0x44444444;
        LoadLevelOverlays(gCurrentSublevel);

        int skipSoundGroup = (gGameMode == 2) ? 1 : 0;
        if (skipSoundGroup == 0) {
            s8 sublevel = gCurrentSublevel;
            int soundGroup = 0;
            u8 bank = 0x36;
            if (gCurrentStarId == 2) {
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
                soundGroup = GetSoundGroupID(sublevel, gCurrentStarId);
                bank = data_02075769[idx];
            }
            Sound::LoadGroupAndSetBank(soundGroup, bank);
        }

        G2_SetBG0Priority(2);
        LoadGraphics2D(gGameMode == 1, gCurrentSublevel);

        {
            u32 i;
            for (i = 0; i < 0xC; i++) {
                dExtSimpleModel_c::LoadFile(*(SharedFilePtr *)data_020756f0[i]);
            }
        }

        int vsModeFaders = (gGameMode == 1) ? 1 : 0;
        u16 *faderFileIds;
        if (vsModeFaders && levelArchive != 0xBF) {
            faderFileIds = data_02075600;
        } else if (gCurrentSublevel == 5) {
            faderFileIds = data_020755f0;
        } else {
            faderFileIds = data_020755e0;
        }

        data_0209f324 = new dFdWipe_c[7];
        int faderIndex = 0;
        int faderOffset = 0;
        do {
            ((dFdWipe_c *)((char *)data_0209f324 + faderOffset))
                ->LoadAndSetFile(faderFileIds[faderIndex]);
            faderIndex += 1;
            faderOffset += 0x60;
        } while (faderIndex < 7);

        data_0209f340 = (char *)data_02092208[gCurrentSublevel];
        scene->LoadModel();
        data_0208ee44 = 2;
        dScStage_c::LoadClsnAndObjects(*(LVL_Overlay *)data_0209f340, gCurrentEntranceId,
                                       scene->mMeshCollider);

        int vsModeMusic = (gGameMode == 1) ? 1 : 0;
        if (vsModeMusic == 0) {
            int skipMusicSetup = (gRunningKuppaScript != 0) ? 1 : 0;
            if (skipMusicSetup == 0) {
                if (!IsLevelInsideCastle(gCurrentSublevel) ||
                    !IsLevelInsideCastle(previousSublevel)) {
                    if (!IsLevelTinyHugeIslandOutside(gCurrentSublevel) ||
                        !IsLevelTinyHugeIslandOutside(previousSublevel)) {
                        if (gCurrentSublevel != 2 || (data_0209caa0.file.flags2 & 0x200)) {
                            Sound::LoadAndSetMusic_Layer1(data_0207576a[gCurrentSublevel * 3]);
                            u8 localPlayer = gLocalPlayerId;
                            if (((u8 *)gPlayers[localPlayer])[0x6ff] != 0) {
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
        scene->mParticle.Initialise();

        u8 f2fc = gPrevReturnState;
        gVsExitScreenActive = 0;
        data_0209f290 = 0;
        gShowTouchScreenArrows = 0;
        sPauseInputLockout = 0;
        if (f2fc != 1) {
            data_0209f20c = 0;
            gLevelClearStep = 0;
        }
        gMainScreenLayers = DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ;

        GX_SetVisiblePlane(DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ);
        int skipSubDisplayLayers = (gGameMode == 2) ? 1 : 0;
        if (skipSubDisplayLayers == 0) {
            GXS_SetVisiblePlane(gSubScreenLayers);
        }
        dCc_c::listHead = 0;
        dExtShadowModel_c::CleanAll();
        gNextSublevel = -1;

        if (data_0209fc68 != 0) {
            scene->mWaitingForLoad = 1;
        }
    }

    if (scene->mWaitingForLoad != 0) {
        if (func_020308a8() == 0)
            return -1;
    }

    data_02092778 = 0;
    func_0203b9b4(&data_0209e650.value, func_0203dad4());
    data_0209f208 = 0;
    u32 idx = func_0203dad4() % 6;
    data_0209f344 = (u8 *)VS_STAR_SPAWN_ORDERS[idx];

    return 1;
}

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

void LoadFont(u8 arg);
int GetOwnerLanguage(void);
void LoadControllerModeText(int a);
void LoadFont3D(void);
extern "C" {
void SetBg0Offset(int a, int b);
u32 LoadCompressedFileAt(int fileID, void *target);
int LoadFileAt(int handle, int dest);
void SetSubBg0Offset(int a, int b);
void SetSubBg1Offset(int a, int b);
void SetSubBg2Offset(int a, int b);
}

void dScStage_c::LoadGraphics2D(bool vsMode, int sublevel) {
    char *screenMap;
    char *subBg2Screen;

    G2_SetBG3Priority(0);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8800,
                         GX_BG_CHARBASE_0x00000);
    G2_BG3Mosaic(0);
    SetBg0Offset(0, 0);

    G2_SetBG2Priority(1);
    G2_BG2Mosaic(0);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6800,
                         GX_BG_CHARBASE_0x10000);

    if (vsMode == 0) {
        LoadFont(0);
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xb00f, G2_GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xac0f, G2_GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xa80f, G2_GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa40f, G2_GetBG2CharPtr());
        } else {
            LoadCompressedFileAt(0xa00f, G2_GetBG2CharPtr());
        }
        LoadFileAt(0x980b, 0x5000000);
        screenMap = (char *)G2_GetBG2ScrPtr();
        LoadCompressedFileAt(0x9808, screenMap);
        LoadCompressedFileAt(0x9809, screenMap + 0x800);
        LoadCompressedFileAt(0x980a, screenMap + 0x1000);
        LoadControllerModeText(0x280);
    } else {
        LoadFont3D();
    }

    if ((int)(gGameMode == 0) != 0 && (data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) == 0 &&
        (int)(gRunningKuppaScript != 0) == 0) {
        G2S_SetBG0Priority(0);
        G2S_BG0Mosaic(0);
        G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_512x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x8000,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        G2S_SetBG1Priority(0);
        G2S_BG1Mosaic(0);
        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_512x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xa000,
                          GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        G2S_SetBG1Priority(0);
        G2S_BG1Mosaic(0);
        G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0xc000,
                              GX_BG_CHARBASE_0x00000);

        LoadCompressedFileAt(ASSET_D_2D_OP_BG_CL01_NCG_BIN, G2S_GetBG0CharPtr());
        LoadFileAt(ASSET_D_2D_OP_BG_CL_NCL_BIN, 0x5000400);
        LoadCompressedFileAt(ASSET_D_2D_OP_BG_CL02_NSC_BIN, G2S_GetBG0ScrPtr());
        LoadCompressedFileAt(ASSET_D_2D_OP_BG_CL01_NSC_BIN, (char *)G2S_GetBG1ScrPtr());
        LoadCompressedFileAt(ASSET_D_2D_OP_OBJ_CL_NCG_BIN, G2S_GetOBJCharPtr());
        LoadFileAt(ASSET_D_2D_OP_OBJ_CL_NCL_BIN, 0x5000600);
        {
            char *clearScreenMap = (char *)G2S_GetBG2ScrPtr();
            volatile u16 fillTile = 0x10d0;
            MI_CpuFill16(clearScreenMap, fillTile, 0x800);
        }
        gSubScreenLayers = 0x17;
        SetSubBg0Offset(0, 0);
        SetSubBg1Offset(0, 0);
        SetSubBg2Offset(0, 0);
        return;
    }

    G2S_SetBG0Priority(0);
    G2S_BG0Mosaic(0);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9000,
                      GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2S_SetBG1Priority(0);
    G2S_BG1Mosaic(0);
    G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x6000,
                      GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
    G2S_SetBG2Priority(2);
    G2S_BG2Mosaic(0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x9800,
                          GX_BG_CHARBASE_0x00000);

    if (GetOwnerLanguage() == 5) {
        LoadCompressedFileAt(0xb00d, G2S_GetBG0CharPtr());
    } else if (GetOwnerLanguage() == 4) {
        LoadCompressedFileAt(0xac0d, G2S_GetBG0CharPtr());
    } else if (GetOwnerLanguage() == 3) {
        LoadCompressedFileAt(0xa80d, G2S_GetBG0CharPtr());
    } else if (GetOwnerLanguage() == 2) {
        LoadCompressedFileAt(0xa40d, G2S_GetBG0CharPtr());
    } else {
        LoadCompressedFileAt(0xa00d, G2S_GetBG0CharPtr());
    }

    LoadFileAt(0x9807, 0x5000400);
    screenMap = (char *)G2S_GetBG1ScrPtr();
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

    subBg2Screen = (char *)G2S_GetBG2ScrPtr();
    LoadCompressedFileAt(0x9805, subBg2Screen);
    LoadCompressedFileAt(0x9806, subBg2Screen + 0x1000);

    if (vsMode != 0) {
        if (data_0209fc68 != 0) {
            return;
        }
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xb00f, G2_GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xac0f, G2_GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xa80f, G2_GetBG2CharPtr());
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa40f, G2_GetBG2CharPtr());
        } else {
            LoadCompressedFileAt(0xa00f, G2_GetBG2CharPtr());
        }
        LoadFileAt(0x980b, 0x5000000);
        LoadCompressedFileAt(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN,
                             (char *)G2_GetBG2ScrPtr() + 0x1800);
        LoadCompressedFileAt(0x9801, screenMap + 0x2800);
        return;
    }

    if (SublevelToLevel(sublevel) == 0x1d) {
        volatile u16 blankTile = 0;
        MI_CpuFill16(screenMap, blankTile, 0x40);
        LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_SELECT_PAUSE_NSC_BIN, screenMap + 0x40);
        LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_PAGE_NSC_BIN, (char *)G2_GetBG2ScrPtr() + 0x1800);
        LoadCompressedFileAt(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN,
                             (char *)G2_GetBG2ScrPtr() + 0x2800);
        LoadCompressedFileAt(0x9802, screenMap + 0x2800);
        return;
    }

    LoadCompressedFileAt(ASSET_D_2D_UPR_TOUCH_TO_SELECT_NSC_BIN,
                         (char *)G2_GetBG2ScrPtr() + 0x1800);
    LoadCompressedFileAt(ASSET_D_2D_FRAMEWORK_SELECT_PAUSE_NSC_BIN, screenMap);
}

/* Slot 1 keeps the fader/sound reset and skips Scene's graphics setup.
 * The boolean result and this pointer pass through the ROM's tail call. */
bool dScStage_c::BeforeInitResources() {
    return ResetFadersAndSound();
}

int IsLevelTinyHugeIslandOutside(int level) {
    return level == 0x18 || level == 0x19;
}

int IsLevelInsideCastle(int level) {
    if (level == 2 || level == 4 || level == 5 || level == 0x32)
        return 1;
    return 0;
}

int GetSoundGroupID(int group, int sub) {
    unsigned char v = data_02075768[group * 3];
    if (group != 6)
        return v;
    if (sub == 1 || sub == 4)
        return 4;
    if (sub == 2)
        return 5;
    return v;
}

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

void func_0202deac(int levelID) {
    LoadLevelOverlays(levelID);
    data_0209f278 = 1;
}

void LoadLevelOverlays(int level) {
    if (data_0209f278) {
        if (data_02092130 != data_020758c8[level])
            OS_Terminate();
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

void LoadOrUnloadObjectOverlays(void (*fn)(int), int idx) {
    unsigned int i;
    int b = (gGameMode == 1);
    if (b != 0)
        return;
    for (i = 0; i < 7; i++) {
        b = (gGameMode == 1);
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
    b = (gGameMode == 1);
    if (b == 0) {
        fn((int)&OVERLAY_98_ID);
    }
    fn((int)&OVERLAY_102_ID);
}

/* The STAGE registry factory. Every instruction falls out of the one `new`:
 * fBase_c::operator new(sizeof(dScStage_c)), fBase_c's C2, the inlined
 * dBase_c and dScene_c vptr stores and pauseFlags ORs, this class's vptr,
 * then the out-of-line constructors of mParticle, mModel and
 * mMeshCollider in member order. The null check is `new`'s own.
 * Reconstructed source-style name (later EAD lineage supplies classInit);
 * historical alias Stage_Spawn. */
dScStage_c *dScStage_c_classInit() {
    return new dScStage_c();
}
