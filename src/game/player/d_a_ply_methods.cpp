//cpp
/* Mario, Luigi, Wario and Yoshi. ov002, 0x020bccfc..0x020e6a08, plus
 * __sinit_ov002_021019d0. ~daPly_c() is the key function, so this file
 * emits daPly_c's vtable, typeinfo and type name.
 * defer_codegen stays off except around ~daPly_c(), where it is on so D0
 * lands before D1.
 * Local optimization pragmas and some copy/reload forms are load-bearing.
 * Evidence: config/tu_manifest.d/ov002/daPly_c.json, notes/data/class-facts/daPly_c.json; measured limits: notes/experiments/player-readability-1014.json.
 *
 * Leftover:
 * - Sound::PlayBank0(0xb9, c + 0x74) still passes the camera-space vector
 *   as a raw pointer. The following mStateArg store is an int-cast of the
 *   address; the member spelling of that store is short.
 * - One ChangeState passes (int)data_ov002_021101b4.
 * - Particle::System::New, Particle::System::NewSimple, cstd::atan2,
 *   Sound::PlaySub and Sound::ChangeMusicVolume stay mangled. The symbol
 *   carries 5Fix12IiE. Sound.h does not declare the last two.
 *   All 16 body-local declarations of Particle::System::New use ONE spelling:
 *       u32 (u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z,
 *            const void *dir, void *callback)
 *   The mangled name gives the real types of the last two -- const Vector3_16f *
 *   and dPa_c::level_c::callback_c * -- but the call sites pass several different
 *   three-by-s16 shapes, so they stay void pointers until those are unified.
 *   Fix12i is a scalar typedef, so it costs nothing here; the by-value
 *   Fix12<int> the symbol names is still the wall (notes/mwccarm-codegen.md
 *   6az). Keep them identical: 16 spellings of one symbol is how a wrong
 *   return type or argument order hides.
 * - __register_global_object (arm9 0x020731dc) is the C++ runtime's
 *   destructor registration. Five function-local Vector3 offsets are real statics;
 *   mwcc emits their guards, stores and ~Vector3() registration.
 * - The static initializer is still written out by hand. Its objects keep
 *   their ROM-owned .bss homes, so it constructs each handle in place and
 *   registers its destructor with __register_global_object, as the
 *   compiler-emitted one did.
 * - Helpers whose r0 is the player are daPly_c members, still named by
 *   address, including the five other classes also call (the stage, the
 *   signs, the camera, the 1-up and Yoshi egg, Lakitu and Luigi). Two
 *   helpers that take the player stay free: one that never uses r0
 *   (func_ov002_020cf700), and func_ov002_020bcdf0, which the melonDS this
 *   sweep did not reach. func_ov002_020e69c0 rotates
 *   data_ov002_0210a760, which has no recovered name.
 * - The byte at 0x6da is pad_6da. InitResources writes it and nothing reads
 *   it; it has no honest name.
 * - daPly_c:: methods with no body-local externs carry no extern "C"
 *   wrapper (288 definitions); the remaining 212 keep it: the block gives C
 *   linkage to body-local externs. Externs declared once at file scope below
 *   take their definition's own prototype where src/ defines them, else the
 *   spelling every body copy agreed on. The ones left in bodies reproduce
 *   under no single declaration, or a header this file does not include
 *   declares them (S42 spelling drift); settling each is matching work.
 *   The wrapper never reaches the method itself -- [dcl.link]/4 makes a C
 *   language linkage ignored for a class member's name and for the function
 *   type of a member function -- so on a method that declares nothing in its
 *   body it is inert, and mwccarm 2.0/b56 emits the identical object without
 *   it (measured: whole-object SHA-256 unchanged over all 44 removals).
 */

#pragma defer_codegen off

#include "KuppaScript.h"
#include "types.h"
#include "ParticleIds.h"
#include "common.h"
#include "daPly_c.h"
#include "EndingFlyPoint.h"
#include "SpawnInfo.h"
#include "dExtAnm2Model_c.h"
#include "PlayerInput.h"
#include "private/controller_input.h"
#include "dFdColor_c.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "Particle__System.h"
#include "SurfaceInfo.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Actr.h"
#include "Sound.h"
#include "math/CalcLib.h"
#include "dCamera_c.h"
#include "SaveData.h"
#include "Timer.h"
#include "daObj1UpLogo_c.h"
#include "daOwl_c.h"
#include "daShl_c.h"
#include "daSoundObj_c.h"
#include "daTor_c.h"
#include "daYegg_c.h"
#include "daObjKey_c.h"
#include "daStar_c.h"
#include "dScene_c.h"
#include "nitro/fx/fx.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/g3x.h"
#include "Memory.h"
#include "cstd.h"
#include "math/Approach.h"
#include "nitro/fx/fx_cp.h"

/* mwccarm rejects a block-scope extern "C" (declarator expected). These six
 * stay at file scope so the calls emit the plain ROM names. The macro is
 * what the compiler sees as extern "C"; writing that keyword here would
 * raise MtxStack.h's src/ redeclaration count. Including the header fights
 * this TU's local data_020a0e68 views. */
#define MTX_C_DECL extern "C"
MTX_C_DECL void MTX_Concat43(const Matrix4x3 *, const Matrix4x3 *, Matrix4x3 *);
MTX_C_DECL void MTX_MultVec43(Vector3 *, Matrix4x3 *, Vector3 *);
MTX_C_DECL void Matrix4x3_FromTranslation(Matrix4x3 *, int, int, int);
MTX_C_DECL void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *, short);
MTX_C_DECL void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *, short);
MTX_C_DECL void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *, short);
#undef MTX_C_DECL

/* local extern: nitro/fx/fx_vec.h cannot be included next to math/CalcLib.h,
 * which still declares the other VEC_* functions with Vector3 (issue #5207). */
extern "C" fx32 VEC_Distance(const VecFx32 *, const VecFx32 *);

extern "C" {
void Vec3_RotateYAndTranslate(Vector3 *, const Vector3 *, short, const Vector3 *);
int *Vec3_LslInPlace(int *, int);
int func_01ffa344(int, int);
void func_0203d384(void);
void *__cxa_vec_new(unsigned int, unsigned int, unsigned int, void (*)(void *), void (*)(void *));
void *_ZN7Vector3D1Ev(void *);
void func_ov002_020bcdf0(char *);
void func_ov002_020cd190(char *);
int func_ov002_020cf700(daPly_c *);
int func_ov002_020d5cec(void *);
int func_ov002_020d85fc(daPly_c *, daPly_c *, int);
void func_ov002_020e25d4(char *);
void func_ov002_020e3e00(dExtSimpleModel_c *, Vector3 *, unsigned int);
void func_ov002_020d2e74(daPly_c *);
int func_ov002_020d5ab4(daPly_c *);
int func_ov002_020d600c(daPly_c *);
int func_ov002_020d9298(daPly_c *);
}

extern "C" daPly_c *daPly_c_classInit();

/* Engine and overlay symbols used here that no header declares yet. Each
 * used to be repeated inside every body that needed it; C linkage keeps
 * the names unmangled, as those body-local copies had it. */
extern "C" {

extern ControllerInput data_0209f498[];
extern PadData data_020a0e58[];
extern dFdColor_c gSceneColorFader;
extern int atan2_hold_Player_11(int a, int b);
extern int atan2_hold_Player_12(int, int);
extern int atan2_hold_Player_13(int a, int b);
extern int atan2_hold_Player_14(int a, int b);
extern int atan2_hold_Player_16(int y, int x);
extern int atan2_hold_Player_17(int y, int x);
extern int atan2_hold_Player_18(int x, int z);
extern s16 atan2_hold_Player_19(s32 y, s32 x);
extern int atan2_hold_Player_2(int, int);
extern int atan2_hold_Player_3(int a, int b);
extern s16 atan2_hold_Player_4(s32, s32);
extern short atan2_hold_Player_8(int a, int b);
extern int atan2_hold_Player_9(int x, int z);
extern int gSubSoundID;
extern signed char gNextSublevel;
extern signed char gCurrentAreaId;
extern signed char gLastCourseSublevel;
extern u8 data_02092128[];
extern int data_0209212c;
extern short gPlayerHealth[];
extern int gSubVolume;
extern Timer gRaceTimer;
extern unsigned char data_0209d660;
extern u8 gReturnEntranceId;
extern u8 data_0209f20c;
extern unsigned char gNumPlayers;
extern u8 gLocalPlayerId;
extern u8 data_0209f254;
extern u8 gCurrentEntranceId;
extern unsigned char data_0209f28c;
extern unsigned char gNewStarCollected;
extern s16 data_0209f4a2[];
extern ControllerInput data_0209f498[];
extern u8 data_0209f4ab[];
extern u8 data_0209fc5c[];
extern s32 data_0209fc68;
extern u8 data_ov002_020ff0ec[];
extern unsigned char data_ov002_020ff0f0[];
extern unsigned char data_ov002_020ff0f4[];
extern u8 data_ov002_020ff0f8[];
extern int data_ov002_020ff100[];
extern s16 data_ov002_020ff108[];
extern u16 data_ov002_020ff110[];
extern unsigned int data_ov002_020ff118[];
extern int data_ov002_020ff120[];
extern u16 data_ov002_020ff128[];
extern int data_ov002_020ff130[];
extern u16 data_ov002_020ff138[];
extern u16 data_ov002_020ff13c[];
extern unsigned short data_ov002_020ff140[];
extern u8 data_ov002_020ff14c[];
extern unsigned short data_ov002_020ff158[];
extern int data_ov002_020ff164[];
extern unsigned short data_ov002_020ff170[];
extern int data_ov002_020ff17c[];
extern int data_ov002_020ff188[];
extern int data_ov002_020ff194[];
extern short data_ov002_020ff1a0[];
extern int data_ov002_020ff1b0[4];
extern int data_ov002_020ff1c0[];
extern int data_ov002_020ff1d0[];
extern int data_ov002_020ff1e0[];
extern int data_ov002_020ff1f0[];
extern s32 data_ov002_020ff200[];
extern s32 data_ov002_020ff210[];
extern int data_ov002_020ff230[];
extern unsigned char data_ov002_020ff240[];
extern int data_ov002_020ff254[];
extern short data_ov002_020ff26c[];
extern unsigned int data_ov002_020ff284[];
extern unsigned short data_ov002_020ff29c[];
extern int data_ov002_020ff2c0[];
extern SharedFilePtr *data_ov002_020ff2f0[];
extern void *data_ov002_020ff790[];
/* Frame windows ahead of the handler PMF band. */
u8 data_ov002_02109db8[] = {46, 20, 0, 0};
u16 data_ov002_02109dbc[] = {6, 9, 15, 0};
u8 data_ov002_02109dc4[] = {24, 24, 9, 10, 46, 18, 0, 0};
u8 data_ov002_02109dcc[] = {9, 10, 0, 0, 0, 0, 0, 0};
extern int data_ov002_02109fe4[];
extern u32 data_ov002_0210a07c[];
int data_ov002_0210a560[3] = {116, 124, 120};
u32 data_ov002_0210a578[3] = {117, 123, 121};
u32 data_ov002_0210a584[3] = {118, 122, 119};
}

/* 20.12 offset added before the camera update: (0, -205.0, 410.0). A C struct
 * keeps the literal in .data; Vector3's constructors would make it dynamic. */
#pragma cplusplus off
typedef struct PlayerPosOffset {
    int x, y, z;
} PlayerPosOffset;
PlayerPosOffset data_ov002_0210a590 = {0, -0xcd000, 0x19a000};
#pragma cplusplus on
extern "C" {
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov002_0210d9c0;
extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov002_0210da40;
extern int data_ov002_0210f92c[];
extern int data_ov055_02111b64;
extern int data_ov055_02111b6c;
}

extern void EnterBigBoosHaunt(void);
extern void ExitLevel(void);
extern void FUN_02029934(void);
extern void FUN_02029980(void);
extern void FUN_020299f4(void);
extern "C" {

extern int func_01ff98f4(int a, int b);
extern int func_01ff99a4(int a, int b);
extern int func_01ffa4bc(int a);
}

extern void func_0201628c(int a, int b);
extern void func_020162a4(int a, int b);
extern void func_020167a4(struct dExtAnmModel_c *p);
extern void func_0201fc88(s16);
extern "C" {

extern void func_02020388(int playerNo);
}

extern int func_02035638(unsigned char *);
extern void *func_0203564c(void *);
extern void func_02035798(dBgCh_Actr *, int);
extern void func_020371fc(char *);
extern void func_02037318(dBgCh_Actr *);
extern int func_02037e20(int *);
extern void func_0203db0c(int idx, unsigned short val);
extern void func_ov002_020ed63c(daYegg_c *, int);
extern int func_ov002_020eea84(dBgCh_Actr *, dActor_c *);
extern int func_ov002_020eeeb8(void *, char *);
extern int func_ov002_020ef070(void *, char *);
extern int func_ov002_020f035c(unsigned int sel, int r1);
extern int func_ov100_02144fcc(void);
extern int func_ov100_02145014(void);
extern void func_ov100_02145070(int);
extern "C" {}

extern int GiveHealth(int, int);
extern void GiveLives(int);
extern "C" {

extern u16 gKuppaScriptFrame;
extern s32 gRunningKuppaScript;
}

extern void HitDeathPlane(int arg);
extern "C" {

extern unsigned char IsPlayerWarping(int a0);
}

extern void KillPlayer(void);
extern void LoadKeyModels(int idx);
extern void LoadSilverStarAndNumber(void);
extern "C" {
extern int Math_Function_0203b14c(void *, int a, int b, int c, int d);
extern int PlayerChangeMusicVolumeHold(u32 vol, int t);
extern void PlayerChangeMusicVolumeHold2(int, int);
extern void PlayerChangeMusicVolumeHold3(unsigned int, int);
extern void PlayerNewSimpleHold(unsigned int id, int x, int y, int z);
extern void PlayerNewSimpleHold1(unsigned int, int, int, int);
extern void PlayerNewSimpleHold13(u32 id, int a, int b, int cc);
extern void PlayerNewSimpleHold16(unsigned int id, int x, int y, int z);
extern void PlayerNewSimpleHold2(unsigned int id, int x, int y, int z);
extern void PlayerNewSimpleHold3(unsigned int, int, int, int);
extern void PlayerNewSimpleHold4(unsigned int, int, int, int);
extern void PlayerNewSimpleHold5(unsigned int id, int x, int y, int z);
extern void PlayerNewSimpleHold6(unsigned int id, int x, int y, int z);
extern void PlayerNewSimpleHold7(u32 id, int x, int y, int z);
extern void PlayerNewSimpleHold9(u32 a, int b, int c, int d);
extern void PlayerPlayLongHold_bfa74(u32 handle, u32 bank, u32 id, const void *pos, s16 e);
extern void PlayerPlayLongHold_deadpit(u32 a, u32 b, u32 c, void *, u32 d);
extern void PlayerPlayLongHold_electro(u32 a, u32 b, u32 c, Vector3 *v, u32 d);
extern void PlayerPlayLongHold_swing(u32 h, u32 a, u32 b, void *v, u32 d);
}

extern int SublevelToLevel(int);
extern int SurfaceInfo_TestFlag0x20(int *);
extern "C" {

extern void Vec3_MulScalar(Vector3 *, const Vector3 *, int s);
extern int atan2_hold_Player_1(Fix12i a, int b);
extern int atan2_hold_Player_10(Fix12i a, int b);
extern int atan2_hold_Player_15(Fix12i, Fix12i);
extern int atan2_hold_Player_5(Fix12i y, Fix12i x);
extern Fix12i atan2_hold_Player_6(Fix12i a, Fix12i b);
extern int atan2_hold_Player_7(Fix12i a, int b);
/* local extern: including nitro/fx/fx_trig.h renumbers this TU; spelled as the header does. */
extern const fx16 FX_SinCosTable_[];
extern signed char gCurrentSublevel;
extern unsigned char data_0209f4ae[];
extern unsigned int data_ov002_020ff220[];
extern SharedFilePtr *data_ov002_020ff480[];
extern unsigned int data_ov002_0210e158;
extern SharedFilePtr data_ov002_0210e1c8;
extern SharedFilePtr data_ov002_0210e1d0;
extern SharedFilePtr data_ov002_0210e1e0;
extern SharedFilePtr data_ov002_0210e1e8;
extern SharedFilePtr data_ov002_0210e230;
extern SharedFilePtr data_ov002_0210e250;
extern SharedFilePtr data_ov002_0210e270;
extern SharedFilePtr data_ov002_0210e288;
extern SharedFilePtr data_ov002_0210e2f0;
extern SharedFilePtr data_ov002_0210e3b0;
extern SharedFilePtr data_ov002_0210e3c8;
extern SharedFilePtr data_ov002_0210e3e0;
extern SharedFilePtr data_ov002_0210e3e8;
extern SharedFilePtr data_ov002_0210e400;
extern SharedFilePtr data_ov002_0210e408;
extern SharedFilePtr data_ov002_0210e430;
extern SharedFilePtr data_ov002_0210e438;
extern SharedFilePtr data_ov002_0210e450;
extern SharedFilePtr data_ov002_0210e458;
extern SharedFilePtr data_ov002_0210e460;
extern SharedFilePtr data_ov002_0210e478;
extern SharedFilePtr data_ov002_0210e4b8;
extern SharedFilePtr data_ov002_0210e4c0;
extern SharedFilePtr data_ov002_0210e4d0;
extern SharedFilePtr data_ov002_0210e4e8;
extern SharedFilePtr data_ov002_0210e4f0;
extern SharedFilePtr data_ov002_0210e500;
extern SharedFilePtr data_ov002_0210e538;
extern SharedFilePtr data_ov002_0210e540;
extern SharedFilePtr data_ov002_0210e588;
extern SharedFilePtr data_ov002_0210e600;
extern SharedFilePtr data_ov002_0210e620;
extern SharedFilePtr data_ov002_0210e640;
extern SharedFilePtr data_ov002_0210e660;
extern SharedFilePtr data_ov002_0210e670;
extern SharedFilePtr data_ov002_0210e680;
extern SharedFilePtr data_ov002_0210e6a0;
extern SharedFilePtr data_ov002_0210e6b8;
extern SharedFilePtr data_ov002_0210e6c0;
extern SharedFilePtr data_ov002_0210e6e0;
extern SharedFilePtr data_ov002_0210e6f8;
extern SharedFilePtr data_ov002_0210e708;
extern SharedFilePtr data_ov002_0210e728;
extern SharedFilePtr data_ov002_0210e738;
extern SharedFilePtr data_ov002_0210e770;
extern SharedFilePtr data_ov002_0210e780;
extern SharedFilePtr data_ov002_0210e788;
extern SharedFilePtr data_ov002_0210e798;
extern SharedFilePtr data_ov002_0210e7b0;
extern SharedFilePtr data_ov002_0210e7e8;
extern SharedFilePtr data_ov002_0210e800;
extern SharedFilePtr data_ov002_0210e898;
extern SharedFilePtr data_ov002_0210e8d0;
extern SharedFilePtr data_ov002_0210e8e8;
extern SharedFilePtr data_ov002_0210e910;
extern SharedFilePtr data_ov002_0210e958;
extern SharedFilePtr data_ov002_0210e9d0;
extern SharedFilePtr data_ov002_0210e9e8;
extern SharedFilePtr data_ov002_0210ea10;
extern SharedFilePtr data_ov002_0210ea30;
extern SharedFilePtr data_ov002_0210ea70;
extern SharedFilePtr data_ov002_0210ea88;
extern SharedFilePtr data_ov002_0210eac8;
extern SharedFilePtr data_ov002_0210eaf0;
extern SharedFilePtr data_ov002_0210eb10;
extern SharedFilePtr data_ov002_0210eb38;
extern SharedFilePtr data_ov002_0210eb58;
extern SharedFilePtr data_ov002_0210eb70;
extern SharedFilePtr data_ov002_0210eb90;
extern SharedFilePtr data_ov002_0210eb98;
extern SharedFilePtr data_ov002_0210eba8;
extern SharedFilePtr data_ov002_0210ebc8;
extern SharedFilePtr data_ov002_0210ebd8;
extern SharedFilePtr data_ov002_0210ec00;
extern SharedFilePtr data_ov002_0210ec10;
extern SharedFilePtr data_ov002_0210ec40;
extern SharedFilePtr data_ov002_0210ec98;
extern SharedFilePtr data_ov002_0210eca8;
extern SharedFilePtr data_ov002_0210ecb8;
}

extern int func_ov002_020eed24(dBgCh_Actr *clsn, daPly_c *player);
extern int func_ov002_020f030c(int sel);
extern "C" {

extern void func_ov002_020d7430(daPly_c &player);
extern int func_ov002_020db5f4(daPly_c *c, dActor_c *arg);
extern void func_ov002_020e6b74(void *model, int *saved);
extern int *func_ov002_020e6bb0(void *model);
}

extern short GetAngleToCamera(int i);
extern struct ObjTeleport *GetTeleportDestObj(int i);
extern "C" {

extern u8 gShowTouchScreenArrows;
}

extern void LoadLevel(int levelID, u32 entranceID, int starID, u32 arg3, s8 returnState);
extern u8 NumStars(void);
extern "C" {

extern void PlayerNewSimpleHold10(unsigned int id, Fix12i a, Fix12i b, Fix12i c);
extern void PlayerNewSimpleHold11(u32 id, Fix12i x, Fix12i y, Fix12i z);
extern void PlayerNewSimpleHold15(unsigned int id, Fix12i a, Fix12i b, Fix12i d);
extern void PlayerNewSimpleHold8(unsigned int id, Fix12i a, Fix12i b, Fix12i c);
}

extern s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
extern int func_ov002_020f02c8(int sel);
extern "C" {

extern int AngleDiff(int a, int b);
extern struct S18a data_0209f4a4[];
extern void func_ov002_020db54c(void *self, int a, int b, short c);
extern void func_ov002_020dc09c(char *c);
extern void func_ov002_020e69c0(void *unused);
extern void func_ov002_020e6a78(void *ramp, int blend);
extern void func_ov002_020e6b3c(void *model);
extern u32 PlayerNewHold(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                         void *callback);
extern u32 PlayerNewHold3(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                          void *callback);
extern u32 PlayerNewHold4(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                          void *callback);
extern void PlayerPlayLongHold_c133c(unsigned int a, unsigned int b, unsigned int c,
                                     Vector3 const *v, unsigned int d);
extern signed char data_02092114;
extern unsigned char data_0209f2bc[];
extern int data_0209f32c;
}

extern void func_02035860(dBgCh_Actr *, const Vector3 *);
extern int func_ov002_020edca4(daYegg_c *);
extern void func_ov002_020eedc0(dBgCh_Actr *, dActor_c *);
extern "C" {

extern u8 gMeterShownHealth;
extern unsigned char gShowRaceTimer[];
extern u32 PlayerNewHold5(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                          void *callback);
extern void PlayerPlayLongHold_cec2c(unsigned int a, unsigned int b, unsigned int c,
                                     const Vector3 *v, unsigned int d);
extern struct P2 data_ov002_0210a084;
extern struct P2 data_ov002_0210a34c;
/* local extern: including CalcLib.h renumbers this TU's @NNN aliases. */
extern int RandomIntInternal(int *seed);
extern u32 PlayerNewHold7(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                          void *callback);
extern u32 PlayerNewHold8(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                          void *callback);
extern u32 PlayerNewHold9(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                          void *callback);
extern u32 PlayerNewHold10(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                           void *callback);
extern u32 PlayerNewHold11(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                           void *callback);
extern u32 PlayerNewHold12(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                           void *callback);
extern u32 PlayerNewHold13(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                           void *callback);
extern u32 PlayerNewHold14(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                           void *callback);
extern u32 PlayerNewHold15(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                           void *callback);
extern void PlayerPlayLongHold_burnfire(unsigned int handle, unsigned int id, unsigned int vol,
                                        const void *vec, unsigned int flags);
extern char *gPlayers[];
extern u32 PlayerNewHold16(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                           void *callback);
}

extern int func_02012120(unsigned int p0, unsigned int p1, unsigned int p2, const Vector3 *p3,
                         short p4);
extern "C" {

extern SharedFilePtr data_ov002_02110aa4;
extern void MulMat4x3Mat4x3(void *a, void *b, void *c);
/* local extern: MtxStack's named parameters take an @NNN in every includer. */
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
/* local extern: MtxStack's named parameters take an @NNN in every includer. */
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
/* local extern: including CalcLib.h renumbers this TU's @NNN aliases. */
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern u8 gReturnSublevel;
}

extern void func_02011f7c(void *a);
extern int func_02011ee4(void *x);

extern u32 PlayerNewHold2(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                          void *callback);

class dFdColor_c;
extern dFdColor_c gSceneColorFader;
extern u32 PlayerNewHold6(u32 handle, u32 kind, Fix12i x, Fix12i y, Fix12i z, const void *dir,
                          void *callback);

/* ---------------------------------------------------------------------------
 * Shadow types and ABI stand-ins that cannot live inside a function body.
 *
 * Everything here is either hoisted for the C++ rule spelled out further down
 * or load-bearing for codegen.  It is NOT a parking lot for POD twins of real
 * types: Vector3 is usable directly, including its (x, y, z) constructor, as
 * long as the locals keep the ROM's declaration order.  The exception is the
 * one include/types.h records at Vector3's constructors: a whole-struct copy
 * can lower memberwise instead of as a block move, and there a twin can still
 * be load-bearing.
 * ------------------------------------------------------------------------- */
struct V3Quad { Vector3 a; Vector3 d; Vector3 e; Vector3 b; };
/* A stand-in for Fix12<int>, and load-bearing as a CLASS rather than as a
 * name: func_ov002_020bf800 builds three of these and passes each as an int
 * read through its address. The ROM stores x, y and z to sp+0xc..0x14 and
 * reloads x and y from there for the call (z goes out from a register). Plain Fix12i
 * locals -- or passing xv/yv/zv straight to the callee -- keep all three in
 * registers and shrink that function from 140 bytes to 120. Measured both
 * ways; it stays until Fix12<int> itself is reconstructed. */
struct Fix12v { int v; Fix12v(int a) : v(a) {} };
namespace Sound { void PlayBank0(unsigned int, const Vector3 &); }
/* Only ever an opaque pointer: the camera's extern, and func_ov002_020ceb7c's
 * parameter, which is a daPly_c (it reads mPosY, mPlayerNo and mStateWork). */
struct CameraRaw;
/* The rest of this block is a hoist, and the tags are unique.  A body may not declare `extern` with a
 * function-local class type: a local class has no linkage, so the declaration
 * names an entity the type cannot describe.  mwccarm accepts it; the C++ rule
 * ([basic.link]) does not, and neither does the host front end.  Each shape
 * below is the body's own, moved out unchanged, and the body keeps its own
 * declaration of the object. */
struct Entry18 { s16 val; /* 0x0 */ char _pad[0x16]; };
extern EndingFlyPoint gJumboStarKeyframes[];
struct ObjTeleport { s16 x, y, z; u16 param; };
struct State2i { int a; int b; };
struct S18a { short f0; char pad[0x16]; };
struct S18b { u16 f0; char pad[0x16]; };
struct S18b2 { short a; short b; int c[5]; };
/* The Kuppa-script command table at the head of the file holds pointers to
 * members of one signature. */
typedef int (daPly_c::*ScriptCommand)(unsigned char *, int, int);

/* The player states: init, main and cleanup handlers, declared once. */
extern "C" {

extern daPly_c::State data_ov002_0210ffec;
extern daPly_c::State data_ov002_02110004;
extern daPly_c::State data_ov002_0211001c;
extern daPly_c::State data_ov002_02110034;
extern daPly_c::State data_ov002_0211004c;
extern daPly_c::State data_ov002_02110064;
extern daPly_c::State data_ov002_0211007c;
extern daPly_c::State data_ov002_02110094;
extern daPly_c::State data_ov002_021100ac;
extern daPly_c::State data_ov002_021100c4;
extern daPly_c::State data_ov002_021100dc;
extern daPly_c::State data_ov002_021100f4;
extern daPly_c::State data_ov002_0211010c;
extern daPly_c::State data_ov002_02110124;
extern daPly_c::State data_ov002_0211013c;
extern daPly_c::State data_ov002_0211016c;
extern daPly_c::State data_ov002_02110184;
extern daPly_c::State data_ov002_0211019c;
extern daPly_c::State data_ov002_021101b4;
extern daPly_c::State data_ov002_021101cc;
extern daPly_c::State data_ov002_021101e4;
extern daPly_c::State data_ov002_021101fc;
extern daPly_c::State data_ov002_02110214;
extern daPly_c::State data_ov002_0211022c;
extern daPly_c::State data_ov002_0211025c;
extern daPly_c::State data_ov002_02110274;
extern daPly_c::State data_ov002_0211028c;
extern daPly_c::State data_ov002_021102a4;
extern daPly_c::State data_ov002_021102bc;
extern daPly_c::State data_ov002_021102d4;
extern daPly_c::State data_ov002_021102ec;
extern daPly_c::State data_ov002_02110304;
extern daPly_c::State data_ov002_0211031c;
extern daPly_c::State data_ov002_02110334;
extern daPly_c::State data_ov002_0211034c;
extern daPly_c::State data_ov002_02110364;
extern daPly_c::State data_ov002_0211037c;
extern daPly_c::State data_ov002_02110394;
extern daPly_c::State data_ov002_021103ac;
extern daPly_c::State data_ov002_021103c4;
extern daPly_c::State data_ov002_021103dc;
extern daPly_c::State data_ov002_021103f4;
extern daPly_c::State data_ov002_0211040c;
extern daPly_c::State data_ov002_02110424;
extern daPly_c::State data_ov002_0211043c;
extern daPly_c::State data_ov002_02110454;
extern daPly_c::State data_ov002_0211046c;
extern daPly_c::State data_ov002_02110484;
extern daPly_c::State data_ov002_0211049c;
extern daPly_c::State data_ov002_021104b4;
extern daPly_c::State data_ov002_021104cc;
extern daPly_c::State data_ov002_021104e4;
extern daPly_c::State data_ov002_021104fc;
extern daPly_c::State data_ov002_02110514;
extern daPly_c::State data_ov002_0211052c;
extern daPly_c::State data_ov002_02110544;
extern daPly_c::State data_ov002_0211055c;
extern daPly_c::State data_ov002_02110574;
extern daPly_c::State data_ov002_0211058c;
extern daPly_c::State data_ov002_021105a4;
extern daPly_c::State data_ov002_021105bc;
extern daPly_c::State data_ov002_021105d4;
extern daPly_c::State data_ov002_021105ec;
extern daPly_c::State data_ov002_02110604;
extern daPly_c::State data_ov002_0211061c;
extern daPly_c::State data_ov002_02110634;
extern daPly_c::State data_ov002_0211064c;
extern daPly_c::State data_ov002_02110664;
extern daPly_c::State data_ov002_0211067c;
extern daPly_c::State data_ov002_02110694;
extern daPly_c::State data_ov002_021106ac;
extern daPly_c::State data_ov002_021106c4;
extern daPly_c::State data_ov002_021106dc;
extern daPly_c::State data_ov002_021106f4;
extern daPly_c::State data_ov002_0211070c;
extern daPly_c::State data_ov002_02110724;
extern ScriptCommand data_ov002_0210a054;
extern ScriptCommand data_ov002_0210a064;
extern ScriptCommand data_ov002_0210a094;
extern ScriptCommand data_ov002_0210a0b4;
extern ScriptCommand data_ov002_0210a0dc;
extern ScriptCommand data_ov002_0210a124;
extern ScriptCommand data_ov002_0210a14c;
extern ScriptCommand data_ov002_0210a36c;
extern ScriptCommand data_ov002_0210a3c4;
extern ScriptCommand data_ov002_0210a3fc;
extern ScriptCommand data_ov002_0210a40c;
extern ScriptCommand data_ov002_0210a44c;
extern ScriptCommand data_ov002_0210a474;
extern ScriptCommand data_ov002_0210a534;
extern ScriptCommand data_ov002_02110794[14];
}

int daPly_c::func_ov002_020bccfc(unsigned char *params, int beginFrame, int endFrame) {
    this->MarkForDestruction();
    return 1;
}

int daPly_c::func_ov002_020bcd18(unsigned char *params, int beginFrame, int endFrame) {
    mHorzSpeed = ScaleByCharFactor(0xa000);
    return 1;
}

int daPly_c::func_ov002_020bcd38(unsigned char *params, int beginFrame, int endFrame) {
    s16 cur;

    if (beginFrame == gKuppaScriptFrame) {
        int dt = endFrame - beginFrame;
        this->mAngleYSpeed = (short)AngleDiff(this->mAngleY, ReadUnalignedShort(params)) / dt;
        this->mStateWork = (u8)dt;
        this->mAnimScriptId = 0;
        this->mAnimScriptStep = 0;
        this->SetAnim(0x48, 0, 0x1000, 0);
    }

    cur = this->mAngleYSpeed;
    ApproachAngle(&this->mAngleY, ReadUnalignedShort(params), this->mStateWork, cur, cur);
    this->mPrevAngleY = this->mAngleY;
    return 1;
}

/* Free C-linkage script-animation update on a daPly_c. The array-backed
 * AnimScriptStep retains the retail two-word stack snapshot under 2004/b56 C++.
 * Scalar SetAnim/particle ABI calls and the raw spawned-prop view remain. */

/* One step of a Player animation script: w[0] is the SetAnim id (0 ends the
 * script), w[1] its argument (0x40000000 or 0). An array, not two named
 * fields: named fields move func_ov002_020bcdf0's stack copy of the step. */
extern "C" {
typedef struct {
    int w[2];
} AnimScriptStep;

/* The data records between the state-handler literals: animation steps,
 * two-id tables and the two-word P2 records. */
int data_ov002_02109fe4[2] = {0xad, 0xad};
AnimScriptStep data_ov002_02109ff4[1] = {{0xb3, 0}};
u32 data_ov002_0210a07c[2] = {4, 3};
}

struct P2 {
    void *w[2];
};
extern "C" {

extern SharedFilePtr data_ov002_0210e6c0;
extern SharedFilePtr data_ov002_0210eb98;
}

struct P2 data_ov002_0210a084 = {{&data_ov002_0210e6c0, &data_ov002_0210eb98}};
extern "C" {

AnimScriptStep data_ov002_0210a1d4[1] = {{0xb7, 0}};
}

struct P2 data_ov002_0210a34c = {{(void *)0x6e, (void *)0x6f}};

/* Free handlers are stored as their actual function pointers; the union gives
 * them the PMF spelling the descriptor tables use. */
extern "C" void func_ov002_020c9de4(char *);
extern "C" void func_ov002_020e200c(char *);
extern "C" void func_ov002_020c72a4(void *);
union PlayerFreeCharPmf {
    void (*free)(char *);
    void (daPly_c::*pmf)();
};
union PlayerFreeVoidPmf {
    void (*free)(void *);
    void (daPly_c::*pmf)();
};
union PlayerStateFn {
    void (daPly_c::*voidPmf)();
    int (daPly_c::*intPmf)();
    void (*freeChar)(char *);
    void (*freePlayer)(daPly_c *);
    void (*freeVoid)(void *);
};
extern "C" {

PlayerFreeCharPmf data_ov002_02109dfc = {func_ov002_020c9de4};
PlayerFreeCharPmf data_ov002_02109f0c = {func_ov002_020e200c};
PlayerFreeCharPmf data_ov002_0210a10c = {func_ov002_020e200c};
PlayerFreeCharPmf data_ov002_0210a23c = {func_ov002_020c9de4};
PlayerFreeCharPmf data_ov002_0210a244 = {func_ov002_020c9de4};
PlayerFreeCharPmf data_ov002_0210a264 = {func_ov002_020c9de4};
PlayerFreeVoidPmf data_ov002_0210a334 = {func_ov002_020c72a4};
}

/* func_ov002_020e1e70 stays a free C-linkage body; the table slot is a PMF. */
union PlayerFreeCleanupTarget {
    void (*code)(char *);
    void (daPly_c::*pmf)();
};
extern "C" {

/* Non-PMF records in the descriptor band and the free cleanup handler's PMF
 * record. */
AnimScriptStep data_ov002_0210a3dc[] = {{1, 0}};
int data_ov002_0210a424[] = {19, 21};
AnimScriptStep data_ov002_0210a434[] = {{188, 0}};
void func_ov002_020e1e70(char *);
PlayerFreeCleanupTarget data_ov002_0210a47c = {func_ov002_020e1e70};
AnimScriptStep data_ov002_0210a4bc[] = {{191, 0}};

/* Player animation scripts, frame windows, hurt tables, and body-model
 * pointer tables. All values and pointer targets are read by the state
 * handlers below; the three SharedFilePtr* arrays feed func_ov002_020e5948. */
extern SharedFilePtr data_ov002_0210ec50;
extern SharedFilePtr data_ov002_0210ebb8;
extern SharedFilePtr data_ov002_0210ec60;
extern SharedFilePtr data_ov002_0210e818;
extern SharedFilePtr data_ov002_0210e690;
extern SharedFilePtr data_ov002_0210eb20;
extern SharedFilePtr data_ov002_0210e758;
extern SharedFilePtr data_ov002_0210e7a8;
extern SharedFilePtr data_ov002_0210ec38;
extern SharedFilePtr data_ov002_0210eb00;
extern SharedFilePtr data_ov002_0210e790;
extern SharedFilePtr data_ov002_0210e7f0;

int data_ov002_0210a59c[3] = {0x2a000, 0x34000, 0x45000};
int data_ov002_0210a5a8[3] = {83, 79, 75};
int data_ov002_0210a5b4[3] = {82, 81, 76};
int data_ov002_0210a5c0[3] = {56, 57, 58};
u16 data_ov002_0210a5cc[8] = {5, 7, 6, 8, 2, 6, 3, 13};
u16 data_ov002_0210a5dc[8] = {5, 9, 6, 10, 2, 8, 0, 0};
AnimScriptStep data_ov002_0210a5ec[2] = {
    {192, 0x40000000},
    {0, 0},
};
AnimScriptStep data_ov002_0210a5fc[2] = {
    {193, 0x40000000},
    {0, 0},
};
u32 data_ov002_0210a60c[4] = {88, 87, 89, 90};
AnimScriptStep data_ov002_0210a61c[2] = {
    {178, 0x40000000},
    {0, 0},
};
AnimScriptStep data_ov002_0210a62c[2] = {
    {184, 0x40000000},
    {185, 0},
};
AnimScriptStep data_ov002_0210a63c[2] = {
    {182, 0x40000000},
    {0, 0},
};
AnimScriptStep data_ov002_0210a64c[2] = {
    {184, 0x40000000},
    {185, 0},
};
AnimScriptStep data_ov002_0210a65c[2] = {
    {190, 0x40000000},
    {188, 0},
};
AnimScriptStep data_ov002_0210a66c[2] = {
    {189, 0x40000000},
    {190, 0},
};
AnimScriptStep data_ov002_0210a67c[2] = {
    {1, 0x40000000},
    {0, 0},
};
SharedFilePtr *data_ov002_0210a68c[4] = {
    &data_ov002_0210ec50,
    &data_ov002_0210ebb8,
    &data_ov002_0210ec60,
    &data_ov002_0210e818,
};
SharedFilePtr *data_ov002_0210a69c[4] = {
    &data_ov002_0210e690,
    &data_ov002_0210eb20,
    &data_ov002_0210e758,
    &data_ov002_0210e7a8,
};
SharedFilePtr *data_ov002_0210a6ac[4] = {
    &data_ov002_0210ec38,
    &data_ov002_0210eb00,
    &data_ov002_0210e790,
    &data_ov002_0210e7f0,
};
AnimScriptStep data_ov002_0210a6bc[3] = {
    {54, 0x40000000},
    {55, 0x40000000},
    {183, 0},
};
u32 data_ov002_0210a6d4[6] = {105, 106, 18, 20, 19, 21};
int data_ov002_0210a6ec[6] = {18, 20, 0, 0, 0, 0};

/* PLAYER's registry descriptor: the factory and the draw/update priorities
 * the actor framework reads from data_020a4bb8[PLAYER]. */
SpawnInfo<daPly_c> g_profile_PLAYER = {
    daPly_c_classInit,
    191,        /* behavior priority */
    18,         /* render priority */
    0x00000002, /* actorFlags */
    0x000fa000,
    0x000fa000,
    0x02ee0000,
    0x02ee0000,
};

extern AnimScriptStep data_ov002_0210e7b8[];

AnimScriptStep data_ov002_0210a720[4] = {
    {181, 0x40000000},
    {180, 0x40000000},
    {178, 0x40000000},
    {179, 0},
};
AnimScriptStep data_ov002_0210a740[4] = {
    {186, 0x40000000},
    {71, 0x40000000},
    {187, 0x40000000},
    {188, 0},
};
/* The 32-colour RGB555 toon table: func_ov002_020e69c0 rotates it two
 * entries per call and func_ov002_020e6a78 blends the player ramp toward it
 * before passing it to G3X_SetToonTable. */
u16 data_ov002_0210a760[32] = {
    0x3e1f, 0x3e1f, 0x3e1f, 0x3e1f, 0x3f1f, 0x3f1f, 0x3f1f, 0x3f1f, 0x43ff, 0x43ff, 0x43ff,
    0x43ff, 0x4bef, 0x4bef, 0x4bef, 0x4bef, 0x7fef, 0x7fef, 0x7fef, 0x7fef, 0x7df0, 0x7df0,
    0x7df0, 0x7df0, 0x7df8, 0x7df8, 0x7df8, 0x7df8, 0x69ff, 0x69ff, 0x69ff, 0x69ff,
};

AnimScriptStep *data_ov002_0210a7a0[] = {
    data_ov002_02109ff4, data_ov002_0210a61c, data_ov002_0210a720, data_ov002_0210a63c,
    data_ov002_0210a1d4, data_ov002_0210a64c, data_ov002_0210a740, data_ov002_0210a66c,
    data_ov002_0210e7b8, data_ov002_0210a67c, data_ov002_0210a4bc, data_ov002_0210a434,
    data_ov002_0210a3dc, data_ov002_0210a5ec, data_ov002_0210a5fc, data_ov002_0210a62c,
    data_ov002_0210a6bc, data_ov002_0210a65c,
};

/* Level-enter SetAnim ids, indexed by mStateStep. St_LevelEnter_Init
 * rewrites 133 to 165 when the cap is lost. The vtable storage begins
 * at the next word. */
u32 data_ov002_0210a7e8[] = {
    71, 168, 74, 86, 84, 16, 84, 84, 133, 16, 86, 133, 16, 83, 83, 133, 133, 83, 133,
};

extern "C" void func_ov002_020bcdf0(char *self) {
    daPly_c *player = reinterpret_cast<daPly_c *>(self);
    AnimScriptStep *base;
    AnimScriptStep cur;
    int id;
    dExtFrameCtrl_c *anim;
    unsigned int frame;
    struct { int x, y, z; } vv[2];
    int x0, y0, z0;

    if (player->mAnimScriptId == 0)
        return;

    base = data_ov002_0210a7a0[player->mAnimScriptId - 1];
    cur = base[player->mAnimScriptStep];

    ((daPly_c *)self)->SetAnim(cur.w[0], cur.w[1], 0x1000, 0);

    if (cur.w[0] == 0xbd) {
        id = player->GetBodyModelID(player->param1 & 0xff, 0);
        anim = &static_cast<dExtFrameCtrl_c &>(*player->mModels.mBodyModels[id]);
        if (anim->WillHitFrame(0xc)) {
            player->mHasNoCap = 1;
        }
    } else if (cur.w[0] == 1) {
        id = player->GetBodyModelID(player->param1 & 0xff, 0);
        anim = &static_cast<dExtFrameCtrl_c &>(*player->mModels.mBodyModels[id]);
        if (anim->WillHitFrame(0x5d)) {
            player->mHasNoCap = 0;
        } else {
            id = player->GetBodyModelID(player->param1 & 0xff, 0);
            anim = &static_cast<dExtFrameCtrl_c &>(*player->mModels.mBodyModels[id]);
            if (anim->WillHitFrame(0x45)) {
                x0 = player->mPosX;
                y0 = player->mPosY + 0x50000;
                z0 = player->mPosZ;
                vv[0].x = x0;
                vv[0].y = y0;
                vv[0].z = z0;
                Particle::System::NewSimple(0x37, vv[0].x, vv[0].y, vv[0].z);

                Sound::PlayCharVoice(player->mCharacter, 0x33, player->mCamSpacePos);
            }
        }
    }

    if (player->param1 == 0) {
        if (cur.w[0] == 0xbb) {
            anim = &static_cast<dExtFrameCtrl_c &>(
                *player->mModels.mBodyModels[player->GetBodyModelID(player->param1 & 0xff, 0)]);
            frame = (unsigned int)(anim->currFrame << 4) >> 0x10;
            // a spawn flag grows the function 0x27c -> 0x288
            if (frame >= 0x1e)
                goto particle;
        }
        if (cur.w[0] == 0xbc) {
particle:
            z0 = player->mPosZ;
            y0 = player->mPosY + 0x4b000;
            x0 = player->mPosX;
            vv[1].x = x0;
            vv[1].y = y0;
            vv[1].z = z0;
            /* Inherited volatile read preserves the retail stack reload of X
             * into r2 at 0x020bcfb8 before the particle call. */
            player->mParticle1 = Particle::System::New(player->mParticle1, 0x36, *(int *)&vv[1].x,
                                                       vv[1].y, vv[1].z, 0, 0);
        }
    }

    if (player->mCapActor != 0) {
        dActor_c *actor = player->mCapActor;
        actor->mAngleX = player->mAngleX;
        actor->mAngleY = player->mAngleY;
        actor->mAngleZ = player->mAngleZ;
        actor = player->mCapActor;
        actor->mPosX = player->mPosX;
        actor->mPosY = player->mPosY;
        actor->mPosZ = player->mPosZ;
    }

    if (!player->FinishedAnim())
        return;

    cur.w[0] = base[player->mAnimScriptStep + 1].w[0];
    cur.w[1] = base[player->mAnimScriptStep + 1].w[1];
    if (cur.w[0] != 0) {
        player->mAnimScriptStep += 1;
    }
}
}

void daPly_c::func_ov002_020bd06c(unsigned int state2) {
    char *self = (char *)this;
    int flag;

    flag = 0;

    switch (this->mAnimScriptId) {
    case 8:
        if (this->param1 != 0) {
            this->mHasNoCap = 0;
        }
        /* fallthrough */
    case 9:
    case 10:
        if (this->mCapActor != 0) {
            this->mCapActor->MarkForDestruction();
            this->mCapActor = 0;
        }
        break;
    case 15:
        this->mRootMotionMode = 0;
        break;
    default:
        break;
    }

    switch (state2) {
    case 8:
        flag = 0x14;
        break;
    case 9:
        flag = 0x15;
        break;
    case 10:
        flag = 0x16;
        break;
    case 5:
        if (!this->IsState(daPly_c::ST_WAIT)) {
            this->ChangeState(daPly_c::ST_WAIT);
        }
        break;
    case 14:
        this->mRootMotionMode = 1;
        break;
    case 2:
        Sound::PlayBank0(0xdc, mCamSpacePos);
        break;
    case 4:
        Sound::PlayBank0(0xe1, mCamSpacePos);
        break;
    default:
        break;
    }

    if (flag == 0) return;
    if (this->mCapActor != 0)
        return;

    {
        s8 var_areaID;
        u32 var_self8;
        s16 var_deathTableID;

        var_areaID = this->mAreaId;
        var_self8 = *(volatile u32 *)&this->param1; // without volatile, 8 words differ
        var_deathTableID = -1;

        this->mCapActor =
            Spawn(0x10d, flag | (var_self8 << 8), mPos, 0, var_areaID, var_deathTableID);
    }

    {
        dActor_c *p2 = this->mCapActor;
        p2->mAngleX = this->mAngleX;
        p2->mAngleY = this->mAngleY;
        p2->mAngleZ = this->mAngleZ;
    }
}

int daPly_c::func_ov002_020bd20c(unsigned char *params, int beginFrame, int endFrame) {
    if (mAnimScriptId != *params) {
        func_ov002_020bd06c(*params);
        mAnimScriptId = *params;
        mAnimScriptStep = 0;
    }
    return 1;
}

int daPly_c::func_ov002_020bd250(unsigned char *params, int beginFrame, int endFrame) {
    char *self = (char *)this;
    Vector3 out1, off1, out2, off2, v;
    this->mAngleY = ReadUnalignedShort(params);
    off1.x = 0;
    off1.y = 0;
    off1.z = 0x50000;
    Vec3_RotateYAndTranslate(&out1, &mPos, this->mAngleY, &off1);
    this->mIsAirborne = 1;
    this->mLandSoundPlayed = 0;
    v.x = out1.x;
    v.y = out1.y;
    v.z = out1.z;
    this->Hurt(v, 0, 0xc000, 1, 0, 1);
    this->mVertSpeed = 0x20000;
    this->mHorzSpeed = 0xc000;
    if (param1 == 1) {
        off2.x = 0;
        off2.y = 0x64000;
        off2.z = 0x32000;
        Vec3_RotateYAndTranslate(&out2, &mPos, this->mAngleY, &off2);
        Particle::System::NewSimple(kPtclKnockAwayA, out2.x, out2.y, out2.z);
        Particle::System::NewSimple(kPtclKnockAwayB, out2.x, out2.y, out2.z);
    }
    return 1;
}

int daPly_c::func_ov002_020bd354(unsigned char *params, int beginFrame, int endFrame) {

    if (beginFrame == gKuppaScriptFrame) {
        this->mHasWings = 1;
        if (this->mPowerupTimer == 0)
            this->mPowerupTimer = 0x198;
    }
    this->mVertSpeed = -0x20000;
    this->mPeakY = this->mPosY;
    return 1;
}

int daPly_c::func_ov002_020bd3a0(unsigned char *params, int beginFrame, int endFrame) {
    extern ControllerInput data_0209f498[];

    u16 val = ReadUnalignedUshort(params);
    if ((unsigned int)beginFrame == gKuppaScriptFrame) {
        data_0209f498[this->mPlayerNo].pressed = val | data_0209f498[gActivePlayerSlot].pressed;
    }
    val |= data_0209f498[gActivePlayerSlot].held;
    data_0209f498[this->mPlayerNo].held = val;
    return 1;
}

int daPly_c::func_ov002_020bd438(unsigned char *params, int beginFrame, int endFrame) {
    extern void func_02012694(unsigned int, const Vector3 *);
    unsigned int v = ReadUnalignedInt(params);
    func_02012694(v, &mCamSpacePos);
    return 1;
}

int daPly_c::func_ov002_020bd45c(unsigned char *params, int beginFrame, int endFrame) {
    unsigned int v = ReadUnalignedInt(params);
    Sound::PlayBank0(v, mCamSpacePos);
    return 1;
}

int daPly_c::func_ov002_020bd480(unsigned char *params, int beginFrame, int endFrame) {
    unsigned int v = ReadUnalignedInt(params);
    Sound::PlayCharVoice(mCharacter, v, mCamSpacePos);
    return 1;
}

int daPly_c::func_ov002_020bd4ac(unsigned char *params, int beginFrame, int endFrame) {
    func_ov002_020d228c();
    return 1;
}

int daPly_c::func_ov002_020bd4c8(unsigned char *params, int beginFrame, int endFrame) {
    mFlags |= 0x24000000;
    return 1;
}

int daPly_c::func_ov002_020bd4e0(unsigned char *data, int beginFrame, int endFrame) {
    extern char data_0209f4a6[];
    extern char data_0209f4a0[];
    extern char data_0209f4ac[];
    extern char data_0209f49c[];
    char *self = (char *)this;
    Vector3 v;
    int y, z;
    int idx;

    func_0203db0c(0, 0);
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    v.x = ReadUnalignedShort(data) << 12;
    v.y = y;
    v.z = z;

    if (Vec3_HorzDist(&mPos, &v) <= this->mHorzSpeed) {
        this->mPosX = v.x;
        this->mPosZ = v.z;
        this->mHorzSpeed = 0;
        return 1;
    } else {
        s16 ang = Vec3_HorzAngle(&mPos, &v);
        idx = this->mPlayerNo;
        *(short*)(data_0209f4a6 + idx * 0x18) = ang;
        s16 v6 = ReadUnalignedShort(data + 6);
        *(short*)(data_0209f4a0 + this->mPlayerNo * 0x18) = v6;
        {
        int pi = this->mPlayerNo * 0x18;
        *(u8*)(data_0209f4ac + pi) = 1;
        }
        *(unsigned short*)(data_0209f49c + this->mPlayerNo * 0x18) = *(unsigned short*)(data_0209f49c + gActivePlayerSlot * 0x18) | 0x800;
        return 1;
    }
}

int daPly_c::func_ov002_020bd600(unsigned char *data, int beginFrame, int endFrame) {

    s32 x, y, z;
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    x = ReadUnalignedShort(data) << 12;
    this->mPosX = x;
    this->mPosY = y;
    this->mPosZ = z;
    this->mAngleY = ReadUnalignedShort(data + 6);
    this->mPrevAngleY = this->mAngleY;
    return 1;
}

int daPly_c::func_ov002_020bd664(unsigned char *cmd, int beginFrame, int endFrame) {
    if (!(data_ov002_0210e158 & 1)) {
        data_ov002_0210e158 |= 1;
        data_ov002_02110794[0] = &daPly_c::func_ov002_020bd600;
        data_ov002_02110794[1] = &daPly_c::func_ov002_020bd4e0;
        data_ov002_02110794[2] = &daPly_c::func_ov002_020bd4c8;
        data_ov002_02110794[3] = &daPly_c::func_ov002_020bd4ac;
        data_ov002_02110794[4] = &daPly_c::func_ov002_020bd480;
        data_ov002_02110794[5] = &daPly_c::func_ov002_020bd45c;
        data_ov002_02110794[6] = &daPly_c::func_ov002_020bd438;
        data_ov002_02110794[7] = &daPly_c::func_ov002_020bd3a0;
        data_ov002_02110794[8] = &daPly_c::func_ov002_020bd354;
        data_ov002_02110794[9] = &daPly_c::func_ov002_020bd250;
        data_ov002_02110794[10] = &daPly_c::func_ov002_020bd20c;
        data_ov002_02110794[11] = &daPly_c::func_ov002_020bcd38;
        data_ov002_02110794[12] = &daPly_c::func_ov002_020bcd18;
        data_ov002_02110794[13] = &daPly_c::func_ov002_020bccfc;
    }
    return (this->*data_ov002_02110794[cmd[6]])(cmd + 7, beginFrame, endFrame);
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_0211067c;
extern daPly_c::State data_ov002_021106ac;
}

int daPly_c::CanPause() {

    if (*(unsigned char *)((char *)&mIsUnderwater)) {
        if (this->IsState(data_ov002_0211067c))
            // early return predicates each exit (movne/popne/bxne) instead of one shared return 0 (size 0x84 -> 0x88)
            goto ret1a;
        if (this->IsState(data_ov002_021106ac))
            goto ret1a;
        goto ret0;
    ret1a:
        return 1;
    }
    if (*(unsigned char *)((char *)&mIsAirborne))
        goto ret0;
    if (*(unsigned char *)((char *)&mIsTakingDamage))
        goto ret0;
    if (*(unsigned char *)((char *)&mIsNoControl))
        goto ret0;
    return 1;
ret0:
  return 0;
}

int daPly_c::func_ov002_020bd8ac() {
    return mIsNoControl == 0;
}

void daPly_c::EndStateMusic(u32 music) {
    u32 other = mPowerupMusic;
    if (other == 0) {
        Sound::EndMusic(mPlayerNo);
        mPlayingMusic = 0;
    } else if (mPlayingMusic == music && other != music) {
        Sound::EndMusic(mPlayerNo);
        Sound::SetMusic(mPlayerNo, mPowerupMusic);
        mPlayingMusic = mPowerupMusic;
    }
    mStateMusic = 0;
}

void daPly_c::StartStateMusic(u32 music) {
    u32 other = mPowerupMusic;
    if (other == music) {
        mStateMusic = music;
        return;
    }
    if ((other | mStateMusic) != 0) {
        Sound::EndMusic(mPlayerNo);
    }
    mStateMusic = music;
    mPlayingMusic = music;
    Sound::SetMusic(mPlayerNo, music);
}

void daPly_c::EndPowerupMusic(u32 music) {
    u32 other = mStateMusic;
    if (other == 0) {
        Sound::EndMusic(mPlayerNo);
        mPlayingMusic = 0;
    } else if (mPlayingMusic == music && other != music) {
        Sound::EndMusic(mPlayerNo);
        Sound::SetMusic(mPlayerNo, mStateMusic);
        mPlayingMusic = mStateMusic;
    }
    mPowerupMusic = 0;
}

void daPly_c::StartPowerupMusic(u32 music) {
    u32 other = mStateMusic;
    if (other == music) {
        mPowerupMusic = music;
        return;
    }
    if ((mPowerupMusic | other) != 0) {
        Sound::EndMusic(mPlayerNo);
    }
    mPowerupMusic = music;
    mPlayingMusic = music;
    Sound::SetMusic(mPlayerNo, music);
}

void daPly_c::func_ov002_020bda48() {
    mPowerupTimer = 0;
    func_ov002_020e032c();
    func_ov002_020bdef0();
    func_ov002_020bdd9c();
    func_ov002_020d80d0();
    func_ov002_020bdd2c();
    LeaveShellRide();
    func_ov002_020de968();
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::HasNoCap() {
    extern unsigned char gGameMode;
    if (gCurrentSublevel == 0x1f)
        return 0;
    int r = (gGameMode == 2);
    if(!r){
        if(!this->IsState(data_ov002_0211058c)){
            r = (gGameMode == 1);
            if(!r){
                if (SaveData::HasPlayerLostCap())
                    return 1;
            }
        }
    }
    if(param1 == 3) return 0;
    r = mHasNoCap != 0;
    return r;
}

void daPly_c::func_ov002_020bdb50(int arg) {
    char* c = (char*)this;
    this->func_ov002_020bdc18();
    ReleaseHeldActor();
    if (arg != 0) return;
    if (this->mObjInMouth == 0) return;
    this->func_ov002_020d71a0();
    {
        int r = mObjInMouth->OnYoshiTryEat();
        if (r == 0) return;
    }
    {
        dActor_c *obj = mObjInMouth;
        int b1 = (obj->actorID == 0xbf);
        if (b1) {
            func_ov002_020d5cec(obj);
            (this->mStateFlags) &= ~2;
        } else {
            obj->OnTurnIntoEgg(*this);
        }
    }
    this->mObjInMouth = 0;
}

void daPly_c::func_ov002_020bdc18() {
    char *c = (char *)this;
    int i;
    for (i = 0; i < 5; i++) {
        dActor_c **a = mHeldObjQueue;
        if (a[i] != 0) {
            func_ov002_020edca4((daYegg_c *)a[i]);
            a = mHeldObjQueue;
            a[i] = 0;
        }
    }
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
void daPly_c::IncMegaKillCount() {
    Vector3 v;
    char *new_var2;
    int new_var3;
    char *a;
    int y;
    if ((mIsMega) == 0) {
        return;
    }
    ++mMegaKillCount;
    new_var3 = -1;
    if (mMegaKillCount >= 8) {
        mMegaKillCount = 8;
        Sound::PlayBank3(0x6e, mCamSpacePos);
        GiveLives(1);
    }
    v.x = mPosX;
    y = mPosY;
    v.y = y;
    v.z = mPosZ;
    v.y = y + 0x190000;
    new_var2 = (char *)Spawn(0x14b, mMegaKillCount, v, 0, mAreaId, new_var3);
    a = new_var2;
    if (a == 0) {
        return;
    }
    ((daObj1UpLogo_c *)a)->mDelay = 0;
    ((daObj1UpLogo_c *)a)->SetFollowTarget(0);
}

void daPly_c::func_ov002_020bdd2c() {
    extern void *gCamera;
    if (this->mIsMega == 1) {
        func_ov002_020db8bc(3);
        {
            ((dCamera_c *)gCamera)->mFlags &= ~0x80;
        }
        this->EndPowerupMusic(0x31);
        this->mWaterStillParticle = 0;
        this->mWaterMoveParticle = this->mWaterStillParticle;
        this->mParticle1 = 0;
        mMegaKillCount = 0;
    }
    this->mIsMega = 0;
}

void daPly_c::func_ov002_020bdd9c() {
    if (mIsMetal != 1)
        return;
    mIsMetal = 0;
    unsigned int v = this->param1;
    int id = this->GetBodyModelID(v & 0xff, 0);
    SharedFilePtr *t = data_ov002_020ff480[mCharFileBase + v];
    mModels.mBodyModels[id]->Copy(*mModels.mBodyModels[4], (char *)t->filePtr, 0);
    EndPowerupMusic(0x34);
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
void daPly_c::InitMetalWario() {
    u32 v;
    u32 id;

    func_ov002_020bda48();
    if (func_ov002_020bea7c() != 0) {
        u32 b = mHatCharacter;
        if (b != 2)
            return;
        this->TurnOffToonShading(b);
        this->TurnOffToonShading(mPrevCharacter);
        mCapFlags = 0;
        param1 = mHatCharacter;
        mHasNoCap = 0;
    }

    mIsMetal = 1;

    v = param1;
    id = this->GetBodyModelID(v & 0xff, 0);
    mModels.mBodyModels[id]->Copy(*mModels.mBodyModels[v],
                                  (char *)data_ov002_020ff480[mCharFileBase + v]->filePtr, 0);

    mPowerupTimer = 0x258;
    StartPowerupMusic(0x34);
    func_ov002_020c43c4(4);
}

void daPly_c::func_ov002_020bdef0() {
    if (mIsVanish == 0)
        return;
    mModels.mTexSeqPlayer[0].Update(mModels.mBodyModels[1]->data);
    mModels.mTexSeqPlayer[1].Update(mModels.mHeadModels[1]->data);
    mModels.mTexSeqPlayer[0].currFrame = 0;
    mModels.mTexSeqPlayer[1].currFrame = 0;
    mIsVanish = 0;
    EndPowerupMusic(0x33);
}

/* recovered: named members + shared header, real C++ method
 *
 * Starts the vanish cap: sets the flag, arms mPowerupTimer to 0x258 (the same 600
 * frames mBalloonTimer uses), then the sound and effect calls.
 *
 * Note the original wrote mPowerupTimer as `c + 0x600 + 0xae` rather than 0x6ae --
 * the split is how the compiler addressed it, not a second field.
 */
void daPly_c::InitVanishLuigi() {
    func_ov002_020bda48();
    mIsVanish = 1;
    mPowerupTimer = 0x258;
    StartPowerupMusic(0x33);
    func_ov002_020c43c4(3);
}

void daPly_c::func_ov002_020bdf8c() {
    extern char *gCamera;
    func_ov002_020bda48();
    mScaleAnimStep = 0;
    if (mIsMega == 0) {
        this->SetNoControlState(0xd, -1, 0);
        func_02012790(0x2e);
    }
    mIsMega = 1;
    mMegaTimer = 0x258;
    ((dCamera_c *)gCamera)->mFlags |= 0x80;
    StartPowerupMusic(0x31);
}

void daPly_c::func_ov002_020be008() {
    extern unsigned char gGameMode;
    int b = (gGameMode == 1);
    if (b != 0)
        return;
    if (func_ov002_020e3078(&data_ov002_021104b4) == 0)
        return;
    {
        int r5 = data_02092114;
        if (r5 < 0)
            return;
        if (r5 == param1)
            return;
        SetNewHatCharacter(r5 & 0xff, 0, 0);
        SetNoControlState(8, -1, 0);
        dActor_c::Spawn(0x10d, (r5 << 8) | 0xa, mPos, 0, mAreaId, -1);
        data_02092114 = -1;
    }
}

/* recovered: named members + shared header, real C++ method */
void daPly_c::SetNewHatCharacter(unsigned int p1, unsigned int p2, bool p3) {

  unsigned int old = param1;
  if (p1 == old) return;
  mPrevCharacter = (unsigned char)old;
  mHatCharacter = (unsigned char)p1;
  mCapFlags = 1;
  if (p1 != mCharacter) {
    this->mCapFlags |= 0x8000;
  }
  func_ov002_020bdb50(p2);
  func_ov002_020bda48();
  if (this->IsState(data_ov002_02110604)) {
      func_ov002_020d9c70();
      func_ov002_020da95c();
      this->ChangeState(data_ov002_0211013c);
  }
  if (p3 == 0) {
    if (p1 != mCharacter) func_02012790(0xb);
    else func_02012790(0xc);
  }
  param1 = mHatCharacter;
  func_ov002_020e6350();
  param1 = mPrevCharacter;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

void daPly_c::SetRealCharacter(unsigned int chr_) {
    u32 chr = chr_;

    u32 cur = param1;
    u32 base = mCharFileBase;
    u32 m1, m2;

    ((data_ov002_020ff480[base + (cur & 3)]))->Release();
    this->SetNewHatCharacter(chr, 0, 1);
    param1 = chr;
    mCharacter = (u8)chr;
    data_02092128[mPlayerNo] = (u8)param1;
    data_0209caa0.file.mCharacter = (u8)param1;
    dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[mCharFileBase + (param1 & 3)]);
    PlayCharacterSound();
    this->Heal(0x880);
    mCapFlags = 0;

    m1 = this->GetBodyModelID(chr, 0);
    m2 = this->GetBodyModelID(param1 & 0xff, 0);
    mModels.mBodyModels[m1]->Copy(*mModels.mBodyModels[m2],
                                  (char *)data_ov002_020ff480[mCharFileBase + chr]->filePtr, 0);
    m1 = this->GetBodyModelID(chr, 0);
    mModels.mBodyModels[m1]->Func_020162C4((BCA_File *)data_ov002_020ff480[chr + 0xc4]->filePtr, 0,
                                           0x1000, 0);
}

/* recovered: named members + shared header, real C++ method */
void daPly_c::TurnOffToonShading(unsigned int j) {
    dExtSimpleModel_c *m;
    m = mModels.mBodyModels[this->GetBodyModelID(j, 0)];
    m->SetPolygonMode(0);
    func_ov002_020e6b74(m, mModels.mBodyTexParams[j]);
    m = mModels.mHeadModels[j];
    m->SetPolygonMode(0);
    func_ov002_020e6b74(m, mModels.mHeadTexParams[j]);
    m = mModels.mHeadModels[j + 4];
    m->SetPolygonMode(0);
    func_ov002_020e6b74(m, mModels.mHeadTexParams[j + 4]);
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

void daPly_c::func_ov002_020be3b0() {
    extern char data_ov002_02110804[];
    void* case3_m1;
    void* case3_m2;
    int var_r5;
    int var_r4;
    u16 flags;
    int idx;

    flags = mCapFlags;
    var_r5 = 3;
    var_r4 = 0;
    if (flags & 0x8000) {
        var_r5 = 2;
        var_r4 = 5;
    }
    idx = flags & 7;

    switch (idx) {
    case 0:
        return;
    case 1: {
        dExtSimpleModel_c *m1 = mModels.mBodyModels[this->GetBodyModelID((u8)(this->param1), 0)];
        m1->SetPolygonMode(2);
        {
            dExtSimpleModel_c *m2 = mModels.mHeadModels[func_ov002_020becf4((u8)(this->param1), 0)];
            m2->SetPolygonMode(2);
        }
        mCapPaletteBlend = 0;
        mCapTimer = data_ov002_020ff14c[var_r4];
        mCapFlags++;
        this->param1 = mPrevCharacter;
        break;
    }
    case 2:
        mCapPaletteBlend += 0x199;
        if ((u32)mCapPaletteBlend >= 0x1000) {
            mCapPaletteBlend = 0x1000;
        }
        if (mCapTimer == 0) {
            mCapFlags++;
            mCapTimer = data_ov002_020ff14c[var_r4 + 1];
        }
        this->param1 = mPrevCharacter;
        break;
    case 3: {
        if ((mCapTimer % var_r5) == 0) {
            case3_m1 = mModels.mBodyModels[this->GetBodyModelID((u8)(this->param1), 0)];
            case3_m2 = mModels.mHeadModels[func_ov002_020becf4((u8)(this->param1), 0)];
            if ((mCapTimer % (var_r5 * 2)) == 0) {
                func_ov002_020e6b74(case3_m1, mModels.mBodyTexParams[this->param1]);
                func_ov002_020e6b74(
                    case3_m2, mModels.mHeadTexParams[func_ov002_020becf4((u8)(this->param1), 0)]);
            } else {
                func_ov002_020e6b3c(case3_m1);
                func_ov002_020e6b3c(case3_m2);
            }
        }
        if (mCapTimer == 0) {
            dExtSimpleModel_c *t1;
            dExtSimpleModel_c *t2;
            func_ov002_020e6b3c(case3_m1);
            func_ov002_020e6b3c(case3_m2);
            mCapFlags++;
            mCapTimer = data_ov002_020ff14c[var_r4 + 2];
            t1 = mModels.mBodyModels[this->GetBodyModelID(mHatCharacter, 0)];
            t1->SetPolygonMode(2);
            func_ov002_020e6b3c(t1);
            t2 = mModels.mHeadModels[func_ov002_020becf4(mHatCharacter, 0)];
            t2->SetPolygonMode(2);
            func_ov002_020e6b3c(t2);
            mQuicksandRunFlags = 0;
            mWaterRunLatch = mQuicksandRunFlags;
            mWaterRunTimer = mWaterRunLatch;
            mDetectWater = 0;
            func_ov002_020d80d0();
            mFireBreaths = 0;
            mUseAltBodyModel = 0;
            dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[mCharFileBase + mHatCharacter]);
            {
                u32 mm1 = this->GetBodyModelID(mHatCharacter, 0);
                mModels.mBodyModels[mm1]->Copy(
                    *mModels.mBodyModels[this->GetBodyModelID(mPrevCharacter, 0)],
                    (char *)data_ov002_020ff480[mCharFileBase + mHatCharacter]->filePtr, 0);
            }
            mLoadedResourceFlags |= 0x40;
        }
        this->param1 = mPrevCharacter;
        break;
    }
    case 4:
        if ((mCapTimer % var_r5) == 0) {
            if ((mCapTimer % (var_r5 * 2)) == 0) {
                this->param1 = mPrevCharacter;
            } else {
                this->param1 = mHatCharacter;
            }
        }
        if (mCapTimer == 0) {
            u32 mm;
            mCapFlags++;
            mCapTimer = data_ov002_020ff14c[var_r4 + 3];
            this->param1 = mHatCharacter;
            data_02092128[mPlayerNo] = (u8)(this->param1);
            if (mHatCharacter == mCharacter) {
                mHasNoCap = 0;
            }
            func_ov002_020e6b74(mModels.mBodyModels[this->GetBodyModelID(mHatCharacter, 0)],
                                mModels.mBodyTexParams[*(u8 *)&this->mHatCharacter]);
            func_ov002_020e6b74(mModels.mBodyModels[this->GetBodyModelID(mPrevCharacter, 0)],
                                mModels.mBodyTexParams[*(u8 *)&this->mPrevCharacter]);
            {
                int h1 = func_ov002_020becf4(mHatCharacter, 0);
                func_ov002_020e6b74(mModels.mHeadModels[h1], mModels.mHeadTexParams[h1]);
            }
            {
                int h2 = func_ov002_020becf4(mPrevCharacter, 0);
                func_ov002_020e6b74(mModels.mHeadModels[h2], mModels.mHeadTexParams[h2]);
            }
            mm = this->param1;
            mModels.mBodyModels[this->GetBodyModelID((u8)mm, 0)]->Func_020162C4(
                (BCA_File *)((SharedFilePtr *)data_ov002_020ff790[mm])->filePtr, 0, 0x1000, 0);
            data_ov002_020ff480[mCharFileBase + mPrevCharacter]->Release();
            mLoadedResourceFlags &= ~0x40;
            func_ov002_020bdb50(0);
        } else {
            u8 a, b;
            u32 idxA;
            b = mPrevCharacter;
            if (b == this->param1) {
                a = mHatCharacter;
            } else {
                a = b;
                b = mHatCharacter;
            }
            idxA = this->GetBodyModelID(a, 0);
            mModels.mBodyModels[idxA]->Copy(*mModels.mBodyModels[this->GetBodyModelID(b, 0)],
                                            (char *)data_ov002_020ff480[mCharFileBase + a]->filePtr,
                                            0);
        }
        break;
    case 5:
        if (mCapTimer == 0) {
            mCapFlags++;
            mCapTimer = data_ov002_020ff14c[var_r4 + 4];
            if (mHatCharacter != mCharacter && this->IsState(data_ov002_021104b4) == 0) {
                Sound::PlayCharVoice(mCharacter, 0x21, mCamSpacePos);
            }
        }
        break;
    case 6: {
        int divv = data_ov002_020ff14c[var_r4 + 4];
        mCapPaletteBlend -= 0x1000 / divv;
        if ((s16)mCapPaletteBlend < 0) {
            mCapPaletteBlend = 0;
        }
        if (mCapTimer == 0) {
            mCapFlags++;
        }
        break;
    }
    case 7:
        this->TurnOffToonShading(mHatCharacter);
        this->TurnOffToonShading(mPrevCharacter);
        mCapFlags = 0;
        break;
    }

    if (mCapTimer != 0) {
        mCapTimer--;
    }
    {
        char* dummy = data_ov002_02110804;
        func_ov002_020e69c0(dummy);
        func_ov002_020e6a78(dummy, mCapPaletteBlend);
    }
}

int daPly_c::func_ov002_020bea7c() {
    return mCapFlags != 0;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::IsCollectingCap() {
    unsigned short v = mCapFlags;
    if (v != 0 && (v & 7) < 5)
        return 1;
    return 0;
}

void daPly_c::func_ov002_020beabc() {
    char *p = (char *)this;
    mCapFlags = 5;
    mCapTimer = 0x2f;
    mCapPaletteBlend = 0x1000;
    this->mPrevCharacter = this->mCharacter;
    this->mHatCharacter = this->mPrevCharacter;
    unsigned int id = GetBodyModelID(this->mCharacter, 0);
    mModels.mBodyModels[id]->SetPolygonMode(2);
    unsigned int id2 = func_ov002_020becf4(param1 & 0xff, 0);
    mModels.mHeadModels[id2]->SetPolygonMode(2);
}

int daPly_c::func_ov002_020beb38() {
    extern unsigned short data_0209f49e;
    extern unsigned short data_020a0e5a;
  unsigned char idx = gActivePlayerSlot;
  unsigned char count = 0;
  unsigned short f = *(unsigned short*)((char*)&data_0209f49e + idx*0x18);
  if (f & 1) count++;
  if (f & 2) count++;
  if (f & 0x400) count++;
  if (f & 0x800) count++;
  if (*(unsigned short*)((char*)&data_020a0e5a + idx*4) & 0xf0) count++;
  if (AngleDiff(mPrevDesiredAngleY, mDesiredAngleY) >= 0x4000) count += 8;
  return count;
}

void daPly_c::func_ov002_020bebd4() {
    int i;
    for (i = 0x44; i < 0x1c8; i += 4) {
        ((data_ov002_020ff480[i]))->Release();
        ((data_ov002_020ff480[i + 1]))->Release();
        ((data_ov002_020ff480[i + 2]))->Release();
        ((data_ov002_020ff480[i + 3]))->Release();
    }
}

void daPly_c::func_ov002_020bec2c() {
    int i;
    for (i = 0x44; i < 0x1c8; i += 4) {
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[i]);
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[i + 1]);
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[i + 2]);
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[i + 3]);
    }
}

extern "C" {

int func_ov002_020bec84(daPly_c *p, unsigned int i) {
    return p->IsAnim(data_ov002_020ff220[i]);
}
}

void daPly_c::func_ov002_020bec9c(unsigned int a, int b, Fix12i d, u16 e) {
    SetAnim(data_ov002_020ff220[a], b, d, e);
}

/* recovered: named members + shared header, real C++ method
 *
 * Metal cap overrides whichever body model the caller asked for. Which metal
 * flag is consulted depends on the second argument -- mIsMetal for the live
 * state, mDrawnIsMetal (func_ov002_020e4bb8's blinked copy) for drawing.
 *
 * const, and the K in the mangled name is what says so: declared without it
 * the compiler emits the non-const name, which names a symbol that does not
 * exist.
 */
unsigned int daPly_c::GetBodyModelID(unsigned int a, bool b_) const {
    if (b_ == 0) {
        if (mIsMetal) a = 4;
    } else {
        if (mDrawnIsMetal)
            a = 4;
    }
    return a;
}

int daPly_c::func_ov002_020becf4(int v, int arg2) {
    int cond;
    int r1;
    if (arg2 == 0) {
        if (this->mIsMetal) v = 9;
        if (this->mIsBalloon) v = 8;
        cond = this->HasNoCap();
    } else {
        if (this->mDrawnIsMetal)
            v = 9;
        if (this->mDrawnIsBalloon)
            v = 8;
        cond = this->mDrawnHasNoCap;
    }
    r1 = 0;
    if ((unsigned int)v >= 4) {
        if (cond) r1 = 1;
    } else if (v == 3) {
        if (this->mUseAltBodyModel) r1 = 4;
    } else {
        if (cond) r1 = 4;
    }
    return (r1 + v) & 0xff;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::FinishedAnim() {
    unsigned int modelID = GetBodyModelID(param1 & 0xff, 0);
    // GetBodyModelID can return 4; the shared array currently declares four slots.
    dExtAnmModel_c *model = mModels.mBodyModels[modelID];
    return model->Finished() != 0;
}

void daPly_c::AdvanceAnims() {
    unsigned int modelID = GetBodyModelID(param1 & 0xff, 0);
    dExtAnmModel_c *model = mModels.mBodyModels[modelID];
    model->Advance();

    if (IsAnim(0x9a) || IsAnim(0x19) || IsAnim(0xac) || IsAnim(0x6b) || IsAnim(0x85) ||
        IsAnim(0x94) || IsAnim(0x98) || IsAnim(0x99)) {
        mModels.mTexSeqBody[param1].currFrame = 0;
    }
    mModels.mTexSeqBody[param1].Advance();
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::IsAnim(unsigned int a) {
  int ip = param1;
  SharedFilePtr *p = data_ov002_020ff480[ip + a * 4];
  BCA_File *v = (BCA_File *)p->filePtr;
  unsigned int id = this->GetBodyModelID(ip&0xff, 0);
  dExtAnmModel_c *model = mModels.mBodyModels[id];
  int matches = (model->file == v);
  return matches ? 1 : 0;
}

void daPly_c::SetAnim(u32 animId, int flags, Fix12i speed, u16 startFrame) {
    extern u8 gGameMode;
    u32 capChar;
    SharedFilePtr *capFile;
    u32 texIdx;
    int character;
    u32 fileBase;
    int flag;
    int texFlags;
    dExtAnmModel_c *body;

    flag = (gGameMode == 1);
    if (flag) {
        if ((int)animId < 0x11 || (int)animId >= 0x72) {
            mStateFlags |= 0x80;
            return;
        }
    }

    fileBase = animId << 2;
    if (fileBase != mCharFileBase) {
        data_ov002_020ff480[mCharFileBase + (param1 & 3)]->Release();
        dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[fileBase + (param1 & 3)]);

        body = mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
        body->file = 0;

        if ((mCapFlags & 7) == 4) {
            capChar = mHatCharacter;
            if (capChar == param1)
                capChar = mPrevCharacter;

            data_ov002_020ff480[mCharFileBase + capChar]->Release();
            capFile = data_ov002_020ff480[fileBase + capChar];
            dExtFrameCtrl_c::LoadFile(*capFile);
            mModels.mBodyModels[GetBodyModelID(capChar & 0xff, 0)]->SetAnim(
                (BCA_File *)capFile->filePtr, flags, speed, startFrame);
            mCapTimer++;
        }
    }

    character = param1;
    texIdx = animId * 4;
    mModels.mBodyModels[GetBodyModelID(character & 0xff, 0)]->SetAnim(
        (BCA_File *)data_ov002_020ff480[(character & 3) + animId * 4]->filePtr, flags, speed,
        startFrame);

    character = param1;
    if ((int)texIdx >= 0x60)
        texIdx = 0x60;
    texIdx += character & 3;
    texFlags = 0;
    if (((int)texIdx >> 2) == 1)
        texFlags = 0x40000000;
    mModels.mTexSeqBody[character].SetFile(*(BTP_File *)data_ov002_020ff2f0[texIdx]->filePtr,
                                           texFlags, 0x1000, 0);

    mCharFileBase = fileBase;
}

/* matched pure C — was HAND-ASM regperm floor; y-early preload cracks r4/r5 */
void daPly_c::func_ov002_020bf13c() {
    /* v is the POD VecFx32, not Vector3, measured: v never escapes, and the ROM keeps
       all three components in registers -- the frame here is 0x10, which is just
       the volatile pad[4] below. Vector3 declares a destructor, so the object
       must have an address, and that pushes v onto the stack and grows the frame
       to 0x20. No other instruction in the function changes. This is a second
       reason for a POD, besides the copy lowering include/types.h describes. */
    volatile int pad[4]; // without volatile, the function shrinks 0xe8 to 0xd8
    int vel; int* dst; short s, cs; int ang; int r6, flags;
    VecFx32 v;
    (void)&pad;
    if (param1 != 3)
        return;
    dst = (int *)mTrailPos;
    if (dst == 0) return;
    vel = this->mHorzSpeed;
    ang = this->mAngleY;
    {
        int idx = (unsigned short)(short)(ang + 0x8000) >> 4;
        s = FX_SinCosTable_[idx * 2];
        cs = FX_SinCosTable_[idx * 2 + 1];
    }
    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    v.x = s * (short)0x78 + v.x;
    v.z = cs * (short)0x78 + v.z;
    r6 = vel >> 5;
    if (r6 > 0x1000) r6 = 0x1000;
    if (r6 < 0) r6 = 0;
    {
        unsigned char f6de = mIsAirborne;
        int ip = (r6 + 0x1000) >> 8;
        flags = ip & 0xff;
        if (f6de != 0)
            flags = (flags | 0x80) & 0xff;
        if (vel != 0)
            flags = (flags | 0x40) & 0xff;
        dst[0] = v.x;
        dst[1] = v.y + 0xc000;
        dst[2] = v.z;
        *mTrailFlags = (unsigned char)flags;
    }
}

/* Scale b by an s16 factor from a 0x18-stride table indexed by byte
 * gActivePlayerSlot (fx12 rounded multiply), clamped below by c. Does not
 * read the player.
 */
int daPly_c::func_ov002_020bf224(int b, int c) {
    extern struct Entry18 data_0209f4a0[];
    b = FX_MUL(b, data_0209f4a0[gActivePlayerSlot].val);
    if (b < c)
        b = c;
    return b;
}

int daPly_c::func_ov002_020bf27c(int r1) {
    int v = param1;
    int b;
    if (mIsMega != 0)
        return r1;
    b = (v == 3);
    if (b != 0)
        return r1 * 0x5a / 100;
    if (v == 1)
        r1 >>= 2;
    return r1;
}

void daPly_c::func_ov002_020bf2d8(Fix12i a) {
    void *c = this;
    unsigned int t = data_ov002_020ff140[param1];
    this->mVertSpeed = FX_MUL(a, (int)t);
}

Fix12i daPly_c::ScaleByCharFactor(Fix12i a) {
    int idx = param1 & 0xff;
    unsigned int t = data_ov002_020ff170[idx];
    return FX_MUL(a, (int)t);
}

/* Add delta to *p and clamp it to [-limit, limit]. Does not read the player. */
void daPly_c::func_ov002_020bf340(int *p, int delta, int limit) {
    int neg = -limit;
    *p = *p + delta;
    if (*p < neg) {
        *p = neg;
        return;
    }
    if (*p > limit) {
        *p = limit;
    }
}

void daPly_c::func_ov002_020bf36c(void *arg) {
    typedef int Fix12i;
    Fix12i saved;
    unsigned char f = this->mIsNoControl;
    if (f == 0)
        this->func_ov002_020bfa74();
    if ((this->mClsnFlags & 1) || this->mIsUnderwater)
        this->func_ov002_020c0108(1);
    if (mClimbClsn != 0)
        return;
    saved = this->mHorzSpeed;
    if (this->mClsnFlags & 1)
        this->mHorzSpeed = FX_MUL(saved, this->mFloorNormalY);
    UpdatePos((dCc_c *)arg);
    this->mHorzSpeed = saved;
    func_ov002_020ce798();
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

int daPly_c::func_ov002_020bf40c() {
    extern dCamera_c *gCamera;
    Vector3 pos;
    Vector3 off;
    if (NumStars() < 0x50 || param1 != 0) {
        VEC_Add(&mPos, (Vector3 *)&data_ov002_0210a590, &mPos);
        off.x = data_ov002_0210a590.x;
        off.y = data_ov002_0210a590.y;
        off.z = data_ov002_0210a590.z;
        gCamera->func_0200d858(&off);
        dBgCh_Gnd rg;
        {
            int py = this->mPosY;
            int pz = this->mPosZ;
            pos.x = this->mPosX;
            pos.y = py + 0xa0000;
            pos.z = pz;
        }
        rg.SetObjAndPos(pos, this);
        if (rg.DetectClsn()) {
            mGroundY = rg.clsnY;
        }
        return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::Heal(int amt) {

  int r = this->IsState(data_ov002_0211010c);
  if(r) return r;
  r = this->IsState(data_ov002_02110124);
  if(r) return r;
  return GiveHealth(mPlayerNo, amt);
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::GetHealth() {
  unsigned char i = mPlayerNo;
  return (signed char)(gPlayerHealth[i] >> 8);
}

int daPly_c::func_ov002_020bf56c(int b) {
    int m;
    switch (func_ov002_020c031c()) {
    case 4:
    case 5:
        m = 0x199;
        break;
    case 3:
        m = 0x599;
        break;
    default:
    case 0:
        m = 0x1000;
        break;
    case 1:
    case 2:
        m = 0x1000;
        break;
    }
    return FX_MUL(b, m);
}

void daPly_c::func_ov002_020bf5e0() {
    int flag;
    int id;
    Vector3 out;
    Vector3 src;
    int target;
    int newId;
    int state;

    if (mIsMega == 0)
        return;

    flag = 0;
    if (this->IsAnim(0x9e) || this->IsAnim(0xa4)) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        // if/else on the two WillHitFrame tests diffs 12 words from +0x74
        if (mModels.mBodyModels[id]->WillHitFrame(0xe))
            goto set_flag;
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (!mModels.mBodyModels[id]->WillHitFrame(0x1c))
            goto flag_done;
set_flag:
        flag = 1;
    }
flag_done:
    if (!flag)
        return;

    id = this->GetBodyModelID(param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x1c)) {
        src.x = 0x40000;
    } else {
        src.x = -0x40000;
    }
    src.y = 0x1b000;
    src.z = 0x40000;

    Vec3_RotateYAndTranslate((Vector3 *)&out.x, (const Vector3 *)&this->mPosX, this->mAngleY,
                             (const Vector3 *)&src.x);

    if (mIsInShallowWater != 0 || mIsUnderwater != 0) {
        Particle::System::NewSimple(kPtclHeavyStepWater, out.x, out.y, out.z);
    } else {
        Particle::System::NewSimple(kPtclHeavyStep, out.x, out.y, out.z);
    }

    Sound::PlayBank0(0xd0, mCamSpacePos);

    if (mIsInShallowWater != 0 && mIsUnderwater == 0) {
        newId = Particle::System::NewCheckWaterRipple(mParticle1, kPtclHeavyStepRipple, this->mPosX,
                                                      data_0209f32c, this->mPosZ, 0);
        mParticle1 = newId;
    }

    state = mGroundSoundType;
    target = -1;
    if (state != 6) {
        if (state != 8) {
            if (state == 9)
                target = 0xe6;
        } else {
            target = 0xe4;
        }
    } else {
        target = 0xe5;
    }

    if (target == -1)
        return;

    src.y = 0;
    Particle::System::NewSimple(target, out.x, out.y, out.z);
}

void daPly_c::func_ov002_020bf800(Vector3_16f v) {
    typedef unsigned int u32;
    typedef short s16;
    int xv = mPosX - v.x * 0x2bc;
    int zv = mPosZ - v.z * 0x2bc;
    int yv = mPosY - v.y * 0x2bc;
    Fix12v a(xv), b(yv), d(zv);
    mParticle1 = Particle::System::New(mParticle1, kPtclCurrentFlow, *(int *)&a, *(int *)&b,
                                       *(int *)&d, (const Vector3_16f *)(&v), 0);
}

void daPly_c::func_ov002_020bf88c() {
    if (mIsInShallowWater) return;
    if (gSubSoundID == 0x2e)
        return;
    if (mIsMega)
        Particle::MegaRunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
    else
        Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
}

void daPly_c::func_ov002_020bf90c() {
    if (this->mHorzSpeed == 0) return;
    if (mIsAirborne != 0) return;
    if (mIsInShallowWater != 0) return;
    if (gSubSoundID == 0x2e)
        return;

    if (mIsMega != 0)
        Particle::MegaRunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
    else
        Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);

    mLoopingSoundHandle = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2,
                                        &mCamSpacePos, this->mHorzSpeed, 0);
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
// func_ov002_020bf9d4 at 0x020bf9d4
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
void daPly_c::func_ov002_020bf9d4() {
  Vector3 v;
  if (gSubSoundID == 0x2e)
      return;
  if (mIsMega == 0){
      func_0200fccc(0);
      return;
  }
  ((int*)&v)[0] = this->mPosX;
  ((int*)&v)[1] = this->mPosY;
  ((int*)&v)[2] = this->mPosZ;
  if (mIsInShallowWater != 0){
    v.y += 0x19000;
    Particle::System::NewSimple(kPtclMegaGroundPuffWater, v.x, v.y, v.z);
  } else {
    v.y += 0x23000;
    Particle::System::NewSimple(kPtclMegaGroundPuff, v.x, v.y, v.z);
  }
}

/* daPly_c on a CLPS surface of type 0xb (a current/conveyor tile): the tile's
 * CLPS word names a two-node path whose direction is the push vector. Spawns the
 * flow particle (func_ov002_020bf800), keeps the surface loop sound alive, and
 * unless the player is metal, uncontrolled or in a blocked state, pushes the
 * player along the path: grounded states get a position nudge scaled by
 * data_ov002_020ff210[def->unk_004], airborne states get their horizontal
 * velocity bent toward the path direction. Returns 1 when the push applied. */
#define SIN(a) FX_SinCosTable_[((u16)(a) >> 4) * 2]
#define COS(a) FX_SinCosTable_[((u16)(a) >> 4) * 2 + 1]

int daPly_c::func_ov002_020bfa74() {
    s32 n;
    s32 i;
    u32 idx;
    s16 horz;
    s16 vert;
    s32 k;
    s32 scale;

    if (mSurfaceType != 0xb) return 0;
    if (mFloorPathID != 0xff) {
        PathPtr path;
        Vector3_16f dir;
        Vector3 nodes[2];
        Vector3 vel;
        path.FromID(mFloorPathID);
        n = path.NumNodes();
        for (i = 0; i < n; i++) {
            path.GetNode(nodes[i], i);
        }
        idx = (u8)path.GetUnk004();
        if (idx >= 4) idx = 1;
        horz = Vec3_HorzAngle(&nodes[0], &nodes[1]);
        vert = Vec3_VertAngle(&nodes[0], &nodes[1]);
        dir.y = SIN(vert);
        dir.x = FX_MUL(COS(vert), SIN(horz));
        dir.z = FX_MUL(COS(vert), COS(horz));
        func_ov002_020bf800(dir);
        mSurfaceSoundHandle = Sound::PlayLong(mSurfaceSoundHandle, 3, 0x90, mCamSpacePos, 0);
        if (mIsMetal != 0 || mIsNoControl != 0 || func_ov002_020e0478() != 0)
            return 0;
        if (mIsAirborne == 0) {
            if (this->IsState(data_ov002_0211013c) || this->IsState(daPly_c::ST_WAIT) ||
                this->IsState(data_ov002_02110574) || this->IsState(data_ov002_02110184) ||
                this->IsState(data_ov002_02110514) || this->IsState(data_ov002_021104e4) ||
                this->IsState(data_ov002_02110364) || this->IsState(data_ov002_0211064c)) {
                if (mHorzSpeed <= 0) {
                    k = 0x8000;
                } else {
                    scale = data_ov002_020ff210[idx];
                    k = FX_MUL(scale, mHorzSpeed);
                }
                mPosX += FX_MUL(k, dir.x);
                mPosZ += FX_MUL(k, dir.z);
            }
        } else {
            if (this->IsState(data_ov002_021101b4) || this->IsState(data_ov002_021102d4)) return 1;
            vel.x = FX_MUL(mHorzSpeed, SIN(mPrevAngleY));
            vel.x += FX_MUL(dir.x, 0x2800);
            vel.z = FX_MUL(mHorzSpeed, COS(mPrevAngleY));
            vel.z += FX_MUL(dir.z, 0x2800);
            mPrevAngleY = cstd::atan2(vel.x, vel.z);
            mHorzSpeed = Vec3_HorzLen(&vel);
        }
        return 1;
    }
    return 0;
}

#undef SIN
#undef COS

int daPly_c::func_ov002_020bfec0() {
    int state = mSurfaceType;
    int inc;

    if (state == 1) {
        if (mIsMetal != 0) state = 8;
    }

    if (state < 6 || state > 9) {
        if (!this->IsState(data_ov002_02110124)) {
            mSinkDepth = 0;
        }
        return 0;
    }

    if (mIsAirborne != 0 || this->IsInAir() || mClimbClsn != 0 || mIsMega != 0 ||
        mIsNoControl != 0) {
        return 0;
    }

    mQuicksandRunFlags |= 1;

    {
        int flag = (mRidingShell != 0);
        if (flag || mQuicksandRunTimer != 0) {
            mSinkDepth = 0;
            return 0;
        }
    }

    inc = 0x400;
    if (this->IsState(daPly_c::ST_WAIT) || this->IsState(data_ov002_0211049c)) {
        inc = 0x800;
    }

    if (this->IsState(data_ov002_02110424)) {
        if (func_ov002_020e3078(&data_ov002_021104cc) == 0) {
            mQuicksandJumpTimer = 2;
        }
    }

    if (mSinkDepth < 0x1100) mSinkDepth = 0x1100;

    switch (state) {
    case 6:
        mSinkDepth += inc;
        if (mSinkDepth >= 0xa000) mSinkDepth = 0xa000;
        break;
    case 7:
        mSinkDepth += inc;
        if (mSinkDepth >= 0x3c000) mSinkDepth = 0x3c000;
        break;
    case 8:
        mSinkDepth += inc;
        if (mSinkDepth < 0xa0000)
            break;
        /* fallthrough */
    case 9:
        func_ov002_020c5dec(3);
        return 1;
    default:
        mSinkDepth = 0;
        break;
    }
    return 0;
}

int daPly_c::func_ov002_020c0108(int p1) {
    char *self = (char *)this;
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef short s16;
    typedef unsigned int u32;
    typedef int s32;
    if (this->mIsMetal != 0 || (this->mIsNoControl != 0 && p1 != 0) || this->IsState(data_ov002_02110304) || this->IsState(data_ov002_0211025c))
        return 0;

    if (this->mSurfaceType == 0xb)
        return 0;

    if (mFloorPathID != 0xff) {
        s32 s;
        s32 iX, iY;
        s32 w;
        int idx;
        int numNodes;
        int i;
        int aH, aV;
        Vector3 nodes[3];
        PathPtr path;

        path.FromID(mFloorPathID);
        numNodes = path.NumNodes();

        for (i = 0; i < numNodes; i++) {
            path.GetNode(nodes[i], i);
        }

        idx = path.GetUnk004() & 0xff;
        if ((u32)idx >= 4)
            idx = 1;

        aH = Vec3_HorzAngle(&nodes[0], &nodes[1]);
        aV = Vec3_VertAngle(&nodes[0], &nodes[1]);

        iY = (u16)aV >> 4;
        iX = (u16)aH >> 4;

        w = data_ov002_020ff200[idx] << 12;

        this->mPosY += FX_MUL(w, FX_SinCosTable_[iY * 2]);
        s = FX_MUL(w, FX_SinCosTable_[iY * 2 + 1]);
        this->mPosX += FX_MUL(s, FX_SinCosTable_[iX * 2]);
        this->mPosZ += FX_MUL(s, FX_SinCosTable_[iX * 2 + 1]);

        return 1;
    }

    return 0;
}

#undef AT

extern "C" {

extern daPly_c::State data_ov002_02110514;
}

int daPly_c::func_ov002_020c031c() {

    int r4 = this->mFloorClass;
    if (this->IsState(data_ov002_02110514) && this->mFloorNormalY > 0x800) {
        if (r4 == 0)
            r4 = 2;
    }
    return r4;
}

void daPly_c::func_ov002_020c0364(u32 arg) {
    int flag;

    if (this->IsState(data_ov002_02110334)) return;
    if (this->IsState(data_ov002_0211031c)) return;

    flag = 0;
    if (arg == 3) {
        if (AngleDiff(this->mFloorAngle, this->mAngleY) < 0x4000)
            flag = 1;
    } else {
        if (arg & 1)
            flag = 1;
    }

    if (flag) {
        this->ChangeState(data_ov002_02110334);
    } else {
        this->ChangeState(data_ov002_0211031c);
        this->SetAnim(0x43, 0x40000000, 0x1000, 0);
    }
}

int daPly_c::func_ov002_020c0434() {
    unsigned short f = mStateFlags & 1;
    if (f) {
        if (mFloorClass == 5)
            return 1;
        if (this->mHorzSpeed < 0)
            return 1;
        if (func_ov002_020c04ac())
            return 1;
    }
    return 0;
}

int daPly_c::func_ov002_020c04ac() {
  return AngleDiff(mFloorAngle, this->mAngleY) <= 0x4000;
}

int daPly_c::func_ov002_020c04e0() {
    int horzLen;
    int angDiff;
    int sel;
    int accelBase;
    int speed;
    int accel;
    int ret;

    horzLen = Vec3_HorzLen((const Vector3 *)&mFloorNormalX);
    angDiff = AngleDiff(mFloorAngle, this->mPrevAngleY);

    if (mWalkTimer != 0) {
        if (mFloorClass == 0)
            return mFloorClass;
    }

    sel = func_ov002_020c031c();
    ret = func_ov002_020f035c(sel, mFloorNormalY);
    if (ret == 0)
        return ret;

    sel = 0;
    if (!this->IsState(data_ov002_0211010c))
        sel = func_ov002_020c031c();
    if (this->IsState(data_ov002_02110304)) {
        if (sel == 4)
            sel = 0;
    }

    switch (sel) {
    case 4:
    case 5:
        accelBase = 0x54cc;
        break;
    case 3:
        accelBase = 0x2b33;
        break;
    case 0:
    default:
        accelBase = 0x1b33;
        break;
    case 1:
    case 2:
        accelBase = 0;
        break;
    }

    speed = this->mHorzSpeed - 0x13000;
    if (speed < 0x1000)
        speed = 0x1000;
    if (speed >= 0x1800)
        speed = 0x1800;

    accel = FX_MUL(accelBase, speed);

    if (angDiff < 0x4000) {
        int *p = &this->mHorzSpeed;
        int v = *p + FX_MUL(accel, horzLen);
        *p = v;
        return v;
    } else {
        int *p = &this->mHorzSpeed;
        int v = *p - FX_MUL(accel, horzLen);
        *p = v;
        return v;
    }
}

int daPly_c::func_ov002_020c0688() {
    if (mIsAirborne == 0)
        return 1;
    if (mSlideType != 0) {
        int d = this->mPosY - mGroundY;
        if (d < 0xb4000 && d >= 0) {
            this->mPosY = mGroundY;
            return 1;
        }
    } else if (mIsSlidingOnGround != 0) {
        int d = this->mPosY - mGroundY;
        if (d < 0x28000) {
            this->mPosY = mGroundY;
            return 1;
        }
    }
    return 0;
}

int daPly_c::func_ov002_020c06fc(int arg) {
    /* The POD VecFx32, not Vector3, and this is the strongest case in the file:
       it is a MEMBER of the local struct Frame below, so a non-POD element
       gives Frame an implicit destructor, and mwccarm emits that as a new
       compiler-numbered local-class symbol -- measured as
       a D1 for daPly_c::Frame$2583Player_cpp. This TU emits no @NNNN/$NNN symbols
       at all today, and such a name depends on declaration order (compare
       Bundle$902d_a_mky_cpp in AGENTS.md). Frame has to stay POD. */
    extern ControllerInput data_0209f498[];
    struct Frame {
        VecFx32 v0;
        VecFx32 v1;
        VecFx32 v2;
        VecFx32 v3;
    } f;
    int a;
    int t;
    int b;
    int v;
    int spd;
    int r5v;
    int r4v;
    int ang;

    if (this->mIsAirborne != 0)
        return 0;
    if ((this->mFloorNormalY | this->mHorzSpeed) == 0)
        return 0;

    a = 0;
    b = a;
    if (data_0209f498[gActivePlayerSlot].mag >= 0x200) {
        s16 d = (s16)(this->mDesiredAngleY - this->mPrevAngleY);
        int j = ((u16)d >> 4) * 2;
        a = FX_SinCosTable_[j + 1];
        b = FX_SinCosTable_[j];
    }

    t = func_ov002_020f030c(this->func_ov002_020c031c());
    t = t + FX_MUL(data_0209f498[gActivePlayerSlot].mag, FX_MUL(a, 0x52));

    v = func_ov002_020f02c8(this->func_ov002_020c031c());
    {
        int j = ((u16)mPrevAngleY >> 4) * 2;
        spd = this->mHorzSpeed;
        r5v = FX_MUL(spd, (int)FX_SinCosTable_[j]);
        r4v = FX_MUL(spd, (int)FX_SinCosTable_[j + 1]);
    }

    if (this->mSlideType != 0) {
        if (data_0209f498[gActivePlayerSlot].touching != 0) {
            int m = (b < 0) ? -b : b;
            s16 s2;
            if (m < 0x100)
                b = 0;
            s2 = data_0209f498[gActivePlayerSlot].mag;
            if (s2 < 0x400)
                mAngleYSpeed = FX_MUL(b, 0x64);
            else if (s2 < 0x800)
                mAngleYSpeed = FX_MUL(b, 0xC8);
            else if (s2 < 0xC00)
                mAngleYSpeed = FX_MUL(b, 0x190);
            else
                mAngleYSpeed = FX_MUL(b, 0x300);
        } else {
            mAngleYSpeed = FX_MUL(b, 0x220);
        }
    } else {
        mAngleYSpeed = (int)((((long long)b << 8) + 0x800) >> 12);
    }

    f.v0.x = FX_MUL(v, (int)FX_SinCosTable_[((u16)mFloorAngle >> 4) * 2]);
    f.v0.z = FX_MUL(v, (int)FX_SinCosTable_[((u16)mFloorAngle >> 4) * 2 + 1]);
    {
        int len = Vec3_HorzLen((const Vector3 *)&mFloorNormalX);
        f.v0.x = FX_MUL(f.v0.x, len);
        f.v0.z = FX_MUL(f.v0.z, len);
    }

    if ((this->mClsnFlags & 2) != 0 && (u16)(this->mStateFlags & 1) == 0 && this->mHorzSpeed <= 0x10000) {
        int r6b = cstd::atan2(f.v0.x, f.v0.z);
        dBgCh_Lin ray;
        {
            int tz, ty, tx;
            int p5c = this->mPosX;
            int p64 = this->mPosZ;
            int j = ((u16)r6b >> 4) * 2;
            int p60 = this->mPosY;
            VecFx32 t;
            t.x = p5c; t.y = p60 + 0x32000; t.z = p64;
            f.v1.x = t.x; f.v1.y = t.y; f.v1.z = t.z;
            tx = FX_SinCosTable_[j] * 0x46 + this->mPosX;
            tz = FX_SinCosTable_[j + 1] * 0x46 + this->mPosZ;
            ty = this->mPosY + 0x32000;
            f.v2.x = tx; f.v2.y = ty; f.v2.z = tz;
        }
        ray.SetObjAndLine(*(const Vector3 *)&f.v1, *(const Vector3 *)&f.v2, this);
        if (ray.DetectClsn() != 0) {
            f.v0.x = 0;
            f.v0.z = 0;
        }
    }

    {
        int s1 = r5v + f.v0.x;
        int s2 = r4v + f.v0.z;
        int fx = FX_MUL(s1, t);
        int fz = FX_MUL(s2, t);
        f.v3.x = fx;
        f.v3.z = fz;
        ang = cstd::atan2(fx, fz);
    }
    this->mHorzSpeed = Vec3_HorzLen((const Vector3 *)&f.v3);
    if (this->mHorzSpeed > 0x64000)
        this->mHorzSpeed = 0x64000;
    this->mPrevAngleY = (s16)(ang + this->mAngleYSpeed);
    {
        s16 cur = this->mPrevAngleY;
        if (AngleDiff(cur, this->mAngleY) >= 0x4000)
            cur = (s16)(this->mPrevAngleY + 0x8000);
        ApproachAngle(&this->mAngleY, cur, 8, 0x4000, 0x10);
    }
    {
        int s = this->mHorzSpeed;
        if (s < 0)
            s = -s;
        if (s < arg) {
            if (func_ov002_020f035c(this->func_ov002_020c031c(), this->mFloorNormalY) == 0) {
                this->mHorzSpeed = 0;
                return 0;
            }
            if (this->mSlideType != 0)
                this->mHorzSpeed = arg;
        }
    }
    return 1;
}

int daPly_c::func_ov002_020c0cbc() {
    int t;
    int d;
    if ((mClsnFlags & 1) == 0)
        return 0;
    d = mGroundY - this->mPosY;
    if (d < 0)
        d = -d;
    if (d >= 0x50000)
        return 0;
    switch (func_ov002_020c031c()) {
    case 4: t = 0xfc1; break;
    case 5: t = 0xfff; break;
    case 3: t = 0xf04; break;
    default:
    case 0: t = 0xc9b; break;
    case 1:
    case 2: t = 0; break;
    }
  if(mFloorNormalY <= t){
    (this->mStateFlags) |= 1;
    return 1;
  }
  return 0;
}

int daPly_c::func_ov002_020c0d90(int y) {
    int flag = (this->mIsInShallowWater != 0);

    this->mIsInShallowWater = 0;
    if (y == (int)0x80000000) return 0;

    if (this->mIsUnderwater != 0) flag = 1;
    if (y >= this->mPosY + 0xa000) {
        this->mIsInShallowWater = 1;
    }

    if (flag != (this->mIsInShallowWater != 0)) {
        if (this->mIsAirborne != 0) {
            if (this->mIsMega == 0) {
                Particle::System::NewSplash(this->mPosX, y, this->mPosZ);
            } else {
                this->mParticle1 = Particle::System::NewCheckWaterRipple(
                    this->mParticle1, kPtclMegaSplash, this->mPosX, data_0209f32c, this->mPosZ, 0);
            }
        }
    }

    if (((y - 0x64000) >> 12) < (this->mPosY >> 12)) return 0;
    if (this->mVertSpeed > 0) return 0;

    if (this->mIsBalloon != 0 || (this->IsState(data_ov002_021102d4) && this->mStateStep < 2) || this->mIsUnderwater != 0 || this->IsState(data_ov002_021106ac) || this->IsState(data_ov002_021106c4) || this->IsState(data_ov002_021102a4)) {
        return 0;
    }

    if (this->func_ov002_020c7f10()) return 1;

    this->mWalkTimer = 0xf;
    if (this->mIsMetal != 0 || this->mIsMega != 0) {
        this->ChangeState(data_ov002_021106ac);
    } else {
        this->ChangeState(data_ov002_0211067c);
        this->mJumpComboTimer = 0x14;
    }

    func_ov002_020ce9c8();
    func_ov002_020be008();
    return 1;
}

int daPly_c::func_ov002_020c0fb4() {
    int r;
    switch (this->mSurfaceType) {
    case 4:

    case 5:
        if ((r = this->func_ov002_020c5d0c()) != 0) {
            return r;
        }
        break;

    case 19:
        if ((r = func_ov002_020dec70()) != 0) {
            return r;
        }
        if ((r = this->func_ov002_020c5d0c()) != 0) {
            return r;
        }
        break;

    case 18:
        if ((r = this->func_ov002_020bf40c()) != 0) {
            return r;
        }
        break;

    case 15:
        if (gShowRaceTimer[0] != 0) {
            break;
        }
        gRaceTimer.ResetTimer();
        gShowRaceTimer[0] = 1;
        gRaceTimer.StartTimer();
        break;

    case 16:
        if (gRaceTimer.mIsRunning == 0) {
            break;
        }
        gRaceTimer.StopTimer();
        {
            unsigned long long t = gRaceTimer.GetTime();
            if (((t << 6) / 0x1ff6210) > 0x14) {
                break;
            }
        }
        {
            Vector3 pos(mPosX, mPosY + 0x82000, mPosZ);
            Spawn(0xb2, 0x42, pos, 0, this->mAreaId, -1);
        }
        break;
    }

    if (gCurrentSublevel == 0x26) {
        if ((this->mPosY) < (-0x3e8000)) {
            this->mPosY = -0x3e8000;
            if ((r = this->func_ov002_020d5338()) != 0) {
                return r;
            }
        }
    }
    if (this->mMeshClsn.IsOnGround()) {
        if (this->mProbedSurfaceType == 1) {
            int d = (this->mPosY) - (this->mProbedGroundY);
            if (d < 0) {
                d = -d;
            }
            if (d < 0x2000) {
                if ((r = this->func_ov002_020d5338()) != 0) {
                    return r;
                }
            }
        }
        if ((r = this->func_ov002_020bfec0()) != 0) {
            return r;
        }
    }
    if ((r = this->func_ov002_020c0d90(this->mWaterSurfaceY)) != 0) {
        return r;
    }
    if ((r = func_ov002_020d06c0()) != 0) {
        return r;
    }
    return this->func_ov002_020c0cbc();
}

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
void daPly_c::func_ov002_020c1234() {
    unsigned int a1, a2;
    int fx;
    if (gCurrentSublevel != 0xc && gCurrentSublevel != 0x2e)
        return;
    switch (mGroundSoundType) {
    case 0xa:
        a1 = 0x2d;
        a2 = 0x28;
        fx = (gSubVolume >> 12 > 0x28) ? 0x8b33 : 0xd1e;
        mGroundSubSoundActive = 1;
        break;
    case 0xb:
        mGroundSubSoundActive = 1;
        a1 = 0;
        a2 = 0x7f;
        fx = 0x8b33;
        break;
    default:
        fx = 0x1451;
        a1 = 0x7f;
        a2 = 0;
        break;
    }
    if (mGroundSubSoundActive == 0)
        return;
    if (Sound::PlaySub(0x35, a1, a2, fx, 0) == 0)
        return;
    if (mGroundSoundType == 0xa) return;
    if (mGroundSoundType != 0xb)
        mGroundSubSoundActive = 0;
}

void daPly_c::func_ov002_020c133c() {
    typedef unsigned char u8;
    if (mIsMetal != 0) return;
    {
        int lim = mToxicSurfaceY;
        if (lim != (int)0x80000000 && this->mPosY < lim - 0x64000) {
            if (!func_ov002_020d82f0())
                return;
            if (mIsAirborne != 0)
                return;
            if (this->GetHealth() <= 2)
                mSurfaceSoundHandle =
                    Sound::PlayLong(mSurfaceSoundHandle, 2, 0x27, mCamSpacePos, 0);
            if (mDamageTickTimer != 0) {
                mDamageTickTimer--;
                return;
            }
            {
                Vector3 v;
                v.x = this->mPosX;
                v.y = this->mPosY;
                v.z = this->mPosZ;
                if (!this->Hurt(v, 0, 0, 1, 0, 0))
                    return;
            }
            Sound::PlayCharVoice(mCharacter, 6, mCamSpacePos);
            if (func_ov002_020d91e0(0x100, 0) != 0 && this->GetHealth() == 0)
                func_ov002_020c5dec(5);
            mDamageTickTimer = 0x40;
            return;
        }
    }
    mDamageTickTimer = 0;
}

void daPly_c::func_ov002_020c14b8() {
    void *arg0 = this;
    daPly_c *c = (daPly_c *)arg0;
    dBgCh_Gnd rg;
    Vector3 pos;
    int savedY;

    savedY = 0x80000000;
    if (c->Unk_020c9e5c(7) || c->Unk_020c9e5c(0xa) || c->Unk_020c9e5c(0xb) || c->Unk_020c9e5c(0xe))
        savedY = mPosY;
    mProbedGroundY = 0x80000000;
    mGroundY = mProbedGroundY;
    mWaterSurfaceY = 0x80000000;
    {
        int t = mPosY + 0xa0000;
        if (t >= mToxicSurfaceY)
            mToxicSurfaceY = 0x80000000;
    }
    mProbedSurfaceType = 0;
    mSurfaceType = mProbedSurfaceType;
    mFloorClass = 0;
    {
        int z = mPosZ;
        int y = mPosY + 0xa0000;
        int x = mPosX;
        pos.x = x;
        pos.y = y;
        pos.z = z;
    }
    rg.SetObjAndPos(pos, this);
    if (rg.DetectClsn()) {
        {
            dBgPi floor(rg);
            SetFloorSurfaceInfo(floor);
        }
        mProbedGroundY = rg.clsnY;
        mGroundY = mProbedGroundY;
        mProbedSurfaceType = mSurfaceType;
    }

    pos.y = mPosY + 0x12c000;
    rg.StartDetectingWater();
    rg.StartDetectingToxic();
    rg.StopDetectingOrdinary();
    rg.SetObjAndPos(pos, this);
    if (rg.DetectClsn()) {
        if (SurfaceInfo_TestFlag0x20((int *)&rg.surface) != 0) {
            mWaterSurfaceY = rg.clsnY;
            data_0209f32c = mWaterSurfaceY;
        }
        if (func_02037e20((int *)&rg.surface) != 0) {
            mToxicSurfaceY = rg.clsnY;
        }
    }

    mStateFlags &= ~1;
    mStateFlags &= ~0x10;
    c->func_ov002_020c133c();

    if (savedY != (int)0x80000000)
        mGroundY = savedY;
}

void daPly_c::SetFloorSurfaceInfo(const dBgPi &hit) {
    Vector3 v;
    mFloorPathID = hit.surface.clps.GetPathID();
    mSurfaceType = hit.surface.clps.GetSurfaceType();
    mFloorCameraBehavior = hit.surface.clps.GetCameraBehavior();
    mFloorViewID = hit.surface.clps.GetViewID();
    mGroundSoundType = hit.surface.clps.GetSoundType();
    if (mFloorClass == 1)
        return;
    mFloorClass = hit.surface.clps.GetFloorClass();
    hit.surface.CopyNormalTo(v);
    mFloorNormalX = v.x;
    mFloorNormalY = v.y;
    mFloorNormalZ = v.z;
    mFloorAngle = cstd::atan2(mFloorNormalX, mFloorNormalZ);
}

/* recovered: shared common types */
// func_ov002_020c179c at 0x020c179c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
int daPly_c::func_ov002_020c179c(int angle) {
    Vector3 v1;
    Vector3 v2;
    int idx = (unsigned short)(short)(mPrevAngleY + angle);
    int y1 = mPosY;
    int y2 = y1;
    idx >>= 4;
    short dx = FX_SinCosTable_[idx * 2];
    short dy = FX_SinCosTable_[idx * 2 + 1];
    int ox = dx * 5;
    int oy = dy * 5;
    dBgCh_Gnd rg;
    {
        int z = mPosZ + oy;
        int y = mPosY + 0x64000;
        int x = mPosX + ox;
        v1.x = x;
        v1.y = y;
        v1.z = z;
    }
    rg.SetObjAndPos(v1, this);
    if (rg.DetectClsn()) y1 = rg.clsnY;
    {
        int z = mPosZ - oy;
        int x = mPosX - ox;
        int y = mPosY + 0x64000;
        v2.x = x;
        v2.y = y;
        v2.z = z;
    }
    rg.SetObjAndPos(v2, this);
    if (rg.DetectClsn()) y2 = rg.clsnY;
    {
        int r = cstd::atan2(y1 - y2, 0xa000);
        return r;
    }
}

void daPly_c::func_ov002_020c18b0(int p1) {
    if (mIsAirborne != 0)
        return;
    if (mFloorNormalY == 0)
        return;
    {
        int a = func_ov002_020c179c(0);
        int idx = (unsigned short)a >> 4;
        int sinv = FX_SinCosTable_[idx * 2 + 1];
        int f554 = mFloorNormalX;
        int f55c = mFloorNormalZ;
        Vector3 v;
        v.x = f554;
        v.y = sinv;
        v.z = f55c;
        mAngleX = (short)func_02010844(&v, mAngleY);
        mAngleZ = (short)func_02010844(&v, (short)(mAngleY - 0x4000));
    }
    if (p1 == 0) return;
    {
        int r0v = ScaleByCharFactor(0x28000);
        int divr = (mHorzSpeed << 12) / r0v;
        mAngleX = (short)FX_MUL(mAngleX, divr);
        mAngleZ = (short)FX_MUL(mAngleZ, divr);
    }
}

int daPly_c::func_ov002_020c19d0(int arg1, int arg2) {
    char* self = (char*)this;
    int angle = (unsigned short)mAngleY;
    dBgCh_Lin line;
    int k = angle >> 4;
    int cs = FX_SinCosTable_[k * 2];
    int sn = FX_SinCosTable_[k * 2 + 1];
    Vector3 v1(this->mPosX, this->mPosY + (arg2 << 12), this->mPosZ);
    Vector3 v2(arg1 * cs + this->mPosX, this->mPosY + (arg2 << 12), arg1 * sn + this->mPosZ);
    Vector3 nrm;
    line.SetObjAndLine(v1, v2, (dActor_c *)self);
    if (!line.DetectClsn()) {
        return 0;
    }
    line.surface.CopyNormalTo(nrm);
    if (nrm.y != 0) {
        return 0;
    }
    return 1;
}

/* daPly_c::func_ov002_020c1ad8(int) at 0x020c1ad8 (ov002)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */

int daPly_c::func_ov002_020c1ad8(int ang) {
    extern ControllerInput data_0209f498[];
    int d;
    int a = ang;

    if (this->mIsAirborne != 0) return 0;
    if (this->func_ov002_020c0434() != 0) {
        this->func_ov002_020c0364(3);
        return 0;
    }
    if (!this->IsState(data_ov002_0211013c)) {
        if (!this->IsState(data_ov002_0211043c)) return 0;
    }
    if (this->func_ov002_020c19d0(0x50, 0x32) == 0) return 0;

    a = (int)(a + 0x8000);
    d = AngleDiff(this->mDesiredAngleY, (s16)a);
    if (d < 0x4000) {
        if (d < 0x1555) {
            if (this->IsState(data_ov002_0211043c)) {
                if (this->mStateStep == 2) return 0;
            }
            if (data_0209f498[gActivePlayerSlot].mag == 0)
                return 0;
            this->mStateStep = 2;
        } else {
            if (this->IsState(data_ov002_0211043c)) {
                if (this->mStateStep != 2) return 0;
            }
            this->mStateStep = 0;
            if ((s16)a == (s16)(this->mAngleY + d)) {
                this->mStateStep = 1;
            }
        }
        mAngleY = a;
        this->ChangeState(data_ov002_0211043c);
        return 1;
    }
    return 0;
}

int daPly_c::func_ov002_020c1c84(int arg) {
    char *self = (char *)this;
    int a;
    int p;

    if ((u16)(this->mStateFlags & 0x200) || !this->mIsAirborne ||
        func_ov002_020cf700((daPly_c *)self) || this->mIsMega || func_ov002_020e0478() ||
        this->mEatingPlayer || this->mIsUnderwater)
        return 0;

    if (AngleDiff(arg, this->mPrevAngleY) < 0x6000)
        return 0;

    if (this->IsState(data_ov002_02110694) || mIsTakingDamage || this->mIsNoControl)
        return 0;

    if (this->mHorzSpeed < 0x3000)
        return 0;

    if (this->mVertSpeed > 0)
        return 0;

    a = arg + 0x8000;
    p = this->mPrevAngleY;
    this->mAngleYSpeed = (s16)(p - (s16)(p - a) * 2);
    this->mAngleY = (s16)a;
    this->mPrevAngleY = this->mAngleY;

    if (param1 == 0) {
        if (this->func_ov002_020c19d0(0x3c, 0x78))
            this->ChangeState(data_ov002_021103ac);
    } else {
        if (this->func_ov002_020c19d0(0x64, 0x96)) {
            this->mHorzSpeed = -0x12000;
            this->ChangeState(data_ov002_021101b4);
        } else {
            this->mHorzSpeed = 0;
        }
    }
    return 1;
}

extern "C" {

int func_ov002_020c1e44(void* a, int x) {
    daPly_c *p = (daPly_c *)a;
    p->mPrevAngleY = (short)((x << 1) - p->mPrevAngleY + 0x8000);
    p->mHorzSpeed = FX_MUL(p->mHorzSpeed, 0xe00);
    if (p->mHorzSpeed < 0x4000)
        p->mHorzSpeed = 0x4000;
    return 1;
}
}

/* recovered: shared common types */

void daPly_c::func_ov002_020c1eb4(int dmg) {
    typedef int Fx12;
    Vector3 out;
    Vector3 offset;
    Vector3_16 vec;

    Sound::PlayBank0(0x13, mCamSpacePos);
    mAngleY = mPrevAngleY;
    mAngleZ = 0;
    mAngleX = mAngleZ;
    mStateStep = 0;
    if (mIsAirborne) {
        mStateStep += 2;
    }
    mAngleY = (short)(dmg * 2 - mAngleY);
    mPrevAngleY = (short)(mAngleY + 0x8000);
    mHorzSpeed = 0xa000;
    mStateStep |= 0x20;
    mHurtDamage = 0;
    this->ChangeState(data_ov002_02110094);

    offset.x = 0;
    offset.y = 0x32000;
    offset.z = 0x28000;
    Vec3_RotateYAndTranslate(&out, &mPos, (short)(dmg + 0x8000), &offset);

    {
        int idx = ((unsigned short)dmg >> 4);
        vec.x = FX_SinCosTable_[idx * 2];
        vec.z = FX_SinCosTable_[idx * 2 + 1];
        vec.y = 0;
        Particle::System::New(0, kPtclWallBonk, out.x, out.y, out.z, (const Vector3_16f *)(&vec),
                              0);
    }
    func_ov002_020d91e0(0, 1);
}

int daPly_c::func_ov002_020c200c(int arg) {
    char* self = (char*)this;
    if (this->mSlideType == 1)
        return func_ov002_020c1e44(self, arg);

    if (this->mIsNoControl)
        return 0;

    if (this->IsState(data_ov002_021105bc) || this->IsState(data_ov002_0211031c) ||
        this->IsState(data_ov002_02110334) || this->IsState(data_ov002_0211055c) ||
        this->IsBeingShotOutOfCannon() || this->IsState(data_ov002_02110214)) {
        if (this->mHorzSpeed <= 0x10000)
            return 0;

        if (this->IsBeingShotOutOfCannon())
            func_ov002_020eedc0(&mMeshClsn, (dActor_c *)self);
    } else {
        return 0;
    }

    this->func_ov002_020c1eb4(arg);
    return 1;
}

// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
int daPly_c::func_ov002_020c2138() {
    char* self = (char*)this;
    typedef int Fix12i;
int ang;
if (this->mIsUnderwater || this->mIsBalloon) return 0;
if (this->mWallNormalY) return 0;
if (mClimbClsn || mIsTakingDamage)
    return 0;
ang = cstd::atan2(this->mWallNormalX, this->mWallNormalZ);
if (AngleDiff(ang, this->mPrevAngleY) < 0x6000) return 0;
if (this->func_ov002_020c200c(ang)) return 1;
if ((this->mHeldObj ? 1 : 0) || (this->mRidingShell ? 1 : 0)) return 0;
if (this->func_ov002_020c1c84(ang)) return 1;
return this->func_ov002_020c1ad8(ang);
}

/* recovered: shared common types */
int daPly_c::func_ov002_020c2270(void *p1, void *p2) {
    dBgCh_Lin line;
    line.SetObjAndLine(*(Vector3 *)p1, *(Vector3 *)p2, this);
    if (line.DetectClsn()) {
        if (line.surface.clps.GetFloorClass() == 1) {
            ::Vector3 clsnPos = line.GetClsnPos();
            mPosX = clsnPos.x;
            mPosY = clsnPos.y;
            mPosZ = clsnPos.z;
            mSnapToFloor = 1;
            return 1;
        }
    }
    return 0;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
int daPly_c::func_ov002_020c231c() {
    Vector3 tmp;
    Vector3 pos;
    u8 f70f;
    s32 p60;
    s32 d;
    s32 p644;
    s32 xx;
    s32 zz;
    int idx;

    f70f = mSnapToFloor;

    if (mGroundY == (s32)0x80000000) {
        if (f70f == 0)
            return 0;

        VEC_Subtract(&mPos, (Vector3 *)&mPreClsnPosX, &tmp);
        tmp.x >>= 1;
        tmp.y >>= 1;
        tmp.z >>= 1;
        VEC_Add((Vector3 *)&mPreClsnPosX, &tmp, &mPos);
        func_ov002_020c14b8();

        if (mGroundY == (s32)0x80000000)
            return 0;
    }

    if (mWaterSurfaceY != (s32)0x80000000)
        return 0;

    if (this->IsState(data_ov002_0211013c) || this->IsState(data_ov002_02110184) || this->IsState(data_ov002_02110304) || (this->IsState(data_ov002_02110364) && (this->IsAnim(0x33) | this->IsAnim(0x87)) == 0) || this->IsState(data_ov002_021104e4) || this->IsState(data_ov002_02110514) || this->IsState(data_ov002_021100dc))
    {
        if (mFloorClass == 1 && mFloorNormalY < 0xfff) {
            p644 = mGroundY;
            p60 = this->mPosY;
            d = p60 - p644;
            if (d < 0) d = -d;
            if (d < 0xc8000) {
                this->mPosY = p644;
                mSnapToFloor = 1;
                return 1;
            }

            if (f70f == 0) {
                idx = (u16)(s16)(this->mPrevAngleY + 0x8000) >> 4;
                zz = FX_SinCosTable_[idx * 2 + 1] * 0x50 + this->mPosZ;
                xx = FX_SinCosTable_[idx * 2] * 0x50 + this->mPosX;
                pos.x = xx;
                pos.y = p60;
                pos.z = zz;
                if (func_ov002_020c2270(&mPos, &pos))
                    return 1;
            }
        }

        if (f70f == 1) {
            d = this->mPosY - mGroundY;
            if (d < 0) d = -d;
            if (d < 0xc8000)
                this->mPosY = mGroundY;
        }
    }

    mSnapToFloor = 0;
    return 0;
}

int daPly_c::func_ov002_020c25a8(int arg1) {
    void *arg0 = this;
    daPly_c *c = (daPly_c *)arg0;
    int ret;
    Vector3 v;
    Vector3 wn;
    dBgPi *fr;

    func_02037318(&mMeshClsn);
    mMeshClsn.StopDetectingWater();

    {
        int b1;
        b1 = (mRidingShell != 0);
        if (b1 || mDetectWater || mIsBalloon)
            mMeshClsn.StartDetectingWater();
    }

    mMeshClsn.StopIgnoringVanishSurfaces();

    if (mIsVanish)
        mMeshClsn.StartIgnoringVanishSurfaces();

    mMeshClsn.ClearFlag200();

    if (mIsMega)
        mMeshClsn.SetFlag200();

    mMeshClsn.StopIgnoringType14Walls();

    if (c->IsState(data_ov002_02110514))
        mMeshClsn.StartIgnoringType14Walls();

    mMeshClsn.ClearFlag400();

    {
        int b2;
        b2 = (mRidingShell != 0);
        if (b2)
            mMeshClsn.SetFlag400();
    }

    mMeshClsn.ClearLimMovFlag();

    if (c->IsState(data_ov002_021100f4) && mVertSpeed > 0)
        mMeshClsn.SetLimMovFlag();

    mMeshClsn.ClearFlag800();

    if (c->IsState(data_ov002_021102ec))
        mMeshClsn.SetFlag800();

    func_020383e4(&mMeshClsn);

    if (mIsMega)
        func_ov002_020eed24(&mMeshClsn, c);

    if (mVertSpeed > 0x1e000)
        mMeshClsn.ClearGroundFlag();

    if (mMeshClsn.IsOnGround())
        mClsnFlags |= 1;

    if (mMeshClsn.IsOnWall())
        mClsnFlags |= 2;

    if (func_02035638((unsigned char *)&mMeshClsn))
        mClsnFlags |= 4;

    if (mIsUnderwater == 0)
        func_020371fc((char *)&mMeshClsn);

    if (mClsnFlags & 1) {
        fr = mMeshClsn.GetFloorResult();
        {
            dBgPi floor(*fr);
            SetFloorSurfaceInfo(floor);
        }

        mGroundY = mPosY;
        ret = fr->surface.clps.GetFloorClass();

        if (mIsUnderwater == 0 && ret != 1) {
            fr->surface.CopyNormalTo(v);
            if (v.y != 0) {
                int t = FX_Div(FX_MUL(v.x, mSpeedX) + FX_MUL(v.z, mSpeedZ), v.y);
                t = -(t + 0x8000);
                if (t < 0)
                    mVertSpeed = t;
            }
        }
        mIsAirborne = 0;
        mIsInAirState = 0;
    } else {
        if (mMeshClsn.TestLeftGroundFlag())
            mPeakY = mPosY;
        mIsAirborne = 1;
        mLandSoundPlayed = 0;
        if (func_02035638((unsigned char *)&mMeshClsn) && mVertSpeed > 0)
            mVertSpeed = -0x1000;
        func_ov002_020cef84();
    }

    if (arg1 != 0) {
        mClsnFlags |= 1;
        mIsAirborne = 0;
        mIsInAirState = 0;
    }

    {
        int wallFlag = mClsnFlags & 2;
        if (wallFlag == 0)
            return wallFlag;
    }

    mMeshClsn.GetWallResult()->surface.CopyNormalTo(wn);
    mWallNormalX = wn.x;
    mWallNormalY = wn.y;
    mWallNormalZ = wn.z;

    if (mIsUnderwater == 0) {
        mPosX -= mWallNormalX * 2;
        mPosZ -= mWallNormalZ * 2;
    }

    ret = func_ov002_020d0580();
    if (ret != 0)
        return ret;

    ret = c->func_ov002_020c2138();
    if (ret != 0)
        return ret;
}

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
void daPly_c::func_ov002_020c29d4() {
    char *base = (char *)this;
    if (param1 != 1)
        return;
    if (this->mIsMega != 0) return;
    unsigned char flags = mQuicksandRunFlags;
    if ((flags & 1) && !(flags & 0x80)) {
        if (this->mHorzSpeed >= ScaleByCharFactor(0x28000)) {
            if (this->IsState(data_ov002_0211013c)) {
                mQuicksandRunTimer = 0x1e;
            } else if (this->IsState(data_ov002_021101b4) && this->mVertSpeed < 0 && (this->mPeakY - this->mPosY) < 0x64000) {
                mQuicksandRunTimer = 0x1e;
            }
        }
        mQuicksandRunFlags = 0x80;
    } else if (flags & 0x80) {
        if (mMeshClsn.IsOnGround()) {
            int r = mMeshClsn.GetFloorResult()->surface.clps.GetSurfaceType();
            if (r >= 6 && r <= 9) return;
            mQuicksandRunFlags = 0;
            mQuicksandRunTimer = 0;
        } else {
            mQuicksandRunFlags = 0;
            mQuicksandRunTimer = 0;
        }
    }
}

void daPly_c::func_ov002_020c2b08() {
    void *arg0 = this;
    daPly_c *c = (daPly_c *)arg0;
    int v[3];

    if (param1 != 1)
        return;
    if (mIsMega != 0)
        return;

    if (mWaterRunLatch == 0) {
        int limit;

        if (!mMeshClsn.IsOnGround())
            return;
        if (!SurfaceInfo_TestFlag0x20((int *)&mMeshClsn.GetFloorResult()->surface.clps))
            return;

        limit = c->ScaleByCharFactor(0x20000);
        if (mHorzSpeed >= limit)
            mWaterRunTimer = 0x3c;
        else
            mWaterRunTimer = 0x1e;
        mWaterRunLatch = 1;
        return;
    }

    if (mWaterRunTimer != 0) {
        // combining the ground tests with || reorders 73 words from +0xd4
        if (!mMeshClsn.IsOnGround())
            goto clear_timer;
        if (!SurfaceInfo_TestFlag0x20((int *)&mMeshClsn.GetFloorResult()->surface.clps))
            goto clear_timer;

        mIsInShallowWater = 1;

        if (mWaterRunTimer != 0) {
            if (mHorzSpeed >= 0x1000)
                goto big_block;
        }

        mWaterRunTimer = 0;
        mPosY += mModelOffsetY;
        if (mPosY < mGroundY)
            mPosY = mGroundY;
        return;

    big_block: {
        int limit2 = c->ScaleByCharFactor(0x28000);

        if (mHorzSpeed < limit2) {
            if (mWaterRunTimer >= 0x1e)
                mWaterRunTimer = 0x1e;
        }

        mFloorNormalZ = 0;
        {
            int t = mFloorNormalZ;
            mFloorNormalX = t;
            mFloorNormalY = 0x1000;
        }

        {
            int yy = mPosY;
            int zz = mPosZ;
            int xx = mPosX;
            int yy2 = yy + 0x64000;
            v[0] = xx;
            v[1] = yy2;
            v[2] = zz;
        }

        {
            dBgCh_Gnd rg;
            rg.SetObjAndPos(*(const Vector3 *)v, c);

            if (rg.DetectClsn() != 0) {
                mGroundY = rg.clsnY;
                if (mPosY + mModelOffsetY < rg.clsnY)
                    mWaterRunTimer = 0;
            }
        }
        return;
    }

    clear_timer:
        mWaterRunTimer = 0;
        return;
    }

    mDetectWater = 0;
    if (!mMeshClsn.IsOnGround())
        return;
    if (SurfaceInfo_TestFlag0x20((int *)&mMeshClsn.GetFloorResult()->surface.clps) != 0)
        return;
    if (mIsInShallowWater != 0)
        return;

    if (c->IsState(data_ov002_0211013c)) {
        mWaterRunLatch = 0;
        mDetectWater = 1;
    }
    return;
}

void daPly_c::func_ov002_020c2db8() {
    unsigned char *c = (unsigned char *)this;
    ((daPly_c *)(c))->func_ov002_020c14b8();
    mClsnFlags = 0;
    if (mSkipGroundUpdate != 0)
        return;
    mSlideType = 0;
    if (mFloorClass == 5)
        mSlideType = 1;
    if (((daPly_c *)(c))->func_ov002_020c231c() == 0) {
        func_02035798(&mMeshClsn, 0x1000);
        ((daPly_c *)(c))->func_ov002_020c25a8(0);
    } else {
        func_02035798(&mMeshClsn, 0);
        ((daPly_c *)(c))->func_ov002_020c25a8(1);
    }
    ((daPly_c *)(c))->func_ov002_020c1234();
    ((daPly_c *)(c))->func_ov002_020c0fb4();
    ((daPly_c *)(c))->func_ov002_020c5d60();
    ((daPly_c *)(c))->func_ov002_020c61ac();
    func_ov002_020eea84(&mMeshClsn, (dActor_c *)c);
    ((daPly_c *)(c))->func_ov002_020c2b08();
    ((daPly_c *)(c))->func_ov002_020c29d4();
}

void daPly_c::func_ov002_020c2e78() {
    unsigned char w = IsPlayerWarping(mPlayerNo);
    if (w == 0)
        return;
    if (!this->IsState(daPly_c::ST_WAIT)) {
        func_02020388(mPlayerNo);
        return;
    }
  mTeleportId = w;
  unsigned short *p = &this->mStateFlags;
  *p |= 8;
}

/* recovered: named members + shared header, real C++ method
 *
 * Warping is allowed from exactly two states. One of them is now spelt as the
 * static member it is -- daPly_c::ST_WAIT, which the compiler mangles for us --
 * instead of a hand-written extern. The other is
 * still reached by its ov002 address; it has no recovered name yet.
 *
 * The file's own `struct State;` stand-in is gone with it: IsState really
 * takes a daPly_c::State&, which is what its mangling says, and the local
 * forward declaration only ever existed to give the extern something to point
 * at.
 */
extern "C" {

extern daPly_c::State data_ov002_021102a4;
}

int daPly_c::CanWarp() {

    if (this->IsState(ST_WAIT) || this->IsState(data_ov002_021102a4))
        return 1;
    return 0;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::IsInAir() {
    if (mIsAirborne)
        return mIsInAirState != 0;
    return 0;
}

void daPly_c::func_ov002_020c2f64() {
    if (this->mLandSoundPlayed)
        return;
    this->mLandSoundPlayed = 1;
    if (this->mIsMetal) {
        Sound::PlayBank0(0xa1, mCamSpacePos);
        return;
    }
    if (this->mIsMega) {
        Sound::PlayBank0(0xd2, mCamSpacePos);
        return;
    }
    Sound::PlayBank0(this->mGroundSoundType + 0x40, mCamSpacePos);
}

/* The states of Super Mario 64's spline animation. The ends of a path
   take clamped variants of the B-spline basis. */
#define CURVE_BEGIN_1 1
#define CURVE_BEGIN_2 2
#define CURVE_MIDDLE 3
#define CURVE_END_1 4
#define CURVE_END_2 5

/* SM64's anim_spline_poll: blend the current four keyframes into out,
   advance, and return 1 once the path has run out. */
int daPly_c::AnimSplinePoll(int *out) {
    Fix12i tmp[4];
    int i;
    int ret;
    EndingFlyPoint *row;

    ret = 0;
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    SplineGetWeights(tmp);

    row = &gJumboStarKeyframes[mSplineKeyframe];
    for (i = 0; i < 4; i++) {
        out[0] += tmp[i] * row[i].x;
        out[1] += tmp[i] * row[i].y;
        out[2] += tmp[i] * row[i].z;
    }

    mSplineKeyframeFraction += (row[0].speed << 12) / 1000;
    if (mSplineKeyframeFraction >= 0x1000) {
        mSplineKeyframe++;
        mSplineKeyframeFraction -= 0x1000;
        switch (mSplineState) {
        case CURVE_END_2:
            ret = 1;
            break;
        case CURVE_MIDDLE:
            if (gJumboStarKeyframes[mSplineKeyframe + 2].speed == 0) {
                mSplineState = CURVE_END_1;
            }
            break;
        default:
            mSplineState++;
            break;
        }
    }
    return ret;
}

/* SM64's anim_spline_init. The keyframes are always gJumboStarKeyframes. */
void daPly_c::AnimSplineInit() {
    mSplineKeyframe = 0;
    mSplineKeyframeFraction = 0;
    mSplineState = CURVE_BEGIN_1;
}

extern "C" {

static inline int FX(int a, int b) {
    return FX_MUL((a), (b));
}
}

/* SM64's spline_get_weights: the uniform cubic B-spline basis at
   mSplineKeyframeFraction, four weights for the four keyframes
   AnimSplinePoll blends; mSplineState picks the clamped-end variant. The
   caller passes a Fix12i[4]. */
void daPly_c::SplineGetWeights(Fix12i *out) {
    typedef long long s64;
    int t = mSplineKeyframeFraction;
    int u = 0x1000 - t;
    int u2 = FX(u, u);
    int u3 = FX(u2, u);
    int t2 = FX(t, t);
    int t3 = FX(t2, t);

    switch (mSplineState) {
    case 0: break;
    case CURVE_BEGIN_1: {
        int a = FX(t, 0x3000);
        int b = FX(t3, 0x1c00);
        int c = FX(t2, 0x4800);
        out[0] = u3;
        out[1] = a + (b - c);
        out[2] = FX(t2, 0x1800) - FX(t3, 0xeaa);
        out[3] = FX(t3, 0x2aa);
        break;
    }
    case CURVE_BEGIN_2: {
        int k1400 = 0x1400;
        s64 p955 = (s64)t3 * 0x955;
        s64 p1400 = (s64)t2 * k1400;
        out[0] = FX(u3, 0x400);
        {
            int b = FX(t, 0x400);
            int c = (int)((p955 + 0x800) >> 12);
            int d = (int)((p1400 + 0x800) >> 12);
            out[1] = b + (c - d) + 0x955;
        }
        out[2] = FX(t, 0x800) + (FX(t2, 0x800) - FX(t3, 0x800)) + 0x2aa;
        out[3] = FX(t3, 0x2aa);
        break;
    }
    case CURVE_MIDDLE: {
        out[0] = FX(u3, 0x2aa);
        out[1] = FX(t3, 0x800) - t2 + 0xaaa;
        out[2] = FX(t, 0x800) + (FX(t2, 0x800) - FX(t3, 0x800)) + 0x2aa;
        out[3] = FX(t3, 0x2aa);
        break;
    }
    case CURVE_END_1: {
        out[0] = FX(u3, 0x2aa);
        out[1] = FX(u, 0x800) + (FX(u2, 0x800) - FX(u3, 0x800)) + 0x2aa;
        out[2] = FX(u, 0x400) + (FX(u3, 0x955) - FX(u2, 0x1400)) + 0x955;
        out[3] = FX(t3, 0x400);
        break;
    }
    case CURVE_END_2: {
        out[0] = FX(u3, 0x2aa);
        out[1] = FX(u2, 0x1800) - FX(u3, 0xeaa);
        out[2] = FX(u, 0x3000) + (FX(u3, 0x1c00) - FX(u2, 0x4800));
        out[3] = t3;
        break;
    }
    }
}

/* recovered: shared common types */

void daPly_c::func_ov002_020c37a4(char *arg) {
    unsigned int id = this->GetBodyModelID(param1 & 0xff, 0);
    Matrix4x3 *bone = &mModels.mBodyModels[id]->data.transforms[8];
    unsigned int id2 = this->GetBodyModelID(param1 & 0xff, 0);
    Vector3 v;
    Vector3 s;
    MTX_MultVec43((Vector3 *)&bone->m[9], &mModels.mBodyModels[id2]->mat4x3, &v);
    Vec3_MulScalar(&s, &v, 0x8000);
    v = s;
    Vec3_RotateYAndTranslate(&v, &v, this->mAngleY, (const Vector3 *)arg);
    mParticle1 = (s32)Particle::System::New(mParticle1, kPtclJumboStarFlyA, v.x, v.y, v.z, 0, 0);
    mParticle2 = (s32)Particle::System::New(mParticle2, kPtclJumboStarFlyB, v.x, v.y, v.z, 0, 0);
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

void daPly_c::JumboStarCutsceneFlying() {
    V3Quad s;
    int hlen;
    int ang;
    unsigned char *p;

    s.a.x = 0; s.a.y = 0; s.a.z = 0x60000;
    s.b.x = 0; s.b.y = 0; s.b.z = 0x60000;
    func_ov002_020c37a4((char *)&s.b);
    s.d.x = 0; s.d.y = 0; s.d.z = 0;
    switch (mStateWork) {
    case 0:
        this->SetAnim(0xc1, 0, 0x1000, 0);
        mRootMotionMode = 0;
        AnimSplineInit();
        mModelOffsetY = -0x20000;
        p = &this->mStateWork;
        *p = *p + 1;
        /* fallthrough */
    case 1:
        if (AnimSplinePoll((int *)&s.d) != 0) {
            p = &this->mStateWork;
            *p = *p + 1;
            return;
        }
        s.e.x = s.d.x - this->mPosX;
        s.e.y = s.d.y - this->mPosY;
        s.e.z = s.d.z - this->mPosZ;
        hlen = Vec3_HorzLen(&s.e);
        ang = cstd::atan2(s.e.x, s.e.z);
        this->mPosX = s.d.x;
        this->mPosY = s.d.y;
        this->mPosZ = s.d.z;
        if ((s.e.x | s.e.z) == 0) {
            this->mAngleX = 0;
            this->mAngleY = 0x7fff;
            return;
        }
        this->mAngleX = -cstd::atan2(s.e.y, hlen);
        ApproachLinear2(this->mAngleZ, 0x2000, 0x10);
        this->mAngleY = ang;
        break;
    case 2:
        break;
    }
}

#undef LA

int daPly_c::JumboStarCutsceneTakingOff() {
    unsigned int r5;
    int id;

    if (mStateWork == 0) {
        mAttachOffsetY = 0;
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x5a)) {
            Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
            Sound::PlayCharVoice(mCharacter, 0, mCamSpacePos);
            mStateWork += 1;
        }
    } else {
        int buf[6];
        buf[0] = 0;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
        buf[4] = 0;
        buf[5] = 0;
        func_ov002_020c37a4((char *)(buf + 3));
        dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
        r5 = (unsigned int)(anim.currFrame << 4) >> 0x10;
        if (r5 == 0x6d || r5 == 0x8d) {
            Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
        }
        switch (r5) {
        case 0x77:
            Sound::PlayCharVoice(mCharacter, 2, mCamSpacePos);
            break;
        case 0x97:
            Sound::PlayCharVoice(mCharacter, 4, mCamSpacePos);
            break;
        }
        mAttachOffsetY -= 0x21000;
        if (this->FinishedAnim()) {
            AdvanceCutsceneStep();
            return 1;
        }
    }

    this->mPosX = 0;
    this->mPosZ = mAttachOffsetY;
    AdvanceAnims();
    return 0;
}

void daPly_c::JumboStarCutsceneFalling() {
    typedef int Fix12i;
  if (mStateWork == 0){
    mHasWings = 1;
    mPowerupTimer = 0x708;
    this->mPrevAngleY = -0x8000;
    this->mAngleY = this->mPrevAngleY;
    this->mPosZ = 0;
    this->mPosX = this->mPosZ;
    this->mHorzSpeed = 0;
    this->mVertAccel = -0x2000;
    if (mIsAirborne == 0){
        this->SetAnim(0x55, 0x40000000, 0x1000, 0);
        this->mStateWork += 1;
    }
  } else {
    if (this->FinishedAnim()){
        KuppaScript::StartEndingCutscene();
        AdvanceCutsceneStep();
        this->SetAnim(0xc0, 0x40000000, 0x1000, 0);
        mRootMotionMode = 3;
        mSkipGroundUpdate = 1;
        mIsBodyClsnEnabled = 0;
        this->mVertSpeed = 0;
        this->mVertAccel = 0;
        this->mPosY = 0x15e000;
    }
  }
  AdvanceAnims();
}

void daPly_c::AdvanceCutsceneStep() {
    mStateStep = 0;
    mStateWork = mStateStep;
    (this->mStatePhase)++;
}

/* recovered: shared header, real C++ method
 *
 * ov002, 0x020c3d1c, size 0x50. The per-tick handler for the ending flight,
 * dispatching on a phase counter at 0x6e6 to one of three ov002 workers.
 *
 * THE FALLTHROUGH IS DELIBERATE. Phase 1 runs its worker and, when that
 * returns non-zero, falls into phase 2's -- so the frame that finishes phase 1
 * also starts phase 2 rather than idling. Written as a `case 1:` with no
 * `break` on the non-zero path, which is what the ROM's branch structure does.
 *
 * St_EndingFly is Super Mario 64's ACT_JUMBO_STAR_CUTSCENE: the three
 * workers are the N64 decomp's jumbo_star_cutscene_falling, _taking_off and
 * _flying, and AdvanceCutsceneStep is its advance_cutscene_step. Their
 * bodies match statement for statement, in fixed point.
 *
 * This file carried the placeholder name func_ov002_020c3d1c until the symbol
 * was moved here from ov007, where it had been sitting on an unrelated 0x50
 * function that ov007 calls from 46 places. See the commit and daPly_c.h.
 */
int daPly_c::St_EndingFly_Main() {
    switch (mStatePhase) {
    case 0:
        JumboStarCutsceneFalling();
        break;
    case 1:
        if (JumboStarCutsceneTakingOff() == 0)
            break;
    case 2:
        JumboStarCutsceneFlying();
        break;
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_EndingFly_Init() {
    DisableInteraction();
    mIsControlDisabled = 1;
    mStatePhase = 0;
    mStateWork = mStatePhase;
    this->SetAnim(0x54, 0x40000000, 0x1000, 0);
    return 1;
}

extern "C" {

void func_ov002_020c3dbc(daPly_c *player) {
    player->ChangeState(data_ov002_0211058c);
}
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_OpeningWakeUp_Main() {
    switch (mStateStep) {
    case 0:
        if (this->FinishedAnim()) {
            this->SetAnim(0xb4, 0, 0x1000, 0);
            *(u8*)(int)((char*)&mStateStep) += 1;
        }
        break;
    case 1:
        break;
    case 2:
        if (this->FinishedAnim()) {
            this->SetAnim(0xb5, 0, 0x1000, 0);
            *(u8*)(int)((char*)&mStateStep) += 1;
        }
        break;
    case 3:
        break;
    }
    AdvanceAnims();
    return 1;
}

extern "C" {

void func_ov002_020c3e8c(daPly_c *player) {
    player->ChangeState(data_ov002_0211013c);
}
}

int daPly_c::func_ov002_020c3ea0() {
    this->SetAnim(0xb3, 0x40000000, 0x1000, 0);
    mStateStep = 2;
    return 2;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_OpeningWakeUp_Init() {
    mSleepStage = 0;
    this->SetAnim(0xb2, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    return 1;
}

extern "C" {

void func_ov002_020c3f18(char* p) {
    ((daPly_c *)p)->mStatePhase = 1;
    func_ov002_020d2e74((daPly_c *)p);
}
}

extern "C" {

void func_ov002_020c3f2c(daPly_c *player) {
    player->ChangeState(data_ov002_02110544);
}
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Respawn_Main() {
    switch (mStateStep) {
    case 0: {
        u16 t = mStateTimer;
        if (t != 0) {
            if (t == 1) {
                mVertAccel = -0x4000;
                if (mPlayerNo == gLocalPlayerId) {
                    FUN_02029980();
                    mStateWaitTimer = 0x26;
                }
            }
        } else {
            if (mIsAirborne == 0) {
                u32 zero = 0;
                this->SetAnim(0x55, 0x40000000, 0x1000, zero);
                mStateStep += 1;
            }
        }
        break;
    }
    case 1:
        if (this->FinishedAnim() != 0) {
            int z = 0;
            this->SetAnim(0x47, z, 0x1000, z);
            mStateStep += 1;
        }
        break;
    case 2:
        if (mStateWaitTimer == 0) {
            if (mPlayerNo == gLocalPlayerId) {
                FUN_02029934();
            }
            this->ChangeState(daPly_c::ST_WAIT);
        }
        break;
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Restores the spawn point saved by daPly_c::InitResources, lifts the player
 * 0x3e8000 above it, re-seats the mesh collider at the new position, then
 * resets speed/angle state and starts the respawn animation.
 */

int daPly_c::St_Respawn_Init() {
    extern char *gCamera;
    DisableInteraction();
    mPosX = mSpawnPosX;
    mPosY = mSpawnPosY;
    mPosZ = mSpawnPosZ;
    mPosY += 0x3e8000;
    func_02035860(&mMeshClsn, (const Vector3 *)&mPosX);
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    mPrevAngleY = mSpawnAngleY;
    mAngleY = mPrevAngleY;
    mIsBodyClsnEnabled = 1;
    mSkipGroundUpdate = 0;
    if (mPlayerNo == gLocalPlayerId) {
        ((dCamera_c *)gCamera)->func_0200cf40();
    }
    this->SetAnim(0x54, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    mPeakY = mPosY;
    mHorzSpeed = 0;
    mVertSpeed = mHorzSpeed;
    mVertAccel = 0;
    mStateTimer = 0xa;
    return 1;
}

int daPly_c::func_ov002_020c4188() {
    extern int *gCamera;
    extern int gNextActorUpdateMask;
    extern void func_0201f32c(int arg0);
    int *p;

    if (mEventId == 0) {
        return 0;
    }

    p = gCamera;

    switch (mEventStep) {
    case 0:
        if (mEventId == 6) {
            mEventStep++;
        } else {
            if (mEventId < 7) {
                ((::dCamera_c *)p)->func_0200d3f8(mPlayerNo, 0);
            } else {
                ((dCamera_c *)p)->SetFlag_3();
            }
            mEventStep++;
            if (mEventId >= 7) {
                break;
            }
            return 0;
        }
    case 1:
        mFlags |= 0x800000;
        gNextActorUpdateMask |= 0x800000;
        mEventWait = 0xa;
        mEventStep++;
        break;
    case 2:
        if (mEventWait != 0) {
            mEventWait--;
            break;
        }
        func_0201f32c(data_ov002_020ff26c[mEventId - 1]);
        if (mEventId == 6) {
            mAttachedActor = SpawnSoundObj(5);
        }
        mEventStep++;
        break;
    case 3:
        if (data_0209d660 != 0) {
            break;
        }
        mEventStep++;
        mFlags &= ~0x800000;
        gNextActorUpdateMask &= ~0x800000;
        if (mEventId >= 7) {
            ((dCamera_c *)p)->mFlags &= ~8;
        }
        ((::dCamera_c *)p)->func_0200d81c(mPlayerNo);
        if (mEventId == 6) {
            daSoundObj_c *obj = (daSoundObj_c *)mAttachedActor;
            if (obj->mSoundID == 0x2a) {
                obj->mCounterLimit = 0;
            }
        }
        break;
    case 4:
        if (((dCamera_c *)p)->mFlags & 0x8000) {
            break;
        }
        mEventId = 0;
        mEventStep = 0;
        mFlags &= ~0x800000;
        gNextActorUpdateMask &= ~0x800000;
        break;
    }

    func_ov002_020e444c();
    return 1;
}

void daPly_c::func_ov002_020c43c4(unsigned int idx) {
    extern unsigned char gGameMode;
    int b = (gGameMode == 1);
    if (b != 0) return;
    switch (idx) {
    case 1:
        if (data_0209caa0.file.flags2 & 0x400)
            return;
        data_0209caa0.file.flags2 |= 0x400;
        break;
    case 2:
        if (data_0209caa0.file.flags2 & 0x800)
            return;
        data_0209caa0.file.flags2 |= 0x800;
        break;
    case 3:
        if (data_0209caa0.file.flags2 & 0x1000)
            return;
        data_0209caa0.file.flags2 |= 0x1000;
        break;
    case 4:
        if (data_0209caa0.file.flags2 & 0x2000)
            return;
        data_0209caa0.file.flags2 |= 0x2000;
        break;
    case 5:
        if (data_0209caa0.file.flags2 & 0x4000)
            return;
        data_0209caa0.file.flags2 |= 0x4000;
        break;
    case 6:
        if (data_0209caa0.file.flags2 & 0x10000000)
            return;
        data_0209caa0.file.flags2 |= 0x10000000;
        break;
    case 0:
        break;
    }
    mEventId = (unsigned char)idx;
    mEventStep = 0;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

int daPly_c::func_ov002_020c44c4() {
    extern u8 gGameMode;
    extern void func_02012694(unsigned int id, const Vector3 *v);
    Vector3 pos;
    int state;
    int r4val;
    u8 f2d8;
    int b0, b1;

    f2d8 = gGameMode;
    r4val = -1;
    b0 = (f2d8 == 1);
    // returns nested ahead of the body grow the function 0x29c -> 0x2a4
    if (b0 != 0) {
        goto cont;
    }
    if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) == 0) {
        goto ret0;
    }
    b1 = (f2d8 == 2);
    if (b1 == 0) {
        goto cont;
    }
ret0:
    return 0;

cont:
    if (SaveData::CountStarsCollectedInLevel(SublevelToLevel(gCurrentSublevel)) != 0) {
        return 0;
    }

    state = gCurrentSublevel;

    switch (state) {
    case 2:
        if ((data_0209caa0.file.flags2 & 0x200) == 0) {
            data_0209caa0.file.flags2 |= 0x200;
            /* First entry constructs it: guard, three stores, ~Vector3() registration. */
            static Vector3 sLevelIntroOff(0, 0x96000, 0x64000);
            Vec3_RotateYAndTranslate(&pos, (const Vector3 *)&this->mPosX, this->mAngleY,
                                     &sLevelIntroOff);
            this->ShowMessage(*this, 0x177, &pos, 2, 2);
            func_ov100_02145070(0x1e);
            func_ov100_02145014();
            func_02012694(0xb8, &mCamSpacePos);
            mPlayMusicAfterTalk = 1;
            return 1;
        }
        break;
    case 6:
        r4val = 7;
        break;
    case 7:
        r4val = 8;
        break;
    case 10:
        if (gCurrentEntranceId == 0) {
            r4val = 9;
        }
        break;
    case 14:
        if (gCurrentEntranceId == 0) {
            r4val = 10;
        }
        break;
    case 35:
        if (gCurrentEntranceId == 0) {
            r4val = 11;
            func_02012790(0x18);
        }
        break;
    case 31:
        if (data_0209caa0.file.flags1 & 0x80000000) {
            return 0;
        }
        r4val = 12;
        break;
    }

    if (r4val == -1) {
        return 0;
    }

    this->SetAnim(0x47, 0, 0x1000, 0);
    func_ov002_020c43c4(r4val & 0xff);
    return 0;
}

/* recovered: named members + shared header, real C++ method
 *
 * Leaving the talk state: clear the "in conversation" bit (0x800000) on the
 * actor being talked to, on the player, and on the global at gNextActorUpdateMask;
 * clear bit 3 on gCamera+0x154; and restore music layer 1 if this state
 * had overridden it.
 *
 * mTalkActor is the actor being talked to; +0xb0 through it is dActor_c's
 * mFlags. The field is typed fBase_c * (StartTalk's argument type), so the
 * bit is cleared through a cast, the same way daPly_c::St_Talk_Main reaches it.
 */

int daPly_c::St_Talk_Cleanup() {
    extern u32 gNextActorUpdateMask;
    extern char *gCamera;
    dActor_c *p = (dActor_c *)mTalkActor;
    if (p) {
        p->mFlags &= ~0x800000;
        mTalkActor = 0;
    }
    mFlags &= ~0x800000;
    gNextActorUpdateMask &= ~0x800000;
    ((dCamera_c *)gCamera)->mFlags &= ~8;
    if (mPlayMusicAfterTalk) {
        Sound::LoadAndSetMusic_Layer1(0x39);
        mPlayMusicAfterTalk = 0;
    }
    return 1;
}

/* recovered: shared common types */

int daPly_c::func_ov002_020c47f4() {
    typedef int Fix12i;
    Vector3 vec;
    int r, a;
    Fix12i y;
    vec.x = this->mPosX;
    y = this->mPosY;
    vec.y = y;
    vec.z = this->mPosZ;
    vec.y = y + 0x64000;
    mHeadRot[0] = 0;
    mHeadRot[1] = 0;
    r = ApproachLinear2(mAngleYSpeed, (short)(-Vec3_VertAngle((Vector3 *)&mStateTargetX, &vec)),
                        0x200);
    mHeadRot[0] = mAngleYSpeed;
    a = ApproachAngle(&this->mAngleY,
                      (short)(Vec3_HorzAngle((Vector3 *)&mStateTargetX, &vec) + 0x8000), 4, 0x1000,
                      0x200);
    mHeadRotMode = 3;
    // early return schedules the r==0 exit before the mHeadRotMode store at +0xb0
    if (r == 0) goto ret0;
    if (a == 0) return 1;
ret0:
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Talk_Main() {
    extern void func_0201f32c(int a);
    extern int gNextActorUpdateMask;
    extern char *gCamera;
    if (mRestoreMusicVolume != 0) {
        if (Sound::ChangeMusicVolume(0x7f, 0x7222) != 0)
            mRestoreMusicVolume = 0;
    }

    if (mNoCtrlKind == 0) {
        dActor_c *p = (dActor_c *)mTalkActor;
        if (p != 0)
            p->mFlags |= 0x800000;
        mFlags |= 0x800000;
        gNextActorUpdateMask |= 0x800000;
        mNoCtrlKind = 1;
    }

    switch (mStateStep) {
    case 0:
        if (mStateWork != 0)
            func_ov002_020c47f4();
        break;
    case 1:
        if (!this->IsAnim(0x64)) {
            if (!func_ov002_020c47f4())
                break;
        }
        mStateStep = 2;
        if (mStateWork == 0)
            func_0201fc88(mAttachOffsetY);
        else
            func_0201f32c((short)mAttachOffsetY);
        {
            char *cam = gCamera;
            switch ((mStateArg >> 2) & 3) {
            case 0:
                ((::dCamera_c *)cam)->func_0200d3f8(mPlayerNo, &mStateTargetX);
                break;
            case 1:
                ((::dCamera_c *)cam)->func_0200d7a4(mPlayerNo);
                break;
            }
        }
        break;
    case 2:
        if (data_0209d660 != 0)
            break;
        switch (mStateArg & 3) {
        case 0:
            mStateStep = 4;
            break;
        case 1:
            mStateStep = 3;
            break;
        case 2:
            mStateStep = 8;
            break;
        }
        break;
    case 8:
        if (func_ov100_02144fcc() != 0)
            mStateStep = 4;
        break;
    case 4:
    case 5:
        if (ApproachLinear2(mHeadRot[0], 0, 0x200) != 0) {
            if (mStateStep != 5)
                mStateStep = 6;
            else
                mStateStep = 7;
            mStateWork = 0;
            mPrevAngleY = mAngleY;
            char *cam = gCamera;
            ((::dCamera_c *)cam)->func_0200d81c(mPlayerNo);
            ((dCamera_c *)cam)->mFlags &= ~8;
            mHeadRotMode = 4;
        }
        break;
    case 7:
        if (mStateWaitTimer != 0)
            break;
    case 6:
        if ((((dCamera_c *)gCamera)->mFlags & 0x8000) != 0)
            break;
        if (mIsOpeningBigDoor != 0) {
            mNoCtrlKind = 0x13;
            this->ChangeState(data_ov002_0211022c);
            mStateStep = 0x13;
        } else if (this->IsAnim(0x64)) {
            this->ChangeState(data_ov002_02110514);
        } else {
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Talk_Init() {
    mIsControlDisabled = 1;
    DisableInteraction();
    this->SetAnim(0x47, 0, 0x1000, 0);
    mAngleYSpeed = 0;
    mHorzSpeed = 0;
    mNoCtrlKind = 0;
    mVertSpeed = 0;
    mVertAccel = mVertSpeed;
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    mPosX = mPreClsnPosX;
    mPosY = mPreClsnPosY;
    mPosZ = mPreClsnPosZ;
    mStateWork = 0;
    mIsOpeningBigDoor = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Starts a message. Already talking is success only at sub-step 0 or 3;
 * otherwise a short list of states may be interrupted, plus the no-control
 * state for all but a handful of mNoCtrlKind values. The last two parameters
 * are unsigned char -- the ROM reads all three of their uses with ldrb, and
 * the symbol was corrected from jj to hh two commits ago.
 */

int daPly_c::ShowMessage2(fBase_c &actor_, unsigned int msg, const Vector3 *pos, unsigned char d,
                          unsigned char e) {
    extern void *gCamera;
    fBase_c *actor = &actor_;
    int r7;

    if (IsState(data_ov002_0211046c)) {
        u8 sub = mStateStep;
        if (!(sub == 0 || sub == 3))
            return 0;
    } else {
        r7 = 0;
        if (IsState(data_ov002_0211013c) || IsState(data_ov002_02110364) ||
            IsState(data_ov002_0211043c) || IsState(daPly_c::ST_WAIT) ||
            IsState(data_ov002_02110424) || IsState(data_ov002_021104e4) ||
            IsState(data_ov002_02110514)) {
            r7 = 1;
        }

        if (IsState(data_ov002_0211022c)) {
            u8 v = mNoCtrlKind;
            if (!(v == 0 || v == 1 || v == 3 || v == 2 || v == 0xd)) {
                if (v != 8)
                    r7 = 1;
            }
        }

        if (!r7)
            return 0;

        this->ChangeState(*&data_ov002_0211046c);
        mTalkActor = actor;
    }
    mStateWork = 0;
    mStateArg = d & 3;
    *(&mStateArg) |= (e & 3) << 2;

    /* First entry constructs it: guard, three stores, ~Vector3() registration. */
    static Vector3 sTalkOff(0, 0x64000, 0x64000);

    Vec3_RotateYAndTranslate((Vector3 *)&mStateTargetX, &mPos, mAngleY, &sTalkOff);

    if (pos) {
        *(Vector3 *)(&mStateTargetX) = *pos;
    }

    mHorzSpeed = 0;
    mStateStep = 1;
    *(unsigned int*)&mAttachOffsetY = msg;

    if (e == 2) {
        ((dCamera_c *)gCamera)->SetFlag_3();
    }

    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Thin wrapper: forwards to ShowMessage2 and, if that took, clears mStateWork
 * to 1.
 *
 * The last two parameters are unsigned char. That is what the ROM does -- both
 * are read with ldrb -- and now what the symbol says, since the preceding
 * commit corrected it from jj to hh. While it said jj this function could not
 * be a real method at all: the compiler mangles FROM the declared types.
 */
int daPly_c::ShowMessage(fBase_c &a_, unsigned int b, const Vector3 *v, unsigned char d_,
                         unsigned char e_) {
    if (this->ShowMessage2(a_, b, v, d_, e_)) {
        mStateWork = 1;
        return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_0211046c;
}

int daPly_c::HasFinishedTalking() {

    if (this->IsState(data_ov002_0211046c) && mStateStep == 3) {
        mStateStep = 4;
        return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_0211046c;
}

int daPly_c::Unk_020c4f40(unsigned short x) {

    if (this->IsState(data_ov002_0211046c) && mStateStep == 3) {
        mStateWaitTimer = x;
        mStateStep = 5;
        return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method
 *
 * Entering conversation. Already talking to this actor with mStateStep 0 is
 * success; talking to anything else is refusal. Otherwise only a short list of
 * states may start a conversation, and when b_ is false the player must also
 * be pressing a button and be walking slowly, crouching or waiting.
 *
 * The third parameter is bool -- the ROM name ends in `b`, and a real method
 * is mangled FROM the declared types.
 */

int daPly_c::StartTalk(fBase_c &actor_, bool b_) {
    extern u8 data_0209f49e[];
    fBase_c *actor = &actor_;
    if (IsState(data_ov002_0211046c)) {
        if (actor == mTalkActor && mStateStep == 0)
            return 1;
        return 0;
    }

    if (!(IsState(data_ov002_0211013c) || IsState(daPly_c::ST_WAIT) ||
          IsState(data_ov002_021104e4) || IsState(data_ov002_02110514) ||
          IsState(data_ov002_02110364) || IsState(data_ov002_02110424) ||
          IsState(data_ov002_021105a4) || IsState(data_ov002_0211043c)))
        return 0;

    if (b_ == 0) {
        // early return 0 grows the function 0x230 -> 0x234
        if (!(*(u16 *)(data_0209f49e + (u8)gActivePlayerSlot * 0x18) & 3))
            goto ret0;

        if (!((IsState(data_ov002_0211013c) && mHorzSpeed < 0x8000) ||
              IsState(data_ov002_0211043c) || IsState(daPly_c::ST_WAIT)))
            goto ret0;

        mStateStep = 0;
        ChangeState(data_ov002_0211046c);
        mTalkActor = actor;
        return 1;
    } else {
        if (IsState(data_ov002_02110424) || IsState(data_ov002_021105a4)) {
            this->mStateFlags |= 0x40;
            return 0;
        }

        mStateStep = 0;
        ChangeState(data_ov002_0211046c);
        mTalkActor = actor;
        return 1;
    }
ret0:
    return 0;
}

extern "C" {

/* The look-at target: daTrs_c passes its own mPos with x negated and y raised
   by 0xc8000, and daHanachan_c passes the segment it wants Mario to face. */
int func_ov002_020c51d0(daPly_c *player, Vector3 *st) {
    if (!player->IsState(data_ov002_0211046c))
        return 0;
    if (player->mStateStep != 0)
        return 0;
    player->mStateTargetX = st->x;
    player->mStateTargetY = st->y;
    player->mStateTargetZ = st->z;
    player->mStateWork = 1;
    return 1;
}
}

int daPly_c::func_ov002_020c5244() {
    return 0;
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_0211046c;
}

int daPly_c::GetTalkState() {

    if (!this->IsState(data_ov002_0211046c))
        return -1;
    unsigned char v = mStateStep;
    if (v == 0)
        return 0;
    if (v == 3)
        return 2;
    if (v == 5 || v == 7)
        return 3;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_StuckInGround_Main() {
    extern ControllerInput data_0209f498[];
    switch (mStateWork) {
    case 0:
        if (this->FinishedAnim()) {
            this->SetAnim(data_ov002_0210a578[mStateStep], 0x40000000, 0x1000, 0);
            mStateWork++;
            return 1;
        }
        break;
    case 1:
        if (this->FinishedAnim() == 0) {
            if ((data_0209f498[gActivePlayerSlot].pressed & 2) == 0)
                break;
        }
        this->SetAnim(data_ov002_0210a584[mStateStep], 0x40000000, 0x1000, 0);
        mStateWork++;
        break;
    case 2: {
        dExtAnmModel_c *anim = mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
        if (anim->WillHitFrame(data_ov002_020ff0ec[mStateStep])) {
            func_ov002_020c5444();
            Sound::PlayBank0(0xb4, mCamSpacePos);
        }
    }
        if (this->FinishedAnim()) {
            mAngleY = mPrevAngleY;
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    }

    AdvanceAnims();
    return 1;
}

/* recovered: shared common types */
void daPly_c::func_ov002_020c5444() {
    Vector3 v;
    ((int *)&v)[0] = this->mPosX;
    ((int *)&v)[1] = this->mPosY;
    ((int *)&v)[2] = this->mPosZ;
    ((int *)&v)[1] = this->mPosY + 0x14000;
    unsigned char b = mStateArg;
    if (b == 0) {
        Particle::System::NewSimple(0xdb, v.x, v.y, v.z);
        Particle::System::NewSimple(0xdc, v.x, v.y, v.z);
        return;
    }
    if (b == 1) {
        Particle::System::NewSimple(0xd8, v.x, v.y, v.z);
        Particle::System::NewSimple(0xd9, v.x, v.y, v.z);
        return;
    }
    Particle::System::NewSimple(kPtclStuckInGround, v.x, v.y, v.z);
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_StuckInGround_Init() {
    DisableInteraction();
    ReleaseHeldActor();
    unsigned char idx = mStateStep;
    this->SetAnim(data_ov002_0210a560[idx], 0x40000000, 0x1000, 0);
    Sound::PlayBank0(mGroundSoundType + 0x80, mCamSpacePos);
    func_ov002_020c5444();
    mVertSpeed = 0;
    mHorzSpeed = 0;
    mStateWork = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_BowserEarthquake_Main() {
    if (mStateStep == 0) {
        ApproachLinear(mHorzSpeed, 0, 0x1800);
        if (this->FinishedAnim()) {
            this->ChangeState(data_ov002_0211013c);
        }
    } else {
        int t = mStateTimer;
        int m = t % 16;
        int idx = (unsigned short)(short)(m << 12) >> 4;

        mHorzSpeed = 0;

        int radius = 0x4000 + ((0x6000 - ((t << 12) / 8)) << 3);

        mVertSpeed = 0;

        int sinVal = FX_SinCosTable_[idx * 2];

        if (sinVal >= 0) {
            mPosY = mGroundY + FX_MUL(radius, sinVal);
        } else {
            mPosY = mGroundY - FX_MUL(radius, sinVal);
        }

        if (mStateTimer == 0) {
            this->ChangeState(data_ov002_0211013c);
        }
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_BowserEarthquake_Init() {
    if (mStateStep == 0) {
        this->SetAnim(0x12, 0x40000000, 0x1000, 0);
    } else {
        this->SetAnim(0x47, 0, 0x1000, 0);
        mStateTimer = 0x30;
    }
    return 1;
}

extern "C" {

int func_ov002_020c56f0(unsigned char* c, unsigned char arg)
{
    if (((daPly_c *)c)->func_ov002_020d82f0() == 0)
        return 0;
    ((daPly_c *)c)->mStateStep = arg;
    if (((daPly_c *)c)->mStateStep == 0 && ((daPly_c *)c)->func_ov002_020d91e0(0x300, 1) != 0) {
        ((daPly_c *)c)->ChangeState(data_ov002_0211010c);
        return 1;
    }
    ((daPly_c *)c)->ChangeState(data_ov002_021103c4);
    return 1;
}
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_DeadPit_Main() {
    extern u8 gGameMode;
    switch (mStateStep) {
    case 0:
        break;
    case 1: {
        int b = (gGameMode == 1);
        if (b) {
            if (mStateWaitTimer == 0) {
                this->ChangeState(data_ov002_021104fc);
                return 1;
            }
        }
        return 1;
    }
    case 2:
    case 5:
        if (mIsAirborne == 0) {
            mHorzSpeed = 0;
            mVertSpeed = 0;
        }
        if (mStateWork == 0) {
            if (this->FinishedAnim() != 0) {
                mStateWork = 1;
                KillPlayer();
            }
        }
        break;
    case 3: {
        func_ov002_020c0108(0);
        u32 snd = Sound::PlayLong(mLoopingSoundHandle, 0, 0x10a, mCamSpacePos, 0);
        int* p = (int*)((char*)&mSinkDepth);
        mLoopingSoundHandle = snd;
        *p += 0x5000;
        if (mStateWork == 0) {
            if (mSinkDepth >= 0xb4000) {
                mStateWork = 1;
                KillPlayer();
            } else {
                *p += 0x2000;
                if (mStateArg == 0) {
                    if (mSinkDepth >= 0x64) {
                        Sound::PlayCharVoice(mCharacter, 0x25, mCamSpacePos);
                        mStateArg = 1;
                    }
                }
            }
        }
        break;
    }
    case 6: {
        int src[3];
        src[0] = 0;
        src[1] = -0x1e000;
        src[2] = 0x64000;
        dActor_c *q = mAttachedActor;
        s16 angle = q->mAngleY;
        Vec3_RotateYAndTranslate((Vector3 *)&mPosX, &q->mPos, angle, (const Vector3 *)src);
        if (mStateTimer < 30) {
            mOpacity = 0;
        }
    }
    /* fallthrough */
    case 4:
        if (mStateTimer == 1) {
            KillPlayer();
            mOpacity = 0;
        }
        break;
    case 7:
    case 8:
        func_ov002_020c9718();
        if (mStateWork == 0) {
            if (this->FinishedAnim() != 0) {
                mStateWork = 1;
                KillPlayer();
                mIsUnderwater = 1;
            }
        }
        break;
    case 9:
        break;
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_DeadPit_Init() {
    extern u8 gGameMode;
    ReleaseHeldActor();
    if (mObjInMouth != 0) {
        int b = (mObjInMouth->actorID == 0xbf);
        if (b) {
            func_ov002_020d5cec(mObjInMouth);
            mStateFlags &= ~2;
            ((daPly_c *)mObjInMouth)->mEatingPlayer = 0;
        }
        mUseAltBodyModel = 0;
        mObjInMouth->mFlags |= 0x80000;
        mObjInMouth->mFlags &= ~0x40000;
        mObjInMouth->mFlags &= ~0x20000;
        mObjInMouth = 0;
    }
    DisableInteraction();
    {
        int t = 0;
        mIsBodyClsnEnabled = t;
        mStateWork = t;
        switch (mStateStep) {
        case 0:
            break;
        case 1:
            if (gGameMode == 1)
                t = 1;
            if (t == 0) {
                int a = 2;
                if (mSurfaceType == 5) a = 0;
                HitDeathPlane(a);
            } else {
                if (mPlayerNo == gLocalPlayerId)
                    FUN_020299f4();
                mStateWaitTimer = 0x1e;
            }
            mSkipGroundUpdate = 1;
            break;
        case 2:
            this->SetAnim(7, 0x40000000, 0x1000, t);
            mVertSpeed = 0xa000;
            mHorzSpeed >>= 1;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            Sound::PlayCharVoice(mCharacter, 0xb, mCamSpacePos);
            break;
        case 3:
            this->SetAnim(0x82, 0x40000000, 0x1000, t);
            mVertSpeed = 0;
            mHorzSpeed = 0;
            mStateArg = 0;
            break;
        case 4:
            this->SetAnim(2, 0x40000000, 0x1000, t);
            mStateTimer = 0x64;
            mVertSpeed = 0;
            mHorzSpeed = 0;
            Sound::PlayCharVoice(mCharacter, 0xb, mCamSpacePos);
            break;
        case 5:
            this->SetAnim(6, 0x40000000, 0x1000, t);
            mVertSpeed = 0;
            mHorzSpeed = 0;
            Sound::PlayCharVoice(mCharacter, 0xb, mCamSpacePos);
            break;
        case 6:
            mStateTimer = 0x3c;
            mVertSpeed = t;
            mHorzSpeed = t;
            mVertAccel = t;
            break;
        case 7:
            this->SetAnim(0x84, 0x40000000, 0x1000, t);
            mVertAccel = 0;
            mTerminalVelocity = -0x1800;
            mVertSpeed = 0;
            mHorzSpeed = 0;
            Sound::PlayCharVoice(mCharacter, 0x34, mCamSpacePos);
            break;
        case 8:
            this->SetAnim(0x83, 0x40000000, 0x1000, t);
            mVertAccel = 0;
            mTerminalVelocity = -0x1800;
            break;
        case 9:
            Sound::PlayCharVoice(mCharacter, 0x36, mCamSpacePos);
            break;
        }
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method */
/* daPly_c::EnterWhirlpool() @ 0x20c5cc8 (ov002) -- tail-call veneer to daPly_c::func_ov002_020c5dec(int) (0x20c5dec) with r1=9.
 */
void daPly_c::EnterWhirlpool() {
    ((this))->func_ov002_020c5dec(9);
}

extern "C" {

int func_ov002_020c5cd8(daPly_c *player, dActor_c *actor) {
    if (player->func_ov002_020c5dec(6)) {
        player->mAttachedActor = actor;
        return 1;
    }
    return 0;
}
}

int daPly_c::func_ov002_020c5d0c() {
    u16 *p;
    if (this->mPosY - mGroundY >= 0x100000)
        return 0;
    p = &this->mStateFlags;
    *p |= 0x400;
    return func_ov002_020c5dec(1);
}

extern "C" {

extern daPly_c::State data_ov002_02110124;
extern daPly_c::State data_ov002_02110064;
}

void daPly_c::func_ov002_020c5d60() {
    void *c = this;

    if (this->IsState(data_ov002_02110124)) {
        if (this->mStateStep == 1)
            return;
    }
  if (this->mPosY >= (int)0xf8ad0000) return;
  if (this->IsState(data_ov002_02110064)) return;
  (this->mStateFlags) |= 0x400;
  this->func_ov002_020c5dec(1);
}

extern "C" {

extern daPly_c::State data_ov002_02110124;
}

int daPly_c::func_ov002_020c5dec(int r1) {

    if (r1 != 1) {
        if (this->mIsNoControl != 0)
            return 0;
    }
    this->mStateStep = (unsigned char)r1;
    this->ChangeState(data_ov002_02110124);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_DeadHit_Main() {
    u32 t;

    if (mIsAirborne == 0) {
        if (mStateArg == 0) {
            if (mPrevVertSpeed < 0) {
                mVertSpeed = -mPrevVertSpeed / 3;
                if (mVertSpeed <= 0x1000)
                    mVertSpeed = 0;
            }
            t = mStateStep & 0xf0;
            if (t == 0 || t == 0x10)
                func_ov002_020d94cc();
            Sound::PlayBank0(mGroundSoundType + 0x50, mCamSpacePos);
            Sound::PlayCharVoice(mCharacter, 0x11, mCamSpacePos);
            mStateArg = 1;
        }
        ApproachLinear(mHorzSpeed, 0, 0x1800);
    }

    switch (mStateWork) {
    case 0:
        if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(
                data_ov002_02109db8[mStateStep & 1]) != 0)
            mStateWork = 1;
        break;
    case 1:
        if (mIsAirborne != 0) break;
        if (mStateArg == 0) break;
        if (mHorzSpeed != 0) break;
        this->SetAnim(data_ov002_0210a07c[mStateStep & 1], 0x40000000, 0x1000, 0);
        Sound::PlayCharVoice(mCharacter, 0xb, mCamSpacePos);
        mStateWork = 2;
        break;
    case 2:
        if (this->FinishedAnim() != 0) {
            if (gMeterShownHealth == 0) {
                KillPlayer();
                mStateWork = 3;
            }
        }
        break;
    }

    if (mStateWork != 1)
        AdvanceAnims();
    mPrevVertSpeed = mVertSpeed;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_DeadHit_Init() {
    extern char *gCamera;
    DisableInteraction();
    mIsBodyClsnEnabled = 0;
    int idx = mStateStep & 1;
    this->SetAnim(data_ov002_0210a424[idx], 0x40000000, 0x1000, 0);
    mStateArg = 0;
    mStateWork = 0;
    ((dCamera_c *)gCamera)->func_0200d89c();
    return 1;
}

int daPly_c::func_ov002_020c607c(int p1, int p2, int *outptr) {
    char *self = (char *)this;
    mLineClsnActor = 0;
    *outptr = (int)0x80000000;

    dBgCh_Lin rc;

    Vector3 a(this->mPosX, p1, this->mPosZ);
    Vector3 b(this->mPosX, p2, this->mPosZ);
    rc.SetObjAndLine(a, b, (dActor_c *)self);
    if (rc.DetectClsn() != 0) {
        Vector3 pos = rc.GetClsnPos();
        *outptr = pos.y;
        if (rc.GetClsnID() != -1) {
            dActor_c *found = dActor_c::FindWithID((u32)rc.GetClsnID());
            mLineClsnActor = found;
            if (found != 0) {
                int flag = (found->mFlags & 0x1000000) ? 1 : 0;
                if (flag != 0) {
                    return 1;
                }
                flag = (found->mFlags & 0x2000000) ? 1 : 0;
                if (flag != 0) {
                    return 2;
                }
            }
        }
    }
    return 0;
}

int daPly_c::func_ov002_020c61ac() {
    int outA;
    int outB;
    int v[3];
    dActor_c *saved;
    int a;
    int b;

    if ((mClsnFlags & 5) == 0)
        return 0;
    if (this->IsState(data_ov002_021102ec) || this->IsState(data_ov002_021106dc) || this->IsState(data_ov002_021106f4) || this->IsState(data_ov002_021102bc) || this->IsState(data_ov002_0210ffec) || this->IsState(data_ov002_0211001c) || mIsUnderwater != 0)
        return 0;
    if ((mClsnFlags & 5) == 5) {
        if (mIsBalloon != 0 || mIsMega != 0) {
            int d;
            int yv, zv, xv;
            zv = this->mPosZ;
            d = 0xc8000;
            if (mIsBalloon != 0)
                d = 0x96000;
            yv = this->mPosY + d;
            xv = this->mPosX;
            v[0] = xv;
            v[1] = yv;
            v[2] = zv;
            dBgCh_Gnd obj;
            obj.StartDetectingWater();
            obj.SetObjAndPos(*(const Vector3 *)v, this);
            if (obj.DetectClsn()) {
                mGroundY = obj.clsnY;
                this->mPosY = mGroundY;
            }
            if (mIsBalloon != 0) {
                func_ov002_020de968();
                this->ChangeState(data_ov002_021101b4);
            } else {
                func_ov002_020bdd2c();
                this->ChangeState(data_ov002_021101b4);
                return 1;
            }
        }
    }
    a = func_ov002_020c607c(this->mPosY - 0x20000, this->mPosY + 0xa0000, &outA);
    saved = mLineClsnActor;
    if (outA == 0x80000000)
        return 0;
    b = func_ov002_020c607c(this->mPosY + 0x1e000, this->mPosY - 0x28000, &outB);
    if (outB == 0x80000000)
        return 0;
    if (saved == mLineClsnActor)
        return 0;
    if (outA + 0x20000 < outB)
        return 0;
    {
        int t = outA - outB;
        if (t < 0)
            t = -t;
        if (t >= 0xa0000)
            return 0;
    }
    if ((a | b) == 0)
        return 0;
    this->mPosY = outB;
    mScaleAnimStep = 0;
    this->ChangeState(data_ov002_021102ec);
    if (a != 0) {
        mStateWork = 1;
        mStatePhase = (unsigned char)a;
    } else if (b != 0) {
        mStateWork = 2;
        mStatePhase = (unsigned char)b;
    }
    return 1;
}

/* recovered: shared common types */
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
int daPly_c::func_ov002_020c647c(int arg1) {
    char* c = (char*)this;
    Vector3 v1;
    Vector3 v2;
    dBgCh_Lin line;
    int z1 = this->mPosZ;
    int x1 = this->mPosX;
    v1.x = x1;
    v1.y = arg1 - 0x1e000;
    v1.z = z1;
    int x2 = this->mPosX;
    int z2 = this->mPosZ;
    v2.x = x2;
    v2.z = z2;
    v2.y = arg1 + 0xaa000;
    int r5 = -64;
    line.SetObjAndLine(v1, v2, (dActor_c*)c);
    if (line.DetectClsn()) {
        ::Vector3 clsnPos = line.GetClsnPos();
        int d = (clsnPos.y - this->mPosY) / 0x1000;
        if (d > 0xa0) d = 0xa0;
        if (d < 0) d = 0;
        r5 = d;
    }
    return r5;
}

int daPly_c::func_ov002_020c6538() {
    dBgCh_Gnd rg;
    Vector3 v;
    int y;
    v.x = this->mPosX;
    y = this->mPosY;
    v.y = y;
    v.z = this->mPosZ;
    v.y = y + 0x32000;
    rg.SetObjAndPos(v, this);
    if (rg.DetectClsn() == 0) {
        return 0;
    }
    {
        int hit = rg.clsnY;
        int d = hit - this->mPosY;
        if (d < 0) d = -d;
        if (d >= 0x64000) {
            return 0;
        }
        this->mPosY = hit;
        mAttachOffsetY = this->mPosY;
        return 1;
    }
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Squish_Cleanup() {
    int val = 0x1000;
    if (mIsMega != 0)
        val = 0x3000;

    mScaleX = val;
    mScaleY = val;
    mScaleZ = val;

    mdCcAcPos_c.flags &= ~4;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Squish_Main() {
    switch (mStateStep) {
    case 0:
        if (func_ov002_020c6908() != 0)
            return 1;
        {
            int r = func_ov002_020c647c(mAttachOffsetY);
            if (r >= 0) {
                mScaleY = (r << 12) / 160;
                if (r >= 0x20)
                    break;
                if (mStatePhase != 2)
                    break;
                Sound::PlayCharVoice(mCharacter, 7, mCamSpacePos);
                mScaleY = 0x100;
                mStateStep = 2;
                mStateArg = 1;
                if (mIsMetal == 0) {
                    if (func_ov002_020d91e0(0x300, 1) != 0)
                        mStateArg = 2;
                }
                {
                    dActor_c *a = mAttachedActor;
                    if (a != 0) {
                        u16 id = a->actorID;
                        int b1 = (id == 0xa4);
                        int b2;
                        if (b1 || (b2 = (id == 0xa5)) != 0)
                            *((u8 *)a + 0x407) = 1;
                    }
                }
            } else {
                mStateTimer = 0xa;
                (*(u8 *)(int)((char *)&mStateStep))++;
            }
        }
        break;
    case 1:
        if (mStateTimer != 0) {
            (*(int *)(int)((char *)&mScaleY)) += 0x100;
            if (mScaleY > 0x1000)
                mScaleY = 0x1000;
        } else {
            mScaleY = 0x1000;
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    case 2:
        if (func_ov002_020c6908() != 0)
            return 1;
        if (func_ov002_020c647c(mAttachOffsetY) < 0) {
            if (mStateArg == 1) {
                func_ov002_020db8bc(1);
                this->ChangeState(data_ov002_0211013c);
            } else {
                (*(u8 *)(int)((char *)&mStateStep))++;
                mStateArg = 0;
            }
        }
        break;
    case 3:
        if (mStateWork == 2 && func_ov002_020c6538() == 0) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
        if (mStateArg != 0)
            break;
        if (this->GetHealth() != 0) {
            func_ov002_020db8bc(1);
            this->ChangeState(data_ov002_0211013c);
        }
        if (gMeterShownHealth != 0)
            break;
        KillPlayer();
        (*(u8 *)(int)((char *)&mStateArg))++;
        break;
    case 4:
        if (mStateTimer != 0)
            break;
        if (mStateArg != 1) {
            mStateStep = 3;
            break;
        }
        func_ov002_020db8bc(1);
        this->ChangeState(data_ov002_0211013c);
        break;
    }
    return 1;
}

int daPly_c::func_ov002_020c6908() {

    if (mStateWork == 1) {
        int old;
        this->mPosY = mAttachOffsetY;
        old = this->mPosY;
        if (func_ov002_020c6538() == 0) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
        this->mPosY = old;
    } else if (mStateWork == 2) {
        if (func_ov002_020c6538() == 0) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Squish_Init() {
    typedef int Fix12i;
    ReleaseHeldActor();
    this->SetAnim(0x47, 0, 0x1000, 0);
    mStateWork = 0;
    mStatePhase = 0;
    mHorzSpeed = 0;
    mVertSpeed = 0;
    mVertAccel = 0;
    mStateStep = 0;
    mSkipGroundUpdate = 1;
    mAttachOffsetY = mPosY;
    mdCcAcPos_c.flags |= 4;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::Unk_020c6a10(unsigned int arg_) {
    u32 arg = arg_;

    int flag;

    if (!(mClsnFlags & 1)) return 0;
    if (this->IsState(data_ov002_021102ec)) return 0;
    if (!func_ov002_020d82f0())
        return 0;

    flag = 1;
    if (mIsMetal == 0) {
        if (func_ov002_020d91e0(arg << 8, 1))
            flag = 0;
    }

    Sound::PlayCharVoice(mCharacter, 7, mCamSpacePos);
    this->ChangeState(data_ov002_021102ec);
    mScaleY = 0x100;
    mStateStep = 4;
    mStateTimer = 0x1e;
    mStateArg = flag;
    mScaleAnimStep = 0;
    return 1;
}

extern "C" {

extern daPly_c::State data_ov002_021102a4;
}

int daPly_c::func_ov002_020c6adc() {

    unsigned short v = (unsigned short)(mStateFlags & 8);
    if (v == 0)
        return 0;
    mStateFlags &= ~8;
    this->ChangeState(data_ov002_021102a4);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Teleport_Main() {
    extern dCamera_c *gCamera;
    switch (mStateStep) {
    case 0:
        if (mOpacity > 1) {
            (this->mOpacity)--;
            break;
        }
        (this->mStateWork)++;
        if (mStateWork >= 0x10) {
            ObjTeleport* obj = GetTeleportDestObj((u8)(mTeleportId - 1));
            int tx = obj->x << 12;
            int tz = obj->z << 12;
            int ty = obj->y << 12;
            mPosX = tx;
            mPosY = ty;
            mPosZ = tz;
            mAreaId = obj->param & 0xf;
            mAngleY = ((obj->param >> 4) & 0xf) << 12;
            mPrevAngleY = mAngleY;
            mGroundY = 0x80000000;
            mWaterSurfaceY = 0x80000000;
            func_02035860(&mMeshClsn, &mPos);
            mOpacity = 0;
            gCamera->func_0200d1e4();
            Sound::PlayCharVoice(mCharacter, 0x19, mCamSpacePos);
            Sound::PlayBank0(0x19, mCamSpacePos);
            mStateStep = 1;
        }
        break;
    case 1:
        if (mStateWork != 0) {
            if (mStateWork >= 0x10) {
                if (data_0209f32c > mPosY + 0x64000)
                    mIsUnderwater = 1;
            }
            (this->mStateWork)--;
            break;
        }
        (this->mOpacity)++;
        func_02020388(mPlayerNo);
        if (mOpacity >= 0x1f) {
            int hit = 0;
            mOpacity = 0x1f;
            if (data_0209f32c >= mPosY + 0x64000) {
                Vector3 v;
                dBgCh_Gnd rc;
                {
                    int vz = mPosZ;
                    int vy = data_0209f32c + 0x64000;
                    int vx = mPosX;
                    v.x = vx;
                    v.y = vy;
                    v.z = vz;
                }
                rc.SetObjAndPos(v, this);
                if (rc.DetectClsn())
                    hit = 1;
            }
            if (hit)
                this->ChangeState(data_ov002_0211067c);
            else
                this->ChangeState(data_ov002_0211013c);
            gShowTouchScreenArrows = 1;
            mTeleportTimer = 0x3c;
            func_02012790(0x24);
        }
        break;
    }
    G2_SetBlendBrightness(0x3f, mStateWork);
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Teleport_Init() {
    DisableInteraction();
    mStateWork = 0;
    mStateStep = 0;
    mHorzSpeed = 0;
    this->SetAnim(0x47, 0, 0x1000, 0);
    Sound::PlayBank0(0x19, mCamSpacePos);
    return 1;
}

/* func_ov002_020c6e14 at 0x020c6e14
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
int daPly_c::func_ov002_020c6e14() {
    extern void *gCamera[];
    int idx;

    if (gNewStarCollected != 0) {
        idx = -1;
        switch (NumStars()) {
        case 1:
            idx = 0;
            break;
        case 3:
            idx = 1;
            break;
        case 8:
            idx = 2;
            break;
        case 12:
            idx = 3;
            break;
        case 30:
            idx = 4;
            break;
        case 50:
            idx = 5;
            break;
        case 80:
            idx = 6;
            break;
        }

        if (idx == -1)
            return 0;

        if (ShowMessage(*this, data_ov002_020ff1a0[idx], 0, 0, 2)) {
            ((dCamera_c *)(gCamera[0]))->SetFlag_3();
            return 1;
        }
    }

    return 0;
}

/* recovered: named members + shared header, real C++ method */
/* daPly_c::St_LevelEnter_Cleanup() at 0x020c6f34 (ov002).
 * Cleanup hook of the ST_LEVEL_ENTER player state-machine state; a no-op
 * that returns VS_FAIL (1). The implicit daPly_c* this is unused.
 */
s32 daPly_c::St_LevelEnter_Cleanup() {
    this;
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Level-entry cutscene, per frame. mStateWork indexes a table of
 * POINTERS TO MEMBER FUNCTION -- the same ARM/Itanium pmf representation
 * daPly_c::State uses -- and the selected step runs against the player. Six of
 * the mStateStep values additionally make the player the camera's focus
 * actor (dCamera_c::mFocusActor, through the camera pointer gCamera).
 */

int daPly_c::St_LevelEnter_Main() {
    extern void (daPly_c::*data_ov002_0211075c[])();
    extern dCamera_c *gCamera;
    switch (mStateStep) {
    case 8: case 9: case 11: case 12: case 16: case 18:
        gCamera->mFocusActor = this;
        break;
    case 10: case 13: case 14: case 15: case 17:
    default:
        break;
    }
    (this->*data_ov002_0211075c[mStateWork])();
    if (mStateArg == 0)
        AdvanceAnims();
    return 1;
}

void daPly_c::func_ov002_020c6fe4() {
    int id;

    id = this->GetBodyModelID(this->param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x58))
        Sound::PlayCharVoice(mCharacter, 0x35, mCamSpacePos);

    id = this->GetBodyModelID(this->param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x75))
        data_0209f20c = 1;

    if (this->FinishedAnim()) {
        this->SetAnim(0x47, 0, 0x1000, 0);
        mStateWork = 2;
    }
}

void daPly_c::func_ov002_020c70ac() {
    if (mStateTimer != 0) {
        this->mVertSpeed = 0;
        this->mHorzSpeed = 0;
        return;
    }
    mStateWaitTimer = 0;
    mStateWork = 0;
    this->mVertAccel = -0x4000;
    switch (mStateStep) {
    case 12:
        this->mAngleY = this->mAngleY + 0x8000;
    case 11:
    case 18:
        this->mHorzSpeed = 0x20000;
        mStateWaitTimer = 6;
        break;
    case 9:
        this->mAngleY = this->mAngleY + 0x8000;
    case 8:
    case 16:
        this->mVertSpeed = 0x40000;
        this->mHorzSpeed = 0x18000;
        break;
    case 13:
        this->mVertSpeed = 0x36000;
        this->mHorzSpeed = 0xa000;
        mStateArg = 0;
        break;
    case 17:
        this->mVertSpeed = 0x3c000;
        this->mHorzSpeed = 0xa000;
        mStateArg = 0;
        break;
    }
}

extern "C" {

extern daPly_c::State data_ov002_02110214;
}

void daPly_c::func_ov002_020c7194() {

    if (this->FinishedAnim() == 0)
        return;
    mStateStep = 1;
    if (func_ov002_020c44c4() != 0)
        return;
    this->ChangeState(data_ov002_02110214);
}

void daPly_c::func_ov002_020c71e0() {
    extern unsigned char gPrevReturnState[];
    mLevelEntryDone = 1;
    if (data_0209f2bc[0] != 0) return;
    if (mStateWaitTimer == 0) {
        this->mPrevAngleY = this->mAngleY;
        if (mStateStep == 1) {
            this->ChangeState(data_ov002_0211067c);
        } else {
            this->ChangeState(data_ov002_0211013c);
            if (mKeyModelId < 0 && gPrevReturnState[0] == 1) {
                if (func_ov002_020c6e14() != 0)
                    return;
            }
            if (func_ov002_020c44c4() != 0) return;
        }
    }
    func_ov002_020be008();
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {

void func_ov002_020c72a4(void* thisptr)
{
    unsigned char* r4 = (unsigned char*)thisptr;
    daPly_c *player = (daPly_c *)r4;
    dBgCh_Gnd rg;
    Vector3 v;
    int z = player->mPosZ;
    int d = data_0209212c;
    int x = player->mPosX;
    v.x = x;
    v.y = d;
    v.z = z;
    rg.mProbeHeight = d << 1;
    rg.SetObjAndPos(v, (dActor_c*)thisptr);
    rg.StartDetectingWater();
    if (rg.DetectClsn()) {
        if (SurfaceInfo_TestFlag0x20((int*)&rg.surface) != 0) {
            player->mWaterSurfaceY = rg.clsnY;
            data_0209f32c = player->mWaterSurfaceY;
        }
    }
    rg.mProbeHeight = 0x1f4000;
    ((daPly_c *)(thisptr))->func_ov002_020c71e0();
}
}

void daPly_c::func_ov002_020c7350() {
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned int u32;
    typedef signed char s8;
    extern u8 gGameMode;
    int id;

    if (mIsAirborne != 0)
        return;

    if (this->IsAnim(0x10)) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x52)) {
            Sound::PlayCharVoice(mCharacter, 0x27, mCamSpacePos);
        }
    }

    if (this->IsAnim(0x85)) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x20)) {
            Sound::PlayCharVoice(mCharacter, 0x35, mCamSpacePos);
        }
    }

    if (this->IsAnim(0xa5)) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x23)) {
            Sound::PlayCharVoice(mCharacter, 0x37, mCamSpacePos);
        }
    }

    id = this->GetBodyModelID(param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x8e)) {
        if (this->IsAnim(0xa5)) {
            data_0209f20c = 1;
        }
    }

    if (!this->FinishedAnim())
        return;

    {
        u8 st = mStateStep;
        if (st == 0x10 || st == 0x12) {
            void *actor;
            this->SetAnim(0x94, 0x40000000, 0x1000, 0);
            actor = Spawn(0x11a, mKeyModelId, mPos, &mAngle, this->mAreaId, -1);
            if (param1 == 2) {
                ((daObjKey_c *)actor)->func_ov089_0213115c(4);
            } else {
                ((daObjKey_c *)actor)->func_ov089_0213115c(1);
            }
            mStateWork = 6;
            return;
        }
    }

    if (this->IsAnim(0x85)) {
        data_0209f20c = 1;
    }

    {
        u8 v = gGameMode;
        int isOne = (v == 1);
        // a doAnim flag grows the function 0x2a0 -> 0x2b0
        if (isOne != false)
            goto setanim2;
        if (!(data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE))
            goto skip_setanim2;
        {
            int isTwo = (v == 2);
            if (isTwo != false)
                goto skip_setanim2;
        }
    setanim2:
        this->SetAnim(0x47, 0, 0x1000, 0);
    skip_setanim2:
        ;
    }

    mStateWork = 2;
}

void daPly_c::func_ov002_020c75f0() {
    extern void func_02012694(unsigned int id, const Vector3 *);
    extern u8 gPrevReturnState;
    if (this->IsAnim(0x56))
    {
        int model = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[model]->WillHitFrame(0)) {
            Sound::PlayBank0(6, mCamSpacePos);
        }
    }

    if (mIsAirborne == 0)
    {
        func_ov002_020c2f64();
        mStateArg = 0;
        mStateWork = 1;
        if (gPrevReturnState == 1 || gPrevReturnState == 2) {
            this->Heal(0x880);
        }
        switch (mStateStep)
        {
        case 3:
        case 10:
            this->SetAnim(0x51, 0x40000000, 0x1000, 0);
            break;
        case 4:
            this->mHorzSpeed = 0;
            this->SetAnim(0x55, 0x40000000, 0x1000, 0);
            break;
        case 6:
        case 7:
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            this->mHorzSpeed >>= 2;
            this->mVertSpeed = -(this->mVertSpeed >> 2);
            mStateArg = 0;
            this->SetAnim(0x13, 0x40000000, 0x1000, 0);
            break;
        case 5:
        case 8:
        case 9:
        case 11:
        case 12:
        case 15:
            this->mHorzSpeed = 0;
            break;
        case 13:
        case 16:
        case 17:
        case 18:
            this->mHorzSpeed = 0;
            this->SetAnim(0x52, 0x40000000, 0x1000, 0);
            break;
        }
        if (gPrevReturnState == 2) {
            Sound::PlayCharVoice(mCharacter, 0x24, mCamSpacePos);
        }
    }
    else
    {
        u8 t = mStateStep;
        if (t == 0xb || t == 0xc || t == 0x12)
        {
            if (mStateWaitTimer == 1)
            {
                func_02012694(0x28, &mCamSpacePos);
            }
        }
    }
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_LevelEnter_Init() {
    extern u8 gGameMode;
    extern u8 gPrevReturnState;

    u32 anim = data_ov002_0210a7e8[mStateStep];
    if (SaveData::HasPlayerLostCap() != 0) {
        if (anim == 0x85) anim = 0xa5;
    }
    this->SetAnim(anim, 0, 0x1000, 0);

    u8 st = mStateStep;
    if (st != 3 && st != 0xa && st != 0) {
        mModels.mBodyModels[this->GetBodyModelID((u8)param1, 0)]->SetFlags(0x40000000);
        if (mStateStep == 2) {
            dExtFrameCtrl_c *p =
                &(dExtFrameCtrl_c &)*mModels.mBodyModels[this->GetBodyModelID((u8)param1, 0)];
            p->currFrame = 0x14000;
        }
    }

    mKeyModelId = -1;
    DisableInteraction();
    mJumpComboStage = 0;
    mHorzSpeed = 0;
    mStateArg = 0;
    mPrevAngleY = mAngleY;
    mStateWork = 0;
    dBgCh_Gnd rg;

    switch (mStateStep) {
    case 0: {
        Vector3 pos;
        int hit;
        int y = mPosY;
        int z = mPosZ;
        int x = mPosX;
        int w = y + 0xc8000;
        pos.x = x;
        pos.y = w;
        pos.z = z;
        rg.SetObjAndPos(pos, this);
        hit = 0x80000000;
        if (rg.DetectClsn() != 0)
            hit = rg.clsnY;
        mPosY = hit;
        mStateWork = 2;
        {
            int b = (gGameMode == 2);
            if (b == 0) mStateWaitTimer = 0x1e;
            else mStateWaitTimer = 0;
        }
        break;
    }
    case 1:
        mVertAccel = 0;
        mStateWork = 4;
        mIsUnderwater = 1;
        break;
    case 5:
    case 15:
        mStateArg = 1;
        break;
    case 11:
    case 12: {
        s32 idx;
        s16 cosv, sinv;
        if (func_ov002_020c7cbc() != 0) mStateStep = 0x12;
        mPosY += 0x64000;
        idx = ((s32)(u16)(s16)(mAngleY + 0x8000) >> 4) * 2;
        cosv = FX_SinCosTable_[idx];
        sinv = FX_SinCosTable_[idx + 1];
        mPosX += FX_MUL(0xa0000, cosv);
        mPosZ += FX_MUL(0xa0000, sinv);
        mStateTimer = 0x20;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        break;
    }
    case 8:
    case 9:
        if (func_ov002_020c7cbc() != 0) mStateStep = 0x10;
        mStateTimer = 2;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        break;
    case 13:
    case 17: {
        u8 st2 = mStateStep;
        if (st2 == 0x11) mStateTimer = 2;
        else mStateTimer = 0x20;
        mStateArg = 1;
        mVertAccel = 0;
        mStateWork = 5;
        if (gCurrentSublevel == 0x18)
            func_02012790(0x16);
        else if (gCurrentSublevel == 0x19)
            func_02012790(0x17);
        break;
    }
    case 2:
        if (param1 != 0) {
            mStateStep = 0xa;
            this->SetAnim(data_ov002_0210a7e8[mStateStep], 0, 0x1000, 0);
        } else {
            this->InitWingFeathers(0);
            mStateWork = 3;
            mVertSpeed = 0;
            mVertAccel = 0;
        }
        break;
    default:
        break;
    }

    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    {
        u8 c2 = gCurrentEntranceId;
        if (gCurrentSublevel == 2) {
            if (c2 == 0 || (c2 == 0xe && gPrevReturnState != 1)) {
                func_02012790(0x1d);
            }
        } else if (gCurrentSublevel == 1 && (c2 == 6 || c2 == 7 || (u32)(c2 - 9) <= 1)) {
            func_02012790(0x1d);
        }
    }

    return 1;
}

int daPly_c::func_ov002_020c7cbc() {
    extern u8 gPrevReturnState;
    if (gPrevReturnState == 1) {
        switch (gLastCourseSublevel) {
        case 0x24: mKeyModelId = 0; break;
        case 0x26: mKeyModelId = 1; break;
        case 0x2d: mKeyModelId = 2; break;
        case 0x2f: mKeyModelId = 3; break;
        case 0x31: mKeyModelId = 4; break;
        }

        if (mKeyModelId >= 0) {
            LoadKeyModels(mKeyModelId);
            mLoadedResourceFlags |= 0x10;
            this->SetAnim(0x53, 0x40000000, 0x1000, 0);
            return 1;
        }
    }
    return 0;
}

void daPly_c::func_ov002_020c7dd0(int i) {
    extern unsigned char gPrevReturnState;
    unsigned char g;
    mStateStep = data_ov002_020ff240[i];
    g = gPrevReturnState;
    if (g == 2) {
        unsigned char s = mStateStep;
        switch (s) {
        case 4:
            mStateStep = 5;
            break;
        case 8:
            mStateStep = 9;
            break;
        case 0xb:
            mStateStep = 0xc;
            break;
        }
        Heal(-0x880);
        Heal(0x100);
    } else if (g == 1) {
        if (mStateStep == 4) {
            mStateStep = 0xf;
        }
    }
    ChangeState(data_ov002_021104b4);
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::IsEnteringLevel() {
    if (this->IsStateEnteringLevel() == 0)
        return 0;
    unsigned char s = mStateStep;
    if (s == 8 || s == 9 || s == 0x10)
        return 1;
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::IsStateEnteringLevel() {

    if (this->IsState(data_ov002_021104b4))
        return 1;
    return this->IsState(data_ov002_0211025c);
}

int daPly_c::func_ov002_020c7f10() {
    unsigned char v;
    if (!this->IsState(data_ov002_0211022c))
        // early return duplicates the shared fail epilogue (size 0xe8 -> 0x10c)
        goto fail;
    if (mStateStep != 0)
        goto fail;
    v = mNoCtrlKind;
    if (v == 0 || v == 1 || v == 0x11 || (unsigned char)(v + 0xfe) <= 1) {
        if (mStateArg != 0)
            goto fail;
        func_ov002_020ce9c8();
        this->mVertAccel = 0;
        mIsUnderwater = 1;
        mStateArg = 1;
        func_ov002_020c7ff8();
        return 1;
    }
  if (v == 8) {
      func_ov002_020ce9c8();
      this->mVertAccel = 0;
      mIsUnderwater = 1;
      this->SetAnim(0x6b, 0x40000000, 0x1000, 0);
      mStateStep = 9;
      goto fail;
  }
fail:
  return 0;
}

void daPly_c::func_ov002_020c7ff8() {
    extern dCamera_c *gCamera;
    u8 s = mNoCtrlKind;
    dCamera_c *g = gCamera;
    if (s == 0 || s == 1 || s == 0x11) {
        g->StartDanceCutscene(mPlayerNo, -1);
    } else if (s == 3) {
        g->StartDanceCutscene(mPlayerNo, 5);
    } else {
        g->func_0200d508(mPlayerNo);
    }
    if (mIsUnderwater != 0) {
        this->SetAnim(0xac, 0x40000000, 0x1000, 0);
        this->mVertAccel = 0;
    } else {
        this->SetAnim(0x19, 0x40000000, 0x1000, 0);
    }
    {
        u8 t = mNoCtrlKind;
        if (t == 0 || t == 3) {
            mStateStep = 1;
        } else {
            mStateStep = 2;
        }
    }
    this->mVertSpeed = 0;
    this->mVertAccel = this->mVertSpeed;
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    Sound::PlayCharVoice(mCharacter, 0x1c, mCamSpacePos);
    if (mStatePhase != 0) return;
    func_02012790(0x2d);
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_NoControl_Cleanup() {
  int v=0x1000;
  if(mIsMega) v=0x3000;
  mScaleX=v;
  mScaleY=v;
  mScaleZ=v;
  if (mNoCtrlKind == 0x11)
      KuppaScript::End();
  return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_NoControl_Main() {
    u8 mode = mNoCtrlKind;
    if (mode == 0 || mode == 1 || (mode == 0x11 && mStateStep != 0x18)) {
        if (KuppaScript::IsDanceRotateCutsceneRunning() == 0) {
            mAngleY = GetAngleToCamera(mPlayerNo);
            mPrevAngleY = mAngleY;
        }
    }

    switch (mStateStep) {
    case 0:
        func_ov002_020c965c();
        break;
    case 1:
        if (func_ov002_020c94a4() != 0)
            return 1;
        break;
    case 2:
    case 9:
        func_ov002_020c92fc();
        break;
    case 10:
        func_ov002_020c9288();
        break;
    case 20:
        func_ov002_020c924c();
        return 1;
    case 21:
        if (func_ov002_020c91bc() != 0)
            return 1;
        break;
    case 3:
    case 6:
        if (mStateWaitTimer == 0)
            mHorzSpeed = 0;
        if (mNoCtrlKind == 6 && mIsUnderwater != 0) {
            mVertAccel = 0;
            mVertSpeed = 0;
        } else {
            func_ov002_020c9718();
        }
        if (mNoCtrlKind == 0 || mNoCtrlKind == 3)
            return 1;
        break;
    case 4:
    case 5:
        if (func_ov002_020c9128() != 0)
            return 1;
        break;
    case 7:
    case 8:
        func_ov002_020c8cb0();
        break;
    case 11:
        func_ov002_020c904c();
        break;
    case 12:
        func_ov002_020c8f80();
        break;
    case 13:
        func_ov002_020c8f0c();
        break;
    case 14:
        func_ov002_020c8b54();
        break;
    case 15:
        func_ov002_020c8a4c();
        break;
    case 16:
        func_ov002_020c897c();
        break;
    case 17:
        func_ov002_020c8d14();
        break;
    case 18:
        func_ov002_020c8b78();
        break;
    case 19:
        if (mIsOpeningBigDoor == 0)
            this->ChangeState(data_ov002_0211013c);
        break;
    case 22:
        if (mIsAirborne == 0) {
            this->SetAnim(0x98, 0x40000000, 0x1000, 0);
            mStateStep = 0x17;
        }
        break;
    case 23: {
        int id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x1c))
            mHasNoCap = 0;
        if (this->FinishedAnim())
            this->ChangeState(data_ov002_0211013c);
        break;
    }
    case 24:
        func_ov002_020c8714();
        break;
    case 25:
        if (func_ov002_020c8540() != 0)
            return 1;
        break;
    case 26:
        if (func_ov002_020c84b0() != 0)
            return 1;
        break;
    }

    AdvanceAnims();
    return 1;
}

unsigned char daPly_c::func_ov002_020c84b0() {
    Vector3 v;
    short ang;
dummy_label_675641:;
    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    v.y += 0x64000;
    ang = Vec3_VertAngle((Vector3 *)&mStateTargetX, &v);
    ApproachLinear2(this->mAngleYSpeed, -ang, 0x400);
    mHeadRot[0] = 0;
    mHeadRot[1] = 0;
    mHeadRot[0] = this->mAngleYSpeed;
    mHeadRotMode = 3;
    return 0;
}

/* func_ov002_020c8540 at 0x020c8540 (ov002)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */

int daPly_c::func_ov002_020c8540() {
    extern void func_0201f32c(int arg0);
    extern int gNextActorUpdateMask;
    u32 s8;
    int id;

    s8 = this->param1;
    if (s8 <= 1) {
        id = this->GetBodyModelID(s8 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(data_ov002_020ff138[s8])) {
            mHasNoCap = 1;
        }
        s8 = this->param1;
        id = this->GetBodyModelID(s8 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(data_ov002_020ff13c[s8])) {
            mHasNoCap = 0;
        }
    }
    if (mStateArg == 0) {
        s8 = this->param1;
        id = this->GetBodyModelID(s8 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(data_ov002_020ff110[s8])) {
            char* o;
            mFlags |= 0x800000;
            o = (char *)mTalkActor;
            if (o != 0) {
                int *p = (int *)&((dActor_c *)o)->mFlags;
                *p &= ~0x800000;
            }
            gNextActorUpdateMask |= 0x800000;
            func_0201f32c(data_ov002_020ff108[this->param1]);
            mStateArg = 1;
            return 1;
        }
    } else {
        char* o;
        if (data_0209d660 != 0) return 1;
        gNextActorUpdateMask &= ~0x800000;
        mFlags &= ~0x800000;
        o = (char *)mTalkActor;
        if (o != 0) {
            int *p = (int *)&((dActor_c *)o)->mFlags;
            *p |= 0x800000;
            mTalkActor = 0;
        }
        if (this->FinishedAnim()) {
            this->ChangeState(data_ov002_0211013c);
        }
    }
    return 0;
}

/* func_ov002_020c8714 @ 0x020c8714 (ov002, size 0x268)
 * daPly_c cap-power ending update: while active, ducks the music, damps the
 * forward speed (or launches on variant 1), and streams the sparkle trail;
 * when the anim finishes either spawns the cap-return actor with the level
 * fanfare (mode 0x10) or spawns the star and hands the player to it.
 */
int daPly_c::func_ov002_020c8714() {
    volatile int v[3]; // without volatile, the function shrinks 0x268 to 0x24c

    if (mStateArg == 0) {
        Sound::ChangeMusicVolume(0x14, 0x15666);
        if (mStateTimer == 0) {
            this->mHorzSpeed = FX_MUL(this->mHorzSpeed, 0xf60);
            if (mIsAirborne == 0) {
                this->mHorzSpeed = 0;
                this->mVertSpeed = 0;
                this->SetAnim(0x51, 0x40000000, 0x1000, 0);
                mStateArg = 1;
            }
        } else if (mStateTimer == 1) {
            this->SetAnim(0x56, 0, 0x1000, 0);
            this->mHorzSpeed = 0x1a000;
            this->mVertSpeed = 0x38000;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        v[0] = this->mPosX;
        v[1] = this->mPosY;
        v[2] = this->mPosZ;
        /* mPosY (+0x60), re-read by index: this->mPosY lets the compiler reuse the
           load above and drops the ROM's second load (one word short, measured). */
        v[1] = ((int *)this)[0x18] + 0x3c000;
        mParticle1 =
            Particle::System::New(*(volatile int *)&this->mParticle1, 0x27, v[0], v[1], this->mPosZ,
                                  0, 0); // without volatile, two instructions swap
    } else if (this->FinishedAnim() != 0) {
        if (mNoCtrlKind == 0x10) {
            mStateStep = 0x19;
            this->SetAnim(0x99, 0x40000000, 0x1000, 0);
            mTalkActor = 0;
            if (this->param1 <= 1) {
                mTalkActor = Spawn(0x10d, (this->param1 << 8) | 0xf, mPos, 0, this->mAreaId, -1);
            }
            switch (this->param1) {
            case 0:
                Sound::PlayBank0(0xd9, mCamSpacePos);
                break;
            case 1:
                Sound::PlayBank0(0xda, mCamSpacePos);
                break;
            case 2:
                Sound::PlayBank0(0xdb, mCamSpacePos);
                break;
            }
        } else {
            daStar_c *st;
            this->mVertSpeed = 0;
            mAttachOffsetY = 0x187;
            func_ov002_020c7ff8();
            st = (daStar_c *)Spawn(0xb2, 0x6f, mPos, &mAngle, this->mAreaId, -1);
            if (st != 0)
                st->CollectNoPose(this);
        }
        mStateArg = 0;
    }
    return 0;
}

int daPly_c::func_ov002_020c897c() {
    extern void func_02012694(unsigned int a, const Vector3 *);
    int id;

    id = this->GetBodyModelID(this->param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x5f)) {
        func_02012694(0x3b, &mCamSpacePos);
    } else {
        id = this->GetBodyModelID(this->param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x3c)) {
            func_02012694(0x42, &mCamSpacePos);
        } else if (this->FinishedAnim()) {
            func_ov002_020ca108();
            this->SetAnim(0x47, 0, 0x1000, 0);
            mRootMotionMode = 0;
        }
    }
    return 0;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

int daPly_c::func_ov002_020c8a4c() {
    int spd = this->mHorzSpeed >> 3;
    if (spd < 0x400) spd = 0x400;
    this->SetAnim(0x48, 0, spd, 0);
    this->mPrevAngleY = Vec3_HorzAngle(&mPos, (Vector3 *)&mStateTargetX);
    this->mAngleY = this->mPrevAngleY;
    if (VEC_Distance((const VecFx32 *)&mPos, (const VecFx32 *)&mStateTargetX) < 0xa000) {
        if (mStateArg == 0) {
            mStateArg = 1;
            /* First entry constructs it: guard, three stores, ~Vector3() registration. */
            static Vector3 sAnchorOff(0, 0, 0x12c000);
            Vec3_RotateYAndTranslate((Vector3 *)&mStateTargetX, (Vector3 *)&mStateAnchorX,
                                     mAngleXSpeed, &sAnchorOff);
        } else {
            func_ov002_020ca108();
        }
    }
    return 0;
}

int daPly_c::func_ov002_020c8b54() {
    if (mStateTimer == 0) {
        this->mHorzSpeed = 0xa000;
        mStateStep = 0xf;
    }
    return 0;
}

/* recovered: shared common types */

int daPly_c::func_ov002_020c8b78() {
    Vector3 v;
    Sound::ChangeMusicVolume(0x14, 0x15666);
    {
        int id = GetBodyModelID(param1 & 0xff, false);
        dExtAnmModel_c *model = mModels.mBodyModels[id];
        dExtFrameCtrl_c *a = &static_cast<dExtFrameCtrl_c &>(*model);
        if (a->WillHitFrame(0x40))
            func_02012790(0x3a);
    }
    if (FinishedAnim() != 0) {
        /* First entry constructs it: guard, three stores, ~Vector3() registration. */
        static Vector3 sDoorMsgOff(0, 0x96000, 0x64000);
        Vec3_RotateYAndTranslate(&v, &mPos, this->mAngleY, &sDoorMsgOff);
        mRestoreMusicVolume = 1;
        {
            u16 *fl = &this->mStateFlags;
            *fl |= 0x400;
        }
        {
            u8 saved = mIsOpeningBigDoor;
            unsigned int msg = 0x188;
            if (mStateArg)
                msg = 0x29;
            ShowMessage(*this, msg, &v, 0, 2);
            mIsOpeningBigDoor = saved;
        }
    }
    return 0;
}

int daPly_c::func_ov002_020c8cb0() {
    if (this->FinishedAnim()) {
        if (gNextSublevel >= 0)
            mIsOpeningBigDoor = 1;
        func_ov002_020ca108();
        mRootMotionMode = 0;
        this->SetAnim(0x47, 0, 0x1000, 0);
    }
    return 0;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
int daPly_c::func_ov002_020c8d14() {
    Vector3 v;
    int val;
    int id;

    val = this->mScaleY + 0xf0;
    if (val >= 0x3000) val = 0x3000;
    this->mScaleX = val;
    this->mScaleY = val;
    this->mScaleZ = val;

    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    v.y = this->mPosY + FX_MUL(this->mScaleY, 0x50000);

    id = this->GetBodyModelID(param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(4)) {
        Particle::System::NewSimple(kPtclMegaGrowA, v.x, v.y, v.z);
    } else {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(2)) {
            Particle::System::NewSimple(kPtclMegaGrowB, v.x, v.y, v.z);
        }
    }

    id = this->GetBodyModelID(param1 & 0xff, 0);
    if (mModels.mBodyModels[id]->WillHitFrame(0x2f)) {
        func_ov002_020c9e18();
        mMegaTimer = 0x258;
    }

    dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
    if ((unsigned int)(anim.currFrame << 4) >> 0x10 >= 0x2f) {
        if (mIsUnderwater == 0) {
            if (func_ov002_020d22ec(0))
                return 0;
        }
    }

    if (this->FinishedAnim()) {
        if (mIsUnderwater != 0)
            this->ChangeState(data_ov002_021106ac);
        else
            this->ChangeState(data_ov002_0211013c);
    }
    return 0;
}

int daPly_c::func_ov002_020c8f0c() {
  int *p0 = &this->mScaleX;
  int *p1 = &this->mScaleY;
  int *p2 = &this->mScaleZ;
  *p0 -= 0x80;
  *p1 -= 0x80;
  *p2 -= 0x80;
  if (this->mScaleX < 0) {
    this->mScaleX = 0;
    this->mScaleY = 0;
    this->mScaleZ = 0;
  }
  if (mIsAirborne == 0) {
    EnterBigBoosHaunt();
    mStateStep = 6;
  }
  return 0;
}

/* recovered: shared common types */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12t<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
int daPly_c::func_ov002_020c8f80() {
    typedef int Fix12t;
    char *p = (char *)this;

    this->mScaleX -= 0x80;
    this->mScaleY -= 0x80;
    this->mScaleZ -= 0x80;

    unsigned int id = GetBodyModelID(param1 & 0xff, false);
    if (mModels.mBodyModels[id]->WillHitFrame(1)) {
        if (this->mStateArg == 0) {
            this->mStateArg = 1;
        } else {
            this->SetAnim(0x43, 0x40000000, 0x1000, 0);
            this->mStateStep = 0xd;
            this->mVertAccel = -0x4000;
            mAngleX = 0x4000;
            Sound::PlayBank0(0x1a, mCamSpacePos);
        }
    }
    return 0;
}

int daPly_c::func_ov002_020c904c() {
    typedef unsigned int u32;
    typedef int s32;
    typedef unsigned char u8;
    int mid;
    char *anim;
    ApproachLinear(this->mPosX, this->mStateTargetX, 0x8000);
    ApproachLinear(this->mPosZ, this->mStateTargetZ, 0x8000);
    if (Math_Function_0203b14c(&mPosY, (this->mGroundY) + 0x190000, 0x200, 0x64000, 0x10000) == 0) {
        this->mStateStep = 0xc;
        this->SetAnim(0x56, 0, 0x1000, 0);
        this->mStateArg = 0;
    }
    if (this->IsAnim(0x56) != 0) {
        mid = this->GetBodyModelID((param1) & 0xff, 0);
        if (mModels.mBodyModels[mid]->WillHitFrame(0) != 0) {
            Sound::PlayBank0(6, mCamSpacePos);
        }
    }
    return 0;
}

int daPly_c::func_ov002_020c9128() {

    if (mStateStep == 4) {
        ApproachLinear2(mAngleYSpeed, 0x1000, 0x400);
    } else {
        if (ApproachLinear2(mAngleYSpeed, 0x4000, 0x100)) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
    }
    mHeadRot[0] = 0;
    mHeadRot[1] = 0;
    mHeadRot[2] = mAngleYSpeed;
    mHeadRotMode = 2;
    return 0;
}

int daPly_c::func_ov002_020c91bc() {
    extern int gNextActorUpdateMask;
    if (data_0209d660 != 0) return 1;
    mFlags &= ~0x800000;
    gNextActorUpdateMask &= ~0x800000;
    if (this->FinishedAnim()) {
        if (mIsUnderwater != 0)
            this->ChangeState(data_ov002_0211067c);
        else
            this->ChangeState(data_ov002_0211013c);
    }
    return 0;
}

int daPly_c::func_ov002_020c924c() {
    extern void func_0201f32c(int x);
    if (mStateTimer == 0) {
        func_0201f32c((short)mAttachOffsetY);
        mStateStep = 0x15;
    }
    return 0;
}

int daPly_c::func_ov002_020c9288() {

    if (!func_ov002_020bea7c()) {
        if (mStatePhase == 0) {
            func_ov002_020c43c4(6);
            mStatePhase = 1;
        } else if (mIsUnderwater != 0) {
            this->ChangeState(data_ov002_0211067c);
        } else {
            this->ChangeState(data_ov002_0211013c);
        }
    }
    return 0;
}

int daPly_c::func_ov002_020c92fc() {
    extern int gNextActorUpdateMask;
    extern unsigned char gGameMode;
    extern void *gCamera;
    int flag = 0;
    this->mVertSpeed = 0;

    if (mAttachOffsetY != -1) {
        if (mIsUnderwater == 0) {
            int id = this->GetBodyModelID(param1 & 0xff, flag);
            if (mModels.mBodyModels[id]->WillHitFrame(0x20))
                flag = 1;
        } else {
            if (this->FinishedAnim()) flag = 1;
        }
    }

    if (flag != 0) {
        mStateTimer = 0x14;
        mStateStep = 0x14;
        this->mVertSpeed = 0;
        mFlags |= 0x800000;
        gNextActorUpdateMask |= 0x800000;
        return 0;
    }

    if (this->FinishedAnim()) {
        if (mStateStep == 9) {
            if ((int)(gGameMode == 1) == 0) {
                if ((data_0209caa0.file.flags2 & 0x10000000) == 0 &&
                    SaveData::IsCharacterUnlocked(0) == 0 && mHatCharacter == 0 &&
                    mPrevCharacter == 3) {
                    mStateStep = 0xa;
                    mStatePhase = 0;
                    ((::dCamera_c *)gCamera)->func_0200d3f8(mPlayerNo, 0);
                    return 0;
                }
            }
        }
        if (mIsUnderwater != 0)
            this->ChangeState(data_ov002_0211067c);
        else
            this->ChangeState(data_ov002_0211013c);
    }
    return 0;
}

int daPly_c::func_ov002_020c94a4() {
    extern u8 gReturnSublevel;
    unsigned short st;

    this->mVertSpeed = 0;

    st = mStateTimer;
    if (st != 0) {
        unsigned char sub;
        if (st == 1) {
            sub = mNoCtrlKind;
            if (sub == 0) {
                if (gCurrentSublevel == 0x21) {
                    gReturnSublevel = 4;
                    gReturnEntranceId = 7;
                }
                ExitLevel();
                mStateStep = 6;
            } else if (sub == 3) {
                switch (gCurrentSublevel) {
                case 0x24:
                    gReturnSublevel = 2;
                    gReturnEntranceId = 0xa;
                    break;
                case 0x26:
                    gReturnSublevel = 4;
                    gReturnEntranceId = 9;
                    break;
                case 0x2d:
                    gReturnSublevel = 0x32;
                    gReturnEntranceId = 3;
                    break;
                case 0x2f:
                    gReturnSublevel = 3;
                    gReturnEntranceId = 0;
                    break;
                case 0x31:
                    gReturnSublevel = 5;
                    gReturnEntranceId = 0xb;
                    break;
                }
                ExitLevel();
                mStateStep = 6;
            }
        }
        return 1;
    }

    {
        int id = this->GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(0x20))
            mStateTimer = 0x3c;
    }
    return 0;
}

int daPly_c::func_ov002_020c965c() {
    u8 st;

    if (mIsAirborne != 0) {
        // early return duplicates each return-0 epilogue (size 0xbc -> 0xe0)
        if (mIsUnderwater == 0) goto done;
    }

    this->mVertSpeed = 0;
    st = mNoCtrlKind;
    if (st == 0 || st == 1 || st == 0x11 || (u8)(st + 0xfe) <= 1) {
        func_ov002_020c7ff8();
        goto done;
    }
    if (st == 8) {
        this->SetAnim(0x6b, 0x40000000, 0x1000, 0);
        mStateStep = 9;
        goto done;
    }
    this->SetAnim(0x47, 0, 0x1000, 0);
    mStateStep = 3;

done:
    return 0;
}

/* daPly_c::func_ov002_020c9718() at 0x020c9718
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */
void daPly_c::func_ov002_020c9718() {
    unsigned char *self = (unsigned char *)this;
    int r2;
    if (mIsUnderwater == 0)
        return;
    mVertSpeed = func_ov002_020ceaf4();
    r2 = data_0209f32c - 0x50000;
    if (mPosY < r2)
        return;
    if (mVertSpeed >= 0)
        mPosY = r2;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_NoControl_Init() {
    extern PlayerStateFn data_ov002_02110884[];
    mIsControlDisabled = 1;
    mStatePhase = 0;
    DisableInteraction();
    ReleaseHeldActor();
    mRidingShell = 0;
    mHeldObj = 0;
    mGrabbedByActor = 0;
    mIsOpeningBigDoor = 0;
    (this->*data_ov002_02110884[mNoCtrlKind].voidPmf)();
    return 1;
}

void daPly_c::func_ov002_020c97e0() {
    mStateStep = 19;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
}

void daPly_c::func_ov002_020c97f8() {
    mStateStep = 0x1a;
    mAngleYSpeed = 0;
    this->SetAnim(0x47, 0, 0x1000, 0);
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
}

void daPly_c::func_ov002_020c9840() {
    mHasNoCap = 0;
    mStateStep = 0x18;
    this->SetAnim(0x47, 0, 0x1000, 0);
    mStateTimer = 2;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    mStateArg = 0;
    Sound::PlayCharVoice(mCharacter, 4, mCamSpacePos);
}

void daPly_c::func_ov002_020c98a4() {
    extern void *gCamera[];
    mHasNoCap = 0;
    mStateStep = 0x18;
    this->SetAnim(0x47, 0, 0x1000, 0);
    mStateTimer = 2;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    mStateArg = 0;
    ((::dCamera_c *)gCamera[0])->func_0200d148(mPlayerNo);
}

void daPly_c::func_ov002_020c990c() {
    typedef int Fix12;
    this->mHasNoCap = 1;
    if (this->mIsAirborne) {
        this->mStateStep = 0x16;
        this->SetAnim(0x54, 0x40000000, 0x1000, 0);
    } else {
        this->mStateStep = 0x17;
        this->SetAnim(0x98, 0x40000000, 0x1000, 0);
    }
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    Sound::PlayCharVoice(this->mCharacter, 0x29, this->mCamSpacePos);
}

/* recovered: shared common types */

void daPly_c::func_ov002_020c9998() {
    typedef int Fix12i;
    this->mStateStep = 0x12;
    this->SetAnim(0x92, 0x40000000, 0x1000, 0);
    dActor_c::Spawn(0xb2, 0xffff, this->mPos, &this->mAngle, this->mAreaId, -1);
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
}

/* recovered: shared common types */
void daPly_c::func_ov002_020c9a04() {
    Vector3 v;

    mStateStep = 0x11;
    this->SetAnim(0x9a, 0x40000000, 0x1000, 0);

    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;

    v.x = this->mPosX;
    v.y = this->mPosY;
    v.z = this->mPosZ;
    v.y = this->mPosY + 0x28000;
    Particle::System::NewSimple(kPtclMegaGrowStartA, v.x, v.y, v.z);

    v.y = this->mPosY + FX_MUL(this->mScaleY, 0x50000);
    Particle::System::NewSimple(kPtclMegaGrowStartB, v.x, v.y, v.z);
}

void daPly_c::func_ov002_020c9ac0() {
    mRootMotionMode = 1;
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    this->mVertAccel = 0;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
    mStateStep = 0x10;
    this->SetAnim(0x93, 0x40000000, 0x1000, 0);
}

int daPly_c::func_ov002_020c9b10() {
    mStateStep=0xe;
    this->SetAnim(0x47, 0, 0x1000, 0);
    mStateTimer=0x14;
    this->mHorzSpeed=0;
    this->mVertSpeed=0;
    return 0;
}

void daPly_c::func_ov002_020c9b5c() {
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    this->mVertAccel = 0;
    func_ov002_020c9b10();
}

void daPly_c::func_ov002_020c9b7c() {
    typedef int Fix12i;
    this->mStateStep = 0xb;
    this->mIsBodyClsnEnabled = 0;
    if (this->mIsAirborne) {
        this->SetAnim(0x56, 0, 0x1000, 0);
        Sound::PlayBank0(6, this->mCamSpacePos);
    } else {
        this->SetAnim(0x4f, 0x40000000, 0x1000, 0);
        this->func_ov002_020e25f0(0);
    }
    this->mVertAccel = 0;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
}

int daPly_c::func_ov002_020c9c00() {
    if(mIsAirborne!=0){
        this->SetAnim(0x54, 0x40000000, 0x1000, 0);
    }
    mStateStep=0;
    this->mHorzSpeed=0;
    this->mVertSpeed=0;
    return 0;
}

void daPly_c::func_ov002_020c9c4c() {
    typedef int Fix12i;
    this->SetAnim(data_ov002_020ff118[mStateStep], 0x40000000, 0x1000, 0);
    this->mStateStep += 7;
    mRootMotionMode = 1;
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    this->mVertAccel = 0;
    this->mHorzSpeed = 0;
    this->mVertSpeed = 0;
}

void daPly_c::func_ov002_020c9cc0() {
    mStateWaitTimer = 3;
    mStateStep = 6;
    mSkipGroundUpdate = 1;
    mIsBodyClsnEnabled = 0;
    if (this->mVertSpeed >= 0x14000)
        this->mVertSpeed = 0x14000;
    if (gNextSublevel == 0x26 || gNextSublevel == 0x12)
        mOpacity = 0;
}

int daPly_c::func_ov002_020c9d1c() {
    mStateStep=4;
    mAngleYSpeed=0x4000;
    this->SetAnim(0x47, 0, 0x1000, 0);
    this->mHorzSpeed=0;
    this->mVertSpeed=0;
    return 0;
}

void daPly_c::func_ov002_020c9d68() {
  if(mIsAirborne == 0){
      int b = (mRidingShell != 0);
      // a flag in front of one SetAnim diffs 7 words from +0x10; duplicating SetAnim grows 0x7c -> 0x98
      if (b == 0)
          goto tail;
  }
  this->SetAnim(0x54, 0x40000000, 0x1000, 0);
tail:
  this->mAngleY = GetAngleToCamera(mPlayerNo);
  this->mPrevAngleY = this->mAngleY;
  mStateStep = 0;
  mStateArg = 0;
  this->mHorzSpeed = 0;
}

extern "C" {

void func_ov002_020c9de4(char* c){
    ((daPly_c *)(c))->func_ov002_020bdd9c();
    ((daPly_c *)(c))->func_ov002_020bdef0();
    ((daPly_c *)(c))->func_ov002_020bdd2c();
    ((daPly_c *)c)->func_ov002_020d80d0();
    ((daPly_c *)(c))->func_ov002_020c9d68();
}
}

void daPly_c::func_ov002_020c9e18() {
    if (mIsNoControl != 1) return;
    mIsNoControl = 0;
    this->mdCcAcPos_c.flags &= ~4;
}

void daPly_c::DisableInteraction() {

    mIsNoControl = 1;
    mdCcAcPos_c.flags |= 4u;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::Unk_020c9e5c(unsigned char h) {
    if (!IsState(data_ov002_0211022c))
        return 0;
    if (*(unsigned char *)((char *)&mNoCtrlKind) != h)
        return 0;
    switch (h) {
    case 0:
    case 3: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if (s == 1)
            return 1;
    } break;
    case 0x11: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if (s == 0x18)
            return 1;
    }
        /* fallthrough */
    case 1:
    case 2: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if (s == 2 || (unsigned char)(s + 0xec) <= 1)
            return 1;
    } break;
    case 4: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if (s == 3)
            return 1;
    } break;
    case 5: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if ((unsigned char)(s + 0xfc) <= 1)
            return 1;
    } break;
    case 6: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if (s == 6)
            return 1;
    } break;
    case 7: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if ((unsigned char)(s + 0xf9) <= 1)
            return 1;
    } break;
    case 8: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if (s == 9 || (unsigned char)(s + 0xec) <= 1)
            return 1;
    } break;
    case 9: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if ((unsigned char)(s + 0xf5) <= 2)
            return 1;
    } break;
    case 0xa:
    case 0xc: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if ((unsigned char)(s + 0xf2) <= 1)
            return 1;
    } break;
    case 0xb: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if (s == 0x10)
            return 1;
    } break;
    case 0xd: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if (s == 0x11)
            return 1;
    } break;
    case 0xe: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if (s == 0x12)
            return 1;
    } break;
    case 0xf: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if ((unsigned char)(s + 0xea) <= 1)
            return 1;
    } break;
    case 0x10: {
        unsigned char s = *(unsigned char *)((char *)&mStateStep);
        if ((unsigned char)(s + 0xe8) <= 1)
            return 1;
    } break;
    }
    return 0;
}

extern "C" {

int func_ov002_020ca0f4(daPly_c *player) {
    return player->IsState(data_ov002_0211022c);
}
}

void daPly_c::func_ov002_020ca108() {

    if (mIsOpeningBigDoor) {
        mStateStep = 0x13;
        return;
    }
    this->ChangeState(data_ov002_0211013c);
}

/* recovered: named members + shared header, real C++ method */
/* daPly_c::OpenBigDoor() at 0x020ca144 (ov002).
 * Sets a u8 flag at offset 0x70b (1803) in the daPly_c object to 1.
 * daPly_c.h has only `u8 unk70b` here, so keep the typed offset access.
 */
void daPly_c::OpenBigDoor() {
    mIsOpeningBigDoor = 1; /* +0x70b */
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::Unk_020ca150(unsigned char a) {
    if (IsState(data_ov002_0211022c)) {
        if (a == 4) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        if (a == 5)
            *(unsigned char *)((char *)&mStateStep) = 5;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::SetNoControlState(unsigned char a_, int b, unsigned char c_) {

    int a = (int)a_;
    u8 c = c_;

    u8 flag;

    if (this->IsState(data_ov002_0211010c) || this->IsState(data_ov002_02110124)) return 0;

    flag = 0;
    if (a <= 3) {
        if (mIsNoControl != 0) return 0;
        flag = c;
    } else {
        if (mIsNoControl != 0) {
            if (c == 0) return 0;
        }
    }

    mNoCtrlKind = a;
    mAttachOffsetY = b;
    this->ChangeState(data_ov002_0211022c);
    mStatePhase = flag;
    return 1;
}

int daPly_c::func_ov002_020ca270() {
    if (IsState(data_ov002_02110124)) {
        if (mStateStep == 1)
            return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method
 *
 * Refuses while airborne and still rising, then takes control, records the
 * cage position in mStateTarget and turns the player to face away from it.
 */

int daPly_c::JumpIntoBooCage(Vector3 &v_) {
    Vector3 *v = &v_;
    if (mIsAirborne) {
        if (mVertSpeed < 0) return 0;
    }
    if (!this->SetNoControlState(9, -1, 0))
        return 0;
    mStateTargetX = v->x;
    mStateTargetY = v->y;
    mStateTargetZ = v->z;
    mAngleY = Vec3_HorzAngle(v, &mPos) + 0x8000;
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::TryTalkToDoor(unsigned char a) {
    if (this->IsState(data_ov002_0211013c) || this->IsState(ST_WAIT) ||
        this->IsState(data_ov002_0211043c)) {
        *(unsigned char *)((char *)&mStateArg) = a;
        this->SetNoControlState(0xe, -1, 0);
        return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::IsOpeningDoorWithStar() {
    return this->Unk_020c9e5c(0xe) || this->GetTalkState() > 0;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::TryTalkToKeyDoor() {
    if (this->IsState(data_ov002_0211013c) || this->IsState(ST_WAIT) ||
        this->IsState(data_ov002_0211043c)) {
        this->SetNoControlState(0xb, -1, 0);
        return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */
/* daPly_c::Unk_020ca488() @ 0x20ca488 (ov002) -- tail-call veneer to daPly_c::Unk_020c9e5c(unsigned char) (0x20c9e5c) with r1=11.
 */
int daPly_c::Unk_020ca488() {
    return this->Unk_020c9e5c(11);
}

/* recovered: named members + shared header, real C++ method
 *
 * Only from the walking, waiting or crouching states. Takes control, records
 * the door position in mStateAnchor and derives the approach point in
 * mStateTarget from it, and turns the player to face the door.
 */

int daPly_c::TryEnterStarDoor(Vector3 &pos_, short kind) {
    int *pos = (int *)&pos_;
    if (IsState(data_ov002_0211013c) || IsState(daPly_c::ST_WAIT) || IsState(data_ov002_0211043c)) {
        if (this->SetNoControlState(0xa, -1, 0)) {
            mStateAnchorX = pos[0];
            mStateAnchorY = pos[1];
            mStateAnchorZ = pos[2];
            mAngleXSpeed = kind;
            mStateArg = 0;
            /* First entry constructs it: guard, three stores, ~Vector3() registration. */
            static Vector3 sStarDoorOff(0, 0, 0x64000);
            Vec3_RotateYAndTranslate((Vector3 *)&mStateTargetX, (const Vector3 *)&mStateAnchorX,
                                     mAngleXSpeed, &sStarDoorOff);
            mAngleY = Vec3_HorzAngle(&pos_, &mPos) + 0x8000;
            return 1;
        }
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */
// daPly_c::CanEnterDoor(unsigned char door)
// The +0x48 vcall on the held actor is dActor_c::OnYoshiTryEat (slot 18), and
// it must be spelled as a real C++ virtual call: the C function-pointer cast
// perturbs callee-saved homing rank to this=r4/door=r5, while virtual dispatch
// keeps the natural reverse-arg homing (door=r4, this=r5) the ROM has. See
// notes/mwccarm-codegen.md 6bc.
int daPly_c::CanEnterDoor(unsigned char door) {
    if (IsState(data_ov002_0211022c)) {
        if (!(mStateStep == 0x13 && mIsOpeningBigDoor != 0))
            return 0;
    }
    if (IsState(data_ov002_0211013c) || IsState(daPly_c::ST_WAIT) || IsState(data_ov002_0211022c) ||
        IsState(data_ov002_0211043c)) {
        /* Yoshi with something in his mouth cannot take a door. If its
           OnYoshiTryEat() code is 6 (enemies such as daKrb_c and daSpd_c,
           and the small crate daObjBlockS_c) or 1 (daBttBk_c, or another
           daPly_c), he spits it out first; any other code (4 for a coin, a
           cap, a key or a star; 5; ...) just refuses the door. */
        if (mObjInMouth != 0) {
            if (mObjInMouth->OnYoshiTryEat() == 6 || mObjInMouth->OnYoshiTryEat() == 1) {
                this->ChangeState(data_ov002_0211004c);
            }
            return 0;
        }
        mStateStep = door;
        if (this->SetNoControlState(7, -1, 1) != 0)
            return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */
void daPly_c::PlayMammaMiaSound() {
    /* mCamSpacePosX/Y/Z are three consecutive words; dActor_c.h declares them
       individually rather than as one Vector3. */
    Sound::PlayCharVoice(mCharacter, 0x27, mCamSpacePos);
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::TryExitWhiteDoorWithStar() {
    if (IsState(data_ov002_0211022c)) {
        if (*(unsigned char *)((char *)&mStateStep) != 0x13 ||
            *(unsigned char *)((char *)&mIsOpeningBigDoor) == 0)
            return 0;
    }
    return this->SetNoControlState(0x11, -1, 1) != 0;
}

extern "C" {

int func_ov002_020ca78c(daPly_c *player) {
    if (player->IsState(data_ov002_0211022c)) {
        if (player->mStateStep != 0x13 || player->mIsOpeningBigDoor == 0)
            return 0;
    }
    return player->SetNoControlState(0x10, -1, 1) != 0;
}
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_DebugFly_Main() {
    extern ControllerInput data_0209f498[];
    extern PadData data_020a0e58[];
    u32 idx;
    u16 flags;

    mHorzSpeed = 0;
    mVertSpeed = 0;
    mClsnFlags = 0;

    if (data_0209f498[gActivePlayerSlot].mag != 0) {
        mPrevAngleY = mDesiredAngleY;
        mHorzSpeed = func_ov002_020bf224(0x50000, 0);
    }

    idx = gActivePlayerSlot;
    flags = data_0209f498[idx].held;
    if (flags & 2) {
        mPosY += 0x28000;
    } else if (flags & 1) {
        mPosY -= 0x28000;
    }

    if (data_020a0e58[idx].pressed & 0x400) {
        this->ChangeState(data_ov002_0211013c);
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_DebugFly_Init() {
    DisableInteraction();
    mHorzSpeed = 0;
    mVertSpeed = 0;
    mVertAccel = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_0211064c;
extern daPly_c::State data_ov002_02110664;
}

int daPly_c::Unk_020ca8f8() {
    if (IsState(data_ov002_0211064c))
        return 1;
    if (IsState(data_ov002_02110664))
        return 2;
    return 0;
}

void daPly_c::func_ov002_020ca940() {
    extern u8 gGameMode;
    extern u8 data_0209f49e[];
    extern dCamera_c *gCamera;
    u8 old;
    dCamera_c *h;
    int b;

    if (mIsNoControl != 0)
        return;

    b = gGameMode;
    b = b == 1;
    if (b == false) {
        if (!(data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE))
            return;
    }

    if (*(u16*)(data_0209f49e + (u8)gActivePlayerSlot * 0x18) & 0x8000) {
        old = mUseFarCamera;
        h = gCamera;
        (mUseFarCamera)++;

        if (mUseFarCamera > 2) {
            mUseFarCamera = 0;
        }

        switch (mUseFarCamera) {
        case 1:
            if (h->func_0200d0ac(mPlayerNo) != 0)
                break;
            mUseFarCamera = 2;
        case 2:
            if (mIsMega == 0) {
                b = gGameMode;
                b = b == 1;
                if (b == false) {
                    if (IsState(data_ov002_0211013c) || IsState(data_ov002_0211043c) ||
                        IsState(daPly_c::ST_WAIT)) {
                        mStateFlags |= 4;
                        break;
                    }
                }
            }
            if (old == 0) {
                func_02012790(0xe);
                mUseFarCamera = old;
                break;
            }
            mUseFarCamera = 0;
        case 0:
            h->func_0200d064(mPlayerNo);
            if ((u16)(mStateFlags & 4) || IsState(data_ov002_0211064c)) {
                mUseFarCamera = 2;
            }
            break;
        default:
            mUseFarCamera = 0;
            h->func_0200d064(mPlayerNo);
            break;
        }
    }

    if (IsState(data_ov002_0211013c))
        return;
    if (IsState(daPly_c::ST_WAIT))
        return;
    if (IsState(data_ov002_0211043c))
        return;
    if (IsState(data_ov002_0211064c))
        return;
    if (IsState(data_ov002_02110664))
        return;

    if ((u16)(mStateFlags & 4)) {
        mStateFlags &= ~4;
    }
}

void daPly_c::func_ov002_020cabe0() {

    unsigned short v = (unsigned short)(mStateFlags & 4);
    if (v == 0)
        return;
    ChangeState(data_ov002_0211064c);
}

/* recovered: named members + shared header, real C++ method */
/* daPly_c::St_Null_Cleanup() at 0x020cac20 (ov002).
 * Cleanup hook of the ST_NULL do-nothing player state; returns VS_FAIL (1).
 * The implicit daPly_c* this is unused.
 */
s32 daPly_c::St_Null_Cleanup() {
    this;
    return 1;
}

/* recovered: named members + shared header, real C++ method */
/* daPly_c::St_Null_Main() at 0x020cac28 (ov002).
 * Main hook of the ST_NULL do-nothing player state; returns VS_FAIL (1).
 * (All ST_NULL state functions are "return 1;" per daPly_c.h.)
 */
s32 daPly_c::St_Null_Main() {
    this;
    return 1;
}

/* recovered: shared header, real C++ method
 *
 * ov002, 0x020cac30, size 0x8 -- `mov r0, #1; bx lr` and nothing else.
 *
 * The whole body IS the evidence for the name. daPly_c::State holds three
 * `int (daPly_c::*)()` members, ChangeState calls mInit last on the state
 * being entered, and a handler returns 1 for success. A state called Null
 * whose Init does nothing but succeed is exactly eight bytes. ov002 reaches
 * this address by `kind:load ... module:overlay(2)` from 0x0210a17c, which
 * sits in the eight-byte-stride pointer-to-member run that holds those
 * State objects.
 *
 * This file carried the placeholder name func_ov002_020cac30 until the symbol
 * was moved here from ov006, where it had been sitting on an unrelated 0x6c
 * function at the same shared address. See the commit and include/daPly_c.h.
 */
int daPly_c::St_Null_Init() {
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_CameraZoom_Cleanup() {
    *(unsigned short *)((char *)&mStateFlags) &= ~4;
    mUseFarCamera = 0;
    return 1;
}

int daPly_c::func_ov002_020cac60() {
    short ang;
    short angle;
    if (mSurfaceType == 0xd && NumStars() >= 0xe) {
        ang = mHeadRot[2];
        angle = GetAngleToCamera(mPlayerNo);
        if (ang <= 0x1c00 && angle >= -0x1000 && angle <= 0x1000) {
            LoadLevel(0x1f, 0, 1, 0, 0xe);
            dScene_c::SetAndStopColorFader();
            gSceneColorFader.color = 0x7fff;
            func_02012790(0x1e);
            return 1;
        }
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method
 *
 * The zoomed-in camera pose. Runs only while gNextSublevel is negative. Losing
 * the 0x4 state flag ends it: back to walking, the body model's animation
 * rewinds to frame 0, and the facing angle is latched. Landing, or
 * any of the 0x8f03 input bits, clears that flag.
 */

int daPly_c::St_CameraZoom_Main() {
    extern unsigned short data_0209f49e;
    if (gNextSublevel < 0) {
        if ((unsigned short)(mStateFlags & 4) == 0) {
            this->ChangeState(data_ov002_0211013c);
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
            anim.currFrame = 0;
            mPrevAngleY = mAngleY;
            return 1;
        }
        if (mIsAirborne) {
            mStateFlags &= ~4;
        }
        func_ov002_020cae10();
        if (*(unsigned short*)((char*)&data_0209f49e + gActivePlayerSlot * 0x18) & 0x8f03) {
            mStateFlags &= ~4;
        }
        func_ov002_020cac60();
    }
    AdvanceAnims();
    return 1;
}

void daPly_c::func_ov002_020cae10() {
    typedef int Fix12i;
    extern void *gCamera;
    int r4 = 0x4000;
    int ang;
    int r8;
    if (mPlayerNo == gLocalPlayerId) {
        r4 -= ((dCamera_c *)gCamera)->mAngleX;
        r4 = (short)r4;
    }
    ang = (short)((mAngleY - GetAngleToCamera(mPlayerNo)) + 0x8000);
    r8 = ang;
    if (r8 < (int)0xffffd556)
        r8 = (int)0xffffd556;
    if (r8 > 0x2aaa)
        r8 = 0x2aaa;
    mHeadRot[1] = (short)FX_MUL(r8, 0xc00);
    mHeadRot[2] = (short)FX_MUL(r4, 0xc00);
    mHeadRotMode = 2;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_CameraZoom_Init() {
    extern dCamera_c *gCamera;
    mHorzSpeed = 0;
    mVertSpeed = 0;
    dCamera_c *cam = gCamera;
    int pid = mPlayerNo;
    cam->func_0200d064(pid);
    cam->SetCameraZoomState(mPlayerNo);
    this->SetAnim(0x47, 0, 0x1000, 0);
    data_0209f28c = 0;
    return 1;
}

void daPly_c::ClearClimbCollider() {
    mClimbClsn = 0;
    mdCcAcPos_c.flags &= ~4u;
    mdCcAcPos_c.flags |= 8u;
    func_ov002_020e63a4();
}

/* dCc_c's player-hit callback (func_02014f5c) offers the player the collider
 * it touched. Refuse while the climb cooldown runs or the player is riding a
 * shell, already climbing, holding something, taking damage, out of control,
 * underwater or mega, and refuse a collider carrying flag 0x400000. Otherwise
 * store it in mClimbClsn and enter the climb state, first lifting a player
 * more than 100 units below the collider's base to 100 below it. The name is
 * coined. */

int daPly_c::StartClimb(dCc_c *clsn) {
    int b354, b358;

    if (IsState(data_ov002_0211013c) == 0 && mClimbCooldown == 0) {
        b354 = (mRidingShell != 0);
        if (b354 == 0 && mClimbClsn == 0) {
            b358 = (mHeldObj != 0);
            if (b358 == 0 && mIsTakingDamage == 0 && mIsNoControl == 0 && mIsUnderwater == 0 &&
                mIsMega == 0) {
                // structured returns grow the function 0x1c4 -> 0x1e0
                goto cont;
            }
        }
    }
    return 0;

cont:
    if (IsInAir() != 0)
        goto state_check;
    if (IsBeingShotOutOfCannon() != 0)
        goto state_check;
    if (IsState(data_ov002_021105bc) != 0)
        goto state_check;
    if (IsState(data_ov002_0211031c) != 0)
        goto state_check;

    if (func_ov002_020e0478() == 0)
        goto ret0a;

state_check:
    if (IsState(data_ov002_021101b4) != 0) {
        if (func_ov002_020e3078(&data_ov002_021106dc) != 0)
            return 0;
    }

    if (clsn->flags & 0x400000)
        return 0;

    {
        Vector3 &pos = clsn->GetPos();
        if (mPosY - pos.y < -0x64000)
            mPosY = pos.y - 0x64000;
    }

    mClimbClsn = clsn;
    ChangeState(data_ov002_021106dc);
    return 1;

ret0a:
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Headstand_Main() {
    extern ControllerInput data_0209f498[];
    Vector3 &r = mClimbClsn->GetPos();
    mPosX = r.x;
    mPosZ = r.z;
    mPosY = r.y + mClimbClsn->height;

    u8 st = mStateStep;
    switch (st) {
    case 2:
        if (this->FinishedAnim()) {
            this->SetAnim(0x1d, 0, 0x1000, 0);
            mStateStep = 3;
        }
        break;
    case 3: {
        s16 val = data_0209f498[gActivePlayerSlot].ny;
        u16 mag = data_0209f498[gActivePlayerSlot].held;
        if (val > 0x200) {
            this->SetAnim(0x1f, 0x40000000, 0x1000, 0);
            mStateStep = 4;
        }
        func_ov002_020cc05c(mag);
        mAngleY += mAngleYSpeed;
        if (data_0209f498[gActivePlayerSlot].pressed & 2) {
            mPrevAngleY = mAngleY;
            this->ChangeState(data_ov002_02110724);
            return 1;
        }
        break;
    }
    case 4:
        if (this->FinishedAnim()) {
            this->ChangeState(data_ov002_021106dc);
        }
        break;
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Headstand_Init() {
    extern struct CameraRaw *gCamera;
    this->SetAnim(0x1e, 0x40000000, 0x1000, 0);
    mStateStep = 2;
    mVertAccel = 0;
    mHorzSpeed = 0;
    mVertSpeed = 0;
    ((::dCamera_c *)gCamera)->SetHeadstandState(mPlayerNo);
    mHideShadow = 1;
    return 1;
}

/* recovered: shared common types */
int daPly_c::func_ov002_020cb354() {
    unsigned char *p = (unsigned char *)this;
    daPly_c *player = (daPly_c *)p;
    int a = IsAnim(0x24);
    if (a == 0) return a;
    dExtFrameCtrl_c &anim = *player->mModels.mBodyModels[GetBodyModelID(player->param1 & 0xff, 0)];
    if ((int)((unsigned int)(anim.currFrame << 4) >> 0x10) < 8) {
        unsigned char f = player->mStateArg;
        if (f != 0) return f;
        if (player->mClimbClsn->flags & 0x1000000)
            Sound::PlayBank0(0xd, player->mCamSpacePos);
        else
            Sound::PlayBank0(0xb0, player->mCamSpacePos);
        player->mStateArg = 1;
        return 1;
    }
    player->mStateArg = 0;
    return 0;
}

int daPly_c::func_ov002_020cb400() {
    int v = mAngleYSpeed / 16;
    if (v >= 0x100)
        v = 0x100;
    int code = 0x107;
    if (mClimbClsn->flags & 0x1000000)
        code = 0x105;
    int r = func_02012194(mLoopingSoundHandle, 0, code, 3, v, &mCamSpacePos, 0);
    mLoopingSoundHandle = r;
    return r;
}

void daPly_c::func_ov002_020cb474() {
    dCc_c *r1 = mClimbClsn;
    if ((r1->flags & 0x1000000) == 0)
        return;
    if ((r1->height >> 2) > mAttachOffsetY)
        return;
    Vector3 v(mPosX, mPosY + 0x32000, mPosZ);
    unsigned int id;
    switch (gCurrentSublevel) {
    case 0x13:
    case 0xa:
        id = 0xba;
        break;
    case 6:
        id = 0xb8;
        break;
    case 0x10:
        return;
    default:
        id = 0xb9;
        break;
    }
    mParticle1 = (s32)Particle::System::New(mParticle1, id, v.x, v.y, v.z, 0, 0);
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Climb_Cleanup() {
    if (mRequestedState != &data_ov002_021106dc && mRequestedState != &data_ov002_021106f4) {
        ClearClimbCollider();
        mClimbCooldown = 8;
    }
  mJumpComboTimer = 0;
  return 1;
}

/* daPly_c::St_Climb_Main (ov002:0x020cb5bc, 0x778).
 *
 * func_ov002_020cc05c takes the pad-held halfword as an argument it never
 * reads (the matched St_Headstand_Main passes it too). Dropping it recolors the whole
 * case-1/tail scratch web down one register (r2 -> r1, 9 words); passing it
 * keeps the data_0209f49c load live in r1 through both call sites, which is
 * the ROM's coloring.
 *
 * SetAnim retains its scalar Fix12 ABI declaration. The climbed object's
 * position comes from mClimbClsn->GetPos() (dCc_c's slot 2). daPly_c fields
 * and state changes use the shared class interface where their types are
 * established.
 */

int daPly_c::St_Climb_Main() {
    extern void *gCamera;
    extern ControllerInput data_0209f498[];
    s32 vbuf[6];

    mPeakY = mPosY;

    {
        Vector3 &p = mClimbClsn->GetPos();
        vbuf[0] = p.x;
        vbuf[1] = p.y;
        vbuf[2] = p.z;
    }
    if (param1 == 2) {
        vbuf[4] = 0;
        vbuf[3] = 0;
        vbuf[5] = -0x10000;
        Vec3_RotateYAndTranslate((Vector3 *)&vbuf[0], &mClimbClsn->GetPos(), mAngleY,
                                 (const Vector3 *)&vbuf[3]);
    }
    mPosX = vbuf[0];
    mPosZ = vbuf[2];

    {
        void *cam = gCamera;
        if (mClimbClsn->flags & 0x1000000) {
            ((::dCamera_c *)cam)->func_0200d5c0(mPlayerNo);
        } else {
            s32 v = FX_MUL(mClimbClsn->height, 0xb33);
            if (v <= mAttachOffsetY) {
                ((::dCamera_c *)cam)->func_0200d5c0(mPlayerNo);
            } else {
                ((::dCamera_c *)cam)->func_0200d5fc(mPlayerNo);
            }
        }
    }

    switch (mStateStep) {
    case 0:
        ApproachAngle(&this->mAngleYSpeed, 0, 8, 0x2000, 0x200);
        if (FinishedAnim()) {
            this->SetAnim(0x26, 0, 0x1000, 0);
            mStateStep = 1;
        }
        break;

    case 1: {
        s16 v;
        u16 flagsE = data_0209f498[gActivePlayerSlot].pressed;
        u16 flagsC = data_0209f498[gActivePlayerSlot].held;
        if (flagsE & 0x400) {
            s16 ang = (s16)(mAngleY + 0x8000);
            s32 t = (u16)ang;
            s32 ti = (t >> 4) * 2;
            s16 cosv = FX_SinCosTable_[ti];
            mPosX += FX_MUL(cosv, 0x64000);
            s16 sinv = FX_SinCosTable_[ti + 1];
            mPosZ += FX_MUL(sinv, 0x64000);
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }

        {
            v = data_0209f498[gActivePlayerSlot].ny;
            if (v < -0x400) {
                func_ov002_020cb354();
                mAngleYSpeed = 0;
                ApproachLinear(this->mAngleY, GetAngleToCamera(mPlayerNo), 0x200);
                if (mClimbClsn->height <= mAttachOffsetY) {
                    if ((mClimbClsn->flags & 0x2000000) && mVertSpeed == 0 &&
                        (s32)(data_0209f498[gActivePlayerSlot].ny) < -0xc00 && mStateWork == 1) {
                        this->ChangeState(data_ov002_021106f4);
                    }
                    mAttachOffsetY = mClimbClsn->height;
                    mVertSpeed = 0;
                } else {
                    s32 mag;
                    if (data_0209f498[gActivePlayerSlot].touching == 0) {
                        mag = (data_0209f498[gActivePlayerSlot].held & 0x800) ? 0xc000 : 0x6660;
                    } else {
                        mag = 0xc000;
                    }
                    s16 av = data_0209f498[gActivePlayerSlot].ny;
                    if (av < 0)
                        av = -av;
                    s32 fx = FX_MUL(mag, av);
                    func_ov002_020bf340(&this->mVertSpeed, 0x800, fx);
                    this->SetAnim(0x24, 0, 0x1000, 0);
                    s32 fx2 = FX_MUL(fx, 0x500);
                    if (fx2 >= 0x4000)
                        fx2 = 0x4000;
                    {
                        dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID((u8)param1, 0)];
                        anim.speed = fx2;
                    }
                    mStateWork = 0;
                }
            } else if (v > 0x200) {
                func_ov002_020cb400();
                mStateTimer = 0x1e;
                {
                    s32 mag;
                    if (data_0209f498[gActivePlayerSlot].touching == 0) {
                        mag = (data_0209f498[gActivePlayerSlot].held & 0x800) ? 0x1e000 : 0xfff0;
                    } else {
                        mag = 0x1e000;
                    }
                    s16 val = data_0209f498[gActivePlayerSlot].ny;
                    s32 fx = FX_MUL(mag, val);
                    func_ov002_020bf340(&this->mVertSpeed, -0x800, fx);
                }
                mAngleYSpeed -= 0x40;
                {
                    s16 av = data_0209f498[gActivePlayerSlot].ny;
                    if (av < 0)
                        av = -av;
                    if (mAngleYSpeed <= av)
                        av = mAngleYSpeed;
                    mAngleYSpeed = av;
                }
                if (!IsAnim(0x25)) {
                    this->SetAnim(0x25, 0, 0x1000, 0);
                }
                if (mAttachOffsetY < -0x64000) {
                    this->ChangeState(data_ov002_021101b4);
                    return 1;
                }
            } else {
                mStateTimer = 0x1e;
                mVertSpeed = 0;
                func_ov002_020cc05c(flagsC);
                if (!IsAnim(0x26)) {
                    this->SetAnim(0x26, 0, 0x1000, 0);
                }
            }
        }
        if (mIsAirborne == 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        if (data_0209f498[gActivePlayerSlot].pressed & 2) {
            mPrevAngleY = mAngleY;
            this->ChangeState(data_ov002_0211070c);
            return 1;
        }
        break;
    }

    default:
        break;
    }

    {
        s32 y = mClimbClsn->GetPos().y;
        if ((mPosY - y) < -0x64000) {
            mPosY = y - 0x64000;
        }
    }
    if (mAttachOffsetY < -0x64000) {
        mAttachOffsetY = -0x64000;
    }
    {
        s32 t2 = mClimbClsn->height;
        if (t2 <= mAttachOffsetY) {
            mAttachOffsetY = t2;
        }
    }
    if (mJumpComboTimer != 0 || (mVertSpeed != 0 && mStateStep == 1)) {
        func_ov002_020cb474();
    }
    {
        s16* p8e = &this->mAngleY;
        *p8e = (s16)(*p8e + mAngleYSpeed);
    }
    {
        s32* p688 = &this->mAttachOffsetY;
        *p688 += mVertSpeed;
    }
    if (func_ov002_020cbea8()) {
        return 1;
    }
    {
        mPosY = mAttachOffsetY + mClimbClsn->GetPos().y;
    }
    if (data_0209f498[gActivePlayerSlot].ny == 0) {
        mStateWork = 1;
    }
    AdvanceAnims();
    return 1;
}

int daPly_c::St_Climb_Init() {
    ReleaseHeldActor();

    int *bitfield = (int *)&mdCcAcPos_c.flags;
    *bitfield |= 4;
    *bitfield &= ~8;

    if (mHorzSpeed <= 0xa000) {
        mAngleYSpeed = 0;
        this->SetAnim(0x26, 0, 0x1000, 0);
        mStateStep = 1;
    } else {
        int val = mHorzSpeed / 16;
        mAngleYSpeed = (short)val;
        if (mAngleYSpeed >= 0x3000)
            mAngleYSpeed = 0x3000;
        this->SetAnim(0x27, 0x40000000, 0x1000, 0);
        mStateStep = 0;
    }

    Vector3 &res = mClimbClsn->GetPos();

    mPosX = res.x;
    mPosZ = res.z;
    mAttachOffsetY = mPosY - res.y;
    mVertAccel = 0;
    mHorzSpeed = 0;
    mVertSpeed = 0;

    Sound::PlayCharVoice(mCharacter, 0xd, mCamSpacePos);

    mStateWork = 0;
    mWalkTimer = 3;
    this->mdCcAcPos_c.height = 0x5a000;
    mHideShadow = 1;

    {
        dCc_c *b = mClimbClsn;
        if (b->flags & 0x1000000) {
            if ((b->height >> 2) <= this->mAttachOffsetY) {
                if (func_ov002_020e3078(&data_ov002_021106f4) == 0) {
                    mJumpComboTimer = 0xf;
                }
            }
        }
    }
    return 1;
}

#define LA(p) ((int)(p))

int daPly_c::func_ov002_020cbea8() {
    daPly_c *o;
    int amt;
    int b;
    int ca8;
    int *p;
    int t;

    if (mdCcAcPos_c.otherOwner == 0)
        return 0;
    if ((mdCcAcPos_c.hitFlags & 0x400000) == 0)
        return 0;
    o = (daPly_c *)dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
    if (o == 0 || o == this)
        return 0;
    b = (o->actorID == 0xbf);
    if (b == 0) return 0;
    if (mClimbClsn == 0 || o->mClimbClsn == 0)
        return 0;
    if (func_ov002_020cc01c() != 0 || o->func_ov002_020cc01c() != 0)
        return 1;
    ca8 = mVertSpeed;
    amt = mdCcAcPos_c.pushback.y;
    if (ca8 == 0 || o->mVertSpeed == 0)
        amt = amt * 2;
    if (ca8 != 0) {
        p = &mAttachOffsetY;
        *p += amt * 2;
        mAngleYSpeed = 0;
        mVertSpeed = 0;
    }
    if (o->mVertSpeed != 0) {
        p = (int *)LA(&o->mAttachOffsetY);
        *p -= amt * 2;
        o->mAngleYSpeed = 0;
        t = o->mSpeedZ;
        o->mVertSpeed = 0;
        o->mSpeedZ = t;
    }
    return 0;
}

#undef LA

int daPly_c::func_ov002_020cc01c() {
    if (mWalkTimer == 0)
        return 0;
    ChangeState(data_ov002_021101b4);
    return 1;
}

void daPly_c::func_ov002_020cc05c(unsigned short) {
    int idx = gActivePlayerSlot * 0x18;
    /* Through (int): plain player->mAngleYSpeed reschedules the 0x69c add and moves bytes. */
    s16 curval = *(s16 *)((int)&mAngleYSpeed);
    *(s16 *)((int)&mAngleYSpeed) =
        curval + (s16)FX_MUL(*(s16 *)((char *)data_0209f4a2 + idx), 0x80);
    {
        s16 tv2 = *(s16*)((char*)data_0209f4a2 + idx);
    {
        s16 av = tv2;
        if (av < 0) av = (s16)(-av);
        {
            int mag32 = FX_MUL(av, 0x480);
            s16 mag = (s16)mag32;
            s16 nmag = (s16)(-mag);
            s16 cur2 = mAngleYSpeed;
            s16 result;
            if (cur2 < nmag) result = nmag;
            else {
                if (cur2 <= mag) mag = cur2;
                result = mag;
            }
            mAngleYSpeed = result;
        }
    }
}
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_02110304;
}

int daPly_c::IsOnShell() {
    if (IsState(data_ov002_02110304))
        return 1;
    mRidingShell = 0;
    return 0;
}

extern "C" {

int func_ov002_020cc16c(daPly_c *player, void *state) {
    if (player->mRidingShell != 0)
        return 0;
    if (player->mIsMetal != 0 || player->mIsVanish != 0) {
        player->func_ov002_020bdef0();
        player->func_ov002_020bdd9c();
    }
    if (player->mIsFire != 0)
        player->func_ov002_020d80d0();
    player->mRidingShell = (daShl_c *)state;
    player->ChangeState(data_ov002_02110304);
    return 1;
}
}

void daPly_c::LeaveShellRide() {
    if (mRidingShell == 0)
        return;
    St_Shell_Cleanup();
    ChangeState(data_ov002_0211013c);
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Shell_Cleanup() {
    daShl_c *p = mRidingShell;
    if (p != 0) {
        p->mCarrier = 0;
        mRidingShell = 0;
        mPosY += 0x46000;
        mIsAirborne = 1;
        mLandSoundPlayed = 0;
        EndStateMusic(0x33);
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Shell_Main() {
    extern int func_ov002_020eeca8(dBgCh_Actr *, dActor_c *);
    extern ControllerInput data_0209f498[];
    int flag = 0;
    dBgPi *res;

    if (func_02035638((u8*)((char*)&mMeshClsn))) {
        flag = 1;
        res = (dBgPi *)func_0203564c(&mMeshClsn);
        if (res->GetClsnID() != -1) {
            dActor_c *a = dActor_c::FindWithID(res->GetClsnID());
            if (a) {
                u16 type = a->actorID;
                if ((type == 0x14 ? flag : 0) || (type == 0x15 ? 1 : 0)) {
                    func_ov002_020eeca8(&mMeshClsn, this);
                    flag = 0;
                }
            }
        }
    }

    if (mMeshClsn.IsOnWall()) {
        int ang = cstd::atan2(mWallNormalX, mWallNormalZ);
        if (AngleDiff(ang, mPrevAngleY) > 0x6000) {
            if (mMeshClsn.GetWallResult()->surface.clps.GetFloorClass() != 1) {
                flag = 1;
            }
        }
    }

    if (flag) {
        func_ov002_020c1eb4((s16)(mAngleY + 0x8000));
        return 1;
    }

    if (mStateStep == 0) {
        s16 target;
        int accel;
        if (data_0209f498[gActivePlayerSlot].mag != 0) {
            target = mDesiredAngleY;
            accel = 0x40000;
        } else {
            target = mAngleY;
            accel = 0x18000;
        }
        s16 angleSave = mPrevAngleY;
        ApproachAngle(&mPrevAngleY, target, 8, 0x800, 0x100);
        mAngleY = mPrevAngleY;
        func_ov002_020cc660(angleSave);
        {
            int spd = mHorzSpeed;
            int step = 0x1100;
            if (spd > 0) step = 0x600;
            ApproachLinear(mHorzSpeed, accel, step);
        }
        func_ov002_020c04e0();
        if (mIsAirborne != 0) {
            mStateStep++;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        {
            if (data_0209f498[gActivePlayerSlot].pressed & 2) {
                mStateStep++;
                mVertSpeed = (mHorzSpeed >> 2) + 0x2a000;
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                func_ov002_020e25f0(0);
                return 1;
            } else if (data_0209f498[gActivePlayerSlot].held & 0x400) {
                if (func_ov002_020d674c()) {
                    this->ChangeState(data_ov002_0211004c);
                } else {
                    mStateStep = 0;
                    this->ChangeState(data_ov002_021104e4);
                }
                return 1;
            }
        }
    } else {
        if (mIsAirborne == 0) {
            mStateStep = 0;
            mHeadRot[2] = 0x1000;
            mSpineRot[2] = mHeadRot[2];
        } else {
            mHeadRot[2] = 0;
            mSpineRot[2] = mHeadRot[2];
            func_ov002_020e28d4(0x1800, 0x800);
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x4b000;
            if (mVertSpeed >= 0) {
                mVertAccel = -0x8000;
                if (data_0209f498[gActivePlayerSlot].held & 2) {
                    mVertAccel = -0x3400;
                }
            } else {
                if ((data_0209f498[gActivePlayerSlot].held & 2) && mHasWings != 0) {
                    mVertAccel = -0x2000;
                    mTerminalVelocity = -0x25800;
                }
            }
        }
    }

    if (this->FinishedAnim() && this->IsAnim(0x35)) {
        this->SetAnim(0x34, 0, 0x1000, 0);
    }
    AdvanceAnims();
    return 1;
}

void daPly_c::func_ov002_020cc660(int angle) {
    s16 v, w;
    int delta;
    int third;
    delta = angle - mPrevAngleY;
    third = mHorzSpeed / 12;
    v = (s16)FX_MUL(delta, third);
    if (v < -0x1800)
        v = -0x1800;
    else if (v > 0x1800)
        v = 0x1800;
    ApproachAngle(&mSpineRot[1], v, 8, 0x200, 0x20);
    mHeadRot[0] = -mSpineRot[1];
    mAngleZ = mSpineRot[1];
    w = (s16)FX_MUL(mHorzSpeed, 0xaa);
    if (w < 0)
        w = 0;
    else if (w > 0x1000)
        w = 0x1000;
    ApproachAngle(&mSpineRot[2], w, 8, 0x200, 0x20);
    mHeadRotMode = 1;
    mModelOffsetY = 0x46000;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Shell_Init() {
    ReleaseHeldActor();
    this->SetAnim(0x35, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    StartStateMusic(0x33);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_HurtWater_Main() {
    if (func_ov002_020cec2c())
        return 1;
    ApproachLinear(mHorzSpeed, 0, 0x800);
    int t = func_ov002_020ceaf4();
    ApproachLinear(mVertSpeed, t, 0x800);
    if (this->FinishedAnim()) {
        if ((mStateStep & 1) == 0) {
            mPrevAngleY = mPrevAngleY + 0x8000;
            mAngleY = mPrevAngleY;
        }
        if (this->GetHealth()) {
            if (mIsMetal == 0)
                this->ChangeState(data_ov002_0211067c);
            else
                this->ChangeState(data_ov002_021106ac);
            mIsTakingDamage = 0;
            if (mHurtDamage)
                mInvincibleTimer = 0x24;
        } else {
            func_ov002_020c5dec(8);
            return 1;
        }
    }
    int lim = data_0209f32c - 0x50000;
    if (mPosY >= lim && (mClsnFlags & 1) == 0) {
        if (mVertSpeed >= 0) {
            mPosY = lim;
            mVertSpeed = 0;
        }
    }
    func_ov002_020ceb7c();
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_HurtWater_Init() {
    extern int gCamera[];
    unsigned a = data_ov002_02109fe4[mStateStep & 1];
    this->SetAnim(a, 0x40000000, 0x1000, 0);
    mStateWork = 0;
    mStateArg = 0;
    mVertAccel = 0;
    mTerminalVelocity = -0xc000;
    mVertSpeed = 0x10000;
    mIsTakingDamage = 1;
    if (mHurtDamage) {
        int b = mStateStep & 0xf0;
        if (b == 0 || b == 0x10) {
            func_ov002_020d93ac();
            func_ov002_020d94cc();
        }
    }
    (*(dCamera_c **)gCamera)->func_0200d89c();
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Metal Mario walking along the bottom underwater. Losing both the metal and
 * mega caps drops back to the swim state. While mStateWork is 0 the player
 * walks and sinks -- gravity and terminal velocity depend on whether the water
 * surface (data_0209f32c) is more than 0x46000 above (0xd2000 when mega).
 * Landing plays a thud, spawns a splash particle, and either returns to the
 * ground state or starts the landing animation.
 */

int daPly_c::St_MetalWaterWater_Main() {
    extern int func_ov002_020eeca8(dBgCh_Actr *, dActor_c *);
    extern struct S18b2 data_0209f4a0[];
    u32 f;
    int v;
    int lim;
    int py;

    if (func_ov002_020cec2c() != 0)
        return 1;
    f = mIsMetal;
    if (f == 0 && mIsMega == 0) {
        this->ChangeState(data_ov002_0211067c);
        return 1;
    }
    if (f != 0) func_ov002_020eeca8(&mMeshClsn, this);

    if (mStateWork == 0) {
        v = 0;
        if (data_0209f4a0[gActivePlayerSlot].a != 0) {
            v = func_ov002_020bf224(0x14000, 0x1000);
            ApproachAngle(&mPrevAngleY, mDesiredAngleY, 0x20, 0x1000, 0x10);
        }
        mAngleY = mPrevAngleY;
        ApproachLinear(mHorzSpeed, v, 0x400);
        py = mPosY;
    lim = py + 0x46000;
    if (mIsMega != 0) lim = py + 0xd2000;
        if (data_0209f32c > lim) {
            mVertAccel = -0x1800;
            mTerminalVelocity = -0x14000;
        } else {
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x4b000;
        }
        if (mIsAirborne == 0) {
            Sound::PlayBank0(0xa9, mCamSpacePos);
            Vector3 pos = mPos;
            if (mIsMega != 0) {
                pos.y += 0x19000;
                Particle::System::NewSimple(kPtclMegaGroundPuffWater, pos.x, pos.y, pos.z);
            } else {
                pos.y += 0x32000;
                Particle::System::NewSimple(0xd7, pos.x, pos.y, pos.z);
            }
            if (data_0209f4a0[gActivePlayerSlot].a != 0) {
                this->ChangeState(data_ov002_021106ac);
            } else {
                mHorzSpeed = 0;
                if (mStateStep != 0)
                    this->SetAnim(0x52, 0x40000000, 0x1000, 0);
                else
                    this->SetAnim(0x55, 0x40000000, 0x1000, 0);
                mStateWork = 1;
            }
        }
    } else {
        if (FinishedAnim() != 0) {
            this->ChangeState(data_ov002_021106ac);
            return 1;
        }
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_MetalWaterWater_Init() {
    mIsInAirState = 1;
    mIsUnderwater = 1;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    if (mStateStep == 0) {
        func_ov002_020bf2d8(0x20000);
        this->SetAnim(0x53, 0x40000000, 0x1000, 0);
    } else {
        this->SetAnim(0x54, 0x40000000, 0x1000, 0);
    }
    Sound::PlayBank0(0xa8, mCamSpacePos);
    mStateWork = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_MetalWaterGround_Main() {
    extern ControllerInput data_0209f498[];
    if (func_ov002_020cec2c())
        return 1;
    int u703 = mIsMega;
    if ((data_0209f32c - 0x50000) < (mPosY - 0xa000)) {
        mIsUnderwater = 0;
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }
    if (data_0209f498[gActivePlayerSlot].pressed & 2) {
        mStateStep = 0;
        this->ChangeState(data_ov002_021106c4);
        return 1;
    }
    if(mIsAirborne != 0 && (mPosY - mGroundY) >= 0x32000){
        mStateStep = 1;
        this->ChangeState(data_ov002_021106c4);
        return 1;
    }
    if(mIsMetal == 0 && u703 == 0){
        this->ChangeState(data_ov002_0211067c);
        return 1;
    }
    func_ov002_020cd190((char *)this);
    int approach = 0;
    if (data_0209f498[gActivePlayerSlot].mag != 0) {
        ApproachAngle(&mPrevAngleY, mDesiredAngleY, 8, 0x1000, 0x80);
        approach = func_ov002_020bf224(0x14000, 0x1000);
    }
    ApproachLinear(mHorzSpeed, approach, 0x1000);
    mAngleY = mPrevAngleY;
    if(mIsMega != 0){
        if(mHorzSpeed == 0){
            this->SetAnim(0xa0, 0, 0x1000, 0);
        }else if(this->IsAnim(0xa0) != 0){
            this->SetAnim(0x9f, 0x40000000, 0x1000, 0);
        }else if(this->IsAnim(0x9f) != 0){
            if(this->FinishedAnim() != 0){
                this->SetAnim(0x9e, 0, data_0209f498[gActivePlayerSlot].mag * 2, 0);
            }
        }else if(this->IsAnim(0x9f) == 0){
            int v = data_0209f498[gActivePlayerSlot].mag;
            if(v != 0){
                this->SetAnim(0x9e, 0, v * 2, 0);
                func_ov002_020bf5e0();
            }else{
                this->SetAnim(0x9d, 0x40000000, 0x1000, 0);
            }
        }
    }else if(mHorzSpeed == 0){
        this->SetAnim(0x47, 0, 0x1000, 0);
    }else{
        this->SetAnim(0x48, 0, 0x1000, 0);
        int r5 = (int)((((long long)mHorzSpeed << 9) + 0x800) >> 12);
        if(r5 < 0x400) r5 = 0x400;
        dExtFrameCtrl_c &anim =
            *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
        anim.speed = r5;
        if (mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(4) !=
                0 ||
            mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(
                0x13) != 0) {
            func_0201251c(0, 0xaa, &mCamSpacePos, mHorzSpeed);
        }
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_MetalWaterGround_Init() {
    dActor_c *p = mHeldObj;
    int b0 = (p != 0);
    if (b0) {
        int h = p->actorID;
        int b1 = (h == 0x10c);
        int b2;
        if (b1 || (b2 = (h == 0x10b)) != 0) {
            p->mFlags &= ~0x4000;
            mHeldObj->mFlags &= ~0x100;
            mHeldObj = 0;
        }
    }
    Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
    mVertAccel = -0x1800;
    mTerminalVelocity = -0x14000;
    if (mHorzSpeed >= 0x14000) mHorzSpeed = 0x14000;
    mIsUnderwater = 1;
    return 1;
}

extern "C" {

extern "C" void func_ov002_020cd190(char *c) {
    daPly_c *self = (daPly_c *)c;
    if (self->mIsMega == 0)
        return;
    if (self->mPosY + 0x180000 <= data_0209f32c)
        return;
    self->func_ov002_020ce8bc(self->mHorzSpeed);
}
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Swim_Cleanup() {
    if (mSwimMusicPushed != 0) {
        mSwimMusicPushed = 0;
        EndStateMusic(0x33);
    }
    return 1;
}

void daPly_c::func_ov002_020cd218(int v, int *out1, int *out2) {
    *out1 = FX_MUL(v, FX_SinCosTable_[((unsigned short)mPrevAngleX >> 4) * 2 + 1]);
    *out2 = -FX_MUL(v, FX_SinCosTable_[((unsigned short)mPrevAngleX >> 4) * 2]);
}

extern "C" {

int func_ov002_020cd28c(daPly_c *self) {
    self->SetAnim(0xaf, 0x40000000, 0x1000, 0);
    self->mStateStep = 7;
    return 7;
}
}

void daPly_c::StartAnim0xAB() {
    SetAnim(0xab, 0x40000000, 0x1000, 0);
    mStateStep = 1;
    mStateTimer = 0;
}

void daPly_c::StartAnim0xAE() {
    SetAnim(0xae, 0x40000000, 0x1000, 0);
    mStateStep = 6;
    mStateTimer = 0x10;
    mStateWork &= 0xa;
}

void daPly_c::func_ov002_020cd364() {
    func_ov002_020dc020();
    mAttackClsn.Clear();
    mAttackClsn.Update();
    mStateTimer = 2;
    mStateStep = 5;
}

void daPly_c::StartAnim0xAA() {
    SetAnim(0xaa, 0x40000000, 0x1000, 0);
    mStateStep = 4;
    mStateTimer = 7;
}

void daPly_c::StartAnim0xA9() {
    SetAnim(0xa9, 0x40000000, 0x1000, 0);
    mStateStep = 2;
    mStateTimer = 0xf;
    mStateWork &= 0xfe;
    Sound::PlayBank0(0x14, mCamSpacePos);
}

void daPly_c::func_ov002_020cd448() {
    extern ControllerInput data_0209f498[];
    unsigned char fl;
    if (mStateStep >= 2) {
        fl = mClsnFlags;
        if (fl & 1) {
            short before;
            mAngleX = mPrevAngleX;
            before = mAngleX;
            func_ov002_020c18b0(0);
            if (before >= mAngleX)
                ApproachLinear2(mPrevAngleX, mAngleX, 0x200);
        } else if (fl & 2) {
            if (data_0209f498[gActivePlayerSlot].ny == 0) {
                if (mPrevAngleX > 0) {
                    short *q = &mPrevAngleX;
                    *q += 0x200;
                    if (mPrevAngleX > 0x3f00)
                        mPrevAngleX = 0x3f00;
                } else {
                    short *q = &mPrevAngleX;
                    *q -= 0x200;
                    if (mPrevAngleX < -0x3f00)
                        mPrevAngleX = -0x3f00;
                }
            }
        }
    }
    ApproachLinear2(mAngleXSpeed, mAngleZ, 0x200);
    mAngleZ = mAngleXSpeed;
}

void daPly_c::func_ov002_020cd550() {
    short lim, v, w, a;
    int step, idx;

    lim = (short)FX_MUL(data_0209f4a4[gActivePlayerSlot].f0, -0x3f00);
    lim &= 0xfff0;

    step = mPrevAngleX > 0 ? 0x100 : 0x200;
    if (mPrevAngleX < lim) {
        mPrevAngleX += step;
        if (mPrevAngleX > lim)
            mPrevAngleX = lim;
    } else if (mPrevAngleX > lim) {
        mPrevAngleX -= step;
        if (mPrevAngleX < lim)
            mPrevAngleX = lim;
    }

    func_ov002_020cd448();

    v = mPrevAngleX;
    if (v < 0)
        mAngleX = v * 6 / 10;
    else
        mAngleX = v * 10 / 8;

    mModelOffsetY = 0;
    w = mAngleX;
    if (w >= 0)
        mModelOffsetY = FX_MUL(w, 0xc000);

    a = (short)(mAngleX - 0x2000);
    if (a < -0x2000) a = -0x2000;
    if (a > 0) a = 0;
    idx = (u16)(short)(a * 2) >> 4;
    mModelOffsetZ = FX_SinCosTable_[idx * 2 + 1] * -0x30;
}

void daPly_c::func_ov002_020cd71c() {
    extern ControllerInput data_0209f498[];
    typedef short s16;
    int ip = data_ov002_020ff1f0[param1];
    int a = FX_MUL(ip, 0x320);
    int c = (int)((((long long)ip << 7) + 0x800) >> 12);
    s16 step = (s16)((((long long)ip << 5) + 0x800) >> 12);
    int b = FX_MUL(ip, 0x240);
    int e = data_0209f498[gActivePlayerSlot].nx;
    s16 target = (s16)FX_MUL(-b, e);

    if (mPrevVertSpeed == 0) {
        step = (s16)((((long long)ip << 6) + 0x800) >> 12);
        target = (s16)FX_MUL(-a, e);
    }

    if (target > 0) {
        if (mAngleYSpeed < 0) {
            s16 *q = &mAngleYSpeed;
            *q = *q + 0x40;
            if (mAngleYSpeed > c)
                mAngleYSpeed = c;
        } else {
            ApproachLinear2(mAngleYSpeed, target, step);
        }
    } else if (target < 0) {
        if (mAngleYSpeed > 0) {
            s16 *q = &mAngleYSpeed;
            *q = *q - 0x40;
            if (mAngleYSpeed < -c)
                mAngleYSpeed = -c;
        } else {
            ApproachLinear2(mAngleYSpeed, target, step);
        }
    } else {
        ApproachLinear2(mAngleYSpeed, 0, 0x40);
    }

    {
        s16 *q = &mAngleY;
        *q = *q + mAngleYSpeed;
    }
    mAngleZ = -(mAngleYSpeed << 3);
    mPrevAngleY = mAngleY;
}

/* recovered: named members + shared header, real C++ method
 *
 * Swimming, per frame. Metal cap diverts to the metal-underwater state and
 * surfacing to the walking one. mStateStep selects the stroke phase; each
 * phase drives the stroke animation, the body model's animation block at
 * +0x50, and the forward impulse, with a kick sound on the frame the
 * animation crosses its trigger.
 */

int daPly_c::St_Swim_Main() {
    extern int *func_02037dc4(int);
    extern ControllerInput data_0209f498[];
    extern char data_0209ee90[];
    int t4;
    Vector3 pos;
    int* fr;
    int ang;
    int out[3];
    int vec[3];
    short yaw;
    int var_r2;
    unsigned short a4;
    int var_r6;
    int var_r1;
    int has;
    int *p;
    unsigned int t5;

    if (mIsMetal != 0) {
        this->ChangeState(data_ov002_021106ac);
        return 1;
    }
    if (this->GetHealth() == 0) {
        func_ov002_020c5dec(8);
        return 1;
    }
    if (this->func_ov002_020cede0() == 0) {
        mIsUnderwater = 0;
        this->ChangeState(data_ov002_021101b4);
        return 1;
    }
    mPeakY = mPosY;
    if (func_ov002_020cec2c() != 0)
        return 1;

    t4 = (data_0209f498[gActivePlayerSlot].held) & 0x802;
    mAngleXSpeed = mAngleZ;
    func_ov002_020cd71c();
    func_ov002_020cd550();

    if (mJumpComboTimer != 0) {
        int eid = 0xcd;
        pos = mPos;
        pos.y += 0x32000;
        mParticle3 = Particle::System::NewCheckWater(mParticle3, eid, pos.x, pos.y, pos.z, 0);
    }

    if (mPrevVertSpeed == 0 && mMeshClsn.IsOnGround() != 0) {
        fr = func_02037dc4((int)&mMeshClsn.GetFloorResult()->surface);
        ang = fr[1];
        if (ang < 0x720 && ang > 0x400 && mPosY >= mWaterSurfaceY - 0xc8000) {
            int at;
            mPosY = mPreClsnPosY - 0x8000;
            at = cstd::atan2(fr[0], fr[2]);
            vec[0] = 0;
            vec[1] = 0;
            at = at + 0x8000;
            yaw = (short)at;
            vec[2] = (*(int*)((char*)data_0209ee90 + 0x244)) << 0xc;
            Vec3_RotateYAndTranslate((Vector3 *)out, (const Vector3 *)&mPosX, yaw,
                                     (const Vector3 *)vec);
            mPosX = out[0];
            mPosY = out[1];
            mPosZ = out[2];
        }
    }

    switch (mStateStep) {
    case 0:
        mPrevAngleX = -0x4000;
        mPrevAngleZ = 0;
        mAngleX = 0;
        mAngleZ = 0;
        mAngleYSpeed = 0;
        ApproachLinear(mPrevVertSpeed, 0, 0x5000);
        if (this->IsAnim(0xa8) == 0 && this->FinishedAnim() != 0) {
            this->SetAnim(0xa8, 0, 0x1000, 0);
        }
        if (mPrevVertSpeed == 0) {
            mStateStep = 1;
            mPrevAngleX = 0;
            mStateTimer = 0x1e;
        }
        break;
    case 1:
        var_r2 = 0x800;
        if (mStateTimer != 0) var_r2 = 0x200;
        ApproachLinear(mPrevVertSpeed, 0, var_r2);
        if (this->IsAnim(0xa8) == 0) {
            if (this->FinishedAnim() != 0) {
                this->SetAnim(0xa8, 0, 0x1000, 0);
            }
        } else if (mAngleX >= 0x1000) {
            dExtFrameCtrl_c &anim =
                *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
            if (anim.speed != 0x3000) {
                dExtFrameCtrl_c &again =
                    *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
                again.speed = 0x3000;
            }
        } else if (mAngleYSpeed != 0) {
            dExtFrameCtrl_c &anim =
                *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
            if (anim.speed != 0x2000) {
                dExtFrameCtrl_c &again =
                    *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
                again.speed = 0x2000;
            }
        } else {
            dExtFrameCtrl_c &anim =
                *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
            if (anim.speed != 0x1000) {
                dExtFrameCtrl_c &again =
                    *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
                again.speed = 0x1000;
            }
        }
        if (func_ov002_020ce324() != 0)
            return 1;
        if ((data_0209f498[gActivePlayerSlot].pressed) & 1) {
            func_ov002_020cd364();
        } else if (t4 != 0) {
            StartAnim0xA9();
        }
        break;
    case 2:
        a4 = mStateTimer;
        if (a4 >= 0xa) var_r6 = 0x800;
        else if (a4 <= 4) var_r6 = 0x1800;
        else var_r6 = 0;
        ApproachLinear(mPrevVertSpeed, func_ov002_020cedb0(0x10000), var_r6);
        if (mStateTimer < 0xa && t4 == 0) {
            mStateWork |= 1;
        }
        if (mStateTimer == 0 && this->IsAnim(0xa9) != 0 && this->FinishedAnim() != 0) {
            if (!(mStateWork & 1)) {
                mStateStep = 3;
            } else {
                StartAnim0xAA();
            }
        }
        break;
    case 3:
        ApproachLinear(mPrevVertSpeed, func_ov002_020cedb0(0xc000), 0x200);
        if (func_ov002_020ce324() != 0)
            return 1;
        if (t4 != 0) {
            this->SetAnim(0xa6, 0, 0x1000, 0);
            if (mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(
                    0) != 0) {
                Sound::PlayBank0(0x15, mCamSpacePos);
            }
        } else {
            StartAnim0xAA();
        }
        break;
    case 4:
        ApproachLinear(mPrevVertSpeed, 0, 0x400);
        if (func_ov002_020ce324() != 0)
            return 1;
        if (mStateTimer == 0 && t4 != 0) {
            StartAnim0xA9();
        }
        if (this->FinishedAnim() != 0 && this->IsAnim(0xaa) != 0) {
            StartAnim0xAB();
        }
        break;
    case 5:
        if (mStateTimer == 0) {
            StartAnim0xAE();
        }
        break;
    case 6:
        func_ov002_020dbf4c();
        var_r1 = func_ov002_020cedb0(0x10000);
        a4 = mStateTimer;
        if (a4 >= 5) var_r2 = 0x1000;
        else if (a4 == 0) { var_r2 = 0x800; var_r1 = 0; }
        else var_r2 = 0;
        ApproachLinear(mPrevVertSpeed, var_r1, var_r2);
        if (this->FinishedAnim() != 0) {
            has = (mHeldObj != 0) ? 1 : 0;
            if (has != 0) {
                mStateStep = 8;
                mStateTimer = 0xf0;
                p = (int *)&mHeldObj->mFlags;
                *p |= 0x4000;
                this->SetAnim(0xb0, 0, 0x1000, 0);
            } else {
                StartAnim0xAB();
            }
        }
        mAttackClsn.Clear();
        {
            dExtFrameCtrl_c &anim =
                *mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)];
            t5 = ((unsigned int)anim.currFrame << 4) >> 0x10;
        }
        if ((int)t5 >= 5 && (int)t5 <= 7) {
            mAttackClsn.Update();
        }
        break;
    case 7:
        if (this->FinishedAnim() != 0) {
            mStateStep = 8;
            mStateTimer = 0xf0;
            this->SetAnim(0xb0, 0, 0x1000, 0);
            p = (int *)&mHeldObj->mFlags;
            *p |= 0x4000;
            StartStateMusic(0x33);
            mSwimMusicPushed = 1;
        }
        break;
    case 8:
        ApproachLinear(mPrevVertSpeed, func_ov002_020cedb0(0x1e000), 0x400);
        if ((data_0209f498[gActivePlayerSlot].pressed) & 1) {
            this->SetAnim(0xb1, 0x40000000, 0x1000, 0);
            mStateStep = 9;
            EndStateMusic(0x33);
        }
        if (mStateTimer != 0) {
            has = (mHeldObj != 0) ? 1 : 0;
            // retesting mStateTimer grows the function 0x9d8 -> 0x9e0
            if (has == 0) goto case8_block107;
        } else {
        case8_block107:
            ReleaseHeldActor();
            StartAnim0xAB();
            EndStateMusic(0x33);
            mSwimMusicPushed = 0;
        }
        if (mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(0) !=
            0) {
            Sound::PlayBank0(0x15, mCamSpacePos);
        }
        break;
    case 9:
        if (mModels.mBodyModels[this->GetBodyModelID((unsigned char)param1, 0)]->WillHitFrame(
                0xd) != 0) {
            func_ov002_020daa74();
        }
        if (this->FinishedAnim() != 0) {
            StartAnim0xAB();
        }
        break;
    }

    func_ov002_020cd218(mPrevVertSpeed, &mHorzSpeed, &mVertSpeed);
    {
        int* pa8 = (int*)((unsigned int)(&mVertSpeed));
        *pa8 = *pa8 + func_ov002_020ceaf4();
    }
    if (func_ov002_020ce5f8() != 0)
        return 1;
    AdvanceAnims();
    return 1;
}

int daPly_c::func_ov002_020ce324() {
    typedef unsigned char u8;
    typedef unsigned short u16;
    extern struct S18b data_0209f49e[];
    Vector3 a1, a2, b1, b2, nrm;
    int ok;

    if (!(data_0209f49e[gActivePlayerSlot].f0 & 2)) return 0;
    if (func_ov002_020ceb54())
        return 0;

    dBgCh_Lin rl1;
    a1.x = mPosX;
    a1.y = mPosY;
    a1.z = mPosZ;
    a2.x = mPosX;
    a2.y = mPosY;
    a2.z = mPosZ;
    a1.y += 0x50000;
    a2.y += 0x96000;
    rl1.SetObjAndLine(a1, a2, this);
    if (rl1.DetectClsn()) {
        return 0;
    }

    ok = 0;
    if (data_0209f4a4[gActivePlayerSlot].f0 >= 0xa00) {
        ok = 1;
    } else {
        int ax, bz, bx, az, by, ay;
        u16 ang = mAngleY;

        dBgCh_Lin rl2;
        ax = mPosX;
        az = mPosZ;
        ay = mPosY + 0x32000;
        b1.x = ax;
        b1.y = ay;
        b1.z = az;
        bz = mPosZ + FX_SinCosTable_[(ang >> 4) * 2 + 1] * 0x64;
        bx = mPosX + FX_SinCosTable_[(ang >> 4) * 2] * 0x64;
        by = mPosY + 0x32000;
        b2.x = bx;
        b2.y = by;
        b2.z = bz;
        rl2.SetObjAndLine(b1, b2, this);
        if (rl2.DetectClsn()) {
            rl2.surface.CopyNormalTo(nrm);
            if (nrm.y == 0) {
                if (AngleDiff(cstd::atan2(nrm.x, nrm.z), mAngleY) >= 0x5000)
                    ok = 1;
            }
        }
    }

    if (ok == 1) {
        mIsUnderwater = 0;
        ChangeState(data_ov002_02110694);
        return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Swim_Init() {
    extern int gCamera[];
    ReleaseHeldActor();
    Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
    this->SetAnim(0xa7, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    mVertAccel = 0;
    mTerminalVelocity = -0xc000;
    mIsUnderwater = 1;
    ((::dCamera_c *)*(void **)gCamera)->func_0200d768(mPlayerNo);
    mPrevVertSpeed = mVertSpeed;
    mHorzSpeed = 0;
    mVertSpeed = 0;
    mAngleYSpeed = 0;
    mStateWork = 0;
    mStateArg = 0;
    mJumpComboTimer = 0;
    return 1;
}

/* recovered: shared common types */

int daPly_c::func_ov002_020ce5f8() {
    int y, threshold;
    Vector3 pos;

    threshold = data_0209f32c - 0x50000;
    y = mPosY;
    if (y >= threshold) {
        if ((mClsnFlags & 1) == 0) {
            if (mVertSpeed >= 0) {
                mPosY = threshold;
                mVertSpeed = 0;
            }
        } else {
            if (threshold < y - 0xa000) {
                mIsUnderwater = 0;
                ChangeState(data_ov002_0211013c);
                return 1;
            }
        }
    }

    if (func_ov002_020ceb54() == 0) {
        if (mStateArg == 0) {
            mStateArg |= 1;
            Sound::PlayBank0(0x17, mCamSpacePos);
            mParticle1 = 0;
        }
        func_ov002_020ce8bc(mPrevVertSpeed);
    } else {
        mStateArg = 0;
    }
    func_ov002_020ceb7c();

    {
        u32 idx = GetBodyModelID(param1 & 0xff, 0);
        dExtAnmModel_c *m1 = mModels.mBodyModels[idx];
        Matrix4x3 *base;
        u32 idx2;
        dExtAnmModel_c *m2;
        base = &m1->data.transforms[15];
        idx2 = GetBodyModelID(param1 & 0xff, 0);
        m2 = mModels.mBodyModels[idx2];
        MTX_MultVec43((Vector3 *)&base->m[9], &m2->mat4x3, &pos);
    }
    Vec3_LslInPlace((int *)&pos, 3);

    if (pos.y < data_0209f32c - 0x1e000) {
        Particle::System::NewBubble(pos.x, *(volatile int *)&pos.y,
                                    pos.z); // without volatile, the function shrinks 0x1a0 to 0x19c
        if (mStateWaitTimer == 0) {
            Sound::PlayBank3(0xb, mCamSpacePos);
            mStateWaitTimer = 0xa;
        }
    }
    return 0;
}

void daPly_c::func_ov002_020ce798() {
    Vector3 v1, v2;

    if (!IsState(data_ov002_0211067c))
        return;

    if (mPosY >= data_0209f32c - 0x50000) {
        if (!mMeshClsn.IsOnGround())
            mPosY = data_0209f32c - 0x50000;
    }

    dBgCh_Lin rl;
    v1.x = mPosX;
    v1.y = mPosY;
    v1.z = mPosZ;
    v2.x = mPosX;
    v2.y = mPosY;
    v2.z = mPosZ;
    v1.y += 0x50000;
    v2.y += 0x96000;
    rl.SetObjAndLine(v1, v2, this);
    if (rl.DetectClsn()) {
        ::Vector3 cp = rl.GetClsnPos();
        if (mPosY >= cp.y - 0x82000) {
            mPosY -= 0x4000;
            if (mPosY < cp.y - 0x82000)
                mPosY = cp.y - 0x82000;
        }
    }
}

void daPly_c::func_ov002_020ce8bc(int arg1) {
    Vector3 pos(mPosX, data_0209f32c + 0x3000, mPosZ);

    if (mIsMega != 0) {
        if (arg1 != 0) {
            mWaterMoveParticle = Particle::System::NewFitWater(
                mWaterMoveParticle, kPtclMegaWaterSurfaceMove, pos.x, pos.y, pos.z, 0);
        } else {
            mWaterStillParticle = Particle::System::NewFitWater(
                mWaterStillParticle, kPtclMegaWaterSurfaceStill, pos.x, pos.y, pos.z, 0);
        }
    } else {
        if (arg1 != 0) {
            mWaterMoveParticle = Particle::System::NewFitWater(
                mWaterMoveParticle, kPtclWaterSurfaceMove, pos.x, pos.y, pos.z, 0);
        } else {
            mWaterStillParticle = Particle::System::NewFitWater(
                mWaterStillParticle, kPtclWaterSurfaceStill, pos.x, pos.y, pos.z, 0);
        }
    }
}

void daPly_c::func_ov002_020ce9c8() {
    Vector3 v = mPos;
    int effectID;

    v.y = data_0209f32c + 0x4b000;
    effectID = 0xce;
    if (mIsMega)
        effectID = 0x54;
    Particle::System::NewFitWaterRaised(0, effectID, v.x, v.y, v.z, 0);

    v.y = data_0209f32c + 0x3000;
    effectID = 0xcf;
    if (mIsMega)
        effectID = 0x55;
    Particle::System::NewFitWater(0, effectID, v.x, v.y, v.z, 0);

    if (mIsMega)
        mParticle1 = Particle::System::NewCheckWaterRipple(mParticle1, kPtclMegaSplash, mPosX,
                                                           data_0209f32c, mPosZ, 0);
    else
        Particle::System::NewBigSplash(mPosX, data_0209f32c, mPosZ);

    if (mIsMega)
        Sound::PlayBank0(0xd4, mCamSpacePos);
    else
        Sound::PlayBank0(0x16, mCamSpacePos);
}

int daPly_c::func_ov002_020ceaf4() {
    int v;
    if (data_0209f32c - 0x50000 - mPosY < 0x1f4000)
        return 0x1800;
    v = data_ov002_020ff1e0[param1];
    return -(int)((((long long)v << 13) + 0x800) >> 12);
}

int daPly_c::func_ov002_020ceb54() {
    return data_0209f32c - 0x50000 >= mPosY + 0x3c000;
}

void daPly_c::func_ov002_020ceb7c() {
    extern struct CameraRaw *gCamera;
    struct CameraRaw *g = gCamera;

    if (mStateWork & 2) {
        if (data_0209f32c - 0x50000 - mPosY >= 0x64000)
            return;
        ((::dCamera_c *)g)->func_0200d768(mPlayerNo);
        mStateWork ^= 2;
    } else {
        if (data_0209f32c - 0x50000 - mPosY <= 0x12c000)
            return;
        ((::dCamera_c *)g)->func_0200d72c(mPlayerNo);
        mStateWork |= 2;
    }
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* func_ov002_020cec2c at 0x020cec2c (ov002), size 0x184
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
int daPly_c::func_ov002_020cec2c() {
    int flag;
    int thresh;

    if (mIsNoControl != 0 || mIsMetal != 0 || mIsMega != 0 || mIsTakingDamage != 0)
        return 0;

    flag = 0;
    if (gCurrentSublevel == 0x13 || gCurrentSublevel == 0x14 || gCurrentSublevel == 0x30)
        flag = 1;

    if (func_ov002_020ceb54() == 0 && flag == 0) {
        mAirDrainCounter = 0;
        if (mWalkTimer == 0) {
            Heal(0x100);
            mWalkTimer = 0xf;
        }
        return 0;
    }

    thresh = 0xf0;
    if (flag != 0) thresh = 0x50;

    if (GetHealth() <= 2) {
        mSurfaceSoundHandle =
            Sound::PlayLong((unsigned int)mSurfaceSoundHandle, 2, 0x27, *&mCamSpacePos, 0);
    }

    {
        u16 *p = &mAirDrainCounter;
        *p = *p + 1;
    }

    if (mAirDrainCounter >= thresh) {
        mAirDrainCounter = 0;
        if (func_ov002_020d91e0(0x100, 0) != 0) {
            DropHeldObj();
            func_ov002_020c5dec(7);
            return 1;
        }
    }
    return 0;
}

Fix12i daPly_c::func_ov002_020cedb0(Fix12i a) {
    unsigned int t = data_ov002_020ff158[param1];
    return FX_MUL(a, (int)t);
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
int daPly_c::func_ov002_020cede0() {
    char *thiz = (char *)this;
    daPly_c *p = (daPly_c *)thiz;
    Vector3 v;
    int y = p->mPosY;
    int g = data_0209f32c;
    if (g < y - 0x64000)
        return 0;
    {
        int d = g - y;
        if (d < 0)
            d = -d;
        if (d >= 0xc8000)
            return 1;
    }
    dBgCh_Gnd rg;
    {
        int z = p->mPosZ;
        int yy = data_0209f32c + 0xc8000;
        int x = p->mPosX;
        v.x = x;
        v.y = yy;
        v.z = z;
    }
    rg.StartDetectingWater();
    rg.StopDetectingOrdinary();
    rg.SetObjAndPos(v, (dActor_c *)thiz);
    if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int *)&rg.surface) != 0) {
        return 1;
    }
    v.y = data_0209212c;
    rg.StartDetectingWater();
    rg.StopDetectingOrdinary();
    rg.SetObjAndPos(v, (dActor_c *)thiz);
    if (rg.DetectClsn() != 0 && SurfaceInfo_TestFlag0x20((int *)&rg.surface) != 0) {
        p->mWaterSurfaceY = rg.clsnY;
        data_0209f32c = p->mWaterSurfaceY;
        return 1;
    }
    if (gCurrentSublevel == 0x21) {
        p->mWaterSurfaceY = 0x80000000;
        data_0209f32c = p->mWaterSurfaceY;
        return 0;
    }
    return 1;
}

int daPly_c::func_ov002_020cef84() {
    Vector3 v1, v2;
    int lim;

    dBgCh_Lin line;
    if (!IsInAir() || mIsUnderwater != 0 || mVertSpeed <= 0 || IsState(data_ov002_0211001c) ||
        mIsMega != 0 || mIsTakingDamage != 0 || mIsNoControl != 0) {
        return 0;
    }

    v1.x = mPosX;
    int y1 = mPosY;
    v1.y = y1;
    v1.z = mPosZ;
    v2.x = mPosX;
    int y2 = mPosY;
    v2.y = y2;
    v2.z = mPosZ;
    v1.y = y1 + 0x64000;
    v2.y = y2 + 0x96000;
    lim = 0x8c000;
    {
        int t = mHeldObj != 0;
        if (t != false) {
            if (param1 == 2) {
                v2.y += 0x32000;
                lim = 0xb4000;
            }
        }
    }
    line.StopIgnoringVanishSurfaces();
    if (mIsVanish != 0)
        line.StartIgnoringVanishSurfaces();
    line.SetObjAndLine(v1, v2, this);
    if (line.DetectClsn()) {
        ::Vector3 cp = line.GetClsnPos();
        if (line.surface.clps.GetSurfaceType() == 3) {
            int t2 = mHeldObj != 0;
            if (t2 == false) {
                ChangeState(data_ov002_0211001c);
                mPosY = cp.y - 0x78000;
                return 1;
            }
        }
        {
            int d = cp.y - mPosY;
            if (d < 0) d = -d;
            if (d < lim) {
                mVertSpeed = -0x1000;
                mClsnFlags |= 8;
                dBgPi hit(line);
                dActor_c *a = dActor_c::FindWithID(hit.GetClsnID());
                if (a)
                    a->OnHitFromUnderneath(*this);
            }
        }
    }
    return 0;
}

/* recovered: shared common types */
// func_ov002_020cf20c at 0x020cf20c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).

int daPly_c::func_ov002_020cf20c() {
    extern dCamera_c *gCamera;
    Vector3 v1;
    Vector3 v2;
    dBgCh_Lin line;
    v1.x = mPosX;
    int y1 = mPosY;
    v1.y = y1;
    v1.z = mPosZ;
    v2.x = mPosX;
    int y2 = mPosY;
    v2.y = y2;
    v2.z = mPosZ;
    v1.y = y1 + 0x64000;
    v2.y = y2 + 0xb4000;
    line.SetObjAndLine(v1, v2, this);
    if (line.DetectClsn()) {
        if (line.surface.clps.GetSurfaceType() == 3) {
            ::Vector3 clsnPos = line.GetClsnPos();
            dCamera_c *cam = gCamera;
            int t = line.surface.clps.GetCameraBehavior();
            cam->func_0200ca14(mPlayerNo, t);
            int ret = clsnPos.y;
            return ret;
        }
    }
    return (int)0x80000000;
}

void daPly_c::func_ov002_020cf2f8() {
    extern ControllerInput data_0209f498[];
    if (data_0209f498[gActivePlayerSlot].mag == 0)
        return;
    ApproachLinear(mHorzSpeed, func_ov002_020bf224(0x8000, 0x2000), 0x1000);
    ApproachAngle(&mPrevAngleY, mDesiredAngleY, 0x10, 0x1000, 0x200);
    mAngleY = mPrevAngleY;
}

void daPly_c::func_ov002_020cf384() {
    mVertAccel = 0;
    mVertSpeed = 0;
    mHorzSpeed = 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_CeilingGrate_Main() {
    extern ControllerInput data_0209f498[];
    int r4;
    int spd;

    r4 = func_ov002_020cf20c();
    if (r4 == 0x80000000) {
        mPosX = mPreClsnPosX;
        mPosY = mPreClsnPosY;
        mPosZ = mPreClsnPosZ;
        r4 = mPosY + 0x90000;
    }

    {
        if ((data_0209f498[gActivePlayerSlot].held & 2) == 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        if (data_0209f498[gActivePlayerSlot].pressed & 0x400) {
            if (func_ov002_020d674c() != 0)
                this->ChangeState(data_ov002_0211004c);
            else
                this->ChangeState(data_ov002_021105a4);
            return 1;
        }
        if (data_0209f498[gActivePlayerSlot].cnt != 0)
            data_0209f498[gActivePlayerSlot].cnt = 5;
    }

    switch (mStateStep) {
    case 0:
        func_ov002_020cf384();
        if (this->FinishedAnim() != 0) {
            mStateStep = 1;
            this->SetAnim(data_ov002_0210a60c[mStateWork & 1], 0, 0x1000, 0);
        }
        {
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
            if ((u32)(anim.currFrame << 4) >> 0x10 < 0x21)
                break;
        }
    case 1:
        func_ov002_020cf384();
        if (data_0209f498[gActivePlayerSlot].mag != 0) {
            mStateWork ^= 1;
            mStateStep = 2;
            spd = mHorzSpeed >> 3;
            if (spd < 0x800)
                spd = 0x800;
            this->SetAnim(data_ov002_0210a60c[(mStateWork & 1) + 2], 0x40000000, spd, 0);
        }
        break;
    case 2:
        func_ov002_020cf2f8();
        spd = mHorzSpeed >> 3;
        if (spd < 0x800)
            spd = 0x800;
        {
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
            anim.speed = spd;
        }
        if (this->FinishedAnim() != 0) {
            Sound::PlayBank0(0xb2, mCamSpacePos);
            if (data_0209f498[gActivePlayerSlot].mag != 0) {
                mStateWork ^= 1;
                this->SetAnim(data_ov002_0210a60c[(mStateWork & 1) + 2], 0x40000000, 0x1000, 0);
            } else {
                mStateStep = 1;
                this->SetAnim(data_ov002_0210a60c[mStateWork & 1], 0, 0x1000, 0);
            }
        }
        break;
    }

    mPosY = r4 - 0x90000;
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_CeilingGrate_Init() {
    func_ov002_020cf384();
    mStateStep = 0;
    mStateWork = 0;
    this->SetAnim(0x5b, 0x40000000, 0x1000, 0);
    return 1;
}

extern "C" {

extern "C" int func_ov002_020cf700(daPly_c *player) {
    return player->IsState(data_ov002_0211001c);
}
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_OnWall_Main() {
    extern ControllerInput data_0209f498[];
    extern void func_ov002_020eee3c(char *, char *);
    u16 f = mStateFlags;
    u16 b0 = f & 1;
    if (b0 != 0) {
        func_ov002_020c0364(3);
        return 1;
    }
    u16 b1 = f & 4;
    if (b1 != 0) {
        func_ov002_020cabe0();
        return 1;
    }
    if (func_ov002_020c5244() != 0)
        return 1;
    if (func_ov002_020d36d8(0) != 0) {
        mPrevAngleY = mAngleY;
        return 1;
    }
    if ((mClsnFlags & 2) == 0) {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }
    if (mStateStep == 2) {
        if (data_0209f498[gActivePlayerSlot].mag == 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        int a = cstd::atan2(mWallNormalX, mWallNormalZ) + 0x8000;
        int d = AngleDiff((s16)a, mDesiredAngleY);
        if (d >= 0x4000) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        if (d >= 0x1555) {
            mStateStep = 0;
            if ((s16)a == (s16)(mDesiredAngleY + d))
                mStateStep = 1;
            this->St_OnWall_Init();
            return 1;
        }
        ApproachLinear(mAngleY, (s16)a, 0x800);
        mPrevAngleY = a;
        mHorzSpeed = func_ov002_020bf224(0xa000, 0x2000);
        func_ov002_020eee3c((char *)&mMeshClsn, (char *)this);
    } else {
        if (data_0209f498[gActivePlayerSlot].mag != 0) {
            int ang = cstd::atan2(mWallNormalX, mWallNormalZ);
            int d = AngleDiff((s16)(ang + 0x8000), mDesiredAngleY);
            if (d >= 0x4000) {
                this->ChangeState(data_ov002_0211013c);
                return 1;
            }
            if (d < 0x1555) {
                mStateStep = 2;
                this->St_OnWall_Init();
                return 1;
            }
            int a2 = ang + 0x8000;
            ApproachLinear(mAngleY, (s16)a2, 0x800);
            u32 anim;
            if ((s16)a2 == (s16)(mDesiredAngleY + d)) {
                mPrevAngleY = ang + 0x4000;
                anim = 0x5d;
            } else {
                mPrevAngleY = ang - 0x4000;
                anim = 0x5c;
            }
            Fix12i spd = FX_MUL(mHorzSpeed, 0x600);
            if (spd < 0x400)
                spd = 0x400;
            this->SetAnim(anim, 0, spd, 0);
            func_ov002_020d4d88(func_ov002_020bf224(0x6000, 0x1000), 0x1000);
        } else {
            mHorzSpeed = 0;
            this->SetAnim(0x5e, 0, 0x1000, 0);
        }
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_OnWall_Init() {
  int r3=0x1000;
  if(param1==2){
    if(mStateStep==2) r3=0x2000;
  }
  this->SetAnim(data_ov002_020ff164[mStateStep], 0, r3, 0);
  mHorzSpeed=0;
  return 1;
}

int daPly_c::func_ov002_020cfaf0() {
    dBgCh_Lin line;
    Vector3 v1;
    Vector3 v2;
    {
        int ang = mAngleY;
        int j = 2 * (((unsigned short)(short)(ang + 0x8000)) >> 4);
        int s = FX_SinCosTable_[j] * 32;
        int t = FX_SinCosTable_[j + 1] * 32;
        int ax = mPosX + s;
        int az = mPosZ + t;
        int ay = mPosY - 0x20000;
        v1.x = ax;
        v1.y = ay;
        v1.z = az;
        int bx = mPosX + s;
        int bz = mPosZ + t;
        int by = mPosY + 0x20000;
        v2.x = bx;
        v2.y = by;
        v2.z = bz;
    }
    line.SetObjAndLine(v1, v2, this);
    if (line.DetectClsn()) {
        return 1;
    }
    return 0;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
int daPly_c::func_ov002_020cfbdc() {
    Vector3 pts[3];
    int idx = (unsigned short)(short)(mAngleY + 0x8000) >> 4;
    int r6 = FX_SinCosTable_[idx * 2];
    int r5 = FX_SinCosTable_[idx * 2 + 1];

    if (mStateWaitTimer == 0) {
        if ((mClsnFlags & 2) != 0) {
            if (mMeshClsn.IsOnGround() != 0) {
                dBgPi *fr = mMeshClsn.GetFloorResult();
                if (fr->GetClsnID() != -1) {
                    if (dActor_c::FindWithID(fr->GetClsnID()) != 0) {
                        func_ov002_020d0948();
                        return 1;
                    }
                }
            }
        }
    }

    {
        int az = mPosZ + (r5 << 5);
        int ay = mPosY - 0xa000;
        int ax = mPosX + (r6 << 5);
        pts[1].x = ax;
        pts[1].y = ay;
        pts[1].z = az;
    }
    {
        int bx = mPosX + r6 * 0x60;
        int bz = mPosZ + r5 * 0x60;
        int by = mPosY - 0xa000;
        pts[2].x = bx;
        pts[2].y = by;
        pts[2].z = bz;
    }

    dBgCh_Lin line;
    line.SetObjAndLine(pts[1], pts[2], this);
    if (line.DetectClsn()) {
        mPosX += r5 * 0x60;
        mPosY -= 0x80000;
        mPosZ += r5 * 0x60;
        ChangeState(data_ov002_021101b4);
        return 1;
    }
    return 0;
}

// func_ov002_020cfd84 at 0x020cfd84
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
int daPly_c::func_ov002_020cfd84(void *a, void *b) {
    dBgCh_Lin line;
    dBgPi result;
    line.SetObjAndLine(*(Vector3 *)a, *(Vector3 *)b, this);
    if (line.DetectClsn()) {
        result.surface = line.surface;
        result.triangleID = line.triangleID;
        result.colliderIdx = line.colliderIdx;
        result.clsnID = line.clsnID;
        result.owner = line.owner;
        result.collider = line.collider;
        if (result.GetClsnID() != -1) {
            void *act = dActor_c::FindWithID(result.GetClsnID());
            if (act != 0) {
                int t = (((dActor_c *)act)->actorID == 0x12a);
                if (t == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* recovered: daPly_c ground/edge probe used while carrying or standing on
 * another actor.
 *
 * With mStateWaitTimer at zero and bit 1 of mClsnFlags set, the player first
 * asks its dBgCh_Actr (mMeshClsn) whether it stands on, or presses against, a
 * collider that belongs to another actor: standing on one that faces more
 * than 90 degrees away, or touching one as a wall, ends the probe with 1.
 * Otherwise a dBgCh_Gnd is built 32 units ahead of the player along its
 * facing angle (sin/cos from FX_SinCosTable_, 1.19.12 units shifted up by 5)
 * and 0x14000 above it; ground within 0x50000 below the player's own height
 * ends the probe with 1. The remaining two checks hand func_ov002_020cfd84 a
 * pair of points 0xa000 below that probe: the point itself, and the point
 * moved 60 units to the player's right (angle + 0x4000), then 60 units to
 * its left. pts[] holds the eight Vector3 slots of the ROM's frame: pts[1]
 * is the ground probe, pts[2]/pts[3] the base pair, pts[4]/pts[5] and
 * pts[6]/pts[7] the copies handed to each call.
 *
 * Two spellings are load-bearing for the register colouring of the tail
 * (the 6bs/6ct receiver lever): the sideways offsets are kept in `dx`/`dz`,
 * the locals that already cross the first func_ov002_020cfd84 guard, as the
 * finished products tbl[t] * 60 (the ROM keeps exactly those products live
 * across the call); and the lowered y is computed before pts[1].x/.z are
 * reloaded, with the second lowered y computed after the two products. With
 * both in place 2004/b56 gives pts[1].x ip, pts[1].z r3, the lowered y lr,
 * the two sums r8/r7 and the second lowered y sb, which is the ROM's tail.
 */
int daPly_c::func_ov002_020cfea4() {

    Vector3 pts[8];
    dBgPi *fr;
    void *other;
    int idx, dx, dz;
    if (mStateWaitTimer == 0 && (mClsnFlags & 2) != 0) {
        if (mMeshClsn.IsOnGround() != 0) {
            fr = mMeshClsn.GetFloorResult();
            if (fr->GetClsnID() != -1) {
                other = dActor_c::FindWithID(fr->GetClsnID());
                if (other != 0) {
                    if (AngleDiff(mAngleY, ((dActor_c *)other)->mAngleY) >= 0x4000)
                        return 1;
                }
            }
        }
        if (mMeshClsn.IsOnWall() != 0) {
            fr = mMeshClsn.GetWallResult();
            if (fr->GetClsnID() != -1) {
                other = dActor_c::FindWithID(fr->GetClsnID());
                if (other != 0)
                    return 1;
            }
        }
    }
    dBgCh_Gnd rg;
    {
        int y;
        const short *tbl;
        int ang = mAngleY;
        tbl = FX_SinCosTable_;
        y = mPosY;
        idx = (unsigned short)(short)(ang + 0x8000) >> 4;
        dz = tbl[(idx << 1) + 1];
        dx = tbl[idx << 1];
        {
            int z = mPosZ;
            int x = mPosX;
            int pz = z + (dz << 5);
            int px = x + (dx << 5);
            int py = y + 0x14000;
            pts[1].x = px;
            pts[1].y = py;
            pts[1].z = pz;
        }
    }
    rg.SetObjAndPos(pts[1], this);
    if (rg.DetectClsn() != 0) {
        if (rg.clsnY >= mPosY - 0x50000) {
            return 1;
        }
    }
    {
        int y = mPosY;
        int yl = y - 0xa000;
        int px = pts[1].x;
        int pz = pts[1].z;
        pts[2].x = px;
        pts[2].y = yl;
        pts[2].z = pz;
        {
            const short *tbl;
            int y2;
            int ang = mAngleY;
            y2 = mPosY;
            tbl = FX_SinCosTable_;
            idx = (unsigned short)(short)(ang + 0x4000) >> 4;
            {
                int t = idx << 1;
                int yl2;
                dx = tbl[t] * 60;
                dz = tbl[t + 1] * 60;
                yl2 = y2 - 0xa000;
                pts[3].x = px + dx;
                pts[3].y = yl2;
                pts[3].z = pz + dz;
            }
        }
    }
    pts[4] = pts[2];
    pts[5] = pts[3];
    if (func_ov002_020cfd84(&pts[4], &pts[5]) != 0) {
        return 1;
    }
    pts[3].x = pts[1].x - dx;
    pts[3].z = pts[1].z - dz;
    pts[6] = pts[2];
    pts[7] = pts[3];
    if (func_ov002_020cfd84(&pts[6], &pts[7]) != 0) {
        return 1;
    }
    return 0;
}

int daPly_c::func_ov002_020d0178(int arg1, int flag) {

    Vector3 normal;
    Vector3 pos1;
    Vector3 lineStart;
    dBgCh_Gnd rg;
    int ang;
    int gy;
    int res;

    if (flag != 0)
        ang = (s16)(mAngleY + 0x8000);
    else
        ang = mAngleY;
    {
        u8 b7 = mIsMega;
        int d = 0x40;
        int i;
        int s, csn;
        int xoff, zoff;
        int h;
        if (b7 != 0)
            d = 0xd0;
        i = (((u16)ang) >> 4) * 2;
        d <<= 12;
        s = FX_SinCosTable_[i];
        csn = FX_SinCosTable_[i + 1];
        xoff = FX_MUL(d, s);
        zoff = FX_MUL(d, csn);
        h = 0xc8;
        if (b7 != 0)
            h = 0x1f4;
        {
            int tz = mPosZ + zoff;
            int ty = mPosY + (h << 12);
            int tx = mPosX + xoff;
            pos1.x = tx;
            pos1.y = ty;
            pos1.z = tz;
        }
    }
    rg.SetObjAndPos(pos1, this);
    if (!rg.DetectClsn()) {
        return 0;
    }
    gy = rg.clsnY;
    if (flag != 0) {
        if (gy > mPosY + 0x64000 || gy < mPosY) {
            return 0;
        }
    } else {
        int lo = 0x64;
        if (mIsMega != 0)
            lo = 0x190;
        if (gy > mPosY + ((lo + 0x64) << 12) || gy < mPosY + (lo << 12)) {
            return 0;
        }
    }
    rg.surface.CopyNormalTo(normal);
    if (normal.y < 0xf04) {
        return 0;
    }
    dBgCh_Lin rl;
    pos1.y = gy;
    {
        int tz = mPosZ;
        int tx = mPosX;
        lineStart.x = tx;
        lineStart.y = arg1;
        lineStart.z = tz;
    }
    rl.SetObjAndLine(lineStart, pos1, this);
    if (!rl.DetectClsn()) {
        return 0;
    }
    rl.surface.CopyNormalTo(normal);
    if (normal.y >= 0x2c7) {
        return 0;
    }
    res = cstd::atan2(normal.x, normal.z);
    {
        int diff = AngleDiff(res, mAngleY);
        if (flag != 0) {
            if (diff < 0)
                diff = -diff;
            if (diff > 0x4000) {
                return 0;
            }
        } else {
            if (diff < 0)
                diff = -diff;
            if (diff < 0x4000) {
                return 0;
            }
        }
    }
    ::Vector3 clsnPos = rl.GetClsnPos();
    lineStart.y += 0x64000;
    pos1.y += 0x64000;
    rl.SetObjAndLine(lineStart, pos1, this);
    if (rl.DetectClsn()) {
        return 0;
    }
    lineStart.y = mPosY;
    Vector3 line2;
    line2.x = mPosX;
    line2.y = mPosY;
    line2.z = mPosZ;
    line2.y = gy + 0xa000;
    rl.SetObjAndLine(lineStart, line2, this);
    if (rl.DetectClsn()) {
        return 0;
    }
    mPosX = clsnPos.x - normal.x;
    mPosZ = clsnPos.z - normal.z;
    mPosY = gy;
    mAngleY = (u16)(res + 0x8000);
    return 1;
}

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
int daPly_c::func_ov002_020d0580() {

    int b354, b358;

    if (GetHealth() != 0 && mIsNoControl == 0 && mIsUnderwater == 0) {
        b354 = (mRidingShell != 0);
        if (b354 == 0 && mClimbClsn == 0) {
            b358 = (mHeldObj != 0);
            if (b358 == 0 && mIsTakingDamage == 0 && mIsBalloon == 0 && IsInAir() != 0 &&
                func_ov002_020d0d2c() == 0) {
                // else-return grows the function 0x140 -> 0x158
                goto cont;
            }
        }
    }
    return 0;

cont:
    if (IsState(data_ov002_021105a4)) {
        if (mStateStep != 0)
            return 0;
    }
    if (mIsAirborne == 0 || mVertSpeed > 0)
        return 0;
    if (func_ov002_020d0178(mPosY, 0) == 0)
        return 0;
    mStateStep = 1;
    ChangeState(data_ov002_0210ffec);
    return 1;
}

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
int daPly_c::func_ov002_020d06c0() {

    int thresh, b354, b358;

    if (!IsState(data_ov002_021101b4))
        return 0;

    if (GetHealth() != 0 && mHoldHeavyTimer == 0 && mIsUnderwater == 0) {
        b354 = (mRidingShell != 0);
        if (b354 == 0 && mClimbClsn == 0) {
            b358 = (mHeldObj != 0);
            if (b358 == 0 && mIsTakingDamage == 0 && mIsBalloon == 0 &&
                IsState(data_ov002_0211001c) == 0 && mIsAirborne != 0) {
                // else-return grows the function 0x164 -> 0x17c
                goto cont;
            }
        }
    }
    return 0;

cont:
    if (mHorzSpeed > ScaleByCharFactor(0x14000))
        return 0;

    thresh = 0xa0;
    if (mIsMega != 0)
        thresh = 0x1c2;

    if (mGroundY > mPosY - thresh * 0x1000)
        return 0;

    if (func_ov002_020d0178(mPosY, 1) == 0)
        return 0;

    mStateStep = 0;
    ChangeState(data_ov002_0210ffec);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_LedgeGrab_Main() {
    if (this->FinishedAnim())
        this->ChangeState(data_ov002_0211013c);
    if (this->IsAnim(0x20)) {
        int id = this->GetBodyModelID(param1 & 0xff, 0);
        dExtAnmModel_c *anim = mModels.mBodyModels[id];
        if (anim->WillHitFrame(0x1e))
            Sound::PlayBank0(mGroundSoundType + 0x40, mCamSpacePos);
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_LedgeGrab_Init() {

    if (mStateStep) {
        this->SetAnim(0x20, 0x40000000, 0x1000, 0);
        Sound::PlayCharVoice(mCharacter, 0x1a, mCamSpacePos);
    } else {
        this->SetAnim(0x23, 0x40000000, 0x1000, 0);
        Sound::PlayCharVoice(mCharacter, 0x17, mCamSpacePos);
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_LedgeHang_Cleanup() {

    func_ov002_020e63a4();
    return 1;
}

void daPly_c::func_ov002_020d0948() {

    /* The two header reads go through a byte pointer: with the int-typed
       self, mwcc materializes the +0x703 offset from the literal pool
       (ldr r1,[pc] / ldrb r2,[r0,r1]) instead of folding it into the ldrb. */
    char *s = (char *)this;
    daPly_c *player = (daPly_c *)s;
    int base = (short)(player->mAngleY + 0x8000);
    if (player->mIsMega != 0) {
        int idx = (unsigned short)base >> 4;
        int *p0 = (&mPosX);
        int *p1 = (&mPosY);
        int *p2 = (&mPosZ);
        *p0 = FX_SinCosTable_[idx * 2] * 0x110 + *p0;
        *p1 = *p1 - 0x12c000;
        *p2 = FX_SinCosTable_[idx * 2 + 1] * 0x110 + *p2;
    } else {
        int idx = (unsigned short)base >> 4;
        int *p0 = (&mPosX);
        int *p1 = (&mPosY);
        int *p2 = (&mPosZ);
        *p0 = *p0 + (FX_SinCosTable_[idx * 2] << 6);
        *p1 = *p1 - 0x64000;
        *p2 = *p2 + (FX_SinCosTable_[idx * 2 + 1] << 6);
    }
    mHorzSpeed = -0x8000;
    ChangeState(data_ov002_021101b4);
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_LedgeHang_Main() {

    extern ControllerInput data_0209f498[];

    int v = mGroundY;
    if (v != (int)0x80000000) {
        int d = v - mPosY;
        if (d < 0) d = -d;
        if (d < 0x28000) {
            mPosY = v;
            mIsAirborne = 0;
            if (mFloorNormalY < 0xddb) {
                func_ov002_020d0948();
                return 1;
            }
        }
    }

    if (mIsAirborne != 0) {
        func_ov002_020d0948();
        return 1;
    }

    if (mStateStep == 0) {
        if (this->FinishedAnim()) {
            this->SetAnim(0x21, 0, 0x1000, 0);
            mStateStep++;
        }
    } else {
        if ((!func_ov002_020cfaf0() && (data_0209f498[gActivePlayerSlot].pressed & 2)) ||
            func_ov002_020cfea4()) {
            mStateStep = 1;
            this->ChangeState(data_ov002_02110004);
        } else {
            if (func_ov002_020cfbdc())
                return 1;
            if (data_0209f498[gActivePlayerSlot].pressed & 0x400) {
                func_ov002_020d0948();
                return 1;
            }
            {
                if (mStateTimer == 0 && data_0209f498[gActivePlayerSlot].mag != 0) {
                    if (AngleDiff(mDesiredAngleY, mAngleY) >= 0x4000) {
                        func_ov002_020d0948();
                    } else if (!func_ov002_020cfaf0()) {
                        mStateStep = 0;
                        this->ChangeState(data_ov002_02110004);
                    }
                }
            }
        }
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_LedgeHang_Init() {
    /* The POD VecFx32, not Vector3, measured: `VecFx32 z3 = {0, 0, 0}` on a POD
       lowers to `add r1, sp, #4` plus three stores through that pointer and
       two reloads, which is what the ROM does. Vector3 has a constructor, so every
       non-POD spelling constant-folds the zeros into the later stores and the
       function loses 12 bytes. Tested as Vector3(0,0,0), default plus three
       stores, copy-assignment, member-wise construction, Vector3(), and stores
       through a plain and a volatile pointer -- none reproduce it. */

    mStateWaitTimer = 0;
    if (mStateStep == 0) {
        this->SetAnim(0x22, 0x40000000, 0x1000, 0);
    } else {
        this->SetAnim(0x21, 0, 0x1000, 0);
    }
    mStateWaitTimer = 2;
    mPrevAngleY = mAngleY;
    mVertSpeed = 0;
    mHorzSpeed = 0;
    mIsAirborne = 0;
    Sound::PlayCharVoice(mCharacter, 0x16, mCamSpacePos);
    {
        VecFx32 z3 = {0, 0, 0};
        VecFx32 off;
        z3.y = -0x64000;
        off.x = z3.x;
        off.y = z3.y;
        off.z = z3.z;
        mdCcAcPos_c.SetPosRelativeToActor(*(Vector3 *)&off);
    }
    mStatePhase = 0;
    return 1;
}

int daPly_c::func_ov002_020d0d2c() {
    if (IsState(data_ov002_0210ffec) || IsState(data_ov002_02110004))
        return 1;
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_WallSlide_Main() {
    typedef int Fix12i;
    extern unsigned char data_0209f49e[];

    mPeakY = mPosY;
    if ((mIsAirborne) == 0) {
        func_ov002_020c2f64();
        this->ChangeState(data_ov002_0211013c);
    } else {
        if ((mIsMega) != 0) {
            Particle::MegaRunningSlidingDustAt(mPosX, mPosY, mPosZ);
        } else {
            Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
        }
        mLoopingSoundHandle = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2,
                                            &mCamSpacePos, mHorzSpeed, 0);
        if ((*((unsigned short *)(&data_0209f49e[gActivePlayerSlot * 0x18]))) & 2) {
            if (mStateTimer) {
                mAngleY = mAngleYSpeed;
            }
            this->ChangeState(data_ov002_021103dc);
        } else {
            if (((mClsnFlags) & 2) == 0) {
                this->ChangeState(data_ov002_021101b4);
            }
        }
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_WallSlide_Init() {

    this->SetAnim(0x29, 0x40000000, 0x1000, 0);
    mHorzSpeed = 0;
    if (mVertSpeed >= 0)
        mVertSpeed = 0;
    mStateTimer = 7;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Crawl_Main() {
    extern ControllerInput data_0209f498[];

    int a = 0;
    int b = 0;

    switch (mStateStep) {
    case 3:
        if (this->FinishedAnim()) {
            mStateStep = 4;
            this->SetAnim(0x63, 0, 0x1000, 0);
        }
        break;
    case 4: {
        s16 val1 = data_0209f498[gActivePlayerSlot].mag;
        if (val1 >= 0x200 &&
            ((data_0209f498[gActivePlayerSlot].held & 0x400) != 0 || func_ov002_020d1164() != 0)) {
            int id = this->GetBodyModelID(param1 & 0xff, 0);
            dExtAnmModel_c *anim = mModels.mBodyModels[id];
            if (anim->WillHitFrame(0xe) ||
                mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(0x1c)) {
                Sound::PlayBank0(mGroundSoundType + 0xc0, mCamSpacePos);
            }
            int shifted = mHorzSpeed >> 2;
            dExtFrameCtrl_c &anim3 = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
            anim3.speed = shifted;
            if (func_ov002_020d36d8(1))
                return 1;
            int r4v = ScaleByCharFactor(0x8000);
            int r2v = ScaleByCharFactor(0x1000);
            a = func_ov002_020bf224(r4v, r2v);
            b = 0x1000;
            ApproachAngle(&mPrevAngleY, mDesiredAngleY, 0x10, 0x2000, 0x100);
            mAngleY = mPrevAngleY;
            func_ov002_020c18b0(0);
        } else {
            mHorzSpeed = 0;
            mStateStep = 5;
            this->SetAnim(0x62, 0x40000000, 0x1000, 0);
        }
        break;
    }
    case 5:
        if (this->FinishedAnim()) {
            mStateStep = 1;
            this->ChangeState(data_ov002_021104e4);
        }
        break;
    }

    func_ov002_020d4d88(a, b);
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Crawl_Init() {

    mStateStep = 3;
    this->SetAnim(0x64, 0x40000000, 0x1000, 0);
    return 1;
}

// func_ov002_020d1164 at 0x020d1164
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).

int daPly_c::func_ov002_020d1164() {
    dBgCh_Lin line;
    Vector3 v1;
    Vector3 v2;
    int y;
    v1.x = mPosX;
    v1.y = mPosY;
    v1.z = mPosZ;
    v2.x = mPosX;
    y = mPosY;
    v2.y = y;
    v2.z = mPosZ;
    v2.y = y + 0x96000;
    line.SetObjAndLine(v1, v2, this);
    if (line.DetectClsn()) {
        return 1;
    }
    return 0;
}

int daPly_c::func_ov002_020d1204() {
    extern unsigned char data_0209f49e[];

    if (*(unsigned short*)(data_0209f49e + gActivePlayerSlot * 0x18) & 2) {
        int v = mHorzSpeed;
        if (v == 0) {
            ChangeState(data_ov002_0211052c);
        } else if (mStateTimer != 0 && v > 0xa000) {
            ChangeState(data_ov002_0211055c);
            return 1;
        } else {
            ChangeState(data_ov002_0211019c);
        }
        return 1;
    }
    return 0;
}

int daPly_c::func_ov002_020d12b0() {
    extern u8 data_0209f49e[];

    // early return drops the shared return-0 block (size 0xa4 -> 0xa0)
    if ((*(u16 *)(data_0209f49e + gActivePlayerSlot * 0x18) & 1) == 0) goto ret0;
    if (mIsMega != 0) {
        mPunchKickStep = 2;
        ChangeState(data_ov002_02110574);
    } else {
        if (mHorzSpeed >= ScaleByCharFactor(0xa000)) {
            ChangeState(data_ov002_02110634);
        } else {
            ChangeState(data_ov002_0211061c);
        }
    }
    return 1;
ret0:
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Crouch_Main() {
    extern ControllerInput data_0209f498[];
    func_ov002_020bf90c();

    if (mIsAirborne != 0) {
        int d = mPosY - mGroundY;
        if (d >= 0x32000) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
    }

    switch (mStateStep) {
    case 0:
        if (this->IsAnim(0x2d) && this->FinishedAnim()) {
            this->SetAnim(0x2c, 0, 0x1000, 0);
            mStateStep = 1;
        }
        if (func_ov002_020d12b0())
            return 1;
        func_ov002_020d1204();
        break;
    case 1:
        if ((data_0209f498[gActivePlayerSlot].held & 0x400) != 0 || mHorzSpeed != 0 ||
            func_ov002_020d1164() != 0) {
            if (mHorzSpeed != 0) {
                func_ov002_020c06fc(0x4000);
                func_ov002_020dd2f4();
            } else {
                if (mIsMega == 0) {
                    if (data_0209f498[gActivePlayerSlot].mag >= 0x200) {
                        this->ChangeState(data_ov002_02110514);
                        return 1;
                    }
                }
            }
            u16 bit = mStateFlags & 1;
            if (bit != 0) {
                func_ov002_020c0364(1);
                return 1;
            }
        } else {
            this->SetAnim(0x2e, 0x40000000, 0x1000, 0);
            mStateStep = 2;
        }
        if (func_ov002_020d12b0())
            return 1;
        if (func_ov002_020d1204() != 0)
            return 1;
        break;
    case 2:
        if (this->IsAnim(0x2e) && this->FinishedAnim()) {
            this->ChangeState(data_ov002_0211013c);
            mCrouchTimer = 0xa;
        } else {
            func_ov002_020d1204();
        }
        break;
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Crouch_Init() {
    extern unsigned char data_0209f49c[];

    if (mDashBoostTimer) {
        Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
    }
    mStateTimer = 0x1e;
    if (data_0209f498[gActivePlayerSlot].held & 2) {
        if (func_ov002_020d1204())
            return 1;
    }
    if (mStateStep == 0) {
        this->SetAnim(0x2d, 0x40000000, 0x1000, 0);
        mPrevAngleY = mAngleY;
    } else if (mStateStep == 1) {
        this->SetAnim(0x2c, 0, 0x1000, 0);
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_HoldHeavy_Main() {
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern u8 data_0209f4ac[];
    extern u16 data_0209f49c[];

    int var_r1 = 0;

    if (mHeldObj == 0) {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }

    if (mStateStep == 0) {
        u32 anim = data_ov002_020ff254[mStateWork];
        if (this->IsAnim(anim)) {
            if (this->FinishedAnim()) {
                mStateStep = 1;
            } else {
                u32 arg = (u8)param1;
                int modelIdx = this->GetBodyModelID(arg, 0);
                if (mModels.mBodyModels[modelIdx]->WillHitFrame(6)) {
                    dActor_c *heavy = mHeldObj;
                    if (heavy != 0) {
                        heavy->mFlags |= 0x4000;
                    }
                }
            }
        }
        ApproachLinear(mHorzSpeed, 0, 0x1000);
    } else {

        {
            u16 flags = data_0209f498[gActivePlayerSlot].pressed;
            if (flags & 1) {
                this->ChangeState(data_ov002_021105d4);
                return 1;
            }

            if (mIsAirborne != 0) {
                DropHeldObj();
                this->ChangeState(data_ov002_021101b4);
                mHoldHeavyTimer = 0xa;
                return 1;
            }

            {
                s16 t = data_0209f498[gActivePlayerSlot].mag;
                if (t != 0) {
                    ApproachAngle(&mPrevAngleY, mDesiredAngleY, 8, 0x1000, 0x400);
                    var_r1 = data_ov002_020ff1d0[param1];
                }
            }
        }

        {
            if (data_0209f498[gActivePlayerSlot].touching == 0) {
                if (!(data_0209f498[gActivePlayerSlot].held & 0x800)) {
                    var_r1 >>= 1;
                }
            } else {
                var_r1 = func_ov002_020bf224(var_r1, 0);
            }
            func_ov002_020d4d88(var_r1, 0x1000);
        }

        if (mHorzSpeed == 0) {
            mAngleY = mPrevAngleY;
        } else if (param1 == 1) {
            ApproachAngle(&mAngleY, mPrevAngleY, 0x10, 0x1000, 0x100);
        } else {
            ApproachAngle(&mAngleY, mPrevAngleY, 8, 0x2000, 0x400);
        }

        if (mHorzSpeed == 0) {
            if (this->FinishedAnim() ||
                !mModels.mBodyModels[this->GetBodyModelID((u8)param1, 0)]->GetFlags()) {
                this->SetAnim(data_ov002_020ff254[mStateWork + 2], 0, 0x1000, 0);
            }
        } else {
            func_ov002_020d1f78(data_ov002_020ff254[mStateWork + 4]);
        }
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_HoldHeavy_Init() {

  mStateWork=0;
  this->SetAnim(data_ov002_020ff254[mStateWork], 0x40000000, 0x1000, 0);
  Sound::PlayCharVoice(mCharacter, 0x12, mCamSpacePos);
  return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_HoldLight_Cleanup() {

    char *p = (char *)mHeldObj;
    if (p) {
        ((dActor_c *)p)->mFlags |= 0x4000;
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_HoldLight_Main() {
    extern s16 data_0209f4a0[];
    extern u8 data_0209f4ac[];
    extern u16 data_0209f49c[];
    extern u16 data_0209f49e[];

    if (mHeldObj == 0) {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }
    if (func_ov002_020c0434()) {
        func_ov002_020c0364(3);
        return 1;
    }

    if (mStateStep == 0) {
        ApproachLinear(mHorzSpeed, 0, 0x800);
        if (this->IsAnim(0x2f) || this->IsAnim(0x86)) {
            if (this->FinishedAnim()) {
                if (func_ov002_020e0ccc(mHeldObj)) {
                    return 1;
                }
                mPrevAngleY = mAngleY;
                mHorzSpeed = 0;
                mStateStep = 1;
            } else {
                u32 arg = (u8)param1;
                int modelIdx = this->GetBodyModelID(arg, 0);
                if (mModels.mBodyModels[modelIdx]->WillHitFrame(6)) {
                    dActor_c *light = mHeldObj;
                    if (light != 0) {
                        u32 *p = &light->mFlags;
                        *p |= 0x4000;
                    }
                }
            }
        }
    } else {

        if (func_ov002_020c5244()) {
            return 1;
        }

        if (mSinkDepth > 0x19000) {
            ReleaseHeldActor();
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }

        int var_r4 = 0;
        if (data_0209f498[gActivePlayerSlot].mag != 0) {
            s16 t = mDesiredAngleY;
            if (mStateArg != 0) {
                mPrevAngleY = t;
                mStateArg = 0;
            }
            ApproachAngle(&mPrevAngleY, t, 4, 0x2000, 0x800);
            var_r4 = data_ov002_020ff1c0[param1];
        }

        if (data_0209f498[gActivePlayerSlot].touching == 0) {
            if (!(data_0209f498[gActivePlayerSlot].held & 0x800)) {
                var_r4 = FX_MUL(var_r4, 0xa00);
            }
        } else {
            var_r4 = func_ov002_020bf224(var_r4, 0);
        }

        mStateArg = 0;
        if (mHorzSpeed == 0) {
            mStateArg = 1;
            mAngleY = mPrevAngleY;
        } else {
            ApproachAngle(&mAngleY, mPrevAngleY, 8, 0x2000, 0x800);
        }

        func_ov002_020d4d88(var_r4, 0x1000);

        {
            u16 flags = data_0209f498[gActivePlayerSlot].pressed;
            if (flags & 1) {
                this->ChangeState(data_ov002_021105d4);
                return 1;
            }
            if (flags & 2) {
                mJumpComboTimer = 0;
                this->ChangeState(data_ov002_0211019c);
                return 1;
            }
            if (flags & 0x400) {
                if (ReleaseHeldActor()) {
                    this->ChangeState(data_ov002_0211013c);
                }
                return 1;
            }
        }

        if (mIsAirborne != 0) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
        if (func_ov002_020d9dcc()) {
            return 1;
        }

        if (mHorzSpeed == 0) {
            if (this->FinishedAnim() ||
                !mModels.mBodyModels[this->GetBodyModelID((u8)param1, 0)]->GetFlags()) {
                dActor_c *light = mHeldObj;
                u32 animId = 0x33;
                if (light != 0) {
                    int b = (light->mFlags & 0x8000) != 0;
                    if (b) {
                        animId = 0x87;
                        if (param1 == 2) {
                            int b2 = (light->actorID == 0xce);
                            if (b2) {
                                animId = 0x33;
                            }
                        }
                    }
                }
                this->SetAnim(animId, 0, 0x1000, 0);
            }
        } else {
            func_ov002_020d1f78(0x48);
        }
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_HoldLight_Init() {

    if (mHorzSpeed >= ScaleByCharFactor(0x1c000)) {
        mHorzSpeed = ScaleByCharFactor(0x1c000);
    }
    if (mStateStep == 0) {
        char *o = (char *)mHeldObj;
        unsigned int anim = 0x2f;
        if (o != 0) {
            int b = (int)((((dActor_c *)o)->mFlags & 0x8000) != 0);
            if (b != 0) {
                anim = 0x86;
                if (param1 == 2) {
                    if ((int)(((dActor_c *)o)->actorID == 0xce) != 0)
                        anim = 0x2f;
                }
            }
        }
        this->SetAnim(anim, 0x40000000, 0x1000, 0);
        Sound::PlayCharVoice(mCharacter, 0x12, mCamSpacePos);
    }
    mStateWork = 0;
    mAngleYSpeed = 0;
    mStateArg = 0;
    return 1;
}

void daPly_c::func_ov002_020d1f78(u32 param) {
    typedef struct { u8 pad[0x18]; } Rec18;
    extern Rec18 data_0209f4ac[];
    extern Rec18 data_0209f49c[];
    extern Rec18 data_0209f4a0[];

    daPly_c *p = this;
    int idx;
    int flag;
    int id;
    u32 w;
    int state;
    int target;

    idx = gActivePlayerSlot;
    flag = 0;
    if (*(u8 *)&data_0209f4ac[idx] == 0) {
        if ((*(u16 *)&data_0209f49c[idx] & 0x800) == 0)
            flag = 1;
    } else {
        if (*(s16 *)&data_0209f4a0[idx] < 0xb33)
            flag = 1;
    }

    if (p->param1 == 2 && flag == 0) {
        dExtFrameCtrl_c &anim = *p->mModels.mBodyModels[GetBodyModelID(p->param1 & 0xff, 0)];
        w = anim.currFrame;
        /* The whole frame of the 20.12 counter; the u16 parameter's narrowing
           folds with the shift into the ROM's lsl #4 / lsr #16. */
        p->SetAnim(0x3f, 0, 0x2800, w >> 12);
    } else {
        int v = p->mHorzSpeed;
        int shift = v >> 3;
        if (shift < 0x400)
            shift = 0x400;
        SetAnim(param, 0, shift, 0);
    }

    id = GetBodyModelID(p->param1 & 0xff, 0);
    if (!p->mModels.mBodyModels[id]->WillHitFrame(4)) {
        id = GetBodyModelID(p->param1 & 0xff, 0);
        if (!p->mModels.mBodyModels[id]->WillHitFrame(0x13))
            return;
    }

    if (p->mIsInShallowWater != 0) {
        u32 newId = Particle::System::NewCheckWaterRipple((u32)p->mParticle1, kPtclShallowWaterLand,
                                                          p->mPosX, data_0209f32c, p->mPosZ, 0);
        p->mParticle1 = newId;
        return;
    }

    func_0201251c(0, p->mGroundSoundType + 0x20, &p->mCamSpacePos, p->mHorzSpeed);

    state = p->mGroundSoundType;
    target = -1;
    if (state != 6) {
        if (state != 8) {
            if (state == 9)
                target = 0xe6;
        } else
            target = 0xe4;
    } else
        target = 0xe5;
    if (target == -1)
        return;

    Particle::System::NewSimple(target, p->mPosX, p->mPosY, p->mPosZ);
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_WaitQuicksand_Main() {

    int v = mSinkDepth;
    if (v < 0x1e000) {
        this->ChangeState(ST_WAIT);
        return 1;
    }
    if (v >= 0x46000) {
        this->SetAnim(0x82, 0, 0x1000, 0);
    }
    if (mSinkDepth >= 0xb4000) {
        if (func_ov002_020c5dec(3))
            return 1;
    }
    func_ov002_020d22ec(0);
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_WaitQuicksand_Init() {

    this->SetAnim(0x47, 0, 0x1000, 0);
    mStateWork = 0;
    return 1;
}

int daPly_c::func_ov002_020d225c() {

    if (param1 == 3 && mSleepStage == 2 && mStatePhase == 0)
        return 1;
    return 0;
}

void daPly_c::func_ov002_020d228c() {

    ChangeState(daPly_c::ST_WAIT);
    SetAnim(9, 0, 0x1000, 0);
    mStateStep = 4;
    mSleepBubbleScaleX = 0;
    mSleepBubbleScaleY = 0;
    mSleepBubbleScaleZ = 0;
    mSleepStage = 2;
    mStateArg = 0;
}

int daPly_c::func_ov002_020d22ec(int arg) {
    extern char data_0209f4a0[];

    if (data_0209f498[gActivePlayerSlot].mag > arg) {
        ChangeState(data_ov002_0211013c);
        return 1;
    }
    if ((unsigned short)(mStateFlags & 4)) {
        func_ov002_020cabe0();
        return 1;
    }
    return func_ov002_020d36d8(0);
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Wait_Cleanup() {
    extern unsigned char gGameMode;
    extern void *gCamera;

    void *r4 = gCamera;
    ((dCamera_c *)r4)->mFlags &= ~0x2000;
    mSleepStage = 0;
    do {
        unsigned char v = gGameMode;
        int b0 = (v == 1);
        if (b0 == 0) {
            if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) != 0) {
                int b1 = (v == 2);
                if (b1 == 0)
                    break;
            }
            if (mAnimScriptId == 0xf)
                mRootMotionMode = 0;
            mAnimScriptId = 0;
            mAnimScriptStep = 0;
        }
    } while (0);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Wait_Main() {

    extern void *gCamera;
    extern unsigned char data_0209f49c[];
    extern char data_0209f4a0[];
    extern unsigned char gGameMode;
    extern unsigned char data_0209f49e[];
    if ((unsigned short)(mStateFlags & 1)) {
        func_ov002_020c0364(3);
        return 1;
    }
    if (func_ov002_020c5244()) {
        return 1;
    }
    if (func_ov002_020c6adc()) {
        return 1;
    }
    if (mSinkDepth >= 0x1e000) {
        this->ChangeState(data_ov002_0211049c);
        return 1;
    }

    {
        unsigned char st = mStateStep;
        void *r4 = gCamera;
        switch (st) {
        default:
            break;

        case 8: {
            if (!(data_0209f498[gActivePlayerSlot].held & 0x800) &&
                mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(0)) {
                Sound::PlayCharVoice(mCharacter, 0x2c, mCamSpacePos);
            }
        }
        case 0: {
            /* structured form changes size 0x99c to 0x9cc */
            unsigned char f2d8_0 = gGameMode;
            int b0 = (f2d8_0 == 1);
            if (b0 == 0) {
                if (data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) {
                    int b1 = (f2d8_0 == 2);
                    if (b1)
                        goto blk0_34;
                    goto blk0_35;
                }
            blk0_34:
                func_ov002_020bcdf0((char *)this);
                goto blk0_36;
            }
        blk0_35:
            if (func_ov002_020d2fdc() == 0) {
            blk0_36: {
                int idx0 = gActivePlayerSlot * 0x18;
                short lvl0 = *(short *)(data_0209f4a0 + idx0);
                if (lvl0 != 0) {
                    int thr0 = (*(data_0209f4ae + idx0) != 2) ? 0x471 : 0x555;
                    if (lvl0 < thr0) {
                        mPrevAngleY = mDesiredAngleY;
                        mAngleY = mPrevAngleY;
                    }
                }
                {
                    int thr0b = (*(data_0209f4ae + idx0) != 2) ? 0x471 : 0x555;
                    func_ov002_020d22ec(thr0b);
                }
            }
            }
            break;
        }

        case 10: {
            if (mIsMetal != 0 || mIsVanish != 0) {
                mStateStep = 0;
                this->SetAnim(0x47, 0, 0x1000, 0);
                mStateTimer = 0x384;
            }
            {
                int idx10 = gActivePlayerSlot * 0x18;
                short lvl10 = *(short *)(data_0209f4a0 + idx10);
                if (lvl10 != 0) {
                    int thr10 = (*(data_0209f4ae + idx10) != 2) ? 0x471 : 0x555;
                    if (lvl10 < thr10) {
                        mPrevAngleY = mDesiredAngleY;
                        mAngleY = mPrevAngleY;
                    }
                }
                {
                    int thr10b = (*(data_0209f4ae + idx10) != 2) ? 0x471 : 0x555;
                    func_ov002_020d22ec(thr10b);
                }
            }
            break;
        }

        case 1: {
            if (this->FinishedAnim()) {
                this->SetAnim(0xc, 0, 0x1000, 0);
                mStateStep = 2;
            }
            {
                unsigned int x1 = param1;
                switch (x1) {
                default:
                    break;
                case 0: {
                    int idA = this->GetBodyModelID(x1 & 0xff, 0);
                    dExtAnmModel_c *animA = mModels.mBodyModels[idA];
                    if (animA->WillHitFrame(0x5a)) {
                        Sound::PlayCharVoice(mCharacter, 0x2d, mCamSpacePos);
                    } else if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]
                                   ->WillHitFrame(0x3a) ||
                               mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]
                                   ->WillHitFrame(0x44)) {
                        Sound::PlayBank0(4, mCamSpacePos);
                    }
                    break;
                }
                case 1: {
                    int idD = this->GetBodyModelID(x1 & 0xff, 0);
                    dExtAnmModel_c *animD = mModels.mBodyModels[idD];
                    if (animD->WillHitFrame(0x6e)) {
                        Sound::PlayCharVoice(mCharacter, 0x2d, mCamSpacePos);
                    } else {
                        int idE = this->GetBodyModelID(param1 & 0xff, 0);
                        dExtAnmModel_c *animE = mModels.mBodyModels[idE];
                        if (animE->WillHitFrame(0x32)) {
                            Sound::PlayBank0(0xb1, mCamSpacePos);
                        }
                    }
                    break;
                }
                case 2: {
                    int idF = this->GetBodyModelID(x1 & 0xff, 0);
                    dExtAnmModel_c *animF = mModels.mBodyModels[idF];
                    if (animF->WillHitFrame(0x20)) {
                        Sound::PlayBank0(2, mCamSpacePos);
                    } else if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]
                                   ->WillHitFrame(0x47) ||
                               mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]
                                   ->WillHitFrame(0x60)) {
                        Sound::PlayBank0(3, mCamSpacePos);
                    }
                    break;
                }
                case 3: {
                    int idI = this->GetBodyModelID(x1 & 0xff, 0);
                    dExtAnmModel_c *animI = mModels.mBodyModels[idI];
                    if (animI->WillHitFrame(0xad)) {
                        Sound::PlayBank0(0x50, mCamSpacePos);
                    }
                    break;
                }
                }
            }
            if (func_ov002_020d22ec(0)) {
                ((dCamera_c *)r4)->mFlags &= ~0x2000;
                Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
            }
            break;
        }

        case 2: {
            if (mMuteSleepVoice == 0 &&
                mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(1)) {
                Sound::PlayCharVoice(mCharacter, 0x2a, mCamSpacePos);
            }
            {
                unsigned char f2d8_2 = gGameMode;
                int b2a = (f2d8_2 == 1);
                if (b2a == 0) {
                    if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) == 0)
                        break;
                    int b2b = (f2d8_2 == 2);
                    if (b2b != false)
                        break;
                }
            }
            if (mStateTimer == 0) {
                Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
                this->SetAnim(0xa, 0x40000000, 0x1000, 0);
                mStateStep = 3;
                mSleepStage = 1;
            }
            if (data_0209f498[gActivePlayerSlot].pressed & 2) {
                this->ChangeState(data_ov002_0211019c);
                ((dCamera_c *)r4)->mFlags &= ~0x2000;
                Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
                return 1;
            }
            if (func_ov002_020d2da0() == 1 || (unsigned short)(mStateFlags & 4) != 0) {
                this->SetAnim(0xb, 0x40000000, 0x1000, 0);
                mStateStep = 9;
            }
            break;
        }

        case 3: {
            if (this->FinishedAnim()) {
                this->SetAnim(9, 0, 0x1000, 0);
                mStateStep = 4;
                mSleepBubbleScaleX = 0;
                mSleepBubbleScaleY = 0;
                mSleepBubbleScaleZ = 0;
                mSleepStage = 2;
                mStateArg = 1;
            }
            if ((unsigned short)(mStateFlags & 4) != 0) {
                func_ov002_020cabe0();
                return 1;
            }
            break;
        }

        case 5: {
            if (this->FinishedAnim()) {
                mStateStep = 6;
                this->SetAnim(0x96, 0, 0x1000, 0);
            }
            func_ov002_020d22ec(0);
            break;
        }

        case 4: {
            if (mStatePhase == 0) {
                func_ov002_020d2f24();
                if (mMuteSleepVoice == 0 &&
                    mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(
                        0xd)) {
                    Sound::PlayCharVoice(mCharacter, 0x2b, mCamSpacePos);
                }
            }
            {
                unsigned char f2d8_4 = gGameMode;
                int b4a = (f2d8_4 == 1);
                if (b4a == 0) {
                    if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) == 0)
                        break;
                    int b4b = (f2d8_4 == 2);
                    if (b4b != false)
                        break;
                }
                if (func_ov002_020d2da0() == 1 || mSleepBubbleScaleX >= 0x3000 ||
                    mIsInShallowWater != 0 || (unsigned short)(mStateFlags & 4)) {
                    func_ov002_020d2e74(this);
                    mSleepStage = 0;
                    this->SetAnim(8, 0x40000000, 0x1000, 0);
                    mStateStep = 9;
                }
            }
            break;
        }

        case 6: {
            if (func_ov002_020d2da0() == 1) {
                this->SetAnim(0x97, 0x40000000, 0x1000, 0);
                mStateStep = 9;
            }
            break;
        }

        case 7: {
            if (this->FinishedAnim()) {
                this->SetAnim(0xf, 0, 0x1000, 0);
                mStateStep = 8;
            }
            if ((unsigned short)(mStateFlags & 0x800) == 0) {
                func_ov002_020d22ec(0);
            }
            break;
        }

        case 9: {
            if (this->FinishedAnim()) {
                this->ChangeState(data_ov002_0211013c);
            }
            break;
        }
        }
    }

    AdvanceAnims();
    return 1;
}

int daPly_c::func_ov002_020d2da0() {
    struct E18 {
        short v;
        char pad[0x16];
    };
    extern struct E18 data_0209f4a0[];
    extern struct E18 data_0209f49e[];
    extern char *gCamera;

    unsigned char idx = gActivePlayerSlot;
    char *p = gCamera;
    int cond = (int)(data_0209f4a0[idx].v >= 0x200);
    if (cond | (((unsigned short)data_0209f49e[idx].v) & 0xc03)) {
        ((dCamera_c *)p)->mFlags &= ~0x2000;
        Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
        return 1;
    }
    if (mIsAirborne == 0)
        return 0;
    ((dCamera_c *)p)->mFlags &= ~0x2000;
    ChangeState(data_ov002_021101b4);
    return 2;
}

/* recovered: shared common types */
extern "C" {

extern "C" void func_ov002_020d2e74(daPly_c *p) {

    Vector3 v;
    Vector3 off;
    if (p->mIsInShallowWater != 0)
        return;
    if (p->param1 != 3)
        return;
    Sound::PlayBank0(0xbc, p->mCamSpacePos);
    Sound::PlayCharVoice(p->mCharacter, 0x2f, p->mCamSpacePos);
    off.x = 0;
    off.y = 0x1e000;
    off.z = 0x64000;
    Vec3_RotateYAndTranslate(&v, &p->mPos, p->mAngleY, &off);
    Particle::System::NewSimple(kPtclPiranhaWakeA, v.x, v.y, v.z);
    Particle::System::NewSimple(kPtclPiranhaWakeB, v.x, v.y, v.z);
}
}

void daPly_c::func_ov002_020d2f24() {

    int v;
    int r4;
    u32 id;

    id = param1;
    if (id != 3) return;

    r4 = mSleepBubbleScaleX;
    {
        dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(id & 0xff, 0)];
        v = (u32)(anim.currFrame << 4) >> 0x10;
    }

    if (v >= 6 && v <= 0x1c) {
        r4 -= 0x180;
    } else if (v >= 0x24 && v <= 0x3a) {
        if (r4 < 0x2800) r4 += 0x180;
        else r4 += 0x300;
    } else if (v == 0x3b) {
        if (mStateArg != 0)
            r4 += 0x10;
    }

    if (r4 < 0x400) r4 = 0x400;
    if (r4 > 0x4000) r4 = 0x4000;
    mSleepBubbleScaleX = r4;
    mSleepBubbleScaleY = r4;
    mSleepBubbleScaleZ = r4;
}

int daPly_c::func_ov002_020d2fdc() {
    typedef struct { u16 flags; char pad[22]; } Entry24;
    extern Entry24 data_0209f49c[];
    extern u8 gGameMode;
    extern void *gCamera;

    int r5;
    int id;

    mDashBoostTimer = 0;
    if (mIsMega != 0) {
        return 0;
    }

    if ((unsigned short)(mStateFlags & 0x800)) {
        return 1;
    }

    if (data_0209f49c[gActivePlayerSlot].flags & 0x800) {
        if (mStateWork < 0x1e) {
            mStateWork += 1;
            r5 = mStateWork * 0x111 + 0x1000;
        } else {
            mDashBoostTimer = 0x1e;
            r5 = 0x3000;
            if (mIsInShallowWater == 0) {
                Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
            }
        }

        SetAnim(0x3f, 0, r5, 0);

        id = GetBodyModelID(param1 & 0xff, 0);
        if (mModels.mBodyModels[id]->WillHitFrame(4) ||
            (id = GetBodyModelID(param1 & 0xff, 0), mModels.mBodyModels[id]->WillHitFrame(0x13))) {
            PlayLandingEffects();
        }
        return 0;
    }

    mStateWork = 0;
    {
        int flag = 0;
        if (gGameMode == 1)
            flag = 1;
        if (flag != 0) {
            SetAnim(0x47, 0, 0x1000, 0);
            return 0;
        }
    }

    if (mSinkDepth > 0x32000) {
        SetAnim(0x7d, 0, 0x1000, 0);
        return 0;
    }

    if (mStateStep == 0) {
        SetAnim(0x47, 0, 0x1000, 0);
        if (mStateTimer == 0) {
            char v = gCurrentSublevel;
            if (v == 0x13 || v == 0xa || (unsigned)(v - 0x30) <= 1) {
                SetAnim(0x95, 0x40000000, 0x1000, 0);
                mStateStep = 5;
            } else {
                mStateTimer = 0x708;
                SetAnim(0xd, 0x40000000, 0x1000, 0);
                mStateStep = 1;
            }
            ((dCamera_c *)gCamera)->mFlags |= 0x2000;
        }
    } else {
        SetAnim(0xf, 0, 0x1000, 0);
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Wait_Init() {
    extern unsigned char gGameMode;

    unsigned char f2d8;
    int b0;

    mStatePhase = 0;
    f2d8 = gGameMode;
    b0 = (f2d8 == 1);
    if (b0 != 0) goto L80;
    if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) == 0)
        goto L50;
    {
        int b1 = (f2d8 == 2);
        if (b1 == 0) goto L80;
    }
L50:
    this->SetAnim(0xb3, 0, 0x1000, 0);
    mStateStep = 0;
    return 1;

L80:
    if (mIsMega == 0) goto Ld8;
    if (b0 != 0) goto Lbc;
    if (data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE)
        goto Lbc;
    this->SetAnim(0xa0, 0, 0x1000, 0);
Lbc:
    mStateTimer = 0x384;
    return 1;

Ld8:
    /* early returns shrink 0x1dc to 0x1d0; || changes beq at +0xe0 from #0x5c to #0x2c */
    if (mToxicSurfaceY == 0x80000000)
        goto L13c;
    if (mPosY >= mToxicSurfaceY - 0x64000)
        goto L13c;
    if (mIsMetal != 0) goto L13c;
    if (mIsVanish != 0) goto L13c;
    mStateStep = 0xa;
    this->SetAnim(5, 0, 0x1000, 0);
    return 1;

L13c:
    mStateStep = 0;
    if (mSinkDepth > 0x32000) {
        this->SetAnim(0x7d, 0, 0x1000, 0);
    } else if (this->GetHealth() <= 2) {
        mStateStep = 7;
        this->SetAnim(0xe, 0x40000000, 0x1000, 0);
    } else {
        this->SetAnim(0x47, 0, 0x1000, 0);
    }
    mStateTimer = 0x384;
    return 1;
}

void daPly_c::func_ov002_020d3498() {

    mHorzSpeed = -mHorzSpeed;
    mAngleY = mAngleY + 0x8000;
    mPrevAngleY = mAngleY;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_TurnAround_Main() {

    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    int r5v;
    s16 angle;
    int sel;
    int raw;
    int tmp;

    raw = mDesiredAngleY;
    if (data_0209f498[gActivePlayerSlot].mag != 0) {
        angle = (s16)(raw + 0x8000);
        sel = (mHorzSpeed < 0) ? 0x1800 : 0x8000;
        r5v = func_ov002_020bf56c(sel);
        ApproachAngle(&mPrevAngleY, angle, 0x10, 0x1000, 0x200);
        tmp = ScaleByCharFactor(0x28000);
        ApproachLinear(mHorzSpeed, -tmp, r5v);
    } else {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }

    ApproachAngle(&mAngleY, mPrevAngleY, 8, 0x2000, 0x800);

    if ((data_0209f498[gActivePlayerSlot].pressed & 2) != 0) {
        this->ChangeState(data_ov002_021101e4);
    } else if (mIsAirborne != 0) {
        this->ChangeState(data_ov002_021101b4);
    } else if (mHorzSpeed <= 0x12000) {
        if (!this->IsAnim(0x45))
            this->SetAnim(0x45, 0x40000000, 0x1000, 0);
        if (this->FinishedAnim()) {
            func_ov002_020d3498();
            this->ChangeState(data_ov002_0211013c);
        }
    } else {
        func_ov002_020bf88c();
        mLoopingSoundHandle = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2,
                                            &mCamSpacePos, mHorzSpeed, 0);
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_TurnAround_Init() {

    this->SetAnim(0x46, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    return 1;
}

int daPly_c::func_ov002_020d36d8(int arg) {
    extern unsigned char data_0209f49e[];
    extern unsigned char data_0209f49c[];

    if ((unsigned short)(mStateFlags & 0x40))
        return 0;

    if (func_ov002_020c0434() != 0) {
        func_ov002_020c0364(3);
        return 1;
    }
    if (func_ov002_020dd824() != 0)
        return 1;
    if (func_ov002_020d5c6c() != 0)
        return 1;

    {
        int idx = gActivePlayerSlot * 0x18;
        unsigned short f49e = *(unsigned short*)(data_0209f49e + idx);
        if (f49e & 1) {
            return func_ov002_020dde74() != 0;
        }
        if ((*(unsigned short*)(data_0209f49c + idx) & 0x400) && (arg & 1) == 0) {
            if (func_ov002_020d674c() != 0) {
                ChangeState(data_ov002_0211004c);
            } else {
                mStateStep = 0;
                ChangeState(data_ov002_021104e4);
            }
            return 1;
        }
        if (f49e & 2) {
            if (mCrouchTimer == 0) {
                ChangeState(data_ov002_0211019c);
                return 1;
            }
            if (func_ov002_020d1204() != 0)
                return 1;
        }
    }

    if (mIsAirborne != 0) {
        if (mPosY - mGroundY >= 0x28000) {
            ChangeState(data_ov002_021101b4);
            return 1;
        }
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Walk_Main() {
    extern s16 data_0209f4a0[];

    if ((u16)(mStateFlags & 0x800)) {
        this->ChangeState(daPly_c::ST_WAIT);
        return 1;
    }
    if (func_ov002_020c5244()) return 1;
    if (func_ov002_020d36d8(0))
        return 1;

    func_ov002_020d45c0();
    short r5 = mPrevAngleY;
    if (func_ov002_020d3b9c())
        return 1;

    func_ov002_020d413c(r5);

    if (mHorzSpeed == 0) {
        mAngleY = mPrevAngleY;
    } else {
        ApproachAngle(&mAngleY, mPrevAngleY, 4, 0x2000, 0x800);
    }

    if (mIsBraking != 0) {
        if (!this->IsAnim(0x37)) {
            int result = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2,
                                       &mCamSpacePos, mHorzSpeed, 0);
            mLoopingSoundHandle = result;
        }
        if (this->FinishedAnim()) {
            if (this->IsAnim(0x36)) {
                if (mHorzSpeed == 0) {
                    this->SetAnim(0x37, 0x40000000, 0x1000, 0);
                }
            } else {
                if (this->IsAnim(0x37)) {
                    func_ov002_020d4540();
                }
            }
        }
    } else {

        /* duplicating func_ov002_020d4748 grows 0x2dc to 0x2f0 */
        if (mHorzSpeed != 0)
            goto label_29c;

        {
            u8 idx = gActivePlayerSlot;
            int off = idx * 0x18;
            u8 b = data_0209f4ae[off];
            int thresh = (b != 2) ? 0x471 : 0x555;
            u8 v710 = mIsWalkPace;
            int rhs = thresh - (v710 << 7);
            short lhs = *(s16 *)((char *)data_0209f4a0 + off);
            if (lhs < rhs)
                goto label_1f0;
            if ((u16)(mStateFlags & 0xc) == 0)
                goto label_29c;
        }

label_1f0:
    if (mIsMega != 0) {
        if (this->FinishedAnim() && this->IsAnim(0x9d)) {
            this->ChangeState(daPly_c::ST_WAIT);
            mStateWork = 1;
            func_ov002_020cabe0();
        }
        goto end;
    }

    if (this->FinishedAnim()) goto label_27c;

    {
        u32 arg = (u8)param1;
        int modelIdx = this->GetBodyModelID(arg, 0);
        if (mModels.mBodyModels[modelIdx]->GetFlags())
            goto end;
    }

label_27c:
    this->ChangeState(daPly_c::ST_WAIT);
    mStateWork = 1;
    func_ov002_020cabe0();
    goto end;

label_29c:
    func_ov002_020d4748();
    mStateWork = 1;
}
end:
    AdvanceAnims();
    return 1;
}

/* recovered: daPly_c walk state helper (between St_Walk_Main and St_Walk_Init): steering and turn approach. */
extern "C" {

static inline s16 GetStick(void)
{
    extern short data_0209f4a0[];
    return *((s16 *) (((char *) data_0209f4a0) + gActivePlayerSlot * 0x18));
}
static inline u8 GetMode(void)
{
    return *((u8 *) (data_0209f4ae + gActivePlayerSlot * 0x18));
}
static inline u8 GetB14(void)
{
    extern unsigned char data_0209f4ac[];
    return *((u8 *) (data_0209f4ac + gActivePlayerSlot * 0x18));
}
static inline int StickLo(void) { return (GetMode() != 2) ? 0x471 : 0x555; }
static inline int StickHi(void) { return (GetMode() != 2) ? 0x955 : 0xAAA; }
static inline int StickScale(void) { return (GetMode() != 2) ? 0x24 : 0x30; }
static inline int StickLoSpd(void) { return (GetMode() != 2) ? 0xa : 0x10; }
static inline int StickHiSpd(void) { return (GetMode() != 2) ? 0x15 : 0x20; }
}

int daPly_c::func_ov002_020d3b9c() {
    extern short data_0209f4a0[];
    extern char data_0209f49c[];
    extern unsigned char data_0209f4ac[];
    int spd;
    int acc;
    s16 stick;
    s16 targ;
    int diff;
    spd = 0;
    stick = *((s16 *)(((char *)data_0209f4a0) + (gActivePlayerSlot * 0x18)));
    acc = spd;
    if ((stick != 0) && (((u16)(mStateFlags & 0xc)) == 0)) {
        mStickGraceTimer = 4;
        if ((*((u8 *)(data_0209f4ac + (gActivePlayerSlot * 0x18)))) == 0) {
            spd = ScaleByCharFactor(0x14000);
            acc = 0x1000;
            if (((*((u16 *)(data_0209f49c + (gActivePlayerSlot * 0x18)))) & 0x800) == 0) {
                if (mStateTimer != 0) {
                    spd = ScaleByCharFactor(0xa000);
                }
            } else {
                spd = ScaleByCharFactor(0x28000);
            }
        } else {
            if (GetStick() <= StickLo()) {
                spd = 0;
            } else if (GetStick() <= StickHi()) {
                int t = (GetStick() * StickScale()) >> 12;
                spd = (t - StickLoSpd()) << 15;
                spd = spd / (StickHiSpd() - StickLoSpd());
            } else {
                int a = ScaleByCharFactor(0x28000);
                int b = ScaleByCharFactor(0xa000);
                spd = func_ov002_020bf224(a, b);
            }
            acc = 0x1000;
            if ((*((s16 *)(((char *)data_0209f4a0) + (gActivePlayerSlot * 0x18)))) >= 0xf00) {
                acc = 0x1000;
            }
        }
        if (mDashBoostTimer != 0) {
            if (((*((u16 *)(data_0209f49c + (gActivePlayerSlot * 0x18)))) & 0x800) != 0) {
                u8 *p = &mDashBoostTimer;
                *p = (u8)((*p) - 1);
                spd = FX_MUL(spd, 0x1333);
                acc = 0x1e000;
                func_ov002_020bf88c();
            } else {
                mDashBoostTimer = 0;
            }
        }
        {
            int v = mSinkDepth;
            if (v > 0xa000) {
                int q = 0x6400 / (v >> 12);
                spd = FX_MUL(spd, q);
            }
        }
        mModelOffsetY = 0;
        if ((param1) == 1) {
            u16 h = mWaterRunTimer;
            if (((h != 0) && (mWaterRunLatch != 0)) && (h < 0x1e)) {
                int a = ScaleByCharFactor(0x28000);
                int prod = h * a;
                {
                    int cap = prod / 30;
                    if (spd > cap) {
                        spd = cap;
                    }
                }
                {
                    int tmp = -((0x1e - ((int)h)) << 12);
                    tmp = tmp * 2;
                    mModelOffsetY = tmp;
                }
            }
        }
        if (func_ov002_020f035c(func_ov002_020c031c(), mFloorNormalY) != 0) {
            acc = 0x1000;
        }
        targ = mDesiredAngleY;
        diff = AngleDiff(mPrevAngleY, targ);
        if (diff < 0) {
            diff = -diff;
        }
        if (diff > 0x6000) {
            int cur = mHorzSpeed;
            if ((cur >= ScaleByCharFactor(0x1c000)) && (mIsMega == 0)) {
                ChangeState(data_ov002_02110184);
                AdvanceAnims();
                return 1;
            }
            if (cur == 0) {
                mPrevAngleY = targ;
            } else {
                ApproachAngle(&mPrevAngleY, targ, 4, 0x2000, 0x800);
            }
        } else {
            int dv;
            int step;
            int idx2;
            step = 0x800;
            idx2 = gActivePlayerSlot * 0x18;
            dv = 4;
            if ((*((data_0209f4ac + idx2))) == 0) {
                if ((*((s16 *)(((char *)data_0209f4a0) + idx2))) >= 0xa00) {
                    if (AngleDiff(mPrevAngleY, targ) >= 0x1000) {
                        if (mTurnRamp < 0x18) {
                            mTurnRamp++;
                            dv = (s16)(0x10 - (mTurnRamp >> 1));
                        }
                    } else {
                        dv = 0x10;
                        if (mTurnRamp >= 2) {
                            mTurnRamp -= 2;
                        }
                    }
                    step = (s16)((0x10 - (dv - 4)) << 3);
                } else if (mTurnRamp >= 2) {
                    mTurnRamp -= 2;
                }
            }
            ApproachAngle(&mPrevAngleY, targ, dv, 0x2000, step);
        }
        if (mIsBraking != 0) {
            func_ov002_020d4540();
        }
    } else {
        mDashBoostTimer = 0;
        if (mIsBraking != 0) {
            acc = func_ov002_020bf56c(0x2000);
            func_ov002_020bf88c();
        } else if (mStickGraceTimer == 0) {
            if (mIsMega == 0)
                acc = 0x1000;
            else
                acc = 0x4000;
            if (mHorzSpeed < (-0xa000)) {
                mHorzSpeed = -0xa000;
            }
            if (mIsMega == 0) {
                if (mHorzSpeed > ScaleByCharFactor(0x10000)) {
                    func_ov002_020d454c();
                }
            }
        }
        if (mHorzSpeed < ScaleByCharFactor(0xa000)) {
            mStateTimer = 0x3c;
        }
    }
    func_ov002_020d4d88(spd, acc);
    func_ov002_020c18b0(1);
    mAngleZ = 0;
    return 0;
}

void daPly_c::func_ov002_020d413c(int arg) {
    int v;
    int diff;
    short s;
    if (mIsMega)
        return;
    v = ScaleByCharFactor(0x1c000);
    diff = mHorzSpeed;
    if (diff < v) {
        mSpineRot[1] = 0;
        mSpineRot[2] = mSpineRot[1];
        return;
    }
    s = (short)(FX_MUL(((diff - v) / 12), 0x1c71));
    {
        short t = s;
        if (t >= 0x2000) t = 0x2000;
        ApproachAngle(&mSpineRot[2], t, 4, 0x2000, 0x100);
    }
    {
        short t = (short)((arg - mPrevAngleY) * 6);
        if (t < -0x1555) t = -0x1555;
        else if (t > 0x1555) t = 0x1555;
        ApproachAngle(&mSpineRot[1], t, 8, 0x100, 0x10);
    }
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Walk_Init() {
    extern short data_0209f4a0;
    extern unsigned char gGameMode;
    extern unsigned char data_0209f4ac;
    extern int data_ov002_0210e160[];
    if (this->GetHealth() == 0) {
        mStateStep = 1;
        this->ChangeState(data_ov002_0211010c);
        return 1;
    }

    if (mHorzSpeed > ScaleByCharFactor(0x28000)) {
        mHorzSpeed = ScaleByCharFactor(0x28000);
    }
    if (mHorzSpeed < -0xe000) {
        mHorzSpeed = -0xe000;
    }

    if (data_0209f498[gActivePlayerSlot].mag == 0) {
        /* structured form rewrites the mode test into r2 and flips == 2 at +0xa4 */
        int byte1 = gGameMode;
        int cond = (byte1 == 1);
        if (cond) goto do_anim;
        if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) == 0)
            goto merge;
        cond = (byte1 == 2);
        if (cond) goto merge;
do_anim:
        if (mIsMega != 0) {
            this->SetAnim(0xa0, 0, 0x1000, 0);
        } else {
            this->SetAnim(0x47, 0, 0x1000, 0);
        }
} else {
    func_ov002_020d4748();
}
merge:
    mStateTimer = 0;
    if (data_0209f498[gActivePlayerSlot].touching == 0) {
        if (mHorzSpeed == 0) {
            mStateTimer = 0x3c;
        }
    }

    mPeakY = mPosY;
    mStateWork = 0;
    mStateArg = 0;
    if (mHorzSpeed > ScaleByCharFactor(0x24000)) {
        mStateArg = 1;
    }
    mStateFlags &= ~0x100;
    mIsUnderwater = 0;
    func_ov002_020d4540();

    ClearClimbCollider();

    mRidingShell = 0;
    mHeldObj = 0;
    mGrabbedByActor = 0;
    mStateStep = 0;
    mEatingPlayer = 0;
    if (func_ov002_020e3078(&daPly_c::ST_WAIT) != 0) {
        mWalkTimer = 0x10;
        if (data_0209f498[gActivePlayerSlot].mag != 0) {
            mPrevAngleY = mDesiredAngleY;
            mAngleY = mPrevAngleY;
        }
        if (mDashBoostTimer != 0) {
            unsigned int rnd = (unsigned int)RandomIntInternal(data_ov002_0210e160);
            int idx = (rnd >> 6) & 3;
            int voiceId = data_ov002_020ff1b0[idx];
            Sound::PlayCharVoice(mCharacter, voiceId, mCamSpacePos);
        }
    }

    if (param1 == 1) {
        if (mIsInShallowWater == 0) {
            mDetectWater = 1;
        }
    }

    if (mIsMega != 0 && mScaleX != 0x3000) {
        this->SetNoControlState(0xd, -1, 0);
    }
    return 1;
}

void daPly_c::func_ov002_020d4540() {
    mIsBraking = 0;
}

void daPly_c::func_ov002_020d454c() {
    if (mIsAirborne != 0)
        return;
    if (mHorzSpeed > ScaleByCharFactor(0x10000))
        mHorzSpeed = ScaleByCharFactor(0x10000);
    mIsBraking = 1;
    SetAnim(0x36, 0x40000000, 0x1000, 0);
}

void daPly_c::func_ov002_020d45c0() {
    extern s16 data_0209f4a0[];
    extern u8 data_0209f4ac[];
    extern u16 data_0209f49c[];
    if (mIsInShallowWater != 0)
        return;
    if (gSubSoundID == 0x2e)
        return;

    if (mHorzSpeed < ScaleByCharFactor(0x24000)) {
        if (data_0209f498[gActivePlayerSlot].mag >= 0xe00) {
            if (data_0209f498[gActivePlayerSlot].touching != 0 ||
                (data_0209f498[gActivePlayerSlot].held & 0x800)) {
                if (mIsMega != 0)
                    Particle::MegaRunningSlidingDustAt(mPosX, mPosY, mPosZ);
                else
                    Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
            }
        }
    }

    if (data_0209f498[gActivePlayerSlot].touching == 0)
        return;

    if (mStateArg != 0) {
        if (mStateWaitTimer != 0) {
            if (mIsMega != 0)
                Particle::MegaRunningSlidingDustAt(mPosX, mPosY, mPosZ);
            else
                Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
        }
        if (mHorzSpeed < ScaleByCharFactor(0x1c000))
            mStateArg = 0;
    } else {
        if (mHorzSpeed > ScaleByCharFactor(0x26000)) {
            mStateWaitTimer = 0x10;
            mStateArg = 1;
        }
    }
}

/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern "C" {

static inline int* GetAnim(char* c)
{
    daPly_c *player = (daPly_c *)c;
    return (
        int *)((char *)player->mModels
                   .mBodyModels[((daPly_c *)c)->GetBodyModelID((unsigned char)player->param1, 0)] +
               0x50);
}
static inline u16 AnimSpeed(char *c) {
    return GetAnim(c)[2] >> 12;
}
}

void daPly_c::func_ov002_020d4748() {
    extern unsigned char data_0209f4ac;
    extern unsigned short data_0209f49c;
    extern short data_0209f4a0;
    extern unsigned char gGameMode;
    char* c = (char*)this;
    int i = gActivePlayerSlot * 0x18;
    int flag = 0;

    if (*(unsigned char*)((char*)&data_0209f4ac + i) == 0) {
        if (!(*(unsigned short*)((char*)&data_0209f49c + i) & 0x800))
            flag = 1;
    } else {
        int k = (*(unsigned char*)((char*)&data_0209f4ae + i) != 2) ? 0xdc7 : 0xe00;
        if (*(short *)((char *)&data_0209f4a0 + i) < k + (this->mIsWalkPace << 7))
            flag = 1;
    }
    this->mIsWalkPace = flag;

    if (this->mIsMega != 0) {
        int i2;
        short v;
        this->func_ov002_020bf5e0();
        if (this->IsAnim(0x9f) != 0)
            return;
        i2 = gActivePlayerSlot * 0x18;
        v = *(short*)((char*)&data_0209f4a0 + i2);
        if (v != 0) {
            if (flag != 0) {
                int spd;
                if (*(unsigned char*)((char*)&data_0209f4ac + i2) == 0) {
                    spd = (this->mStateTimer != 0) ? 0x1000 : 0x2000;
                } else {
                    spd = v * 3;
                }
                ((daPly_c *)c)->SetAnim(0x9e, 0, spd, 0);
                return;
            } else {
                ((daPly_c *)c)->SetAnim(0xa4, 0, 0x2000, 0);
                return;
            }
        } else {
            ((daPly_c *)c)->SetAnim(0x9d, 0x40000000, 0x1000, 0);
            return;
        }
    } else {
        int i3 = gActivePlayerSlot * 0x18;
        if (*(unsigned char*)((char*)&data_0209f4ac + i3) != 0) {
            unsigned char* p = &this->mStateStep;
            unsigned char d;
            int b;
            int i4;
            int thr;
            *p = *p + 1;
            i4 = gActivePlayerSlot * 0x18;
            thr = (*(unsigned char*)((char*)&data_0209f4ae + i4) != 2) ? 0x955 : 0xaaa;
            if (*(short*)((char*)&data_0209f4a0 + i4) > thr)
                this->mStateStep = 0;
            d = gGameMode;
            b = (d == 1);
            /* one condition shrinks 0x4e8 to 0x4dc; two stores grow it to 0x4f0 */
            if (b == false) {
                if (data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) {
                    int b2 = (d == 2);
                    if (b2 == false) goto after6e3;
                }
                this->mStateStep = 0;
            }
        after6e3:;
        } else {
            this->mStateStep = 0;
        }

        if (this->mSinkDepth > 0x32000) {
            ((daPly_c *)c)
                ->SetAnim(0x7e, 0, *(short *)((char *)&data_0209f4a0 + gActivePlayerSlot * 0x18),
                          AnimSpeed(c));
        } else if (this->mStateStep >= 0x17) {
            this->mStateStep = 0x17;
            ((daPly_c *)c)->SetAnim(0x61, 0, 0x1000, AnimSpeed(c));
        } else if (this->mFloorClass == 4 && *(short*)((char*)&data_0209f4a0 + gActivePlayerSlot * 0x18) == 0) {
            Particle::RunningSlidingDustAt(this->mPosX, this->mPosY, this->mPosZ);
            ((daPly_c *)c)->SetAnim(0x47, 0, 0x1000, 0);
        } else if (this->mIsWalkPace != 0) {
            u16 s5 = AnimSpeed(c);
            int spd = this->mHorzSpeed >> 3;
            if (spd < 0x400)
                spd = 0x400;
            ((daPly_c *)c)->SetAnim(0x48, 0, spd, s5);
        } else {
            int t = 0x1000 - this->mFloorNormalY;
            int f = (int)((((long long)t << 10) + 0x800) >> 12);
            int v;
            int diff = ScaleByCharFactor(0x1c000) - this->mHorzSpeed;
            v = FX_MUL(diff, f);
            if (v < 0)
                v = 0;
            ((daPly_c *)c)->SetAnim(0x3f, 0, v + 0x2800, AnimSpeed(c));
        }

        if (((dExtFrameCtrl_c *)(GetAnim(c)))->WillHitFrame(4) != 0 ||
            ((dExtFrameCtrl_c *)(GetAnim(c)))->WillHitFrame(0x13) != 0) {
            PlayLandingEffects();
        }
    }
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

void daPly_c::PlayLandingEffects() {
    int id;
    if (mIsInShallowWater) {
        mParticle1 = Particle::System::NewCheckWaterRipple(mParticle1, kPtclShallowWaterLand, mPosX,
                                                           data_0209f32c, mPosZ, 0);
    }
    if (mSinkDepth > 0x32000) {
        Sound::PlayBank0(0xb3, mCamSpacePos);
    } else if (mStateStep >= 0x1e) {
        if (mIsMetal == 0) {
            Sound::PlayBank0(mGroundSoundType + 0x70, mCamSpacePos);
        } else {
            Sound::PlayBank0(0xa4, mCamSpacePos);
        }
    } else {
        if (mIsMetal == 0) {
            func_0201251c(0, mGroundSoundType + 0x20, &mCamSpacePos, mHorzSpeed);
        } else {
            func_0201251c(0, 0xaa, &mCamSpacePos, mHorzSpeed);
        }
    }
    if (gSubSoundID == 0x2e)
        return;
    id = -1;
    if (mGroundSoundType != 6) {
        if (mGroundSoundType != 8) {
            if (mGroundSoundType == 9)
                id = 0xe6;
        } else
            id = 0xe4;
    } else
        id = 0xe5;
    if (id == -1) return;
    Particle::System::NewSimple(id, mPosX, mPosY, mPosZ);
}

/* func_ov002_020d4d88 at 0x020d4d88
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
int daPly_c::func_ov002_020d4d88(int a, int b) {
    ApproachLinear(mHorzSpeed, a, b);
    if (a != 0 || (unsigned short)(mStateFlags & 1)) {
        func_ov002_020c04e0();
        return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Bonk_Main() {
    extern dCamera_c *gCamera;
    int rate;

    if (mStateTimer != 0) {
        mHorzSpeed = 0x40000;
        mVertSpeed = 0x8000;
        /* early return duplicates the shared epilogue, 0x1a4 to 0x1b0 */
        goto ret1;
    }

    rate = 0x1000;
    if (mIsAirborne == 0) {
        int v = mPrevVertSpeed;
        if (v < 0) {
            mVertSpeed = (-v) / 3;
            if (mVertSpeed <= 0x1000) {
                mVertSpeed = 0;
            }
        }

        if (mStateArg == 0) {
            Sound::PlayBank0(mGroundSoundType + 0x50, mCamSpacePos);
            Sound::PlayCharVoice(mCharacter, 0x11, mCamSpacePos);
            mStateArg = 1;
        }

        if (mPeakY - mPosY > 0xbb8000) {
            gCamera->func_0200d8c8(&mPos, 0x7d0000);
        }

        rate = 0x1800;
    }

    ApproachLinear(mHorzSpeed, 0, rate);

    if (mHorzSpeed != 0 || mIsAirborne != 0) {
        u32 arg = (u8)param1;
        int modelIdx = this->GetBodyModelID(arg, 0);
        if (mModels.mBodyModels[modelIdx]->WillHitFrame(0x2e) != 0) {
            mStateWork = 1;
        }
    } else {
        mStateWork = 0;
        if (this->FinishedAnim() != 0) {
            mPrevAngleY = mPrevAngleY + 0x8000;
            mAngleY = mPrevAngleY;
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
    }
    if (mStateWork == 0) {
        AdvanceAnims();
    }
    mPrevVertSpeed = mVertSpeed;
ret1:
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Bonk_Init() {
    this->SetAnim(0x13, 0x40000000, 0x1000, 0);
    mStateTimer = 0x10;
    mHorzSpeed = 0x40;
    mVertSpeed = 0xa000;
    mStateWork = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_0211037c;
}

void daPly_c::BlowAway(short v) {

    if (mIsMetal)
        return;
    if (mIsBalloon)
        return;
    mPrevAngleY = v;
    mAngleY = (short)(v + 0x8000);
    this->ChangeState(data_ov002_0211037c);
}

/* recovered: named members + shared header, real C++ method
 *
 * Burn-in-lava, per frame: kills the player once outside battle levels, spawns
 * smoke/steam particles with the burn SFX, then runs the three-hop bounce
 * (halving horizontal speed each hop, hop height 0x14000 then 0xa000) until
 * the burnt animation finishes, and leaves via state change or death.
 */
/* current level id */
/* sin/cos pair table, indexed by angle >> 4 */
/* the walk state */
/* Particle::System::New takes Fix12<int> by value -- the
   mwccarm 6az wall, runbook section 7 -- so it stays extern "C" with scalars
   in those slots. */
int daPly_c::St_BurnLava_Main() {
    if (gMeterShownHealth == 0 && mStateArg == 0) {
        KillPlayer();
        mStateArg = 1;
        mIsBodyClsnEnabled = 0;
    }

    {
        s8 lv = gCurrentSublevel;
        if (lv != 0x13 && lv != 0x31 && mVertSpeed >= 0) {
            /* Field order is the stack layout the original built, not a choice. */
            struct Info {
                s16 vec[3];
                int pos[3];
            } info;
            int zz = mPosZ;
            int yy = mPosY;
            int xx = mPosX;
            info.pos[0] = xx;
            info.pos[2] = zz - 0x1e000;
            info.pos[1] = yy;
            /* The u16 cast is the semantics, not a missing type fix. mAngleY
               really is signed -- AngleDiff takes it signed and mPrevAngleY is
               assigned from it directly -- but indexing this table wants the
               angle to WRAP, so the shift must be logical. Spelling it
               `mAngleY >> 4` costs 2 words and changes the meaning for
               negative angles. */
            info.vec[0] = FX_SinCosTable_[(*(u16 *)&mAngleY >> 4) * 2];
            info.vec[1] = 0;
            info.vec[2] = FX_SinCosTable_[(*(u16 *)&mAngleY >> 4) * 2 + 1];
            mParticle1 =
                Particle::System::New(mParticle1, kPtclPlayerBurnA, info.pos[0], info.pos[1],
                                      info.pos[2], (const Vector3_16f *)(info.vec), 0);
            mParticle2 =
                Particle::System::New(mParticle2, kPtclPlayerBurnB, info.pos[0], info.pos[1],
                                      info.pos[2], (const Vector3_16f *)(info.vec), 0);
            mLoopingSoundHandle = Sound::PlayLong(mLoopingSoundHandle, 0, 0x106, mCamSpacePos, 0);
        }
    }

    if (mStateStep == 0) {
        func_ov002_020e28d4(0x1000, 0x1000);
        if (mIsAirborne == 0) {
            mStateWork++;
            if (mStateWork >= 3) {
                this->SetAnim(0x17, 0x40000000, 0x1000, 0);
                mHorzSpeed = 0;
                mStateStep++;
            } else {
                mHorzSpeed >>= 1;
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                if (mStateWork == 1)
                    mVertSpeed = 0x14000;
                else
                    mVertSpeed = 0xa000;
            }
        }
    } else {
        if (FinishedAnim() != 0) {
            mPrevAngleY = mAngleY;
            if (GetHealth() == 0)
                func_ov002_020c5dec(2);
            else
                this->ChangeState(data_ov002_0211013c);
        }
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_BurnLava_Init() {
    mIsInAirState = 1;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    mIsTakingDamage = 1;
    this->SetAnim(0x16, 0, 0x1000, 0);
    mHorzSpeed = 0;
    mVertSpeed = 0x64000;
    mStateWork = 0;
    mStateStep = 0;
    if (func_ov002_020e3078(&data_ov002_021100f4) == 0) {
        mStateArg = 0;
    }
    func_ov002_020d91e0(0x300, 1);
    Sound::PlayCharVoice(mCharacter, 0x23, mCamSpacePos);
    return 1;
}

int daPly_c::func_ov002_020d5338() {


    if (this->IsState(data_ov002_02110304) || this->IsState(data_ov002_02110484) ||
        this->mIsMetal != 0 || this->mIsNoControl != 0)
        return 0;
    this->ChangeState(data_ov002_021100f4);
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * daPly_c burn-in-fire state: early-exits via ChangeState on a couple of
 * flag checks, then (unless blocked by a hazard-level check) spawns two
 * fire/smoke particles plus a burn sound at the player's position offset
 * upward, then runs a small state machine (0/1) driving the burn anim and
 * a homing turn+speed approach before handing off to the common per-frame
 * tail function.
 */

int daPly_c::St_BurnFire_Main() {

    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern void func_02012694(unsigned int id, const Vector3 *v);
    int speed = 0;

    if (mStateWork >= 0xa0 || mIsMetal != 0) {
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }

    if (mIsInShallowWater != 0) {
        func_02012694(3, (const Vector3 *)&mCamSpacePosX);
        this->ChangeState(data_ov002_0211013c);
        return 1;
    }

    {
        u16 lvl = mStateTimer;
        if (lvl == 0x50 || lvl == 0x32 || lvl == 0x14) {
            if (func_ov002_020d91e0(0x100, 1) != 0) {
                func_ov002_020c5dec(2);
                return 1;
            }
        }
    }

    *(&mStateWork) += 2;

    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = mPosZ - 0x1e000;
        int yy = mPosY + 0x32000;
        int xx = mPosX;

        info.pos[0] = xx;
        info.pos[1] = yy;
        info.pos[2] = zz;
        info.vec[0] = FX_SinCosTable_[(*(u16 *)&mAngleY >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = FX_SinCosTable_[(*(u16 *)&mAngleY >> 4) * 2 + 1];

        mParticle1 = Particle::System::New(mParticle1, kPtclPlayerBurnA, info.pos[0], info.pos[1],
                                           info.pos[2], (const Vector3_16f *)(info.vec), 0);
        mParticle2 = Particle::System::New(mParticle2, kPtclPlayerBurnB, info.pos[0], info.pos[1],
                                           info.pos[2], (const Vector3_16f *)(info.vec), 0);
        mLoopingSoundHandle = Sound::PlayLong(mLoopingSoundHandle, 0, 0x106, mCamSpacePos, 0);
    }

    switch (mStateStep) {
    case 0:
        if (this->IsAnim(0x53) != 0) {
            this->mStateWork += 1;
        }
        if (mIsAirborne == 0) {
            mStateStep = 1;
            this->SetAnim(0x3f, 0, 0x1000, 0);
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
            anim.speed = 0x4000;
        }
        break;
    case 1:
        {
            int idx = gActivePlayerSlot * 0x18;
            if ((*(u16 *)((char *)data_0209f49e + idx) & 2) != 0) {
                mStateStep = 0;
                this->SetAnim(0x53, 0, 0x1000, 0);
                mIsAirborne = 1;
                mLandSoundPlayed = 0;
                mVertSpeed = 0x1e000;
                break;
            }
        }
        if (mIsAirborne != 0) {
            mStateStep = 0;
            this->SetAnim(0x54, 0, 0x1000, 0);
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            break;
        }
        this->SetAnim(0x3f, 0, 0x1000, 0);
        {
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)];
            anim.speed = 0x4000;
        }
        {
            int idx = gActivePlayerSlot * 0x18;
            speed = 0x28000;
            if (*(s16 *)((char *)data_0209f4a0 + idx) != 0) {
                ApproachAngle(&mAngleY, mDesiredAngleY, 8, 0x4000, 0x10);
            }
            mPrevAngleY = mAngleY;
        }
        break;
    }

    ApproachLinear(mHorzSpeed, speed, 0x1000);
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_BurnFire_Init() {
    mIsTakingDamage = 1;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x16, 0, 0x1000, 0);
    mStateStep = 0;
    mStateTimer = 0x51;
    /* mFloorClass == 1 is the class SetFloorSurfaceInfo latches on (see the
       CLPS block in daPly_c.h): on it the player is planted on the ground
       instead of being popped upward by the burn. */
    if (mFloorClass != 1) {
        mVertSpeed = 0x1e000;
    } else {
        mPosY = mGroundY;
    }
    mHorzSpeed = 0;
    mStateWork = 0;
    Sound::PlayCharVoice(mCharacter, 0x23, mCamSpacePos);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::Burn() {

    if (func_ov002_020d82f0() == 0)
        return 0;
    if (mIsMetal || mIsUnderwater)
        return 0;
    this->ChangeState(data_ov002_021100dc);
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Electrocution, per frame: a looping shock sound and three stacked particle
 * systems at the player's position raised 0x32000, then when the state timer
 * expires the two low bits of mStateWork decide where to go -- bit 0 selects
 * the "took damage" path over a plain exit, bit 1 selects the swimming exit
 * over the walking one.
 *
 * The particle point is a Vector3 declared up front and filled after the
 * sound call; constructing it at the declaration is 23 words off.
 */
/* Particle::System::New takes Fix12<int> by value -- the mwccarm 6az wall,
   runbook section 7 -- so it stays extern "C" with scalars in those slots. */
/* the walk state */
/* the swim state */
int daPly_c::St_Electrocute_Main() {
    Vector3 pos;

    mLoopingSoundHandle = Sound::PlayLong(mLoopingSoundHandle, 0, 0x10b, mCamSpacePos, 0);

    pos = mPos;
    pos.y += 0x32000;
    mParticle1 = Particle::System::New(mParticle1, kPtclElectrocuteA, pos.x, pos.y, pos.z, 0, 0);
    mParticle2 = Particle::System::New(mParticle2, kPtclElectrocuteB, pos.x, pos.y, pos.z, 0, 0);
    mParticle3 = Particle::System::New(mParticle3, kPtclElectrocuteC, pos.x, pos.y, pos.z, 0, 0);

    if (mStateTimer == 0) {
        u8 flags = mStateWork;
        if ((flags & 1) == 0) {
            mInvincibleTimer = 0x24;
            flags = mStateWork;
            if ((flags & 2) == 0) {
                this->ChangeState(data_ov002_0211013c);
            } else {
                this->ChangeState(data_ov002_0211067c);
            }
        } else {
            if ((flags & 2) != 0) {
                func_ov002_020c5dec(8);
            } else {
                mInvincibleTimer = 0x24;
                if (func_ov002_020c5dec(4) == 0) {
                    this->ChangeState(data_ov002_0211013c);
                }
            }
        }
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Electrocute_Init() {
    DropHeldObj();
    mIsTakingDamage = 1;
    mStateTimer = 0x1e;
    mInvincibleTimer = 0x1e;
    mHorzSpeed = 0;
    mVertSpeed = 0;
    mVertAccel = 0;
    this->SetAnim(0x11, 0, 0x1000, 0);
    Sound::PlayCharVoice(mCharacter, 7, mCamSpacePos);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::Shock(unsigned int j) {

    if (func_ov002_020d82f0() == 0)
        return 0;
    if (mIsMetal)
        j = 0;
    mStateWork = 0;
    if (func_ov002_020d91e0(j << 8, 1))
        mStateWork = 1;
    if (mIsUnderwater) {
        unsigned char *p = &this->mStateWork;
        *p |= 2;
    }
    this->ChangeState(data_ov002_021100c4);
    return 1;
}

extern "C" {

extern "C" int func_ov002_020d5ab4(daPly_c *p) {
    typedef long long s64;
    Vector3 pos;
    void *spawned;
    short *rp;
    int *pp;
    int a;

    if (p->IsState(data_ov002_02110064) == 0) {
        return 0;
    }

    rp = &p->mEatingPlayer->mAngleX;
    p->mAngleX = rp[0];
    p->mAngleY = rp[1];
    p->mAngleZ = rp[2];
    p->mPrevAngleX = p->mAngleX;
    p->mPrevAngleY = p->mAngleY;
    p->mPrevAngleZ = p->mAngleZ;

    pp = &p->mEatingPlayer->mPosX;
    pos.x = pp[0];
    pos.y = pp[1];
    pos.z = pp[2];

    a = (unsigned short)(short)(*(&p->mAngleY) + 0x8000) >> 4;
    pos.x = pos.x + FX_MUL(FX_SinCosTable_[a * 2], 0x64000);
    a = (unsigned short)(short)(*(&p->mAngleY) + 0x8000) >> 4;
    pos.z = pos.z + FX_MUL(FX_SinCosTable_[a * 2 + 1], 0x64000);

    p->mPosX = pos.x;
    p->mPosY = pos.y;
    p->mPosZ = pos.z;

    spawned = dActor_c::Spawn(9, 2, pos, 0, p->mAreaId, -1);
    ((daYegg_c *)spawned)->mPlayer = p;
    p->mOpacity = 0;
    p->mStateTimer = 0xd2;
    p->mdCcAcPos_c.flags &= ~2;
    p->mStateStep = 3;
    p->mPrevVertSpeed = 0;
    p->mEatingPlayer = 0;
    return 1;
}
}

int daPly_c::func_ov002_020d5c6c() {
    void* p;
    u16 f = mStateFlags & 2;
    if (f == 0)
        return 0;
    p = (void *)mObjInMouth;
    if (p == 0 || func_ov002_020d600c((daPly_c *)p) == 0) {
        *(u16 *)(void *)(int)(&mStateFlags) &= ~2;
        return 0;
    }
    ChangeState(data_ov002_02110034);
    return 1;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

extern "C" int func_ov002_020d5cec(void *self) {
    daPly_c *player = (daPly_c *)self;
    Vector3 v1;
    Vector3 v2;
    daPly_c *d0;
    short* m;
    Vector3 *p;
    int y1;
    int shifted;

    if (player->IsState(data_ov002_02110064) == 0)
        return 0;
    player->SetAnim(0x56, 0x40000000, 0x1000, 0);

    d0 = player->mEatingPlayer;
    m = &d0->mAngleX;
    player->mPrevAngleX = m[0];
    player->mPrevAngleY = m[1];
    player->mPrevAngleZ = m[2];
    player->mAngleY = player->mPrevAngleY;
    player->mHorzSpeed = 0x18000;
    player->mVertSpeed = 0x18000;

    p = &player->mEatingPlayer->mPos;
    v1.x = p->x;
    y1 = p->y;
    v1.y = y1;
    v1.z = p->z;
    v1.y = y1 + 0x32000;

    shifted = (unsigned short)player->mAngleY >> 4;
    v2.x = v1.x + FX_MUL(FX_SinCosTable_[shifted * 2], 0x50000);
    v2.y = v1.y;
    v2.z = v1.z + FX_MUL(FX_SinCosTable_[shifted * 2 + 1], 0x50000);

    dBgCh_Lin line;
    line.SetObjAndLine(v1, v2, player);
    if (line.DetectClsn()) {
        player->mPosX = v1.x;
        player->mPosY = v1.y;
        player->mPosZ = v1.z;
        player->mHorzSpeed = 0x8000;
    } else {
        player->mPosX = v2.x;
        player->mPosY = v2.y;
        player->mPosZ = v2.z;
    }
    player->mOpacity = 0x1f;
    player->mdCcAcPos_c.flags &= ~2;
    player->mStateStep = 2;
    player->mStateTimer = 4;
    player->mIsTakingDamage = 1;
    player->mEatingPlayer = 0;
    return 1;
}

int daPly_c::func_ov002_020d5ed0() {

    if (!this->IsState(data_ov002_02110064)) {
        this->mEatingPlayer = 0;
        this->ChangeState(data_ov002_0211013c);
        return 0;
    }
    this->mStateStep = 1;
    this->mStateTimer = 0x5a;
    this->mOpacity = 0;
    return 1;
}

extern "C" {

int func_ov002_020d5f34(daPly_c *c, void *st) {
    daPly_c *player = c;
    if (player->mEatingPlayer != 0)
        return 0;
    player->mEatingPlayer = (daPly_c *)st;
    (c)->SetAnim(0x3f, 0, 0x3000, 0);
    player->mStateStep = 0;
    (c)->ChangeState(data_ov002_02110064);
    return 1;
}
}

int daPly_c::func_ov002_020d5f98(unsigned char *a, unsigned char *b) {
    extern int data_ov002_0210e160[];

  if (this->IsState(data_ov002_02110064)
      && this->mStateStep == 3
      && this->mStateWork != 0) {
    *a = ((unsigned)RandomIntInternal(data_ov002_0210e160) >> 4) & 3;
    *b = this->mStateWork;
    return 1;
  }
  return 0;
}

extern "C" {

extern "C" int func_ov002_020d600c(daPly_c *player) {
    if (player->IsState(data_ov002_02110064)) {
        if (player->mStateStep == 1)
            return 1;
    }
    return 0;
}
}

extern "C" {

extern daPly_c::State data_ov002_02110064;
}

int daPly_c::func_ov002_020d6048() {


    if (this->IsState(data_ov002_02110064)) {
        if (this->mStateStep == 3)
            return 1;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method
 *
 * ov002, 0x020d6084, size 0x34. Leaving the state Yoshi has you in: two body
 * collision bits are cleared and two markers set, then it returns 1.
 *
 * `return 1` is not decoration here. ChangeState calls mCleanup FIRST, on the
 * state being left, and TESTS the result -- a 0 vetoes the transition. So this
 * handler's return value is what lets the player leave Yoshi's mouth at all.
 *
 * The word at 0x2ec used to be a daPly_c field called mBodyClsnFlags. It is
 * not one: it is the body collider's own `flags` -- dCc_c's, at +0x18
 * -- reached through mdCcAcPos_c now that the member has its
 * real type. Clearing 0x2000 and 0x2 in it is what a cleanup of a carried
 * state looks like. The other two writes are still unnamed offsets -- 0x713
 * and 0x6f5 -- and are left as such rather than guessed at.
 *
 * THE TWO CLEARS DO NOT MERGE. `*p &= ~0x2002` is one instruction shorter and
 * changes the size, so the ROM really does mask twice; measured, not assumed.
 * The `(long long)` round-trip the placeholder body carried was free, and is
 * gone.
 *
 * This file carried the placeholder name func_ov002_020d6084 until the symbol
 * was moved here from ov006. See the commit and include/daPly_c.h.
 */
int daPly_c::St_InYoshiMouth_Cleanup() {
    unsigned int *p = &mdCcAcPos_c.flags;
    *p &= ~0x2000;
    *p &= ~2;
    mIsBodyClsnEnabled = 1;
    mOpacity = 0x1f;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_InYoshiMouth_Main() {
    switch (mStateStep) {
    case 0:
        break;
    case 1:
        {
        daPly_c *r4 = mEatingPlayer;
        daPly_c *eater = r4;
        u32 t;
        int d;
        if (r4 != 0 && (char *)eater->mObjInMouth != ((char *)this)) {
            if (func_ov002_020d5cec(this))
                return 1;
        }
            {
                int *p = &eater->mPosX;
                mPosX = p[0];
                mPosY = p[1];
                mPosZ = p[2];
            }
            d = func_ov002_020beb38();
            t = eater->mMouthHoldTimer;
            if (eater->mIsNoControl == 0 && eater->mIsTakingDamage == 0) {
                t = (u16)(t - d);
                mStatePhase += d;
                if ((s16)t < 0)
                    t = 0;
            }
            if (t <= 1 && mStatePhase >= 0x1e) {
                Vector3 res;
                Vector3 addv;
                Vector3 hv;
                addv.x = 0;
                addv.y = 0x64000;
                addv.z = 0x64000;
                Vec3_RotateYAndTranslate(&res, &eater->mPos, eater->mAngleY, &addv);
                hv.x = res.x;
                hv.y = res.y;
                hv.z = res.z;
                r4->Hurt(hv, 1, 0, 0, (eater->param1 + 1) & 0xff, 1);
                this->ChangeState(data_ov002_0211019c);
                eater->mObjInMouth = 0;
                mEatingPlayer = 0;
                return 1;
            }
            eater->mMouthHoldTimer = (s16)t;
            break;
        }
    case 2:
        if (mStateTimer == 0)
            mdCcAcPos_c.flags |= 0x2000;
        if (this->FinishedAnim()) {
            mStateStep = 3;
            mHurtDamage = 1;
            this->ChangeState(data_ov002_02110094);
        }
        break;
    case 3:
        {
            int d = func_ov002_020beb38();
            mStateTimer -= (d << 2);
            mStateWork = d;
            if ((s16)mStateTimer <= 0) {
                mStateTimer = 0;
                mOpacity = 0x1f;
                this->ChangeState(data_ov002_0211007c);
            }
        }
        break;
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_InYoshiMouth_Init() {
    unsigned int r3;

    func_ov002_020bdb50(0);
    r3 = 0;
    mStateWork = (unsigned char)r3;
    mStatePhase = (unsigned char)r3;
    mdCcAcPos_c.flags |= 2u;
    mIsBodyClsnEnabled = (unsigned char)r3;
    return 1;
}

int daPly_c::func_ov002_020d6334(dActor_c *val) {
    int i;
    dActor_c **arr;
    arr = mHeldObjQueue;
    for (i = 0; i < 5; i++) {
        if (arr[i] == 0) {
            arr[i] = val;
            return 1;
        }
    }
    return 0;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
void daPly_c::func_ov002_020d6368() {
    typedef long long s64;
    Vector3_16 rot;
    Vector3 pos;
    daYegg_c *spawned;
    int a;
    int zero = 0;
    short rotY;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;

    a = *(unsigned short *)(&mAngleY) >> 4;
    pos.x = pos.x - FX_MUL(FX_SinCosTable_[a * 2], 0x14000);

    a = *(unsigned short *)(&mAngleY) >> 4;
    pos.y = pos.y - FX_MUL(FX_SinCosTable_[a * 2 + 1], 0x14000);

    rotY = mAngleY;
    rot.x = zero;
    rot.z = zero;
    rot.y = rotY;

    spawned =
        (daYegg_c *)dActor_c::Spawn(9, mEggParams << 4, pos, (const Vector3_16 *)&rot, mAreaId, -1);
    spawned->mPlayer = this;
    func_ov002_020d6334(spawned);
    mEggParams = zero;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Swallow_Main() {
    extern int data_0209ee90[];
    dActor_c *p360;

    if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(3)) {
        func_ov002_020d71a0();
    } else if (mModels.mBodyModels[this->GetBodyModelID(param1 & 0xff, 0)]->WillHitFrame(7) != 0) {
        p360 = mObjInMouth;
        if (p360 != 0) {
            int b = (p360->actorID == 0xbf);
            if (b != 0) {
                p360->mAngleX = mAngleX;
                p360->mAngleY = mAngleY;
                p360->mAngleZ = mAngleZ;
                func_ov002_020d5ab4((daPly_c *)(void *)mObjInMouth);
                Sound::PlayCharVoice(0, 0x100, mCamSpacePos);
            } else {
                p360->OnTurnIntoEgg(*this);
                void *q = mObjInMouth;
                if (((dActor_c *)q)->OnYoshiTryEat() != 1 &&
                    (unsigned short)(mStateFlags & 0x1000) == 0) {
                    unsigned char flag = 0;
                    int b_c2 = (mObjInMouth->actorID == 0xc2);
                    if (b_c2 || *(int *)((char *)data_0209ee90 + 0x238) != 0) {
                        flag |= 1;
                    }
                    mEggParams |= (flag + 1);
                    func_ov002_020d6368();
                    Sound::PlayCharVoice(0, 0x100, mCamSpacePos);
                }
            }
        }
        mObjInMouth = 0;
    }
    if (mIsUnderwater != 0) {
        mVertSpeed += func_ov002_020ceaf4();
    }
    if (this->FinishedAnim()) {
        if (mIsUnderwater != 0) {
            this->ChangeState(data_ov002_0211067c);
        } else {
            this->ChangeState(data_ov002_0211013c);
        }
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Swallow_Init() {
    daPly_c::State;

    if (mObjInMouth) {
        void *o = mObjInMouth;
        if (((dActor_c *)o)->OnYoshiTryEat() == 4) {
            DisableInteraction();
        }
        int eq = mObjInMouth->actorID == 0xbf;
        if (eq != 0) {
            this->ChangeState(data_ov002_02110034);
            return 1;
        }
    }
    this->SetAnim(0x70, 0x40000000, 0x1000, 0);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
void daPly_c::RegisterEggCoinCount(unsigned int count, bool b2, bool b3) {
    mEggParams = (count & 0xf) << 2;
    if (b3)
        mEggParams |= 0x40;
    if (b2)
        mEggParams |= 0x80;
}

int daPly_c::func_ov002_020d674c() {
    dActor_c *o = mObjInMouth;
    if (o == 0)
        return 0;
    return o->OnYoshiTryEat() == 6;
}

int daPly_c::func_ov002_020d6790() {
    if (mMouthHoldTimer != 0)
        return 0;

    dActor_c *o = mObjInMouth;
    if (o == 0) return 0;

    int isbf = (o->actorID == 0xbf);
    if (isbf) {
        if (mIsNoControl == 0 && mIsTakingDamage == 0) {
            ChangeState(data_ov002_02110034);
            DisableInteraction();
            return 1;
        }
        return 0;
    }

    if (!IsState(data_ov002_0211013c)) {
        if (!IsState(daPly_c::ST_WAIT)) {
            if (!IsState(data_ov002_0211067c))
                return 0;
        }
    }

    o = mObjInMouth;
    switch (o->OnYoshiTryEat()) {
    case 2:
    case 4:
        func_ov002_020d7430(*this);
        /* a return in each case grows 0x208 to 0x214 */
        goto ret0;
    case 5: {
        int eq132 = (mObjInMouth->actorID == 0x132);
        if (eq132) {
            func_ov002_020d8118();
        }
        mObjInMouth->OnTurnIntoEgg(*this);
        mObjInMouth = 0;
        mFireBreaths = 1;
        goto ret0;
    }
    case 3:
        ChangeState(data_ov002_02110034);
        return 1;
    case 1:
    case 6:
        ChangeState(data_ov002_0211004c);
        return 1;
    case 7:
        if (mUseAltBodyModel != 0)
            goto case7_ret1;
        mObjInMouth->mFlags |= 0x40000;
        mObjInMouth->mFlags &= ~0x20000;
        mUseAltBodyModel = 1;
    case7_ret1:
        return 1;
    default:
    ret0:
        return 0;
    }
}

/* recovered: shared common types */

int daPly_c::func_ov002_020d6998() {
    typedef int (*VFunc)(void*);
    char* self = (char*)this;
    s16 saved;
    void* other;
    u32 id;

    if (func_ov002_020bea7c()) {
        if (IsCollectingCap() != 0) return 0;
        if (this->mHatCharacter != 3) return 0;
    }
    {
        u16 flag = (u16)(this->mStateFlags & 0x1000);
        if (flag) return 0;
    }
    {
        s16* p = &this->mAngleY;
        s16 diff = *p - mHeadRot[1];
        saved = this->mAngleY;
        *p = diff;
    }
    if (TongueWallProbe() != 0) {
        func_ov002_020d71ec(2);
        this->mAngleY = saved;
        return 1;
    }
    this->mAngleY = saved;

    if (mObjInMouth == 0) {
        id = this->mAttackClsn.otherOwner;
        if (id != 0) {
            other = dActor_c::FindWithID(id);
            if (other != 0 && other != this) {
                if (IsCollectingCap() == 0 && ((dActor_c *)other)->OnYoshiTryEat() != 0) {
                    u8 isType = (((dActor_c *)other)->actorID == 0xbf);
                    if (isType) {
                        if (((dActor_c *)other)->param1 == 3) {
                            if (func_ov002_020d6c60((daPly_c *)other) != 0)
                                return 1;
                        } else {
                            Vector3 out;
                            Vector3_16 pos16;
                            Vector3 rotSrc;
                            s16 angle;
                            int idx;

                            angle = Vec3_HorzAngle(&((dActor_c *)other)->mPos, &mPos);
                            idx = ((u16)angle >> 4) * 2;

                            pos16.x = FX_SinCosTable_[idx];
                            pos16.y = 0;
                            pos16.z = FX_SinCosTable_[idx + 1];

                            rotSrc.x = 0;
                            rotSrc.y = 0x32000;
                            rotSrc.z = 0x32000;

                            Vec3_RotateYAndTranslate((Vector3 *)&out.x,
                                                     (const Vector3 *)&((dActor_c *)other)->mPosX,
                                                     angle, (const Vector3 *)&rotSrc.x);

                            Particle::System::New(0, kPtclTongueHitA, out.x, out.y, out.z,
                                                  (const Vector3_16f *)(&pos16), 0);
                            Particle::System::New(0, kPtclTongueHitB, out.x, out.y, out.z,
                                                  (const Vector3_16f *)(&pos16), 0);
                        }
                    } else {
                        int r;
                        if (func_ov002_020d7030(other) != 0) {
                            r = ((dActor_c *)other)->OnYoshiTryEat();
                            if (r == 4)
                                DisableInteraction();
                            return 1;
                        }
                    }
                }
                if ((this->mAttackClsn.hitFlags & 2) == 0) {
                    func_ov002_020d71ec(2);
                    Sound::PlayCharVoice(0, 0xfe, mCamSpacePos);
                    return 1;
                }
            }
        }
    }
    func_ov002_020db8d8();
    mAttackClsn.Clear();
    mAttackClsn.Update();
    return 0;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types
 *
 * Yoshi (this) tries to swallow another player (other, already checked to be
 * actor 0xbf by func_ov002_020d6998). */
int daPly_c::func_ov002_020d6c60(daPly_c *other) {
    Vector3 v;
    int b;
    int y;
    int *q;
    void *sp;
    /* early returns change size 0x14c to 0x148 with || and to 0x15c when split */
    if (other->func_ov002_020d82f0() == 0)
        goto ret0a;
    b = (other->mObjInMouth != 0);
    if (b == 0)
        goto l48;
ret0a:
    return 0;
l48:
    b = (other->mCharacter != other->param1);
    if (b == 0)
        goto leq;
    v.x = mPosX;
    y = mPosY;
    v.y = y;
    v.z = mPosZ;
    v.y = y + 0x64000;
    sp = dActor_c::Spawn(0x10d, (other->param1 << 8) | 5, v, &mAngle, mAreaId, -1);
    if (sp == 0)
        goto ret0b;
    func_ov002_020d7030(sp);
    other->SetNewHatCharacter(mCharacter, 0, 0);
    DisableInteraction();
    return 1;
leq:
    if (func_ov002_020d5f34(other, this) == 0)
        goto ret0b;
    mObjInMouth = other;
    q = (int *)&(other)->mFlags;
    *q = *q | 0x20000;
    func_ov002_020d71ec(1);
    DisableInteraction();
    return 1;
ret0b:
    return 0;
}

/* The line hit is copied into a local dBgPi (its implicit copy
 * constructor) before the collider owner is looked up. */

int daPly_c::TongueWallProbe() {
    daPly_c *player = this;
    Vector3_16f dir;
    Vector3 p1;
    Vector3 p2;
    dBgCh_Lin rl;
    char *actor;
    s16 ang;
    int t;
    int idx;

    {
        u8 b = player->mUseAltBodyModel;
        int model;
        Matrix4x3 *m1 = player->mModels.mHeadModels[(b << 2) + 3]->data.transforms;
        model = (this)->GetBodyModelID(player->param1 & 0xff, 0);
        MTX_MultVec43((Vector3 *)&m1->m[9], &player->mModels.mBodyModels[model]->mat4x3, &p1);
        Vec3_MulScalarInPlace(&p1, 0x8000);
    }

    {
        u8 b2 = player->mUseAltBodyModel;
        Matrix4x3 *m2i = &player->mModels.mHeadModels[(b2 << 2) + 3]->data.transforms[3];
        int model2 = (this)->GetBodyModelID(player->param1 & 0xff, 0);
        void *a0, *a1, *a2;
        a1 = player->mModels.mBodyModels[model2];
        a0 = &m2i->m[9];
        a1 = &((dExtAnmModel_c *)a1)->mat4x3;
        a2 = &p2;
        MTX_MultVec43((Vector3 *)a0, (Matrix4x3 *)a1, (Vector3 *)a2);
        Vec3_MulScalarInPlace(&p2, 0x8000);
    }

    rl.SetObjAndLine(p1, p2, this);

    if (rl.DetectClsn() == 0) {
        return 0;
    }

    dBgPi res(rl);
    actor = (char *)dActor_c::FindWithID(res.GetClsnID());
    if (actor != 0 && (t = (int)(((dActor_c *)actor)->actorID == 0xc2)) != 0) {
        return 0;
    }

    ::Vector3 clsnPos = rl.GetClsnPos();
    player->mTongueHitDist = VEC_Distance((const VecFx32 *)&p1, (const VecFx32 *)&clsnPos);
    Vector3 normal;
    rl.surface.CopyNormalTo(normal);
    ang = cstd::atan2(normal.x, normal.z);
    idx = (u16)ang >> 4;
    dir.x = FX_SinCosTable_[idx * 2];
    dir.y = 0;
    dir.z = FX_SinCosTable_[idx * 2 + 1];
    Particle::System::New(0, kPtclTongueHitA, clsnPos.x, clsnPos.y, clsnPos.z,
                          (const Vector3_16f *)(&dir), 0);
    Particle::System::New(0, kPtclTongueHitB, clsnPos.x, clsnPos.y, clsnPos.z,
                          (const Vector3_16f *)(&dir), 0);

    return 1;
}

int daPly_c::func_ov002_020d7030(void *b) {
    int flag = (((dActor_c *)b)->mFlags & 0x60000) != 0;
    if (flag)
        return 0;
    ((dActor_c *)b)->mFlags |= 0x20000;
    *(void **)((char *)b + 0xd0) = this;
    mObjInMouth = (dActor_c *)b;
    func_ov002_020d71ec(1);
    return 1;
}

char *daPly_c::func_ov002_020d708c() {
    char* self = (char*)this;
    char* a;
    char* best = 0;
    int bestAng = 0x238e;
    a = (char *)Next((dActor_c *)best);
    if (a) {
        int thresh = bestAng;
        do {
            if (a != self) {
                if (((dActor_c *)a)->OnYoshiTryEat()) {
                    if (Vec3_Dist(&mPos, &((dActor_c *)a)->mPos) < 0x15e000) {
                        int diff =
                            (short)(this->mAngleY - Vec3_HorzAngle(&mPos, &((dActor_c *)a)->mPos));
                        int ad = diff < 0 ? -diff : diff;
                        if (ad <= thresh) {
                            int ad2 = diff < 0 ? (short)(-diff) : diff;
                            if (ad2 < bestAng) {
                                bestAng = diff;
                                best = a;
                            }
                        }
                    }
                }
            }
            a = (char *)Next((dActor_c *)a);
        } while (a);
    }
    if (best) {
        short v = Vec3_VertAngle(&mPos, &((dActor_c *)best)->mPos);
        this->mHeadRot[0] = 0;
        this->mHeadRot[1] = (short)bestAng;
        this->mHeadRot[2] = v;
        this->mHeadRotMode = 1;
    }
    return best;
}

void daPly_c::func_ov002_020d718c() {

    this->mObjInMouth = 0;
    func_ov002_020d71a0();
}

void daPly_c::func_ov002_020d71a0() {
    struct G {
        int w[2];
    };
    extern struct G data_ov002_0210eb88;
    char *p = (char*)this;
    this->mUseAltBodyModel = 0;
    ((dExtAnmModel_c *)mModels.mHeadModels[3])
        ->SetAnim((BCA_File *)((void *)data_ov002_0210eb88.w[1]), 0x40000000, 0x1000, 0);
    ((dExtAnmModel_c *)mModels.mHeadModels[3])->currFrame = 0;
}

/* P2's only member is an array. With named members, C++ copies each
 * table into arr[] member by member, and the arr base and the zero for
 * the stack argument take their registers in the other order (5 words
 * differ, measured); a copy of an array member is one block move, as C
 * made it. */

void daPly_c::func_ov002_020d71ec(int arg1) {
    extern struct P2 data_ov002_0210eb88;
    int arr[4];
    u32 shifted;
    int val;

    if (arg1 != 2) {
        dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
        shifted = ((u32)anim.currFrame << 4) >> 0x10;
        *(struct P2*)&arr[0] = data_ov002_0210a34c;
        *(struct P2*)&arr[2] = data_ov002_0210a084;
        SetAnim(arr[arg1], 0x40000000, 0x1000, 0);

        if (mUseAltBodyModel == 0) {
            ((dExtAnmModel_c *)mModels.mHeadModels[3])
                ->SetAnim((BCA_File *)(*(void **)((char *)(&arr[2])[arg1] + 4)), 0x40000000, 0x1000,
                          0);
        }

        if (arg1 == 1) {
            val = data_ov002_020ff0f8[shifted];
            dExtFrameCtrl_c &body = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
            val = ((u32)val << 0x10) >> 4;
            body.currFrame = val;
            ((dExtAnmModel_c *)mModels.mHeadModels[3])->currFrame = val;
        }

        mStateStep = 1;
        return;
    }

    SetAnim(0x6c, 0x40000000, -0x1000, 0);
    ((dExtAnmModel_c *)mModels.mHeadModels[3])
        ->SetAnim((BCA_File *)(*(void **)((char *)&data_ov002_0210eb88 + 4)), 0x40000000, -0x1000,
                  0);
    mStateStep = 2;
    mStateTimer = 0xa;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_YoshiPower_Cleanup() {
    char *r2 = (char *)mObjInMouth;
    if (r2 != 0) {
        int t = (int)((((dActor_c *)r2)->mFlags & 0x20000) != 0);
        if (t != 0) {
            ((dActor_c *)r2)->mFlags &= ~0xe0000;
            func_ov002_020d718c();
            return 1;
        }
        ((dActor_c *)r2)->mFlags &= ~0x20000;
    }
    if (mUseAltBodyModel == 0) {
        char *q = (char *)mObjInMouth;
        if (q != 0 && (int)(((dActor_c *)q)->actorID == 0xbf) == 0) {
            func_ov002_020d6790();
        } else {
            func_ov002_020d718c();
        }
    } else {
        if ((char *)mObjInMouth == 0 && mFireBreaths == 0)
            func_ov002_020d71a0();
    }
    return 1;
}

// State globals still have incompatible declarations across the legacy callers.
extern "C" {

void func_ov002_020d7430(daPly_c &player) {
    dActor_c *mouthActor = player.mObjInMouth;
    if (mouthActor->OnYoshiTryEat() == 2) {
        Vector3 pos;
        pos.x = player.mPosX;
        pos.y = player.mPosY;
        pos.z = player.mPosZ;
        player.Hurt(pos, 1, 0xc000, 1, 0, 0);
        return;
    }
    player.func_ov002_020c9e18();
    mouthActor = player.mObjInMouth;
    mouthActor->OnTurnIntoEgg(player);
    if (player.IsState(data_ov002_02110034)) {
        player.ChangeState(data_ov002_0211013c);
    }
    player.func_ov002_020d718c();
    Sound::PlayCharVoice(0, 0xfd, player.mCamSpacePos);
}
}

/* recovered: named members + shared header, real C++ method
 *
 * Yoshi swallowing and spitting, per frame. mStateStep walks the sequence, and
 * most transitions are gated on the body model's dExtFrameCtrl_c reaching a specific
 * frame -- which is why GetBodyModelID is re-fetched at each one rather than
 * hoisted; the original does that too.
 *
 * daPly_c::St_YoshiPower_Main at 0x020d7504 (ov002), size 0x9cc
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (also base/sp2)
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
/* local extern: the call passes an untyped this, so it cannot use the header method. */
/* local extern: the call passes an untyped this, so it cannot use the header method. */
struct PVec { int x, y, z; PVec() {} ~PVec() {} };

int daPly_c::St_YoshiPower_Main() {
    extern int data_ov002_0210e3d8[];
    /* The helpers this calls are all declared taking char*, and there are
       thirty-odd call sites. One alias keeps them untouched; the field
       accesses below are the part that needed recovering. */
    char* c = (char*)this;
    Vector3_16f dir;
    Vector3 tmp;
    PVec pos;
    dActor_c *obj0;
    dActor_c *dp;

    if (mIsAirborne == 0) {
        ApproachLinear(mHorzSpeed, 0, 0x1000);
        this->func_ov002_020c2f64();
    }

    switch (mStateStep) {
    case 0: {
        if (mStateArg == 0) {
            int t4 = (((dExtAnmModel_c *)mModels.mHeadModels[3])->currFrame >> 0xc) << 0x10;
            dExtFrameCtrl_c &anim0 = *mModels.mBodyModels[GetBodyModelID(this->param1 & 0xff, 0)];
            anim0.currFrame = (unsigned int)t4 >> 4;
            mStateArg = 1;
        }
        if (this->func_ov002_020d6998() != 0) {
            return 1;
        }
        if (this->FinishedAnim() != 0) {
            func_ov002_020d71ec(0);
        }
        break;
    }

    case 1: {
        ApproachLinear2(mHeadRot[1], 0, 0x200);
        ApproachLinear2(mHeadRot[2], 0, 0x200);
        int flag = 1;
        mHeadRotMode = flag;
        if (mObjInMouth != 0) {
            int b = mUseAltBodyModel;
            Matrix4x3 *t4 = &mModels.mHeadModels[(b << 2) + 3]->data.transforms[3];
            if (mObjInMouth != 0) {
                if (!(mObjInMouth->mFlags & 0x20000))
                    flag = 0;
                if (flag != 0) {
                    int id1 = this->GetBodyModelID(this->param1 & 0xff, 0);
                    MTX_MultVec43((Vector3 *)&t4->m[9], &mModels.mBodyModels[id1]->mat4x3, &tmp);
                    Vec3_MulScalarInPlace(&tmp, 0x8000);
                    dp = mObjInMouth;
                    dp->mPosX = tmp.x;
                    dp->mPosY = tmp.y;
                    dp->mPosZ = tmp.z;
                }
            }
            int id2 = this->GetBodyModelID(this->param1 & 0xff, 0);
            if (mModels.mBodyModels[id2]->WillHitFrame(0xa) != 0) {
                Vector3 *src = &mPos;
                dp = mObjInMouth;
                dp->mPosX = src->x;
                dp->mPosY = src->y;
                dp->mPosZ = src->z;
                dActor_c *o = mObjInMouth;
                int cond = (o->actorID == 0xbf);
                if (cond != 0) {
                    o->mFlags &= ~0x20000;
                    if (((daPly_c *)mObjInMouth)->func_ov002_020d5ed0() == 0) {
                        func_ov002_020d718c();
                    }
                } else {
                    (o)->mFlags |= 0x40000;
                    mObjInMouth->mFlags &= ~0x20000;
                    if (mObjInMouth->OnYoshiTryEat() == 4 || mObjInMouth->OnYoshiTryEat() == 2) {
                        mStateStep = 3;
                        int f2 = 1;
                        mUseAltBodyModel = f2;
                        mStateTimer = 8;
                        if (mObjInMouth->actorID != 0x10d)
                            f2 = 0;
                        if (f2 != 0) mStateTimer = 0xc;
                        unk_6ee = 1;
                        break;
                    }
                }
            } else {
                int id3 = this->GetBodyModelID(this->param1 & 0xff, 0);
                if (mModels.mBodyModels[id3]->WillHitFrame(0xc) != 0) {
                    mUseAltBodyModel = 1;
                    if (mObjInMouth->OnYoshiTryEat() == 5) {
                        int cond2 = (mObjInMouth->actorID == 0x132);
                        if (cond2 != 0) {
                            func_ov002_020d8118();
                        }
                        mObjInMouth->OnTurnIntoEgg(*(daPly_c *)c);
                        mObjInMouth = 0;
                        mFireBreaths = 1;
                    }
                }
            }
        }
        if (this->FinishedAnim() != 0) {
            obj0 = mObjInMouth;
            if (obj0 != 0) {
                if (obj0->OnYoshiTryEat() == 3) {
                    this->ChangeState(data_ov002_02110034);
                    return 1;
                }
                if (func_ov002_020e0ccc(mObjInMouth) != 0) {
                    return 1;
                }
                mMouthHoldTimer = 0x5a;
            }
            if (mIsAirborne != 0) {
                this->ChangeState(data_ov002_021101b4);
            } else {
                this->ChangeState(data_ov002_0211013c);
            }
            return 1;
        }
        break;
    }

    case 2: {
        ApproachLinear2(mHeadRot[1], 0, 0x200);
        ApproachLinear2(mHeadRot[2], 0, 0x200);
        mHeadRotMode = 1;
        if (mStateTimer != 0) {
            return 1;
        }
        dExtFrameCtrl_c &anim4 = *mModels.mBodyModels[GetBodyModelID(this->param1 & 0xff, 0)];
        if ((unsigned short)(anim4.currFrame >> 0xc) == 0) {
            if (mIsAirborne != 0) {
                this->ChangeState(data_ov002_021101b4);
            } else {
                this->ChangeState(data_ov002_0211013c);
            }
            return 1;
        }
        break;
    }

    case 3:
        if (mStateTimer == 0) {
            if (mObjInMouth == 0) {
                if (mIsUnderwater != 0) {
                    this->ChangeState(data_ov002_0211067c);
                } else {
                    this->ChangeState(data_ov002_0211013c);
                }
            } else {
                func_ov002_020d7430(*this);
            }
            return 1;
        }
        break;

    case 4: {
        int id5 = this->GetBodyModelID(this->param1 & 0xff, 0);
        if (mModels.mBodyModels[id5]->WillHitFrame(3) != 0) {
            int z = 0;
            mUseAltBodyModel = z;
            if (mUseAltBodyModel == 0) {
                dExtFrameCtrl_c &anim6 =
                    *mModels.mBodyModels[GetBodyModelID(this->param1 & 0xff, z)];
                /* The u16 startFrame takes the whole frame, currFrame >> 12;
                   its narrowing folds into the ROM's lsl #4 / lsr #16. */
                ((dExtAnmModel_c *)mModels.mHeadModels[3])
                    ->SetAnim((BCA_File *)data_ov002_0210e3d8[1], 0x40000000, 0x1000,
                              anim6.currFrame >> 12);
            }
        }
        int id7 = this->GetBodyModelID(this->param1 & 0xff, 0);
        if (mModels.mBodyModels[id7]->WillHitFrame(4) != 0) {
            obj0 = mObjInMouth;
            if (obj0 != 0) {
                int cond3 = (obj0->actorID == 0xbf);
                if (cond3 != 0) {
                    func_ov002_020d5cec(obj0);
                    mStateFlags &= ~2;
                } else {
                    (obj0)->mFlags |= 0x80000;
                    mObjInMouth->mFlags &= ~0x40000;
                }
            }
            mObjInMouth = 0;
            Sound::PlayCharVoice(0, 0xff, mCamSpacePos);
        }
        if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        break;
    }

    case 5: {
        if (mStateTimer != 0) {
            dExtFrameCtrl_c &anim8 = *mModels.mBodyModels[GetBodyModelID(this->param1 & 0xff, 0)];
            if (((unsigned int)(anim8.currFrame << 4) >> 0x10) >= 4) {
                int i2 = (*(unsigned short*)&mAngleY >> 4) * 2;
                const short *tbl = FX_SinCosTable_;
                int zv = mPosZ + (int)(((((long long)tbl[i2+1]) << 18) + 0x800) >> 12);
                int yv = mPosY + 0x37000;
                pos.x = mPosX + (int)(((((long long)tbl[i2]) << 18) + 0x800) >> 12);
                pos.z = zv;
                pos.y = yv;
                dir.x = FX_SinCosTable_[(*(unsigned short *)&mAngleY >> 4) * 2];
                dir.y = 0;
                dir.z = FX_SinCosTable_[(*(unsigned short *)&mAngleY >> 4) * 2 + 1];
                mParticle1 = Particle::System::New(mParticle1, kPtclYoshiFireA, pos.x, pos.y, pos.z,
                                                   (const Vector3_16f *)(&dir), 0);
                mParticle2 = Particle::System::New(mParticle2, kPtclYoshiFireB, pos.x, pos.y, pos.z,
                                                   (const Vector3_16f *)(&dir), 0);
                func_ov002_020dc09c(c);
                mAttackClsn.Clear();
                mAttackClsn.Update();
                if (mStateTimer == 1 && mFireBreaths == 0) {
                    int z2 = 0;
                    mUseAltBodyModel = z2;
                    dExtFrameCtrl_c &anim9 =
                        *mModels.mBodyModels[GetBodyModelID(this->param1 & 0xff, z2)];
                    ((dExtAnmModel_c *)mModels.mHeadModels[3])
                        ->SetAnim((BCA_File *)data_ov002_0210e3d8[1], 0x40000000, 0x1000,
                                  anim9.currFrame >> 12);
                }
                return 1;
            }
        }
        if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        break;
    }

    case 6: {
        dActor_c *p = mHeldObj;
        if (p != 0) {
            ApproachAngle(&mAngleY, p->mAngleY, 4, 0x2000, 0x10);
        }
        mPrevAngleY = mAngleY;
        int id10 = this->GetBodyModelID(this->param1 & 0xff, 0);
        if (mModels.mBodyModels[id10]->WillHitFrame(6) != 0) {
            this->func_ov002_020daa74();
            Sound::PlayCharVoice(0, 0x102, mCamSpacePos);
            break;
        }
        if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        break;
    }

    default:
        break;
    }

    AdvanceAnims();
    if (mUseAltBodyModel == 0 && mStateStep != 6) {
        ((dExtAnmModel_c *)mModels.mHeadModels[3])->Advance();
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* daPly_c::St_YoshiPower_Init at 0x020d7ed0 (ov002), size 0x200
 * Matched byte-for-byte with mwccarm 1.2/sp2p3
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
int daPly_c::St_YoshiPower_Init() {
    mStateFlags |= 0x200;
    unk_6ee = 0;
    mEggParams = 0;
    if (mFireTimer != 0) {
        mFireBreaths = 2;
    }
    if (mHorzSpeed >= 0x1c000) {
        mHorzSpeed = 0x1c000;
    }
    if (mHeldObjQueue[0] == 0) {
        if (mFireBreaths != 0) {
            this->SetAnim(0x6d, 0x40000000, 0x1000, 0);
            mStateStep = 5;
            mStateTimer = 0x1e;
            mFireBreaths -= 1;
            Sound::PlayCharVoice(0, 0x101, mCamSpacePos);
        } else if (mObjInMouth == 0) {
            this->SetAnim(0x6c, 0x40000000, 0x1000, 0);
            if (mUseAltBodyModel != 0) {
                func_ov002_020d71a0();
            }
            mStateStep = 0;
            mStateArg = 0;
            mAttackClsn.Clear();
            func_ov002_020d708c();
            Sound::PlayCharVoice(0, 0xfc, mCamSpacePos);
        } else {
            this->SetAnim(0x6d, 0x40000000, 0x1000, 0);
            mStateStep = 4;
        }
    } else {
        int i;
        this->SetAnim(0x30, 0x40000000, 0x1000, 0);
        mStateStep = 6;
        mHeldObj = mHeldObjQueue[0];
        mHeldObj->mFlags |= 0x100;
        func_ov002_020ed63c((daYegg_c *)mHeldObj, 1);
        mHeldObjQueue[0] = 0;
        for (i = 1; i < 5; i++) {
            dActor_c **p = mHeldObjQueue;
            dActor_c *v = p[i];
            if (v != 0) {
                (p + i)[-1] = v;
                mHeldObjQueue[i] = 0;
            }
        }
    }
    return 1;
}

void daPly_c::func_ov002_020d80d0() {
    if (mIsFire != 1)
        return;
    mFireTimer = 0;
    mIsFire = 0;
    mFireBreaths = 0;
    mUseAltBodyModel = 0;
    EndPowerupMusic(0x32);
}

void daPly_c::func_ov002_020d8118() {
    (this)->func_ov002_020bda48();
    mFireTimer = 0x258;
    StartPowerupMusic(0x32);
    (this)->func_ov002_020c43c4(5);
    mIsFire = 1;
}

void daPly_c::func_ov002_020d8158() {
    if (param1 != 3) {
        *(u16 *)(int)(&mStateFlags) &= ~0x1000;
        return;
    }
    if (IsState(data_ov002_02110034))
        return;
    if (IsState(data_ov002_0211004c))
        return;
    if (mIsNoControl != 0)
        return;
    if ((u16)(mStateFlags & 0x1000) == 0)
        return;
    func_ov002_020d8118();
    mFireBreaths = 1;
    func_ov002_020bdc18();
    *(u16 *)(int)(&mStateFlags) &= ~0x1000;
    mUseAltBodyModel = 1;
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_0211004c;
}

void daPly_c::InitFireYoshi() {

    u16 *p = &this->mStateFlags;
    *p |= 0x1000;
    if (mObjInMouth == 0)
        return;
    this->ChangeState(data_ov002_0211004c);
}

int daPly_c::St_DizzyStars_Cleanup() {

    mFlags &= ~0x80u;
    return 1;
}

/* recovered: named members + shared header, real C++ method */
/* the walk state */
int daPly_c::St_DizzyStars_Main() {

    func_ov002_020d98b4();
    /* Read SIGNED on purpose. mStateTimer is u16 and every other user treats
       it that way, but this test is `> 0` against a signed short, which is a
       bgt rather than a bhi. Spelling it unsigned changes the instruction. */
    if (*(s16*)&mStateTimer > 0) return 1;
    this->ChangeState(data_ov002_0211013c);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_DizzyStars_Init() {
    mStateTimer = 0x12c;
    mFlags |= 0x80;
    return 1;
}

int daPly_c::func_ov002_020d82f0() {
    if (mInvincibleTimer != 0)
        return 0;
    if (mIsMega != 0)
        return 0;
    if (mCapFlags != 0)
        return 0;
    if (mIsVanish != 0)
        return 0;
    // structured early return shrinks this function 0x70 -> 0x6c
    if (mIsTakingDamage != 0)
        goto ret0;
    if (mIsNoControl == 0)
        goto ret1;
ret0:
    return 0;
ret1:
    return 1;
}

/* func_ov002_020d8360 at 0x020d8360
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */

int daPly_c::func_ov002_020d8360(daPly_c *other, unsigned int flags) {

    {
        int r = func_ov002_020d82f0();
        if (!r) return r;
    }

    {
        unsigned char t = other->mAttackClashed;
        if (t) return t;
    }

    if (func_ov002_020d853c(other) == 0) {
        unsigned int m = mdCcAcPos_c.hitFlags & 0x3c0;
        if (m == 0) return m;

        {
            int r = func_ov002_020d8944(other, this);
            if (r != 0) {
                r = 1;
                other->mAttackClashed = r;
                mAttackClashed = r;
            }
            return r;
        }
    }

    {
        int flag = flags & 0x380;
        if (flag != 0) {
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        {
            Vector3 out, tr, v, cp;
            int *p = &other->mPosX;
            v.x = p[0];
            v.y = p[1];
            v.z = p[2];
            {
                int hr = Hurt(v, 1, 0, 0, (other->param1 + 1) & 0xff, 1);
                if (!hr) return hr;
            }
            tr.x = 0;
            tr.y = 0x50000;
            tr.z = 0x64000;
            Vec3_RotateYAndTranslate(&out, (const Vector3 *)&mPosX, mAngleY, &tr);
            cp.x = out.x;
            cp.y = out.y;
            cp.z = out.z;
            func_ov002_020d8d10(&cp);
        }
        func_020105cc(flags);
        {
            unsigned char b6f9 = mIsMetal;
            unsigned short idx = (unsigned short)(param1 + (other->param1 << 2));
            int t98;
            if (b6f9 != 0) idx = 0x11;
            t98 = data_ov002_020ff29c[idx] << 3;
            mHorzSpeed = t98;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            if (flag != 0) {
                mVertSpeed = t98 + 0x20000;
                {
                    int *hp = (int *)(int)(&mHorzSpeed);
                    int half = *hp >> 1;
                    *hp = half;
                    return half;
                }
            }
            mVertSpeed = 0x18000;
            return 0x18000;
        }
    }
}

int daPly_c::func_ov002_020d853c(dActor_c *o) {
    if (IsState(data_ov002_02110574)) {
        int j = (mPunchKickStep << 1) & 0xff;
        dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
        int v = anim.currFrame;
        int k = (unsigned short)(v >> 12);
        if (k < data_ov002_0210a5dc[j] || k > data_ov002_0210a5dc[j + 1])
            return 1;
        if (AngleDiff(Vec3_HorzAngle(&mPos, &o->mPos), mAngleY) < 0x4000)
            return 0;
    }
    return 1;
}

extern "C" int func_ov002_020d85fc(daPly_c *player, daPly_c *arg, int flags) {
    if ((flags & 0x1000) && arg->param1 == 2 &&
        (player->IsState(data_ov002_0211034c) ||
         (player->IsState(data_ov002_02110094) && player->mStateTimer != 0)) &&
        arg->TryGrab(*(dActor_c *)player)) {
        func_ov002_020db5f4(player, arg);
        return 1;
    }
    return 0;
}

struct PVec869c { s32 x, y, z; ~PVec869c() {} };

int daPly_c::func_ov002_020d869c() {
    char *r6;
    u32 flags;
    u32 f2;
    int t;
    PVec869c p;
    s32 x;
    s32 y;
    s32 z;

    if (mEatingPlayer != 0 || mIsBodyClsnEnabled == 0)
        return 0;
    if (mdCcAcPos_c.otherOwner == 0)
        return 0;
    r6 = (char *)dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
    if (r6 == 0 || r6 == (char *)this)
        return 0;
    t = (int)(((dActor_c *)r6)->actorID == 0xbf);
    if (t == 0)
        return 0;

    flags = mdCcAcPos_c.hitFlags & 0x27fe0;
    if (flags != 0) {
        if (func_ov002_020d85fc((daPly_c *)(char *)this, (daPly_c *)r6, (int)flags) == 1)
            return 0;
        f2 = flags & ~0x1000u;
        if (f2 != 0)
            func_ov002_020d8360((daPly_c *)r6, f2);
    } else {
        if (mIsNoControl != 0)
            return 0;
        if (JumpedOnByPlayer(mdCcAcPos_c, *(daPly_c *)r6) != 0) {
            if (((daPly_c *)r6)->IsInAir() == 0)
                return 0;
            if (func_ov002_020d9298((daPly_c *)r6) != 0) {
                func_ov002_020db8bc(0);
                x = mPosX;
                y = mPosY + 0x82000;
                z = mPosZ;
                p.x = x;
                p.y = y;
                p.z = z;
                Particle::System::NewSimple(kPtclLandingDust, p.x, p.y, p.z);
                Sound::PlayBank0(0x12, mCamSpacePos);
                Sound::PlayCharVoice(mCharacter, 0x19, mCamSpacePos);
            }
        }
    }
    return 1;
}

// Set bit 0x20 in the halfword flag at self+0x6ce and return. The u64-mask launder
// forces the base+offset to materialize into a register (pool-loaded 0x6ce + add) so
// the ldrh/strh reuse it, matching the ROM instead of folding the address.
extern "C" {

void func_ov002_020d8838(char *self)
{
    daPly_c *player = (daPly_c *)self;
    player->mStateFlags |= 0x20;
}
}

/* recovered: shared common types */

void daPly_c::func_ov002_020d8854() {
    daPly_c *other = (daPly_c *)dActor_c::FindWithID(mAttackClsn.otherOwner);
    if (other == 0)
        return;
    {
        int b = (other->actorID == 0xbf);
        if (b != 0) {
            if (other->mPlayerNo <= mPlayerNo)
                return;
            if ((mAttackClsn.hitFlags & 0x3c0) == 0)
                return;
            if (func_ov002_020d8944(this, other) != 0) {
                other->mAttackClashed = 1;
                mAttackClashed = 1;
            }
            return;
        }
    }
    {
        Vector3 out;
        Vector3 tr;
        Vector3 cp;
        tr.x = 0;
        tr.y = 0x50000;
        tr.z = 0x64000;
        Vec3_RotateYAndTranslate(&out, &mPos, mAngleY, &tr);
        cp = out;
        func_ov002_020d8d10(&cp);
    }
}

/* recovered: shared common types */
// non-trivial destructor keeps the particle-spawn vector on the stack (no SROA)
struct PVec8944 {
    int x, y, z;
    ~PVec8944() {}
};

int daPly_c::func_ov002_020d8944(daPly_c *enemy, daPly_c *v1) {
    s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
    void PlayerNewSimpleHold12(unsigned int id, int x, int y, int z);
    PVec8944 p;
    Vector3 sum;
    Vector3 asr;
    int b0 = ((unsigned short)(v1)->actorID == 0xbf);
    if (b0) {
        s16 ang = Vec3_HorzAngle(&(enemy)->mPos, &(v1)->mPos);
        if (AngleDiff(ang, (enemy)->mAngleY) >= 0x4000)
            return 0;
        (enemy)->mHorzSpeed = 0xa000;
        (v1)->mHorzSpeed = 0xa000;
        (enemy)->mPrevAngleX = 0;
        (enemy)->mPrevAngleY = ang + 0x8000;
        (enemy)->mPrevAngleZ = 0;
        (v1)->mPrevAngleX = 0;
        (v1)->mPrevAngleY = ang;
        (v1)->mPrevAngleZ = 0;
        Vec3_Add(&sum, &(enemy)->mPos, &(v1)->mPos);
        Vec3_Asr(&asr, &sum, 1);
        p.x = asr.x;
        p.y = asr.y;
        p.z = asr.z;
        p.y += 0x50000;
        Particle::System::NewSimple(kPtclAttackClashA, p.x, p.y, p.z);
        Particle::System::NewSimple(kPtclAttackClashB, p.x, p.y, p.z);
        enemy->mAttackClashed = 1;
        v1->mAttackClashed = 1;
        return 1;
    }
    return 0;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

void daPly_c::func_ov002_020d8a50(int which) {
    int flag = 1;
    u32 i2 = (u8)(which << 1);
    dExtAnmModel_c *model;
    int frame;

    dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
    frame = (u16)(anim.currFrame >> 12);
    if (frame < data_ov002_0210a5cc[i2] || frame > data_ov002_0210a5cc[i2 + 1]) {
        flag = 0;
        mAttackClsn.Clear();
    }

    if ((u8)mAttackClashed != 0)
        return;

    switch (which) {
    case 0:
        func_ov002_020dbf4c();
        break;
    case 1:
        func_ov002_020dbe70();
        break;
    case 2:
        func_ov002_020dbd94();
        break;
    case 3:
        func_ov002_020dbbc0();
        break;
    }

    if (mAttackClsn.otherOwner != 0) {
        mHorzSpeed = 0xa000;
        mPrevAngleY = mAngleY + 0x8000;
        func_ov002_020d8854();
    }

    mAttackClsn.Clear();
    if (flag == 1)
        mAttackClsn.Update();

    model = mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
    if (!model->WillHitFrame(data_ov002_0210a5cc[i2]))
        return;

    /* func_ov002_020ef070/020eeeb8 (d_bg_actor.cpp) take the mover as char *. */
    switch (which) {
    case 0:
    case 1:
        func_ov002_020ef070(&mMeshClsn, (char *)this);
        break;
    case 2:
    case 3:
        func_ov002_020eeeb8(&mMeshClsn, (char *)this);
        break;
    }

    {
        u16 t = mStateFlags & 0x20;
        if (t == 0)
            return;
    }

    mStateFlags &= ~0x20;
    Sound::PlayBank0(0xb5, mCamSpacePos);

    {
        u16 ang = (u16)mAngleY;
        dBgCh_Lin ray;
        Vector3 v1;
        Vector3 v2;
        Vector3 normal;
        int idx;

        idx = ang >> 4;
        v1.x = mPosX;
        v1.y = mPosY;
        v1.z = mPosZ;
        v1.y += 0x32000;
        v2.x = v1.x;
        v2.y = v1.y;
        v2.z = v1.z;
        v2.x += FX_SinCosTable_[idx * 2] * 0x64;
        v2.z += FX_SinCosTable_[idx * 2 + 1] * 0x64;
        ray.SetObjAndLine(v1, v2, this);
        if (ray.DetectClsn()) {
            ray.surface.CopyNormalTo(normal);
            if (normal.y == 0) {
                mHorzSpeed = 0xa000;
                mPrevAngleY = mAngleY + 0x8000;
                ::Vector3 clsnPos = ray.GetClsnPos();
                func_ov002_020d8d10(&clsnPos);
            }
        }
    }
}

/* recovered: shared common types */
/* func_ov002_020d8d10 at 0x020d8d10
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
void daPly_c::func_ov002_020d8d10(Vector3 *pos) {
    struct Callback;
    Vector3_16 vec;

    vec.x = FX_SinCosTable_[((unsigned short)(short)(mAngleY + 0x8000) >> 4) * 2];
    vec.y = 0;
    vec.z = FX_SinCosTable_[(((unsigned short)(short)(mAngleY + 0x8000) >> 4) * 2) + 1];

    if (param1 != 2) {
        Particle::System::New(0, kPtclAttackHitA, pos->x, pos->y, pos->z,
                              (const Vector3_16f *)(&vec), 0);
        Particle::System::New(0, kPtclAttackHitB, pos->x, pos->y, pos->z, 0, 0);
    } else {
        Particle::System::New(0, kPtclWarioAttackHitA, pos->x, pos->y, pos->z,
                              (const Vector3_16f *)(&vec), 0);
        Particle::System::New(0, kPtclWarioAttackHitB, pos->x, pos->y, pos->z,
                              (const Vector3_16f *)(&vec), 0);
        Particle::System::New(0, kPtclWarioAttackHitC, pos->x, pos->y, pos->z, 0, 0);
    }
}

/* recovered: named members + shared header, declarations from a shared header */
/* recovered: named members + shared header */

int daPly_c::Hurt(const Vector3 &a, u32 b, Fix12i dmg, u8 arg4, u8 arg5, u8 arg6) {
    s16 ang;
    void* p360;
    int hasCap;

    if (func_ov002_020d82f0() == 0) {
        return 0;
    }

    if (arg5 != 0) {
        if ((int)b < 2) {
            b += 2;
        }
    }

    hasCap = this->HasNoCap();
    if (hasCap == 0 && this->mCharacter != this->param1 && this->mIsMetal == 0 && b != 0) {
        Sound::PlayCharVoice(this->mCharacter, 0x22, this->mCamSpacePos);
    } else {
        int t = 0;
        if ((int)b > 1) t = 1;
        func_ov002_020d91b8(t);
    }

    if (this->mIsMetal != 0) {
        b = 0;
    }
    p360 = (void *)this->mObjInMouth;
    if (p360 != 0) {
        int match = (((dActor_c *)p360)->actorID == 0xbf);
        if (match) {
            func_ov002_020d5cec((char*)p360);
            this->mStateFlags &= ~2;
        }
        this->mUseAltBodyModel = 0;
        {
            int *p = (int *)&this->mObjInMouth->mFlags;
            *p |= 0x80000;
        }
        {
            int *p = (int *)&this->mObjInMouth->mFlags;
            *p &= ~0x40000;
        }
        {
            int *p = (int *)&this->mObjInMouth->mFlags;
            *p &= ~0x20000;
        }
        this->mObjInMouth = 0;
    }

    this->mStateStep = 0;

    if (b != 0) {
        int neq = 0;
        if (this->mCharacter != this->param1)
            neq = 1;
        if (neq)
            this->mStateStep = 2;
    }
    if ((int)b > 1) {
        this->mStateStep = 2;
    }
    if (this->mIsAirborne != 0) {
        this->mStateStep = 4;
    }

    ang = Vec3_HorzAngle(&this->mPos, &a);
    if (AngleDiff(this->mAngleY, ang) > 0x4000) {
        this->mStateStep += 1;
    }
    this->mPrevAngleY = ang + 0x8000;
    if (this->mStateStep & 1) {
        this->mAngleY = this->mPrevAngleY;
    } else {
        this->mAngleY = ang;
    }

    if (arg4 == 1) {
        u16 tv = data_ov002_020ff128[this->param1];
        dmg = (dmg * tv) / 100;
    }
    this->mHorzSpeed = dmg;

    if (arg5 != 0) {
        u8 t = arg5 - 1;
        this->mStateStep |= 0x10;
        this->mStateWork = t;
    }

    this->mAttackClsn.Unlink();
    this->mHurtDamage = b;

    if (this->mIsUnderwater == 0) {
        if (this->mHurtDamage != 0) {
            if (arg6 != 0) {
                volatile struct {
                    int x, y, z;
                } v; // without volatile, Hurt shrinks 0x348 to 0x344
                int y = this->mPosY;
                int z = this->mPosZ;
                int x = this->mPosX;
                y = y + 0x50000;
                v.x = x;
                v.y = y;
                v.z = z;
                if (*(int *)((char *)&this->mHurtDamage) == 1) {
                    Particle::System::NewSimple(kPtclPlayerHurtOneA, x, y, z);
                    Particle::System::NewSimple(kPtclPlayerHurtOneB, v.x, v.y, v.z);
                } else {
                    Particle::System::NewSimple(kPtclPlayerHurtA, x, y, z);
                    Particle::System::NewSimple(kPtclPlayerHurtB, v.x, v.y, v.z);
                }
            }
        }

        this->ChangeState(data_ov002_02110094);
        if (func_ov002_020d91e0(b << 8, 1) != 0) {
            this->mStateStep &= 1;
            this->ChangeState(data_ov002_0211010c);
        }
    } else {
        this->ChangeState(data_ov002_021100ac);
        func_ov002_020d91e0(b << 8, 1);
    }

    return 1;
}

void daPly_c::func_ov002_020d91b8(int a) {
    Sound::PlayCharVoice(mCharacter, data_ov002_020ff120[a & 1], mCamSpacePos);
}

int daPly_c::func_ov002_020d91e0(int damage, int doPre) {
    extern u8 gGameMode;
    extern char data_0209ee90[];
    if (doPre != 0)
        DropHeldObj();

    if (SaveData::HasPlayerLostCap()) {
        if (mHasWings == 0)
            damage = damage + (damage >> 1);
    }

    {
        int b = (gGameMode == 1);
        if (b != 0) return 0;
    }

    if (*(int*)(data_0209ee90 + 0x248) == 0) {
        Heal(-damage);
    }
    if (GetHealth() != 0)
        return 0;

    Heal(-0x100);
    return 1;
}

extern "C" {

extern "C" int func_ov002_020d9298(daPly_c *player) {
    if (player->IsState(data_ov002_021105a4))
        return 0;
    Sound::PlayCharVoice(player->mCharacter, 0x14, player->mCamSpacePos);
    if (player->IsState(data_ov002_021100f4)) {
        player->mVertSpeed = 0x40000;
        return 1;
    }
    if (player->IsState(data_ov002_0211019c)) {
        player->mJumpComboTimer = 0xa;
    }
    player->ChangeState(data_ov002_0211019c);
    return 1;
}
}

/* recovered: named members + shared header, declarations from a shared header */
/* recovered: named members + shared header */
void daPly_c::Bounce(Fix12i f) {
    if (this->IsState(data_ov002_021105a4))
        return;
    if (this->GetHealth() == 0)
        return;
    this->ChangeState(data_ov002_0211019c);
    this->mVertSpeed = f;
    Sound::PlayCharVoice(this->mCharacter, 0x14, this->mCamSpacePos);
    Sound::PlayBank0(0xb6, this->mCamSpacePos);
}

/* recovered: shared common types */

int daPly_c::func_ov002_020d93ac() {
    short GetAngleToCamera(int i);
    extern u8 gGameMode;
    char *self = (char*)this;
    Vector3 pos;
    char *spawned;
    u32 chr;
    int xv, yv, sum, zv;
    int b;
    int ang, xx, zz;

    // return 0 at each check grows this function 0x120 -> 0x140
    if (this->HasNoCap()) goto fail;

    chr = this->param1 & 0xff;
    if (this->mCharacter == chr) goto fail;
    if (chr == 3) goto fail;

    this->SetNewHatCharacter(this->mCharacter, 0, false);
    this->mHasNoCap = 1;

    yv = this->mPosY;
    zv = this->mPosZ;
    xv = this->mPosX;
    sum = yv + 0x82000;
    pos.x = xv;
    pos.y = sum;
    pos.z = zv;
    spawned = (char *)Spawn(0x10d, (chr << 8) | 5, pos, 0, this->mAreaId, -1);
    if (spawned == 0) goto fail;

    b = (gGameMode == 1);
    if (!b) {
        ang = GetAngleToCamera(this->mPlayerNo);
        zz = this->mPrevAngleZ;
        xx = this->mPrevAngleX;
        ang += 0x8000;
        ((dActor_c *)spawned)->mPrevAngleX = xx;
        ((dActor_c *)spawned)->mPrevAngleY = ang;
        ((dActor_c *)spawned)->mPrevAngleZ = zz;
    } else {
        ((dActor_c *)spawned)->mPrevAngleX = this->mPrevAngleX;
        ((dActor_c *)spawned)->mPrevAngleY = this->mPrevAngleY;
        ((dActor_c *)spawned)->mPrevAngleZ = this->mPrevAngleZ;
    }
    ((dActor_c *)spawned)->mHorzSpeed = 0xb000;
    return 1;
fail:
    return 0;
}

/* recovered: shared common types */

int daPly_c::func_ov002_020d94cc() {
    short GetAngleToCamera(int i);
    void GiveVsStars(int idx, int delta);
    extern u8 gGameMode;
    extern s8 gNumStageStars;
    char *self = (char*)this;
    Vector3 pos;
    dActor_c *sndobj;
    char *spawned;
    int ang, xx, zz;
    int xv, yv, sum, zv;
    int b;

    // return 0 at each check grows this function 0x188 -> 0x190
    if ((&gNumStageStars)[this->mPlayerNo] <= 0)
        goto fail;

    sndobj = SpawnSoundObj(3);
    b = (gGameMode == 1);
    if (b) {
        if (sndobj != 0)
            ((daSoundObj_c *)sndobj)->mSoundID = 0x50;
    }

    GiveVsStars(this->mPlayerNo, -1);

    yv = this->mPosY;
    zv = this->mPosZ;
    xv = this->mPosX;
    sum = yv + 0x82000;
    pos.x = xv;
    pos.y = sum;
    pos.z = zv;

    b = (gGameMode == 1);
    if (b)
        spawned = (char *)Spawn(0xb2, 0x10, pos, 0, this->mAreaId, -1);
    else
        spawned = (char *)Spawn(0xb3, 0x10, pos, 0, this->mAreaId, -1);

    if (spawned == 0) goto fail;

    ((daStar_c *)spawned)->SeekNewMarker(self, 0);

    b = (gGameMode == 1);
    if (!b) {
        ang = GetAngleToCamera(this->mPlayerNo);
        zz = this->mPrevAngleZ;
        xx = this->mPrevAngleX;
        ang += 0x8000;
        ((dActor_c *)spawned)->mPrevAngleX = xx;
        ((dActor_c *)spawned)->mPrevAngleY = ang;
        ((dActor_c *)spawned)->mPrevAngleZ = zz;
    } else {
        ((dActor_c *)spawned)->mPrevAngleX = this->mPrevAngleX;
        ((dActor_c *)spawned)->mPrevAngleY = this->mPrevAngleY;
        ((dActor_c *)spawned)->mPrevAngleZ = this->mPrevAngleZ;
    }
    ((dActor_c *)spawned)->mHorzSpeed = this->mHorzSpeed;
    return 1;
fail:
    return 0;
}

/* recovered: named members + shared header, real C++ method */
// daPly_c::St_Hurt_Cleanup - clear flag bit 0x80 in the u32 at this+0xb0, return 1.
int daPly_c::St_Hurt_Cleanup() {
    mFlags &= ~0x80;
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Three-phase hurt state, sequenced by mStatePhase: 0 lands the hit (sound, voice,
 * knockback), 1 waits for horizontal speed to bleed off, 2 runs out the timer
 * and then hands back to the swimming or walking state depending on
 * mIsUnderwater. mStateWork is used as two nibbles here -- the high one
 * selects the hurt variant, the low one gates animation advance.
 */
/* the swim state */
/* the walk state */
int daPly_c::St_Hurt_Main() {
    mIsBodyClsnEnabled = 1;
    if (mJumpComboTimer != 0) {
        mIsBodyClsnEnabled = 0;
    }

    switch (mStatePhase) {
    case 0:
        func_ov002_020d9a4c();
        StartHurtFallAnim();
        if (mIsAirborne != 0) break;
        func_ov002_020d9aac();
        mStateTimer = 0;
        if ((mStateStep & 7) >= 2) {
            Sound::PlayBank0((u32)mGroundSoundType + 0x50, mCamSpacePos);
            Sound::PlayCharVoice(mCharacter, 0x11, mCamSpacePos);
        }
        if (mHurtDamage != 0) {
            u32 masked = mStateStep & 0xf0;
            if (masked == 0 || masked == 0x10) {
                func_ov002_020d94cc();
            }
            mHurtDamage = 0;
        }
        if (mIsInShallowWater == 0) {
            func_ov002_020bf9d4();
        }
        mStatePhase = 1;
        break;

    case 1:
        func_ov002_020d9a4c();
        StartHurtFallAnim();
        if (mIsAirborne != 0) break;
        func_ov002_020d9aac();
        if (mHorzSpeed != 0) break;
        mStatePhase = 2;
        if ((mStateWork >> 4) == 2) {
            mStateTimer = 0x5a;
        }
        break;

    case 2:
        if (mStateTimer != 0) {
            func_ov002_020d98b4();
            return 1;
        }
        mFlags &= ~0x80;
        mStateWork &= 0xf0;
        if (FinishedAnim()) {
            if ((mStateStep & 1) == 0) {
                mPrevAngleY = mPrevAngleY + 0x8000;
                mAngleY = mPrevAngleY;
            }
            mIsTakingDamage = 0;
            if (mStateArg & 2) {
                mInvincibleTimer = 0x24;
            }
            if (mIsUnderwater != 0) {
                this->ChangeState(data_ov002_0211067c);
            } else {
                this->ChangeState(data_ov002_0211013c);
            }
        }
        break;

    default:
        break;
    }

    if ((mStateWork & 0xf) == 0) {
        AdvanceAnims();
    }
    mPrevVertSpeed = mVertSpeed;
    if (mIsAirborne == 0) {
        func_ov002_020e2c84();
    }
    return 1;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

void daPly_c::func_ov002_020d98b4() {
    int t = this->func_ov002_020beb38();
    this->mStateTimer -= t;
    {
        if ((s16)mStateTimer < 0)
            mStateTimer = 0;
    }
    {
        unsigned int id = this->GetBodyModelID(param1 & 0xff, 0);
        Matrix4x3 *bone = &mModels.mBodyModels[id]->data.transforms[15];
        unsigned int id2 = this->GetBodyModelID(param1 & 0xff, 0);
        Vector3 v;
        Vector3 s;
        MTX_MultVec43((Vector3 *)&bone->m[9], &mModels.mBodyModels[id2]->mat4x3, &v);
        Vec3_MulScalar(&s, &v, 0x8000);
        s.y += 0x50000;
        mParticle1 =
            (s32)Particle::System::New((u32)mParticle1, kPtclDizzyStars, s.x, s.y, s.z, 0, 0);
    }
    mFlags |= 0x80;
}

void daPly_c::StartHurtFallAnim() {
    daPly_c *player = this;
    u8 idx;
    if (player->mIsAirborne == 0)
        return;
    idx = player->mStateStep;
    if (idx > 1) return;
    (this)->SetAnim(data_ov002_0210a6ec[idx & 7], 0x40000000, 0x1000,
                    (u16)(data_ov002_02109dcc[idx] + 1));
    player->mStateStep += 2;
    player->mStateWork |= 1;
}

void daPly_c::func_ov002_020d9a4c() {
    int idx = GetBodyModelID(param1 & 0xff, 0);
    dExtAnmModel_c *anim = mModels.mBodyModels[idx];
    if (anim->WillHitFrame(data_ov002_02109dc4[mStateStep & 7]) != 0)
        mStateWork |= 1;
}

void daPly_c::func_ov002_020d9aac() {
    int v;

    if ((mStateStep & 7) >= 2) {
        if (mPrevVertSpeed < 0) {
            mVertSpeed = -(mPrevVertSpeed >> 2);
            v = mVertSpeed;
            if (v <= 0x1000) {
                mVertSpeed = 0;
            } else if (v >= 0xa000) {
                mVertSpeed = 0xa000;
            }
            if (mHorzSpeed >= 0xa000)
                mHorzSpeed = 0xa000;
        }
    } else {
        if (mHorzSpeed >= 0x12000)
            mHorzSpeed = 0x12000;
    }

    if ((mStateStep & 7) < 2) {
        ApproachLinear(mHorzSpeed, 0, 0x800);
    } else {
        ApproachLinear(mHorzSpeed, 0, 0x1800);
    }
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Hurt_Init() {
    extern char *gCamera;
    int mid;
    char *m;
    u8 old;
    this->SetAnim(data_ov002_0210a6d4[mStateStep & 7], 0x40000000, 0x1000, 0);
    mid = this->GetBodyModelID(param1 & 0xff, 0);
    m = (char *)mModels.mBodyModels[mid];
    m = (char *)((unsigned int)&(dExtFrameCtrl_c &)*(dExtAnmModel_c *)m & 0xFFFFFFFFU);
    ((dExtFrameCtrl_c *)m)->currFrame = 0;
    mStatePhase = 0;
    old = mStateWork;
    mStateWork = 0;
    if ((mStateStep & 0xf0) == 0x10)
        mStateWork = old << 4;
    mStateArg = 0;
    if (mHurtDamage != 0) {
        mStateArg = 2;
        func_ov002_020d93ac();
    }
    mIsTakingDamage = 1;
    mPrevVertSpeed = mVertSpeed;
    ((dCamera_c *)gCamera)->func_0200d89c();
    mJumpComboTimer = 5;
    return 1;
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_02110094;
}

int daPly_c::GetHurtState() {
    if (this->IsState(data_ov002_02110094))
        return mStateStep & 7;
    return -1;
}

void daPly_c::func_ov002_020d9c70() {
    short v = mAngleYSpeed;
    int absv;
    int A, Braw;
    short Bnarrow, Bresult;
    char *p;

    absv = (v < 0) ? (short)(-v) : v;
    A = FX_MUL(absv, 0x28000);

    absv = (v < 0) ? (short)(-v) : v;
    Braw = FX_MUL(absv, 0x1c71);
    Bnarrow = (short)Braw;

    Bresult = (v < 0) ? (short)(mAngleY - Bnarrow) : (short)(mAngleY + Bnarrow);

    p = (char *)mHeldObj;
    if (p == 0)
        return;

    {
        int t = ((dActor_c *)p)->actorID;
        t = t == 0xbf;
        if (t != false) {
            func_ov002_020db54c(p, A, 0x20000, Bresult);
            return;
        }
    }

    ((dActor_c *)p)->mPrevAngleX = 0;
    ((dActor_c *)p)->mPrevAngleY = Bresult;
    ((dActor_c *)p)->mPrevAngleZ = 0;
    {
        char *q = (char *)mHeldObj;
        short *r = (short *)(int)(&((dActor_c *)q)->mPrevAngle);
        short first = r[0];
        ((dActor_c *)q)->mAngleX = first;
        ((dActor_c *)q)->mAngleY = r[1];
        ((dActor_c *)q)->mAngleZ = r[2];
    }
    mHeldObj->mHorzSpeed = A;
    {
        char *q = (char *)mHeldObj;
        ((dActor_c *)q)->mSpeedX = 0;
        ((dActor_c *)q)->mVertSpeed = 0x20000;
        ((dActor_c *)q)->mSpeedZ = 0;
    }
}

/* The state test compares the comparison as an int,
 * `(int)(x == 0xbf) == 0`.  In C++ the comparison is a bool: tested
 * directly, the 0/1 is folded into the branch (12 bytes short), and
 * widened to long long, as the C shard spelled it, the early return's 0
 * is loaded ahead of the branch and reused below it (29 words differ).
 * The int view keeps the 0/1 and gives the return 0 its own path. */
int daPly_c::func_ov002_020d9dcc() {
    extern s16 data_0209f4a0[];
    char* p;

    if (param1 != 2)
        return 0;

    p = (char *)mHeldObj;
    if (p == 0 || (int)(((dActor_c *)p)->actorID == 0xbf) == 0)
        return 0;

    if (data_0209f498[gActivePlayerSlot].mag == 0 || mIsAirborne != 0) {
        mStateTimer = 0;
        return 0;
    }

    if (mStateTimer == 0) {
        mStateTimer = 0x3c;
        mAngleYSpeed = mAngleY;
        mStateWork = 0;
        return 0;
    }

    if (mStateWork == 0) {
        if (mAngleY < mAngleYSpeed)
            mStateWork = 0xff;
        else
            mStateWork = 1;
    }

    s16 diff = mAngleY - mPreClsnAngleY;
    if ((s8)mStateWork >= 0) {
        if (diff < 0) {
            mStateTimer = 0;
            return 0;
        }
        if (AngleDiff(mAngleY, mAngleYSpeed) >= 0x4000) {
            mStateWork += 1;
            mAngleYSpeed += 0x4000;
        }
    } else {
        if (diff > 0) {
            mStateTimer = 0;
            return 0;
        }
        if (AngleDiff(mAngleY, mAngleYSpeed) >= 0x4000) {
            mStateWork -= 1;
            mAngleYSpeed -= 0x4000;
        }
    }

    s8 v = (s8)mStateWork;
    if (v < 0)
        v = -v;
    if (v < 5)
        return 0;

    ChangeState(data_ov002_02110604);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_SwingPlayer_Cleanup() {
    char *p = (char *)mHeldObj;
    if (p) {
        ((dActor_c *)p)->mFlags &= ~0x800;
    }
    mPrevAngleY = mAngleY;
    return 1;
}

/* recovered: daPly_c::St_SwingPlayer_Main, the per-frame body of the swing state (animation, particles, the approach helpers). */
// Provenance: first drafted from nearmiss/db.jsonl (divergence 8) and kept on main as a
// decompiled-not-matched draft; matched 2026-10-12 under mwccarm 2004/b56. The swing step
// is the s16 angle difference shifted IN PLACE (`diff >>= 6` / `diff >>= 7`), which keeps
// the difference and the step in one register across both if-converted arms, and the
// angular speed is declared before the difference, which gives it ip and the step r3.

int daPly_c::St_SwingPlayer_Main() {
    char* c = (char*)this;
    typedef s32 Fix12;
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    switch (this->mStateStep) {
    case 0:
        if (this->FinishedAnim()) {
            ((daPly_c *)c)->SetAnim(0x6d, 0x40000000, 0x1000, 0);
            this->mStateStep = 1;
        }
        this->mAngleY += this->mAngleYSpeed;
        break;
    case 1: {
        Vector3 offset;
        Vector3 v;
        if ((data_0209f498[gActivePlayerSlot].pressed & 1) != 0) {
            char *p = (char *)mHeldObj;
            if (p != 0) {
                int b = (((dActor_c *)p)->mFlags & 0x200) != 0;
                if (!b) {
                    func_ov002_020d9c70();
                    func_ov002_020da95c();
                    ((daPly_c *)c)->SetAnim(0x6e, 0x40000000, 0x1000, 0);
                    this->mStateStep = 3;
                } else {
                    DropHeldObj();
                }
            }
            return 1;
        }
        if (data_0209f498[gActivePlayerSlot].mag != 0) {
            s16 av = this->mAngleYSpeed;
            s16 diff = this->mDesiredAngleY - this->mPrevDesiredAngleY;
            if (av < 0) av = -av;
            if (av < 0x800) diff >>= 6; else diff >>= 7;
            if (diff != 0) {
                this->mAngleYSpeed += diff;
                if (this->mAngleYSpeed >= 0x1800)
                    this->mAngleYSpeed = 0x1800;
                else if (this->mAngleYSpeed <= -0x1800)
                    this->mAngleYSpeed = -0x1800;
            } else {
                ApproachLinear2(this->mAngleYSpeed, 0, 0x20);
            }
        } else {
            ApproachLinear2(this->mAngleYSpeed, 0, 0x80);
        }
        {
            s16 a2 = this->mAngleYSpeed;
            if (a2 < 0) a2 = -a2;
            if (a2 >= 0xf99) {
                u32 id = 0x132;
                v.x = this->mPosX;
                v.y = this->mPosY;
                v.z = this->mPosZ;
                v.y = this->mPosY + 0x3c000;
                if (this->mAngleYSpeed > 0)
                    id = 0x133;
                mParticle1 = Particle::System::New((u32)mParticle1, id, *&v.x, *&v.y, *&v.z, 0, 0);
                mLoopingSoundHandle =
                    Sound::PlayLong((u32)mLoopingSoundHandle, 0, 0x1c, mCamSpacePos, 0);
            }
            if (this->mAngleYSpeed != 0)
                this->mStateTimer = 0xa;
        }
        if (this->mStateTimer == 0) {
            ((daPly_c *)c)->SetAnim(0x6f, 0x40000000, 0x1000, 0);
            this->mStateStep = 2;
        }
        {
            s16 before = this->mAngleY;
            this->mAngleY += this->mAngleYSpeed;
            if ((this->mAngleYSpeed <= -0x100 && before < this->mAngleY) ||
                (this->mAngleYSpeed >= 0x100 && before > this->mAngleY))
                Sound::PlayBank0(0x1b, mCamSpacePos);
        }
        offset.x = 0;
        offset.y = 0x32000;
        offset.z = 0x64000;
        func_ov002_020dc174(&offset, 0x32000, 0x32000, 0x80, 0);
        mAttackClsn.Clear();
        mAttackClsn.Update();
        break;
    }
    case 2:
        if (this->FinishedAnim())
            this->ChangeState(data_ov002_0211013c);
        break;
    case 3:
        if (this->FinishedAnim())
            this->ChangeState(data_ov002_0211013c);
        break;
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_SwingPlayer_Init() {
    mHorzSpeed = 0;
    this->SetAnim(0x6c, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    mAngleYSpeed = mAngleY - mPreClsnAngleY;
    {
        dActor_c *obj = mHeldObj;
        if (obj != 0)
            obj->mFlags |= 0x800;
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_GrabBowserTail_Cleanup() {
    func_ov002_020daa74();
    return 1;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_GrabBowserTail_Main() {
    extern void func_02012694(unsigned int id, const Vector3 *v);
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    extern char data_0209ee90[];
    switch (mStateStep) {
    case 0:
        if (this->FinishedAnim()) {
            mStateStep = 1;
            this->SetAnim(0x80, 0, 0x1000, 0);
        }
        break;
    case 1:
        if ((data_0209f498[gActivePlayerSlot].pressed & 1) != 0) {
            s16 x = mAngleYSpeed;
            if (x < 0) x = -x;
            if (x > 0xe00)
                Sound::PlayCharVoice(mCharacter, 0x32, mCamSpacePos);
            else
                Sound::PlayCharVoice(mCharacter, 0x33, mCamSpacePos);
            mStateStep = 2;
            this->SetAnim(0x7f, 0x40000000, 0x1000, 0);
            func_ov002_020daa74();
            return 1;
        }
        if (*(int*)(data_0209ee90 + 0x24c) != 0)
            mStateWork = 0;
        if (data_0209f498[gActivePlayerSlot].mag == 0) {
            u8 v = mStateWork;
            (this->mStateWork)++;
            if (v > 0x78) {
                mStateStep = 2;
                this->SetAnim(0x7f, 0x40000000, 0x1000, 0);
                func_ov002_020daa74();
                return 1;
            }
        } else {
            mStateWork = 0;
        }
        if (data_0209f498[gActivePlayerSlot].mag > 0x200) {
            if (mStateArg == 0) {
                mStateArg = 1;
                mPrevDesiredAngleY = mDesiredAngleY;
            } else {
                s16 xv = mAngleYSpeed;
                if (xv < 0) xv = -xv;
                s32 r3;
                if (xv < 0x800)
                    r3 = ((mDesiredAngleY - mPrevDesiredAngleY) << 10) >> 16;
                else
                    r3 = ((mDesiredAngleY - mPrevDesiredAngleY) << 9) >> 16;
                if (r3 < -0x80) r3 = -0x80;
                if (r3 > 0x80) r3 = 0x80;
                mAngleYSpeed += r3;
                if (mAngleYSpeed > 0x1000) mAngleYSpeed = 0x1000;
                if (mAngleYSpeed < -0x1000) mAngleYSpeed = -0x1000;
            }
        } else {
            mStateArg = 0;
            ApproachLinear2(this->mAngleYSpeed, 0, 0x40);
        }
        {
            s16 before = mAngleY;
            mAngleY += mAngleYSpeed;
            if ((mAngleYSpeed <= -0x100 && before < mAngleY) ||
                (mAngleYSpeed >= 0x100 && before > mAngleY))
                func_02012694(0xb4, &mCamSpacePos);
            s16 sp = mAngleYSpeed;
            if (sp >= 0)
                mAngleX = -sp;
            else
                mAngleX = sp;
        }
        break;
    case 2:
        if (this->FinishedAnim())
            this->ChangeState(daPly_c::ST_WAIT);
        break;
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_GrabBowserTail_Init() {
    extern int *gCamera;
    this->SetAnim(0x81, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    mAngleYSpeed = 0;
    mStateWork = 0;
    mStateArg = 0;
    ((::dCamera_c *)gCamera)->SetGrabBowserTailState(mPlayerNo);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::DropActor() {

    u32 *s = (u32 *)mHeldObj;
    int present = (s != 0);
    if (present) {
        int held = ((((dActor_c *)s)->mFlags & 0x200) != 0);
        if (!held) {
            u32 *s_ptr1 = &((dActor_c *)s)->mFlags;
            *s_ptr1 &= ~0x4000u;

            u32 *s_ptr2 = (u32 *)mHeldObj;
            u32 *field_ptr2 = &((dActor_c *)s_ptr2)->mFlags;
            *field_ptr2 &= ~0x100u;

            u32 *s_ptr3 = (u32 *)mHeldObj;
            u32 *field_ptr3 = &((dActor_c *)s_ptr3)->mFlags;
            *field_ptr3 &= ~0x400u;

            u32 *s_ptr4 = (u32 *)mHeldObj;
            u32 *field_ptr4 = &((dActor_c *)s_ptr4)->mFlags;
            *field_ptr4 &= ~0x2000u;

            u32 *s_ptr5 = (u32 *)mHeldObj;
            u32 *field_ptr5 = &((dActor_c *)s_ptr5)->mFlags;
            *field_ptr5 &= ~0x800u;

            u32 *s_ptr6 = (u32 *)mHeldObj;
            u32 *field_ptr6 = &((dActor_c *)s_ptr6)->mFlags;
            *field_ptr6 &= ~0x1000u;

            mHeldObj = 0;

            if (this->IsState(data_ov002_02110364) ||
                this->IsState(data_ov002_02110394) ||
                this->IsState(data_ov002_02110604))
            {
                this->ChangeState(data_ov002_0211013c);
            }
        } else {
            this->ChangeState(data_ov002_021105d4);
        }
        return 1;
    }
    {
        int b = (mObjInMouth != 0);
        if (!b) {
            return 0;
        }
        this->ChangeState(data_ov002_02110034);
        return 1;
    }
}

int daPly_c::func_ov002_020da95c() {
    dActor_c *p = mHeldObj;
    int t = (p != 0);
    if (t == 0) return 0;
    p->mFlags &= ~0x800;
    mHeldObj->mFlags &= ~0x4000;
    mHeldObj->mFlags |= 0x1000;
    mHeldObj->mFlags &= ~0x100;
    mHeldObj = 0;
    return 1;
}

int daPly_c::DropHeldObj() {
    char *s = (char *)mHeldObj;
    int b = (s != 0);
    int* p;
    if (!b)
        return 0;
    {
        int e = (((dActor_c *)s)->actorID == 0xbf);
        if (e)
            func_ov002_020db54c(s, 0x10000, 0x10000, mAngleY);
    }
    p = (int *)&mHeldObj->mFlags;
    *p &= ~0x4000;
    p = (int *)&mHeldObj->mFlags;
    *p |= 0x2000;
    p = (int *)&mHeldObj->mFlags;
    *p &= ~0x100;
    mHeldObj = 0;
    return 1;
}

int daPly_c::func_ov002_020daa74() {
    char* self = (char*)this;
    char *s = (char *)mHeldObj;
    int b = (s != 0);
    if (!b)
        return 0;
    ((dActor_c *)s)->mFlags &= ~0x4000;
    mHeldObj->mFlags |= 0x400;
    mHeldObj->mFlags &= ~0x100;
    {
        char *s2 = (char *)mHeldObj;
        int e = (((dActor_c *)s2)->actorID == 0xbf);
        if (e) {
            int c_val = this->mAngleY;
            func_ov002_020db54c(s2, 0x30000, 0x10000, c_val);
        }
    }
    mHeldObj = 0;
    return 1;
}

int daPly_c::ReleaseHeldActor() {
    char *p = (char *)mHeldObj;
    int ok = (p != 0);
    int is_bf;
    if (ok == 0) return 0;
    is_bf = (int)(((dActor_c *)p)->actorID == 0xbf);
    if (is_bf != 0) {
        func_ov002_020db54c(p, 0x10000, 0x10000, mAngleY);
    }
    mHeldObj->mFlags &= ~0x4000;
    mHeldObj->mFlags &= ~0x100;
    mHeldObj = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Refuses if already holding something or if the target does not carry the
 * grabbable flag (0x80). Grabbing is allowed from the punch state past step 2,
 * from swimming, or -- for actor IDs other than 0xbd/0xbe -- from the crouch
 * state.
 */
/* punch */
/* swim */
/* crouch */

int daPly_c::TryGrab(dActor_c &actor_) {
    dActor_c *actor = &actor_;
    int b = (mHeldObj != 0);
    if (b) return 0;
    if ((actor->mFlags & 0x80) == 0) return 0;
    if ((IsState(data_ov002_02110574) && mPunchKickStep != 2) || IsState(data_ov002_0211067c)) {
        func_ov002_020dacb4(actor);
        return 1;
    }
    {
        unsigned short t = actor->actorID;
        int b0 = (t == 0xbd);
        if (!b0) {
            int b1 = (t == 0xbe);
            if (!b1) {
                if (IsState(data_ov002_021105bc)) {
                    func_ov002_020dacb4(actor);
                    return 1;
                }
            }
        }
    }
    return 0;
    }

int daPly_c::func_ov002_020dacb4(dActor_c *actor) {
    int t;

    mHeldObj = actor;
    mHeldObj->mFlags &= ~0x400;
    mHeldObj->mFlags &= ~0x2000;
    mHeldObj->mFlags |= 0x100;
    mHorzSpeed = 0;

    if (mIsUnderwater != 0) {
        func_ov002_020cd28c(this);
    } else {
        t = 0;
        if (mHeldObj->actorID == 0x116)
            t = 1;
        if (t != 0) {
            mStateStep = 0;
            ChangeState(data_ov002_021105ec);
            return 1;
        }
        if (!IsState(data_ov002_021105bc)) {
            unsigned short fc;
            int u;
            mStateStep = 0;
            fc = mHeldObj->actorID;
            t = 0;
            if (fc == 0xbd) t = 1;
            // structured form grows this function 0x1b8 -> 0x1c0
            if (t != 0) goto chg394;
            u = fc;
            u = u == 0xbe;
            if (u == false) goto chg364;
        chg394:
            ChangeState(data_ov002_02110394);
            goto after;
        chg364:
            ChangeState(data_ov002_02110364);
        after: ;
        }
    }

    {
        char *a2 = (char *)mHeldObj;
        int offset = 0xc4;
        int s = ((dActor_c *)a2)->mFlags;
        s = (s & 0x8000) != false;
        if (s != false) {
            offset = 0x224;
            if (param1 == 2) {
                int c = ((dActor_c *)a2)->actorID;
                c = c == 0xce;
                if (c != false) offset = 0xc4;
            }
        }
        {
            int player_var = param1;
            int model_idx = GetBodyModelID(player_var & 0xff, 0);
            dExtAnm2Model_c *model = mModels.mBodyModels[model_idx];
            BCA_File *j = (BCA_File *)data_ov002_020ff480[offset + player_var]->filePtr;
            model->Func_020162C4(j, 0, 0x1000, 0);
        }
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
int daPly_c::St_Throw_Main() {
    if (mIsAirborne == 0) {
        ApproachLinear(mHorzSpeed, 0, 0x1000);
    }
    if ((char *)mHeldObj) {
        int r4 = 5;
        int id = this->GetBodyModelID(param1 & 0xff, 0);
        dExtAnmModel_c *anim = mModels.mBodyModels[id];
        if (anim->WillHitFrame(0)) {
            if (func_ov002_020c19d0(0x40, 0x32))
                r4 = 0;
        }
        char *q = (char *)mHeldObj;
        unsigned short a = ((dActor_c *)q)->actorID;
        int t1 = (a == 0xbd);
        // folding the two id tests shrinks this function 0x168 -> 0x164
        if (!t1) {
            int t2 = (a == 0xbe);
            if (!t2)
                goto Lb8;
        }
        r4 = 7;
        goto Ld0;
    Lb8: {
        int b = (((dActor_c *)q)->mFlags & 0x200) ? 1 : 0;
        if (b)
            r4 = 0xd;
    }
    Ld0:
        int id2 = this->GetBodyModelID(param1 & 0xff, 0);
        dExtAnmModel_c *anim2 = mModels.mBodyModels[id2];
        if (anim2->WillHitFrame(r4)) {
            int b2 = (mHeldObj->mFlags & 0x200) ? 1 : 0;
            if (b2 == 0)
                func_ov002_020daa74();
            else
                DropHeldObj();
        }
    }
    if (this->FinishedAnim())
        this->ChangeState(data_ov002_0211013c);
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * mHeldObj is an actor pointer: this reads fBase_c's mFlags (+0xb0) and
 * actorID (+0xc) through it. dActor_c IDs 0xbd
 * and 0xbe get the alternate throw animation.
 */
int daPly_c::St_Throw_Init() {
    dActor_c *held = mHeldObj;
    int b = (held->mFlags & 0x200) ? 1 : 0;
    if (!b) {
        unsigned short a = held->actorID;
        int t1 = (a == 0xbd);
        // folding the two id tests differs at +0x3c
        if (!t1) {
            int t2 = (a == 0xbe);
            if (!t2)
                goto anim30;
        }
        this->SetAnim(0x91, 0x40000000, 0x1000, 0);
        goto voice;
    anim30:
        this->SetAnim(0x30, 0x40000000, 0x1000, 0);
    voice:
        Sound::PlayCharVoice(mCharacter, 0x13, mCamSpacePos);
    } else {
        this->SetAnim(0x8a, 0x40000000, 0x1000, 0);
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method */
// daPly_c::St_Thrown_Cleanup - clear flag bit 0x2000 in the u32 at this+0x2ec, return 1.
int daPly_c::St_Thrown_Cleanup() {
    mdCcAcPos_c.flags &= ~0x2000;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Thrown_Main() {
    if (mStateTimer == 1) {
        mdCcAcPos_c.flags |= 0x2000;
    }
    func_ov002_020bf90c();

    u8 state = mStateStep;
    switch (state) {
    case 0:
        if (mIsAirborne == 0) {
            mAngleX = 0;
            mdCcAcPos_c.flags &= ~0x2000;
            func_ov002_020c06fc(0x8000);
            if (mStateArg == 0) {
                mStateArg = 1;
                Sound::PlayCharVoice(mCharacter, 6, mCamSpacePos);
            }
            int r5 = func_ov002_020e2c84();
            if (r5 != 2 && mIsInShallowWater == 0) {
                func_ov002_020bf9d4();
            }
            if (r5 != 0) {
                return 1;
            }
            if (mFloorNormalY == 0x1000) {
                if (mHorzSpeed == 0) {
                    mStateStep = 1;
                    break;
                }
            } else {
                this->ChangeState(data_ov002_0211031c);
                return 1;
            }
        }
        mAngleY = mPrevAngleY;
        mHorzSpeed = FX_MUL(mHorzSpeed, 0xfae);
        if (mIsAirborne != 0) {
            int a = cstd::atan2(mHorzSpeed, mVertSpeed);
            s16 b = a - 0x4000;
            if (b > 0x3000) b = 0x3000;
            mAngleX = b;
        } else {
            ApproachLinear2(mAngleX, 0, 0x400);
        }
        break;
    case 1:
        if (this->GetHealth() == 0) {
            mStateStep = 1;
            this->ChangeState(data_ov002_0211010c);
            return 1;
        }
        if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            mPrevAngleY = mAngleY;
        }
        AdvanceAnims();
        break;
    }

    if (mIsAirborne == 0) {
        func_ov002_020e2c84();
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Thrown_Init() {
    int b = (mFlags & 0x400) ? 1 : 0;
    if (b)
        mStateTimer = 4;
    Sound::PlayCharVoice(mCharacter, 0x1d, mCamSpacePos);
    this->SetAnim(0x15, 0x40000000, 0x1000, 0x12);
    mStateStep = 0;
    mStateWork = 0;
    mAngleY = mPrevAngleY;
    mStateArg = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Clears bit 1 of the body-collision flags, then if something is holding the
 * player at +0x35c and its actorID is 0xbf, tells it to drop and clears the
 * slot. Note the callee is daPly_c::DropActor() --
 * invoked with THAT actor as its this, not the player's: whatever occupies
 * 0x35c is itself a daPly_c. Kept as an extern "C" call rather than a method
 * call so the source does not assert a type this function cannot evidence.
 *
 * 0x35c is mGrabbedByActor, NOT mHeldObj at 0x358 -- those are different slots; the
 * object the player holds and the actor holding the player.
 */
int daPly_c::St_Grabbed_Cleanup() {
    daPly_c *a;
    *&mdCcAcPos_c.flags &= ~2;
    a = (daPly_c *)mGrabbedByActor;
    if (a) {
        int isBob = (a)->actorID == 0xbf;
        if (isBob) {
            a->DropActor();
            mGrabbedByActor = 0;
        }
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Grabbed_Main() {
    char* grab;
    u8* p;
    unsigned int id;
    int ret;
    int *pRate;
    char* model;

    mPeakY = mPosY;
    grab = (char *)mGrabbedByActor;
    if (grab != 0) {
        int isBob = (((dActor_c *)grab)->actorID == 0xbf);
        if (isBob != 0) {
            ret = func_ov002_020beb38();
            if (ret != 0) {
                p = (u8*)(int)((char*)&mStateStep);
                *p = (u8)(*p + ret);
                mStateTimer = 6;
                if ((s16)(mStateStep - ret) < 0) {
                    ((daPly_c *)mGrabbedByActor)->DropActor();
                    func_ov002_020db54c(this, 0x10000, 0x10000, mAngleY);
                    return 1;
                }
            }
        }
    }

    if (mStateTimer != 0) {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        model = (char *)mModels.mBodyModels[id];
        pRate = (int *)&(dExtFrameCtrl_c &)*(dExtAnmModel_c *)model;
        pRate = (int *)&((dExtFrameCtrl_c *)pRate)->speed;
        *pRate = 0x4000;
    } else {
        id = this->GetBodyModelID(param1 & 0xff, 0);
        model = (char *)mModels.mBodyModels[id];
        pRate = (int *)&(dExtFrameCtrl_c &)*(dExtAnmModel_c *)model;
        pRate = (int *)&((dExtFrameCtrl_c *)pRate)->speed;
        *pRate = 0x1000;
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Grabbed_Init() {
    DropHeldObj();
    mdCcAcPos_c.flags |= 2;
    mIsBodyClsnEnabled = 0;
    mSkipGroundUpdate = 1;
    Sound::PlayCharVoice(mCharacter, 6, mCamSpacePos);
    this->SetAnim(0x71, 0, 0x1000, 0);
    mHideShadow = 1;
    mStateStep = 0;
    return 1;
}

/* recovered: shared common types */

extern "C" void func_ov002_020db54c(void *self, int a, int b, short c) {
    daPly_c *p = (daPly_c *)self;
    void *node;
    p->mPrevAngleY = c;
    p->mHorzSpeed = a;
    p->mVertSpeed = b;
    node = p->mGrabbedByActor;
    if (node != 0) {
        int bb = (int)(((dActor_c *)node)->actorID == 0xbf);
        if (bb != 0) {
            Vector3 *src = (Vector3 *)((int)&((dActor_c *)node)->mPos);
            Vector3 v;
            int ty;
            v.x = src->x;
            ty = src->y;
            v.y = ty;
            v.z = src->z;
            v.y = ty + 0x64000;
            func_02035860(&p->mMeshClsn, &v);
            p->mPrevPosX = v.x;
            p->mPrevPosY = v.y;
            p->mPrevPosZ = v.z;
        }
    }
    p->mGrabbedByActor = 0;
    p->ChangeState(data_ov002_021101cc);
}

extern "C" {

int func_ov002_020db5f4(daPly_c *c, dActor_c *arg) {
    daPly_c *player = c;
    int b = ((arg)->actorID == 0xbf);
    if (b == 0) {
        if (c->func_ov002_020d82f0() == 0)
            return 0;
    }
    if (player->mGrabbedByActor != 0)
        return 0;
    player->mGrabbedByActor = arg;
    (c)->ChangeState(data_ov002_0211016c);
    return 1;
}
}

extern "C" {

int func_ov002_020db674(char* c, int a1, int a2, int a3)
{
    daPly_c *p = (daPly_c *)c;
    if (p->func_ov002_020d82f0() == 0)
        return 0;
    Sound::PlayCharVoice(p->mCharacter, 0x1d, p->mCamSpacePos);
    p->mPrevAngleY = a3;
    p->mAngleY = a3 + 0x8000;
    p->mHorzSpeed = a1;
    p->mVertSpeed = a2;
    p->mHurtDamage = 0;
    p->mStateStep = 4;
    p->mIsAirborne = 1;
    p->mLandSoundPlayed = 0;
    ((daPly_c *)c)->ChangeState(data_ov002_02110094);
    return 1;
}
}

void daPly_c::UpdateScaleAnim() {
    int s;
    u8 i;
    int j;

    s = mScaleAnimStep;
    if (s == 0)
        return;

    i = 0;
    j = 0;
    switch (mScaleAnimKind) {
    case 1:
        i = 2;
        j = 6;
    case 0:
        if (mScaleAnimTimer == 0) {
            if (s == 1)
                i++;
            if (ApproachLinear(mScaleY, data_ov002_020ff2c0[s - 1 + j], data_ov002_020ff230[i])) {
                if (mScaleAnimStep == 1 || mScaleAnimStep == 6) {
                    mScaleAnimTimer = 10;
                } else {
                    mScaleAnimStep++;
                }
            }
        } else if (mScaleAnimTimer == 1) {
            if (s == 1) {
                mScaleAnimStep++;
            } else {
                mScaleAnimStep = 0;
                mScaleAnimTimer = 0;
                if (mIsMega) {
                    mScaleX = 0x3000;
                    mScaleY = 0x3000;
                    mScaleZ = 0x3000;
                    return;
                }
            }
        }
        mScaleZ = (0x1000 - mScaleY) / 2 + 0x1000;
        mScaleX = mScaleZ;
        return;
    case 2:
        if (ApproachLinear(mScaleY, 0x3000, 0x200)) {
            mScaleAnimStep = 0;
        }
        mScaleZ = mScaleY;
        mScaleX = mScaleZ;
        return;
    case 3:
        if (ApproachLinear(mScaleY, 0x1000, 0x200)) {
            mScaleAnimStep = 0;
        }
        mScaleZ = mScaleY;
        mScaleX = mScaleZ;
        return;
    }
}

void daPly_c::func_ov002_020db8bc(unsigned char kind) {
    mScaleAnimStep = 1;
    mScaleAnimTimer = 0;
    mScaleAnimKind = kind;
}

/* recovered: shared common types */

void daPly_c::func_ov002_020db8d8() {
    extern unsigned char gGameMode;
    Vector3 backup;
    Vector3 tmp;
    Vector3 dir;
    Vector3 scaled;
    int model;
    Matrix4x3 *m;
    int b;
    int cond;
    backup = mPos;
    b = this->mUseAltBodyModel;
    m = &mModels.mHeadModels[(b << 2) + 3]->data.transforms[3];
    model = this->GetBodyModelID(param1 & 0xff, 0);
    MTX_MultVec43((Vector3 *)&m->m[9], &mModels.mBodyModels[model]->mat4x3, &tmp);
    Vec3_MulScalar(&scaled, &tmp, 0x8000);
    mPos = scaled;
    dir.x = 0;
    dir.z = 0;
    dir.y = -0x32000;
    cond = (gGameMode == 1);
    if (cond != 0) {
        func_ov002_020dc174(&dir, 0x46000, 0x96000, 0x8000, 0);
    } else {
        func_ov002_020dc174(&dir, 0x32000, 0x64000, 0x8000, 0);
    }
    mPos = backup;
}

void daPly_c::func_ov002_020dba0c() {
    extern unsigned char gGameMode;
    Vector3 v;
    int r2, r3;
    unsigned int a5;
    if ((int)(gGameMode == 1) != 0) {
        v.z = 0xa0000;
        v.x = 0;
        r2 = 0x78000;
        v.y = 0;
        v.z = 0x78000;
        r3 = 0x50000;
    } else if (mIsMega == 0) {
        r2 = 0x78000;
        v.z = 0x78000;
        r3 = 0x50000;
        v.x = 0;
        v.y = 0;
        r2 = r3;
        if (param1 == 2) {
            v.z = 0xa0000;
            r2 = 0x78000;
        }
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0x80000;
        r3 = 0x100000;
        r2 = 0xc0000;
    }
    a5 = 0x1400;
    if (param1 == 3)
        a5 = 0x400;
    func_ov002_020dc174(&v, r2, r3, a5, 0);
}

void daPly_c::func_ov002_020dbaec() {
    extern unsigned char gGameMode;
    Vector3 v;
    int r2, r3;
    if ((int)(gGameMode == 1) != 0) {
        v.z = 0xa0000;
        v.x = 0;
        r2 = 0x78000;
        v.y = 0;
        v.z = 0x78000;
        r3 = 0x50000;
    } else if (mIsMega == 0) {
        r2 = 0x78000;
        v.z = 0x78000;
        r3 = 0x50000;
        v.x = 0;
        v.y = 0;
        r2 = r3;
        if (param1 == 2) {
            v.z = 0xa0000;
            r2 = 0x78000;
        }
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0x80000;
        r3 = 0x100000;
        r2 = 0xc0000;
    }
    func_ov002_020dc174(&v, r2, r3, 0x200, 0);
}

void daPly_c::func_ov002_020dbbc0() {
    extern unsigned char gGameMode;
    Vector3 v;
    int r2, r3;
    if ((int)(gGameMode == 1) != 0) {
        v.z = 0xa0000;
        v.x = 0;
        r2 = 0x78000;
        v.y = 0;
        v.z = 0x78000;
        r3 = 0x50000;
    } else if (mIsMega == 0) {
        r2 = 0x78000;
        v.z = 0x78000;
        r3 = 0x50000;
        v.x = 0;
        v.y = 0;
        r2 = r3;
        if (param1 == 2) {
            v.z = 0xa0000;
            r2 = 0x78000;
        }
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0x80000;
        r3 = 0x100000;
        r2 = 0xc0000;
    }
    func_ov002_020dc174(&v, r2, r3, 0x100, 0);
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
void daPly_c::func_ov002_020dbc94() {
    extern unsigned char gGameMode;
    Vector3 v;
    int b = (gGameMode == 1);
    if (b) {
        int r2 = 0x82000;
        v.x = 0;
        v.y = -0x32000;
        v.z = 0;
        if (param1 == 2)
            r2 = 0xc8000;
        func_ov002_020dc174(&v, r2, 0x5a000, 0x20, 0);
    } else if (mIsMega == 0) {
        v.x = 0;
        v.y = -0x32000;
        v.z = 0;
        func_ov002_020dc174(&v, 0x32000, 0x5a000, 0x20, 0);
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0;
        func_ov002_020dc174(&v, 0xc0000, 0x80000, 0x20, 0);
    }
}

void daPly_c::func_ov002_020dbd94() {
    extern unsigned char gGameMode;
    Vector3 v;
    int r2, r3;
    if ((int)(gGameMode == 1) != 0) {
        v.z = 0x8c000;
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x64000;
        r3 = 0xbe000;
        r2 = 0x78000;
    } else if (mIsMega == 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x64000;
        r2 = 0x50000;
        r3 = 0xbe000;
        if (param1 == 2) {
            v.z = 0x8c000;
            r2 = 0x78000;
        }
    } else {
        v.x = 0;
        v.y = 0x40000;
        v.z = 0xd0000;
        r3 = 0x200000;
        r2 = 0x90000;
    }
    func_ov002_020dc174(&v, r2, r3, 0x80, 0xc0);
}

void daPly_c::func_ov002_020dbe70() {
    extern unsigned char gGameMode;
    Vector3 v;
    int r2, r3;
    if ((int)(gGameMode == 1) != 0) {
        v.z = 0x78000;
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x5a000;
        r3 = 0x82000;
        r2 = 0x64000;
    } else if (mIsMega == 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x5a000;
        r2 = 0x46000;
        r3 = 0x82000;
        if (param1 == 2) {
            v.z = 0x78000;
            r2 = 0x64000;
        }
    } else {
        v.x = 0;
        v.y = 0x40000;
        v.z = 0xe0000;
        r3 = 0x140000;
        r2 = 0xa0000;
    }
    func_ov002_020dc174(&v, r2, r3, 0x40, 0xc0);
}

void daPly_c::func_ov002_020dbf4c() {
    extern unsigned char gGameMode;
    Vector3 v;
    int r2, r3;
    if ((int)(gGameMode == 1) != 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x50000;
        r3 = 0x6e000;
        r2 = 0x5a000;
    } else if (mIsMega == 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x50000;
        r2 = 0x3c000;
        r3 = 0x46000;
        if (param1 == 2) {
            v.z = 0x6e000;
            r2 = 0x5a000;
        }
    } else {
        v.x = 0;
        v.y = 0x40000;
        v.z = 0xc0000;
        r3 = 0x100000;
        r2 = 0x80000;
    }
    func_ov002_020dc174(&v, r2, r3, 0x40, 0xc0);
}

void daPly_c::func_ov002_020dc020() {
    Vector3 v;
    int r2, r3;
    if (mIsMega == 0) {
        v.x = 0;
        v.y = 0xa000;
        v.z = 0x3c000;
        r3 = 0x46000;
        r2 = 0x28000;
    } else {
        v.x = 0;
        v.y = 0x40000;
        v.z = 0xc0000;
        r3 = 0x100000;
        r2 = 0x80000;
    }
    func_ov002_020dc174(&v, r2, r3, 0x1000, 0);
}

/* func_ov002_020dc09c at 0x020dc09c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
extern "C" {

void func_ov002_020dc09c(char *c) {
    daPly_c *player = (daPly_c *)c;
    Vector3 v;
    int r2, r3;
    if (player->mIsMega == 0) {
        if (player->mStateTimer & 1) {
            r3 = 0x28000;
            v.x = 0;
            v.y = 0x28000;
            v.z = 0x87000;
            r2 = 0x3c000;
        } else {
            r3 = 0x50000;
            v.x = 0;
            v.y = 0x28000;
            v.z = 0x10e000;
            r2 = 0x50000;
        }
    } else {
        r2 = 0x80000;
        if (player->mStateTimer & 1) {
            v.x = 0;
            v.y = 0x80000;
            v.z = 0xc0000;
            r3 = 0x80000;
        } else {
            v.x = 0;
            v.y = 0x80000;
            v.z = 0x1c0000;
            r3 = 0x100000;
        }
    }
    player->func_ov002_020dc174(&v, r2, r3, 0x40000, 0);
}
}

/* The attack collision's offset, reach and height. pos is a const Vector3 *: it
   goes straight to dCcAcPos_c::Init, whose ROM symbol mangles that
   parameter as RK7Vector3, a const Vector3 &. Eleven callers in this file each used to spell it void *. */
void daPly_c::func_ov002_020dc174(const Vector3 *pos, Fix12i reach, Fix12i height, u32 flags,
                                  u32 a6) {
    int f = mIsMega ? 0x10 : 0;
    mAttackClsn.Init(this, *pos, reach, height, flags | 2 | f, a6);
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_SlideKick_Main() {
    extern u16 data_0209f49e[];
    func_ov002_020bf90c();
    if (mIsAirborne == 0) {
        mIsSlidingOnGround = 1;
        func_ov002_020c06fc(0x4000);
        func_ov002_020dd2f4();
    }

    if (mHorzSpeed != 0)
        goto La8;

    if (this->FinishedAnim() == 0)
        goto L1d8;

    if (this->IsAnim(0x67) == 0)
        goto L88;

    this->SetAnim(0x66, 0x40000000, 0x1000, 0);
    goto L1d8;

L88:
    mPrevAngleY = mAngleY;
    mStateStep = 1;
    this->ChangeState(data_ov002_021104e4);
    goto L1d8;

La8:
    if (func_ov002_020c0688() == 0)
        goto L14c;

    if (mStateWork == 0) {
        if (mPrevVertSpeed < 0) {
            u16 bit = mStateFlags & 1;
            if (bit == 0) {
                int t = -mPrevVertSpeed;
                if (t > 0x14000)
                    t = 0x14000;
                mVertSpeed = t;
                mStateWork = 1;
            }
        }
    }

    {
        u16 r1 = data_0209f498[gActivePlayerSlot].pressed;
        if ((r1 & 1) || (r1 & 2)) {
            if (mSlideType == 0) {
                this->ChangeState(data_ov002_021101fc);
            }
        }
    }
    mStateStep = 0;
    goto L18c;

L14c:
    mStateStep++;
    if (mStateStep <= 0x1e)
        goto L18c;
    if (mPosY - mGroundY <= 0x64000)
        goto L18c;
    this->ChangeState(data_ov002_021101b4);

L18c: {
    dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
    u32 w = anim.currFrame;
    if ((u16)(w >> 12) < 4)
        goto L1d8;
    func_ov002_020dbaec();
    mAttackClsn.Clear();
    mAttackClsn.Update();
}

L1d8:
    AdvanceAnims();
    mPrevVertSpeed = mVertSpeed;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_SlideKick_Init() {
    extern int data_ov002_0210e160;
    Sound::PlayBank0(0x11, mCamSpacePos);
    unsigned int r = (unsigned int)RandomIntInternal(&data_ov002_0210e160);
    Sound::PlayCharVoice(mCharacter, data_ov002_020ff130[(r >> 4) & 1], mCamSpacePos);
    this->SetAnim(0x67, 0x40000000, 0x1000, 0);
    int *yv = &mHorzSpeed;
    mVertSpeed = 0x14000;
    yv[0] += 0xf000;
    if (mHorzSpeed > 0x28000) mHorzSpeed = 0x28000;
    mStateWork = 0;
    mIsSlidingOnGround = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_SweepKick_Main() {
    extern unsigned short data_0209f49e[];
    if (data_0209f498[gActivePlayerSlot].pressed & 2)
        this->ChangeState(data_ov002_0211019c);
    func_ov002_020d8a50(3);
    if (this->FinishedAnim()) {
        mStateStep = 1;
        this->ChangeState(data_ov002_021104e4);
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_SweepKick_Init() {
    this->SetAnim(0x68, 0x40000000, 0x1000, 0);
    mHorzSpeed = 0;
    mAttackClashed = 0;
    /* mCamSpacePosX/Y/Z are three consecutive words; dActor_c.h declares them
       individually rather than as one Vector3. */
    Sound::PlayCharVoice(mCharacter, 0xa, mCamSpacePos);
    return 1;
}

#define ADDR(p) ((int *)(p))

void daPly_c::func_ov002_020dc560() {
    extern char data_0209f4a0[];
    ApproachLinear(mHorzSpeed, 0, 0x599);

    {
        int tblval = data_0209f498[gActivePlayerSlot].mag;
        if (tblval != 0) {
            short diff = (short)(mDesiredAngleY - mPrevAngleY);
            int idx2 = ((unsigned short)diff >> 4) * 2;
            int r1 = func_01ffa344(0x48400000, (unsigned short)diff >> 4);
            int sinv = FX_SinCosTable_[idx2 + 1];
            int scaled = FX_MUL(sinv, 0x1800);
            int combined = FX_MUL(scaled, tblval);
            ApproachLinear(mHorzSpeed, r1, combined);

            {
                short *p94 = (short *)ADDR(&mPrevAngleY);
                int cosv = FX_SinCosTable_[idx2];
                int tblval2 = data_0209f498[gActivePlayerSlot].mag;
                int scaled2 = FX_MUL(cosv, 0x200);
                int combined2 = FX_MUL(scaled2, tblval2);
                *p94 = (short)(*p94 + combined2);
            }
        }
    }

    {
        short cur = mPrevAngleY;
        int ad = AngleDiff(cur, mAngleY);
        short target = cur;
        if (ad >= 0x4000)
            target = (short)(mPrevAngleY + 0x8000);
        ApproachAngle(&mAngleY, target, 8, 0x4000, 0x10);
    }

    if (func_01ff99a4(func_01ffa4bc(mHorzSpeed), 0x48400000) != 0) {
        *ADDR(&mHorzSpeed) -= 0x1000;
    }

    if (func_01ff98f4(func_01ffa4bc(mHorzSpeed), -0x3e800000) != 0) {
        *ADDR(&mHorzSpeed) += 0x2000;
    }
}

#undef ADDR

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_ButtSlide_Main() {
    extern u16 data_0209f49e[];
    switch (mStatePhase) {
    case 0:
        func_ov002_020bf90c();
        if (mIsAirborne == 0) {
            mIsSlidingOnGround = 1;
            func_ov002_020c06fc(0x4000);
            func_ov002_020dd2f4();
        }
        if (mHorzSpeed == 0) {
            if (mSlideType == 0) {
                mStatePhase = 2;
                mStateWork = 0;
                break;
            }
            (this->mSlideStoppedTimer)++;
            if (mSlideStoppedTimer >= 0x1e) {
                mStatePhase = 2;
                mStateWork = 0;
                break;
            }
        } else {
            mSlideStoppedTimer = 0;
        }
        if (func_ov002_020c0688() != 0) {
            if ((data_0209f498[gActivePlayerSlot].pressed & 2) != 0 && mStateWaitTimer == 0) {
                mJumpComboStage = 0;
                this->ChangeState(data_ov002_0211019c);
                return 1;
            }
            mLoopingSoundHandle = func_0201226c(mLoopingSoundHandle, 0, mGroundSoundType + 0xe2,
                                                &mCamSpacePos, mHorzSpeed, 0);
        } else {
            mStateStep = 0;
            mStatePhase = 1;
            mStateWork = 0;
            if ((mStateArg | mSlideType) != 0)
                mVertSpeed = 0x15000;
            func_ov002_020e25f0(0);
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            break;
        }
        func_ov002_020c18b0(0);
        func_ov002_020dcafc();
        break;
    case 1:
        mAngleZ = 0;
        mAngleX = mAngleZ;
        if (mIsAirborne == 0) {
            if (mLandSoundPlayed == 0) {
                mLandSoundPlayed = 1;
                Sound::PlayBank0(mGroundSoundType + 0x50, mCamSpacePos);
            }
            if (mStateWork == 0 && mFloorNormalY >= 0xfc1) {
                mVertSpeed = -mPrevVertSpeed / 2;
                (this->mStateWork)++;
            } else {
                mStatePhase = 0;
                mVertSpeed = 0;
                break;
            }
        } else {
            func_ov002_020dc560();
        }
        (this->mStateStep)++;
        if (mStateStep > 0x1e
            && mPosY - mGroundY > 0x1f4000) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
        break;
    case 2:
        mAngleZ = 0;
        mAngleX = mAngleZ;
        if (mStateWork == 0) {
            this->SetAnim(0x42, 0x40000000, 0x1000, 0);
            mPrevAngleY = mAngleY;
            mStateWork = 1;
        } else if (this->FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    }

    mPrevVertSpeed = mVertSpeed;
    AdvanceAnims();
    mStateArg = mSlideType;
    return 1;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_ButtSlide_Init() {
    struct dCamera_c;
    extern struct dCamera_c *gCamera;
    this->SetAnim(0x41, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    if (mSlideType) {
        ((::dCamera_c *)gCamera)->func_0200d544(mPlayerNo);
    }
    mStatePhase = 0;
    mStateArg = mSlideType;
    mStateWaitTimer = 6;
    mIsSlidingOnGround = 0;
    return 1;
}

/* func_ov002_020dcafc at 0x020dcafc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */

void daPly_c::func_ov002_020dcafc() {
    s16 v5 = 0, v4 = 0;
    int idx;
    s16 a, b;

    if (mHorzSpeed != 0) {
        idx = gActivePlayerSlot * 0x18;
        a = *(short*)((char*)data_0209f4a2 + idx);
        b = *(short*)((char*)data_0209f4a4 + idx);
        v5 = (s16)FX_MUL(a, 0xaab);
        v4 = (s16)FX_MUL(-b, 0x1555);
    }

    if (AngleDiff(mAngleY, mPrevAngleY) >= 0x4000) {
        v5 = (s16)(-v5);
        v4 = (s16)(-v4);
    }

    ApproachAngle(&mSpineRot[1], v5, 8, 0x200, 0x20);
    ApproachAngle(&mSpineRot[2], v4, 8, 0x200, 0x20);
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_StomachSlide_Main() {
    extern u16 data_0209f49e[];
    extern u8 data_0209ee90;
    dActor_c *light0 = mHeldObj;
    if (light0 != 0) {
        u32 *p = &light0->mFlags;
        *p |= 0x4000;
    }

    switch (mStatePhase) {
    case 0:
        func_ov002_020bf90c();
        if (mIsAirborne == 0) {
            mIsSlidingOnGround = 1;
            if (this->IsState(data_ov002_021105bc) && mHeldObj != 0) {
                mHorzSpeed = 0;
            } else {
                {
                    u32 arg = 0x4000;
                    if (mSlideType == 0)
                        arg = 0x8000;
                    func_ov002_020c06fc(arg);
                }
                func_ov002_020dd2f4();
                if (this->IsState(data_ov002_021105bc) && (u16)(mStateFlags & 1)) {
                    func_ov002_020c0364(2);
                }
            }
        }
        if (mHorzSpeed == 0) {
            if (mSlideType == 0) {
                mStateStep = 0;
                mStatePhase = 2;
                break;
            }
            {
                {
                    u8 *p = (u8 *)((char *)&mSlideStoppedTimer);
                    *p = (u8)(*p + 1);
                }
            }
            if (mSlideStoppedTimer >= 0x1e) {
                mStateStep = 0;
                mStatePhase = 2;
                break;
            }
        } else {
            mSlideStoppedTimer = 0;
        }

        if (func_ov002_020c0688()) {
            if (TryEnterBigFallLanding()) {
                return 1;
            }
            if (this->IsAnim(0x40) && this->FinishedAnim()) {
                this->SetAnim(0x43, 0x40000000, 0x1000, 0);
            }
            {
                u16 flags = data_0209f498[gActivePlayerSlot].pressed;
                if ((flags & 1) || (flags & 2)) {
                    if (!(u16)(mStateFlags & 1)) {
                        this->ChangeState(data_ov002_021101fc);
                    }
                }
            }
            func_ov002_020c18b0(0);
        } else {
            if (this->IsState(data_ov002_0211031c)) {
                mStatePhase = 1;
                mStateStep = 0;
                if (*(int*)((char*)&data_0209ee90 + 0x244) != 0) {
                    if ((mStateArg | mSlideType) != 0) {
                        mVertSpeed = 0x15000;
                    }
                }
                func_ov002_020e25f0(0);
                this->SetAnim(0x54, 0x40000000, 0x1000, 0);
            }
        }
        func_ov002_020dba0c();
        mAttackClsn.Clear();
        mAttackClsn.Update();
        break;

    case 1:
        func_ov002_020dba0c();
        mAttackClsn.Clear();
        mAttackClsn.Update();
        if (mIsAirborne == 0) {
            mStatePhase = 0;
            mVertSpeed = 0;
            mStateStep = 0;
            this->SetAnim(0x43, 0x40000000, 0x1000, 0);
            break;
        }
        func_ov002_020dc560();
        {
            u8* p = (u8*)((char*)&mStateStep);
            *p = (u8)(*p + 1);
        }
        if (mStateStep <= 0x1e)
            break;
        if (mPosY - mGroundY <= 0x1f4000)
            break;
        this->ChangeState(data_ov002_021101b4);
        return 1;

    case 2:
        mAngleZ = 0;
        mAngleX = mAngleZ;
        if (mStateStep == 0) {
            dActor_c *light = mHeldObj;
            int haveLight = (light != 0);
            if (haveLight) {
                u32 animId = 0x18;
                int b1 = (light->mFlags & 0x8000) != 0;
                if (b1) {
                    animId = 0x8b;
                    if (param1 == 2) {
                        int b2 = (light->actorID == 0xce);
                        if (b2) {
                            animId = 0x18;
                        }
                    }
                }
                this->SetAnim(animId, 0x40000000, 0x1000, 0);
            } else {
                this->SetAnim(0x3e, 0x40000000, 0x1000, 0);
            }
            mPrevAngleY = mAngleY;
            {
                u8* p = (u8*)((char*)&mStateStep);
                *p = (u8)(*p + 1);
            }
            break;
        }
        if (this->FinishedAnim()) {
            if (func_ov002_020e0ccc(mHeldObj)) {
                return 1;
            }
            this->ChangeState(data_ov002_0211013c);
        }
        break;
    }

    if (mIsAirborne != 0) {
        s16 v = cstd::atan2(mHorzSpeed >> 8, mVertSpeed >> 8) - 0x4000;
        if (v < 0) v = 0;
        if (v >= 0x2aaa) v = 0x2aaa;
        ApproachLinear2(mAngleX, v, 0x200);
    }
    AdvanceAnims();
    mStateArg = mSlideType;
    return 1;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_StomachSlide_Init() {
    struct dCamera_c;
    extern struct dCamera_c *gCamera;
    this->SetAnim(0x40, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    if (mSlideType) {
        ((::dCamera_c *)gCamera)->func_0200d544(mPlayerNo);
    }
    mStateWork = 0;
    mStatePhase = 0;
    mSlideStoppedTimer = 0;
    mStateArg = mSlideType;
    mIsSlidingOnGround = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Dive_Init() {
    extern int data_ov002_0210e160;
    Sound::PlayBank0(0x11, mCamSpacePos);
    unsigned int r = (unsigned int)RandomIntInternal(&data_ov002_0210e160);
    Sound::PlayCharVoice(mCharacter, data_ov002_020ff100[(r >> 4) & 1], mCamSpacePos);
    this->SetAnim(0x40, 0x40000000, 0x1000, 0);
    mStateStep = 0;
    int *yv = &mHorzSpeed;
    mVertSpeed = 0x1e000;
    yv[0] += 0xf000;
    if (mHorzSpeed > 0x28000) mHorzSpeed = 0x28000;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    mStateWork = 0;
    mStatePhase = 0;
    mSlideStoppedTimer = 0;
    mIsSlidingOnGround = 0;
    mStateArg = mSlideType;
    return 1;
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_021105bc;
}

int daPly_c::IsDiving() {

    return this->IsState(data_ov002_021105bc) != 0;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::LostGrabbedObject() {
    return ((this))->IsAnim(0x18) || ((this))->IsAnim(0x8b);
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::IsFrontSliding() {
    return ((this))->IsAnim(0x43) || ((this))->IsAnim(0x40);
}

int daPly_c::func_ov002_020dd2f4() {
    int atan;

    if (!(mClsnFlags & 2))
        return 0;

    atan = cstd::atan2(mWallNormalX, mWallNormalZ);
    if (AngleDiff(atan, mPrevAngleY) < 0x6000)
        return 0;

    if (func_ov002_020c0cbc() != 0) {
        mHorzSpeed = FX_MUL(mHorzSpeed, 0xe66);
        if (mHorzSpeed < 0x4000)
            mHorzSpeed = 0x4000;
        mPrevAngleY = atan - (s16)(mPrevAngleY - atan) + 0x8000;
    }
    return 0;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_PunchKick_Main() {
    extern u16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    u16 st = mStateTimer;

    if (st != 0) {
        if (st == 1) {
            func_ov002_020dd5ec();
        }
    } else {
        if (mPunchKickCooldown == 0) {
            u8 idx = gActivePlayerSlot;
            u16 flags = data_0209f498[idx].pressed;
            if ((flags & 1) != 0 && mPunchKickStep < 2) {
                mPunchKickStep++;
                this->St_PunchKick_Init();
                return 1;
            }
        }

        func_ov002_020d8a50(mPunchKickStep);

        if (mIsAirborne != 0)
            goto end;

        if (mPunchKickStep == 2 && mStatePhase == 0) {
            mStatePhase = 1;
            if (func_ov002_020e2c84() != 0)
                return 1;
        }

        ApproachLinear(mHorzSpeed, 0, 0x800);
        func_ov002_020c2f64();

        if (func_ov002_020c0434() != 0) {
            func_ov002_020c0364(3);
            return 1;
        }

        if (this->FinishedAnim() || mPunchKickStep == 2) {
            this->ChangeState(data_ov002_0211013c);
            mPunchKickCooldown = 0x10;

            if (mPunchKickStep != 2)
                mHorzSpeed = 0;

            if (mPrevAngleY != mAngleY) {
                mHorzSpeed = 0;
                mPrevAngleY = mAngleY;
            }

            {
                u8 idx2 = gActivePlayerSlot;
                s16 val = data_0209f498[idx2].mag;
                if (val == 0 && mPunchKickStep == 2) {
                    this->SetAnim(0x52, 0x40000000, 0x1000, 0);
                    mPrevAngleY = mAngleY;
                    mHorzSpeed = 0;
                }
            }

            if (mPunchKickStep != 2)
                goto end;
            if (mIsInShallowWater != 0)
                goto end;

            func_ov002_020bf9d4();
        }
    }

end:
    AdvanceAnims();
    return 1;
}

void daPly_c::func_ov002_020dd5ec() {
    extern u16 data_0209f49c[];
    extern int data_ov002_0210e160;
    int model;
    int *new_var3;
    int *p;
    int r;
    int idx;
    int *new_var;
    int new_var2;
    if (data_0209f498[gActivePlayerSlot].held & 2) {
        mPunchKickStep = 2;
    }
    SetAnim(data_ov002_0210a5c0[mPunchKickStep], 0x40000000, 0x1000, 0);
    model = GetBodyModelID(param1 & 0xff, 0);
    new_var2 = -0x4000;
    new_var = (int *)&mModels.mBodyModels[model];
    new_var3 = (int *)&(dExtFrameCtrl_c &)*(dExtAnmModel_c *)*new_var;
    if ((this && this) && this) {
    }
    p = new_var3;
    ((dExtFrameCtrl_c *)p)->currFrame = 0;
    mPunchKickCooldown = data_ov002_02109dbc[mPunchKickStep];
    if ((mPunchKickStep) == 2) {
        if (((data_0209f498[gActivePlayerSlot].held & 2) != 0) || ((mIsAirborne) != 0)) {
            mVertSpeed = 0x20000;
            mVertAccel = new_var2;
        } else {
            mVertSpeed = 0xc000;
            mVertAccel = -0x1800;
        }
        mIsAirborne = 1;
        mLandSoundPlayed = 0;
    }
    idx = mPunchKickStep;
    if (idx != 2) {
        r = RandomIntInternal(&data_ov002_0210e160);
        idx = (idx + (((unsigned int)r) >> 4)) & 1;
    }
    Sound::PlayCharVoice(mCharacter, data_ov002_020ff194[idx], mCamSpacePos);
    if ((mIsMega) != 0) {
        Sound::PlayBank0(0xd5, mCamSpacePos);
        return;
    }
    if ((mPunchKickStep) != 2) {
        Sound::PlayBank0(7, mCamSpacePos);
    } else {
        Sound::PlayBank0(8, mCamSpacePos);
    }
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_PunchKick_Init() {
    mPeakY = mPosY;
    mAttackClashed = 0;
    mStateTimer = 2;
    mStatePhase = 0;
    if (mPunchKickStep == 2 || this->IsCollectingCap())
        return 1;
    if (mIsMega == 0) {
        func_ov002_020dc020();
        mAttackClsn.Clear();
        mAttackClsn.Update();
    } else {
        mAttackClsn.Clear();
    }
  return 1;
}

int daPly_c::func_ov002_020dd824() {
    if (mIsAirborne)
        return 0;
    if (func_ov002_020d82f0() == 0)
        return 0;
    if (mIsMetal)
        return 0;
    if (mdCcAcPos_c.otherOwner == 0)
        return 0;
    if (mdCcAcPos_c.hitFlags & 0x10000) {
        return ChangeState(data_ov002_0211034c) != 0;
    }
    return 0;
}

extern "C" {

int func_ov002_020dd8b8(daPly_c *player) {
    if (!player->IsState(data_ov002_021105a4))
        return 0;
    return player->mStateStep == 0 ? 1 : 0;
}
}

void daPly_c::func_ov002_020dd8f4() {
    ChangeState(data_ov002_021101b4);
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

void daPly_c::func_ov002_020dd908() {
    extern unsigned char gGameMode;
    int i;
    Vector3 v;
    if ((int)(gGameMode == 1) == 0)
        return;
    for (i = 0; i < gNumPlayers; i++) {
        char *o = gPlayers[i];
        daPly_c *player = (daPly_c *)o;
        int *p;
        if (o == 0)
            continue;
        if (player->mClimbClsn == 0)
            continue;
        p = &player->mPosX;
        v.x = p[0];
        v.y = p[1];
        v.z = p[2];
        v.y = mPosY;
        if (Vec3_Dist(&mPos, &v) >= 0x12c000)
            continue;
        player->func_ov002_020dd8f4();
    }
}

/* recovered: named members + shared header, real C++ method */
// daPly_c::St_GroundPound_Cleanup - clear flag bit 0x20 in the u32 at this+0x2ec, return 1.
int daPly_c::St_GroundPound_Cleanup() {
    mdCcAcPos_c.flags &= ~0x20;
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Ground pound, per frame. mStateStep is read SIGNED and acts as a three-way
 * selector: positive counts down the spin-up animations, 0 is the falling and
 * impact phase, and negative (0xff, set on impact) is the recovery. The impact
 * branch picks one of three particle sets by cap state and character, shakes
 * the camera, and skips the upper particles in shallow water.
 */

int daPly_c::St_GroundPound_Main() {
    void func_ov002_020ef2a4(dBgCh_Actr * c, dActor_c * arg);
    void PlayerNewSimpleHold14(unsigned int id, int x, int y, int z);
    extern dCamera_c *gCamera;
    extern char data_0209f4a0[];
    s8 timer = *(s8*)&mStateStep;
    if (timer > 0) {
        mVertSpeed = 0;
        if (mStateStep == 2)
            mVertSpeed = 0xa000;
        if (FinishedAnim() != 0) {
            mStateStep -= 1;
            if (mStateStep != 0) {
                u32 anim = 0x3c;
                if (mIsMega != 0) anim = 0xa2;
                this->SetAnim(anim, 0x40000000, 0x1000, 0);
            } else {
                u32 anim = 0x3d;
                if (mIsMega != 0) anim = 0xa1;
                this->SetAnim(anim, 0x40000000, 0x1000, 0);
                mVertSpeed = -0x32000;
                Sound::PlayCharVoice(mCharacter, 0x15, mCamSpacePos);
                mPeakY = mPosY;
            }
        }
    } else if (timer == 0) {
        u16 st;
        func_ov002_020ef2a4(&mMeshClsn, this);
        st = mStateTimer;
        if (st != 0) {
            if (st == 1) {
                func_ov002_020c2f64();
                this->SetAnim(0x42, 0x40000000, 0x1000, 0);
                mStateStep = 0xff;
                *&mdCcAcPos_c.flags &= ~0x20;
            }
            {
                u16 f = mStateFlags & 1;
                if (f != 0) {
                    func_ov002_020c0364(1);
                    return 1;
                }
            }
        } else {
            if (mIsAirborne == 0) {
                if (func_ov002_020e2c84() == 2)
                    return 1;
                func_ov002_020dd908();
                if (mIsMega != 0) {
                    Sound::PlayBank0(0xd3, mCamSpacePos);
                } else {
                    Sound::PlayBank0((u32)mGroundSoundType + 0x90, mCamSpacePos);
                }
                {
                    Vector3 v;
                    int x = mPosX;
                    v.x = x;
                    v.y = mPosY;
                    int z = mPosZ;
                    v.z = z;
                    if (mIsMega != 0) {
                        int yy = mPosY + 0x14000;
                        v.y = yy;
                        Particle::System::NewSimple(kPtclMegaGroundPoundA, x, yy, z);
                        Particle::System::NewSimple(kPtclMegaGroundPoundB, v.x, v.y, v.z);
                        if (mIsInShallowWater == 0) {
                            Particle::System::NewSimple(kPtclMegaGroundPoundDryA, v.x, v.y, v.z);
                            int y2 = mPosY + 0x3c000;
                            int xx = v.x;
                            int zz = v.z;
                            v.y = y2;
                            Particle::System::NewSimple(kPtclMegaGroundPoundDryB, xx, y2, zz);
                        }
                    } else if (param1 != 2) {
                        int yy = mPosY + 0xa000;
                        v.y = yy;
                        Particle::System::NewSimple(kPtclGroundPound, x, yy, z);
                        if (mIsInShallowWater == 0) {
                            int y2 = mPosY + 0x3c000;
                            int xx = v.x;
                            int zz = v.z;
                            v.y = y2;
                            Particle::System::NewSimple(kPtclGroundPoundDryA, xx, y2, zz);
                            Particle::System::NewSimple(kPtclGroundPoundDryB, v.x, v.y, v.z);
                        }
                    } else {
                        int yy = mPosY + 0xa000;
                        v.y = yy;
                        Particle::System::NewSimple(kPtclWarioGroundPoundA, x, yy, z);
                        Particle::System::NewSimple(kPtclWarioGroundPoundB, v.x, v.y, v.z);
                        if (mIsInShallowWater == 0) {
                            int y2 = mPosY + 0x32000;
                            int xx = v.x;
                            int zz = v.z;
                            v.y = y2;
                            Particle::System::NewSimple(kPtclWarioGroundPoundDryA, xx, y2, zz);
                            Particle::System::NewSimple(kPtclWarioGroundPoundDryB, v.x, v.y, v.z);
                        }
                    }
                }
                gCamera->func_0200d8c8(&mPos, 0x578000);
                mStateTimer = 6;
            } else {
                func_ov002_020dbc94();
                mAttackClsn.Clear();
                mAttackClsn.Update();
            }
        }
    } else {
        if (data_0209f498[gActivePlayerSlot].mag != 0 || FinishedAnim() != 0) {
            this->ChangeState(data_ov002_0211013c);
            return 1;
        }
        func_ov002_020d36d8(1);
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_GroundPound_Init() {
  unsigned int anim;
  ReleaseHeldActor();
  mJumpComboStage = 0;
  mVertSpeed = 0;
  mHorzSpeed = 0;
  mPrevAngleY = mAngleY;
  anim=0x3b;
  if (mIsMega)
      anim = 0xa3;
  this->SetAnim(anim, 0x40000000, 0x1000, 0);
  mStateStep = 2;
  Sound::PlayBank0(6, mCamSpacePos);
  mdCcAcPos_c.flags |= 0x20;
  return 1;
}

int daPly_c::func_ov002_020dde74() {
    extern char data_0209f49c[];
    int b = (mHeldObj != 0);
    if (b) {
        ChangeState(data_ov002_021105d4);
        return 1;
    }
    if (param1 == 3 && mIsMega == 0) {
        ChangeState(data_ov002_02110034);
        return 1;
    }
    if ((data_0209f498[(u8)gActivePlayerSlot].held & 2) || mIsMega != 0)
        goto change574;

    if (mHorzSpeed < ScaleByCharFactor(0x20000)) {
        if (IsState(data_ov002_021101e4))
            goto change5bc;
        if (!IsState(data_ov002_0211019c))
            goto change574;
        if (mJumpComboStage != 2)
            goto change574;
    }
change5bc:
    if (mHorzSpeed < ScaleByCharFactor(0x24000))
        mHorzSpeed = ScaleByCharFactor(0x24000);
    return ChangeState(data_ov002_021105bc);

change574:
    if (mIsAirborne != 0)
        mPunchKickStep = 2;
    else
        mPunchKickStep = 0;
    return ChangeState(data_ov002_02110574);
}

/* recovered: named members + shared header, real C++ method
 *
 * Being carried up a tornado. The player orbits mAttachedActor: the horizontal
 * offset is rotated by an ever-increasing mAngleYSpeed while mAttachOffsetY
 * climbs at mVertSpeed. Leaving happens at the tornado's own height limit
 * (+0xdc on it) or on a collision bit. A dBgCh_Gnd from 0x64000 above the
 * candidate position decides whether the player is placed there or clamped to
 * mGroundY.
 */
int daPly_c::St_TornadoSpin_Main() {
    Vector3 pos;
    Vector3 clsnPos;
    int dx = mPosX - mAttachedActor->mPosX;
    int dz = mPosZ - mAttachedActor->mPosZ;
    int sx = FX_MUL(dx, 0xf33);
    int sz = FX_MUL(dz, 0xf33);
    int spd = mVertSpeed;
    if (spd < 0) {
        mVertSpeed = 0x1000;
    } else if (spd < 0x3c000) {
        mVertSpeed += 0x1000;
    }
    mAttachOffsetY += mVertSpeed;
    if (mAttachOffsetY < 0)
        mAttachOffsetY = 0;
    /* The attached actor is the tornado: func_ov002_020de33c, the only way
       into this state, is called only by daTor_c with itself. */
    if (mAttachOffsetY > ((daTor_c *)mAttachedActor)->mdCcAc_c.height || (mClsnFlags & 4) != 0) {
        mStatePhase = 0;
        this->ChangeState(data_ov002_02110454);
        if (mVertSpeed < 0x14000)
            mVertSpeed = 0x14000;
        return 1;
    }
    if (mAngleYSpeed < 0x3000)
        mAngleYSpeed += 0x100;
    mAngleY += mAngleYSpeed;
    {
        int v = mAngleYSpeed;
        if (v >= 0x1000) v = 0x1000;
        {
            int k = (u16)v >> 4;
            int sinv = FX_SinCosTable_[k * 2 + 1];
            int cosv = FX_SinCosTable_[k * 2];
            ((int *)&pos)[0] = FX_MUL(sz, cosv) + (mAttachedActor->mPosX + FX_MUL(sx, sinv));
            ((int *)&pos)[2] = FX_MUL(sz, sinv) + (mAttachedActor->mPosZ - FX_MUL(sx, cosv));
            ((int *)&pos)[1] = mAttachOffsetY + mAttachedActor->mPosY;
        }
    }
    dBgCh_Gnd rc;
    clsnPos = pos;
    clsnPos.y += 0x64000;
    rc.SetObjAndPos(clsnPos, this);
    if (rc.DetectClsn() != 0) {
        mPosX = pos.x;
        mPosY = pos.y;
        mPosZ = pos.z;
    } else {
        if (mPosY >= mGroundY)
            mPosY = pos.y;
        else
            mPosY = mGroundY;
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_TornadoSpin_Init() {
    extern int gCamera[];
    ReleaseHeldActor();
    this->SetAnim(0x5f, 0, 0x1000, 0);
    mVertSpeed = 0;
    mVertAccel = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    ((::dCamera_c *)*(void **)gCamera)->func_0200d6b4(mPlayerNo);
    mAttachOffsetY = mPosY - mAttachedActor->mPosY;
    Sound::PlayCharVoice(mCharacter, 0x26, mCamSpacePos);
    return 1;
}

extern "C" {

int func_ov002_020de328(daPly_c *player) {
    return player->IsState(data_ov002_021102bc);
}
}

extern "C" {

int func_ov002_020de33c(daPly_c *c, void *actor) {
    daPly_c *player = c;
    if (player->mIsNoControl != 0 || (c)->IsState(data_ov002_021102bc) ||
        (c)->IsState(data_ov002_02110484))
        return 0;
    player->mAttachedActor = (dActor_c *)actor;
    (c)->ChangeState(data_ov002_021102bc);
    return 1;
}
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Balloon_Cleanup() {
    func_ov002_020de968();
    return 1;
}

void daPly_c::func_ov002_020de3d0(short *r1, short *r2) {
    if (*r2 >= 0x100)
        *r2 -= 0x20;
    if (*r2 <= -0x100)
        *r2 += 0x20;
    if (*r1 < 0)
        *r2 += 0x10;
    else
        *r2 -= 0x10;
    *r1 = *r1 + *r2;
}

/* 0x74 is mCamSpacePos and 0x70c is mStateArg; St_Balloon_Main counts the
 * latter down and calls this on a bounce. */
void daPly_c::func_ov002_020de428() {
    if (mStateArg)
        return;
    Sound::PlayBank0(0xbd, mCamSpacePos);
    mStateArg = 0x1e;
}

/* recovered: named members + shared header, real C++ method
 *
 * Balloon (wing-cap style) flight, per frame. Bails out to one of two states
 * when the balloon timer runs out or the global flag says so; otherwise the
 * A-button bit adds lift in one of two bands, a collision bit bounces the
 * player off a wall by reflecting mPrevAngleY, and the pitch/roll pair is
 * driven from the turn rate.
 */
/* local extern: the call passes an untyped this, so it cannot use the header method. */

s32 daPly_c::St_Balloon_Main() {
    extern s16 data_0209f49e[];
    extern s16 data_0209f4a0[];
    if (mStateArg != 0) {
        mStateArg--;
    }
    mPeakY = mPosY;
    mVertAccel = 0;
    mTerminalVelocity = -0xc000;
    ApproachLinear(mVertSpeed, -0x6000, 0x800);

    if (mBalloonTimer == 0) {
        func_ov002_020de968();
        this->ChangeState(data_ov002_021101b4);
        return 1;
    }

    u16 flags = data_0209f498[gActivePlayerSlot].pressed;
    if (flags & 0x400) {
        func_ov002_020de968();
        this->ChangeState(data_ov002_021105a4);
        return 1;
    }

    if ((flags & 2) && mStateWaitTimer == 0) {
        if (mStateTimer == 0) {
            mVertSpeed += 0xc000;
            if (mVertSpeed < 0xc000)
                mVertSpeed = 0xc000;
            if (mVertSpeed > 0x10000)
                mVertSpeed = 0x10000;
        } else {
            mVertSpeed += 0x6000;
            if (mVertSpeed < 0x6000)
                mVertSpeed = 0x6000;
            if (mVertSpeed > 0xc000)
                mVertSpeed = 0xc000;
        }
        Sound::PlayBank0(0xbe, mCamSpacePos);
        mStateTimer = 8;
    }

    if (mClsnFlags & 2) {
        s16 ang = cstd::atan2(mWallNormalX, mWallNormalZ);
        s16 diff = AngleDiff(ang, mPrevAngleY);
        if ((s32)diff > 0x4000) {
            this->func_ov002_020de428();
            mPrevAngleY = (s16)((ang * 2 + 0x8000) - mPrevAngleY);
            mHorzSpeed = FX_MUL(mHorzSpeed, 0x1400);
            if (mHorzSpeed > 0x14000)
                mHorzSpeed = 0x14000;
            if (mHorzSpeed < 0x4000)
                mHorzSpeed = 0x4000;
            s32 idx = ((s32)(u16)diff >> 4) * 2;
            s16 t0 = FX_SinCosTable_[idx + 1];
            s32 v1 = FX_MUL(mHorzSpeed, t0);
            s32 v = FX_MUL(v1, 0xa0);
            s16 t1 = FX_SinCosTable_[idx];
            mAngleYSpeed = (s16)FX_MUL(t1, v);
            mAngleXSpeed = (s16)FX_MUL(t0, v);
        }
    }

    u8 mode = mClsnFlags;
    if (mode & 1) {
        s32 g = mPrevVertSpeed;
        if (g < 0) {
            mVertSpeed = -FX_MUL(g, 0x2800);
            this->func_ov002_020de428();
        }
    } else if (mode & 4) {
        s32 g = mPrevVertSpeed;
        if (g > 0) {
            mVertSpeed = -(s32)((((long long)g << 14) + 0x800) >> 12);
            mStateWaitTimer = 0x1e;
            this->func_ov002_020de428();
        }
    }

    if (mVertSpeed >= 0xa000)
        mVertSpeed = 0xa000;

    s32 r4 = 0;
    if (data_0209f498[gActivePlayerSlot].mag != 0) {
        ApproachAngle(&mPrevAngleY, mDesiredAngleY, 0x20, 0x2000, 0x10);
        r4 = 0x10000;
    }
    ApproachLinear2(mAngleY, mPrevAngleY, 0x200);
    ApproachLinear(mHorzSpeed, func_ov002_020bf224(r4, 0), 0x100);
    func_ov002_020de3d0(&mAngleZ, &mAngleYSpeed);
    func_ov002_020de3d0(&mAngleX, &mAngleXSpeed);
    mPrevVertSpeed = mVertSpeed;
    mModels.mAnmModel1.Advance();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Balloon_Init() {
    extern int gCamera[];
    extern int data_ov002_0210e750[];
    ReleaseHeldActor();
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    if (mVertSpeed >= 0x10000)
        mVertSpeed = 0x10000;
    if (mVertSpeed <= -0x10000)
        mVertSpeed = -0x10000;
    mHorzSpeed = 0;
    mModelOffsetY = 0x64000;
    ((::dCamera_c *)gCamera[0])->func_0200d63c(mPlayerNo);
    mAngleYSpeed = 0x100;
    mAngleXSpeed = 0x100;
    mPrevVertSpeed = mVertSpeed;
    mModels.mAnmModel1.SetAnim((BCA_File *)((void *)data_ov002_0210e750[1]), 0x40000000, 0x1000, 0);
    mModels.mAnmModel1.currFrame = 0;
    return 1;
}

// local extern: this file's local Vector3/Fix12i shadow the shared headers, so it cannot include daPly_c.h and use the member

struct Vector3e968 { int x, y, z; ~Vector3e968(){} };

void daPly_c::func_ov002_020de968() {
    mBalloonTimer = 0;
    if (mIsBalloon != 1)
        return;
    mIsBalloon = 0;
    mOpacity = 0x1f;
    EndPowerupMusic(0x30);
    Vector3e968 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = mPosY + 0x50000;
    Particle::System::NewSimple(kPtclBalloonPowerA, v.x, v.y, v.z);
    Particle::System::NewSimple(kPtclBalloonPowerB, v.x, v.y, v.z);
    Sound::PlayBank0(0xd8, mCamSpacePos);
}

/* recovered: named members + shared header, real C++ method */
void daPly_c::InitBalloonMario() {
    Vector3 v;

    if (mIsBalloon != 0) return;

    func_ov002_020bda48();
    mIsBalloon = 1;
    mBalloonTimer = 0x258;
    this->ChangeState(data_ov002_0211028c);
    StartPowerupMusic(0x30);
    func_ov002_020c43c4(2);

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y = mPosY + 0x50000;
    Particle::System::NewSimple(kPtclBalloonPowerA, v.x, v.y, v.z);
    Particle::System::NewSimple(kPtclBalloonPowerB, v.x, v.y, v.z);

    Sound::PlayBank0(0xd7, mCamSpacePos);
}

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
int daPly_c::St_WindCarry_Main() {
    char* c = (char*)this;

    if (mIsAirborne == 0) {
        this->func_ov002_020c2f64();
        this->ChangeState(data_ov002_021105bc);
        this->SetAnim(0x43, 0x40000000, 0x1000, 0);
        mVertSpeed = 0;
        return 1;
    }

    func_ov002_020e28d4(0x1800, 0x800);
    {
        int val = mPosY + 0x514000;
        if (mSurfaceType == 0x13 && val > (int)0xfdecc000 && val < 0x898000) {
            int speed;
            if (val < 0)
                speed = 0x32000;
            else
                speed = 0x2710000 / ((val + 0xc8000) >> 12);
            if (mVertSpeed < speed) {
                int* p = &this->mVertSpeed;
                *p = *p + (speed >> 3);
                if (mVertSpeed > speed) mVertSpeed = speed;
            }
        }
    }

    if (mStateStep == 0 && FinishedAnim() != 0) {
        this->SetAnim(0x73, 0, 0x1000, 0);
        {
            u8* q = &this->mStateStep;
            *q = *q + 1;
        }
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_WindCarry_Init() {
    extern int gCamera[];
    DropHeldObj();
    this->SetAnim(0x4a, 0x40000000, 0x1000, 0);
    mVertSpeed = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    ((::dCamera_c *)gCamera[0])->func_0200d63c(mPlayerNo);
    return 1;
}

int daPly_c::func_ov002_020dec70() {
    int t = mPosY + 0x514000;
    if (t < (int)0xfdecc000) return 0;
    {
        Vector3_16f v;
        v.z = 0;
        v.y = 0x1000;
        v.x = v.z;
        func_ov002_020bf800(v);
    }
    mSurfaceSoundHandle = Sound::PlayLong((u32)mSurfaceSoundHandle, 3, 0x90, *&mCamSpacePos, 0);
    if (IsState(data_ov002_02110274) || IsState(data_ov002_021102d4) || IsState(daPly_c::ST_OWL) ||
        mIsMetal || mIsMega || mIsNoControl || mIsAirborne == 0)
        return 0;
    if (func_ov002_020e0478() != 0 || mIsBalloon != 0) {
        if (t < 0x898000)
            mPosY += 0x8000;
        return 0;
    }
    ChangeState(data_ov002_02110274);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_JumpQuicksand_Main() {

    u8 t = mStateWork;
    mStateWork++;
    if (t < 6) {
        mSinkDepth -= FX_MUL((7 - mStateWork) << 12, 0xccc);
        if (mSinkDepth < 0x1000)
            mSinkDepth = 0x1100;
    } else {
        this->ChangeState(data_ov002_02110424);
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_JumpQuicksand_Init() {
    mJumpedFromQuicksand = 1;
    this->SetAnim(0x53, 0x40000000, 0x1000, 0);
    mStateWork = 0;
    mVertSpeed = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method */
// daPly_c::St_Cannon_Cleanup - clear flag bit 0x20 in the u32 at this+0x2ec, return 1.
int daPly_c::St_Cannon_Cleanup() {
    mdCcAcPos_c.flags &= ~0x20;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Cannon_Main() {
    extern void func_02012694(unsigned int id, const Vector3 *v);
    extern u16 data_0209f49e[];
    extern char *gCamera;
    switch (mStateStep) {
    case 0:
        break;
    case 1: {
        if ((data_0209f498[gActivePlayerSlot].pressed & 3) == 0)
            break;

        char *p = gCamera;
        mPrevAngleY = GetAngleToCamera(mPlayerNo) + 0x8000;
        mAngleY = mPrevAngleY;

        int ang = (u16)((dCamera_c *)p)->mAngleX;
        int j = ang >> 4;
        mHorzSpeed = FX_MUL(FX_SinCosTable_[j * 2 + 1], 0x64000);
        mVertSpeed = FX_MUL(FX_SinCosTable_[j * 2], 0x64000);
        mAngleX = 0;
        mStateStep = 2;
        mOpacity = 0x1f;
        mIsBodyClsnEnabled = 1;
        mdCcAcPos_c.flags |= 0x20;
        ((::dCamera_c *)p)->func_0200d6f0(mPlayerNo);
        func_02012694(0x14f, &mCamSpacePos);
        Sound::PlayBank0(0xb9, mCamSpacePos);
        Sound::PlayCharVoice(mCharacter, 0xc, mCamSpacePos);
        func_ov002_020c9e18();
        mIsControlDisabled = 0;
        break;
    }
    case 2:
        if ((mClsnFlags & 6) != 0) {
            func_ov002_020c1eb4((short)(mAngleY + 0x8000));
            return 1;
        }
        if (mIsAirborne == 0) {
            if (mStateWork != 0) {
                this->ChangeState(data_ov002_0211031c);
                return 1;
            }
            this->ChangeState(data_ov002_0211031c);
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
            mVertSpeed = 0xa000;
            return 1;
        }
        if (mHasWings != 0 && mVertSpeed < 0) {
            this->SetAnim(0x72, 0x40000000, 0x1000, 0);
            mStateStep = 3;
            return 1;
        }
        mHorzSpeed -= 0x80;
        if (mHorzSpeed < 0xa000)
            mHorzSpeed = 0xa000;
        if (mVertSpeed >= 0)
            Particle::RunningSlidingDustAt(mPosX, mPosY, mPosZ);
        if (mStateWork == 0)
            mAngleX = cstd::atan2(mHorzSpeed >> 8, mVertSpeed >> 8) - 0x4000;
        AdvanceAnims();
        // fall through
    case 3:
        if (this->FinishedAnim()) {
            mStateStep = 1;
            this->ChangeState(data_ov002_02110214);
        }
        AdvanceAnims();
        break;
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Cannon_Init() {
    DisableInteraction();
    mIsUnderwater = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    ReleaseHeldActor();
    mStateStep = 0;
    this->SetAnim(0x73, 0, 0x1000, 0);
    mVertAccel = -0x1200;
    mOpacity = 0;
    mIsBodyClsnEnabled = 0;
    mStateWork = 0;
    mIsControlDisabled = 1;
    return 1;
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_021102d4;
}

int daPly_c::IsInsideOfCannon() {

    if (!this->IsState(data_ov002_021102d4))
        return 0;
    return mStateStep != 2 ? 1 : 0;
}

/* recovered: named members + shared header, real C++ method */
extern "C" {

extern daPly_c::State data_ov002_021102d4;
}

int daPly_c::IsBeingShotOutOfCannon() {

    if (!this->IsState(data_ov002_021102d4))
        return 0;
    return mStateStep == 2 ? 1 : 0;
}

extern "C" {

int func_ov002_020df300(char* p) {
    extern int gCamera;
    daPly_c *player = (daPly_c *)p;
    if (player->mStateStep)
        return 0;
    player->mStateStep = 1;
    ((::dCamera_c *)gCamera)->func_0200d438(player->mPlayerNo);
    return 1;
}
}

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

int daPly_c::TryEnterCannonState() {
    struct dCamera_c {};
    extern dCamera_c *gCamera;
    if (mIsNoControl != 0 || func_ov002_020e0478() != 0 || IsState(data_ov002_0210ffec) != 0) {
        return 0;
    }
    mStateStep = 0;
    ChangeState(data_ov002_021102d4);
    ((::dCamera_c *)gCamera)->func_0200d474(mPlayerNo);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Owl_Cleanup() {
    EndStateMusic(0x2f);
    mHeldObj = 0;
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Owl_Main() {
    extern void func_0201f32c(int a);
    extern unsigned char data_0209f49c[];
    extern char data_0209f4a0[];
    extern char *gCamera;
    unsigned char st;

    mPeakY = mPosY;
    st = mStateStep;
    if (st != 2 && st != 3) {
        if (mHeldObj == 0 || (data_0209f498[gActivePlayerSlot].held & 2) == 0 || (mClsnFlags & 2)) {
            this->ChangeState(data_ov002_021101b4);
            return 1;
        }
    }

    switch (st) {
    case 0:
        if (mAttachOffsetY <= mPosY) {
            (this->mStateStep)++;
            mVertAccel = -0x2c00;
        }
        if (mIsAirborne != 0) {
            if (mVertSpeed < 0) {
                ApproachLinear(mVertSpeed, 0x3c000, 0x8000);
            } else {
                ApproachLinear(mVertSpeed, 0x3c000, 0x2000);
            }
        } else {
            mVertSpeed = 0x8000;
            mIsAirborne = 1;
            mLandSoundPlayed = 0;
        }
        ApproachAngle(&mAngleY, mAngleYSpeed, 0x10, 0x100, 0);
        break;

    case 1:
    case 4: {
        int x;
        short ang;
        int i;
        short mag = 10;
        x = ((daOwl_c *)mHeldObj)->mAnmModel.currFrame;
        x = (unsigned short)((unsigned int)x >> 12);
        x += 0xb;
        ang = x * 0xccc;
        i = (unsigned short)ang >> 4;
        mVertSpeed = FX_SinCosTable_[i * 2 + 1] * mag;
        mHorzSpeed = 0x14000;
        if (data_0209f498[gActivePlayerSlot].mag != 0) {
            ApproachAngle(&mAngleY, mDesiredAngleY, 0x20, 0x100, 0);
        }
        if (mStateStep == 1) {
            if (mAttachOffsetY - 0xed8000 > mPosY) {
                mOwlFarCamera = 1;
                func_0201f32c(0xa3);
                ((::dCamera_c *)gCamera)->func_0200d3f8(mPlayerNo, 0);
                mVertAccel = 0;
                DisableInteraction();
                mStateStep = 2;
            }
        } else {
            if (mStateTimer == 0) {
                this->ChangeState(data_ov002_021101b4);
                return 1;
            }
        }
        break;
    }

    case 2:
        mHorzSpeed = 0;
        mVertSpeed = mHorzSpeed;
        if (data_0209d660 == 0) {
            mStateStep = 3;
            ((::dCamera_c *)gCamera)->func_0200d6b4(mPlayerNo);
        }
        break;

    case 3:
        if ((((dCamera_c *)gCamera)->mFlags & 0x8000) == 0) {
            mStateStep = 4;
            mStateTimer = 0x3c;
            func_ov002_020c9e18();
            mOwlFarCamera = 0;
        }
        break;
    }

    if (this->FinishedAnim()) {
        this->SetAnim(0x58, 0, 0x1000, 0);
    }
    mPrevAngleY = mAngleY;
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Owl_Init() {
    extern int gCamera[];
    mTerminalVelocity = -0x4b000;
    mVertAccel = 0;
    mStateStep = 0;
    mHorzSpeed = 0;
    this->SetAnim(0x5b, 0x40000000, 0x1000, 0);
    ((::dCamera_c *)gCamera[0])->func_0200d6b4(mPlayerNo);
    StartStateMusic(0x2f);
    return 1;
}

int daPly_c::func_ov002_020df7ac() {
    if (this->IsState(daPly_c::ST_OWL) == 0)
        return 0;
    this->St_Owl_Cleanup();
    this->ChangeState(data_ov002_021101b4);
    return 1;
}

extern "C" {

int func_ov002_020df7f4(daPly_c *player) {
    if (!player->IsState(daPly_c::ST_OWL) || player->mHeldObj == 0)
        return -1;
    return player->mStateStep != 0;
}
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

int daPly_c::func_ov002_020df840(void *arg1, Vector3 *arg2) {
    extern u16 data_0209f49c[];
    if (this->mIsMega != 0 || this->mIsBalloon != 0)
        return 0;
    if ((data_0209f498[gActivePlayerSlot].held & 2) == 0)
        return 0;
    if (this->mHeldObj != 0)
        return 0;
    this->ChangeState(daPly_c::ST_OWL);
    this->mHeldObj = (dActor_c *)arg1;
    this->mAttachOffsetY = arg2->y;
    this->mAngleYSpeed = Vec3_HorzAngle(&this->mPos, arg2);
    return 1;
}

extern "C" {

void func_ov002_020df8f0(char* c)
{
    typedef unsigned long long u64;
    daPly_c *player = (daPly_c *)c;
    int vb = player->mVertSpeed;
    int va = player->mHorzSpeed;
    int b = vb >> 8;
    int sq = b * b;
    int a = va >> 8;
    sq += a * a;
    player->mPrevVertSpeed = cstd::sqrt((u64)sq) << 8;

    {
        int idx = gActivePlayerSlot * 0x18;
        s16 tval = *(s16*)((char*)data_0209f4a4 + idx);
        int div5 = player->mPrevVertSpeed / 5;
        int v = FX_MUL((tval >> 6), div5);
        if (v < -0x8000) v = -0x8000;
        if (v >= 0x8000) v = 0x7fff;

        if (v > 0) {
            if (player->mAngleXSpeed < 0) {
                player->mAngleXSpeed += 0x40;
                if (player->mAngleXSpeed > 0x20)
                    player->mAngleXSpeed = 0x20;
            } else {
                s16 step;
                if (player->mAngleXSpeed < v)
                    step = 0x20;
                else
                    step = 0x40;
                ApproachLinear2(player->mAngleXSpeed, (s16)v, step);
            }
        } else if (v < 0) {
            if (player->mAngleXSpeed > 0) {
                player->mAngleXSpeed -= 0x40;
                if (player->mAngleXSpeed < -0x20)
                    player->mAngleXSpeed = -0x20;
            } else {
                s16 step;
                if (player->mAngleXSpeed < v)
                    step = 0x40;
                else
                    step = 0x20;
                ApproachLinear2(player->mAngleXSpeed, (s16)v, step);
            }
        } else {
            ApproachLinear2(player->mAngleXSpeed, 0, 0x40);
        }
    }

    {
        int idx = gActivePlayerSlot * 0x18;
        int u2 = player->mPrevVertSpeed;
        s16 tval2 = *(s16*)((char*)data_0209f4a2 + idx);
        int quarter = u2 / 4;
        int v2 = 0 - FX_MUL((tval2 >> 6), quarter);
        if (v2 < -0x8000) v2 = -0x8000;
        if (v2 >= 0x8000) v2 = 0x7fff;

        if (v2 > 0) {
            if (player->mAngleYSpeed < 0) {
                player->mAngleYSpeed += 0x40;
                if (player->mAngleYSpeed > 0x10)
                    player->mAngleYSpeed = 0x10;
            } else {
                s16 step;
                if (player->mAngleYSpeed < v2)
                    step = 0x10;
                else
                    step = 0x20;
                ApproachLinear2(player->mAngleYSpeed, (s16)v2, step);
            }
        } else if (v2 < 0) {
            if (player->mAngleYSpeed > 0) {
                player->mAngleYSpeed -= 0x40;
                if (player->mAngleYSpeed < -0x10)
                    player->mAngleYSpeed = -0x10;
            } else {
                s16 step;
                if (player->mAngleYSpeed < v2)
                    step = 0x20;
                else
                    step = 0x10;
                ApproachLinear2(player->mAngleYSpeed, (s16)v2, step);
            }
        } else {
            ApproachLinear2(player->mAngleYSpeed, 0, 0x40);
        }
    }

    player->mPrevAngleY += player->mAngleYSpeed;
    player->mPrevAngleZ = (s16)((0 - player->mAngleYSpeed) * 0x14);

    player->mPrevVertSpeed -= ((player->mPrevAngleX >> 1) + 0x100);
    if (player->mPrevVertSpeed < 0)
        player->mPrevVertSpeed = -player->mPrevVertSpeed;

    {
        int *p = &player->mPrevVertSpeed;
        *p -= (0x1000 - FX_SinCosTable_[((u16)(player->mAngleYSpeed) >> 4) * 2 + 1]) >> 1;
    }
    if (player->mPrevVertSpeed < 0)
        player->mPrevVertSpeed = 0;

    if (player->mPrevVertSpeed > 0x10000) {
        player->mPrevAngleX += ((player->mPrevVertSpeed - 0x20000) * 6) >> 12;
    } else if (player->mPrevVertSpeed <= 0x4000) {
        player->mPrevAngleX -= 0x600;
    } else {
        player->mPrevAngleX += ((player->mPrevVertSpeed - 0x20000) * 0xa) >> 12;
    }

    player->mPrevAngleX += player->mAngleXSpeed;
    if (player->mPrevAngleX > 0x2aaa)
        player->mPrevAngleX = 0x2aaa;
    if (player->mPrevAngleX < (s16)0xd556)
        player->mPrevAngleX = (s16)0xd556;

    player->mVertSpeed =
        FX_MUL(player->mPrevVertSpeed, FX_SinCosTable_[((u16)player->mPrevAngleX >> 4) * 2]);
    player->mHorzSpeed =
        FX_MUL(player->mPrevVertSpeed, FX_SinCosTable_[((u16)player->mPrevAngleX >> 4) * 2 + 1]);

    {
        player->mAngleX = player->mPrevAngleX;
        player->mAngleY = player->mPrevAngleY;
        player->mAngleZ = player->mPrevAngleZ;
        s16 r = (s16)(0 - player->mPrevAngleX);
        int spd = player->mPrevVertSpeed;
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s16 P = (s16)FX_MUL(t, 0x2aaaU);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        player->mAngleX = r;
    }
}
}

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
#define c ((char *)this)

int daPly_c::St_Fly_Main() {
    extern u16 data_0209f49e;
    extern u16 data_0209f49c;
    mPeakY = mPosY;

    if ((data_0209f498[gActivePlayerSlot].pressed & 0x400) == 0) {
        goto skip;
    }
    if (func_ov002_020d674c() != 0) {
        this->ChangeState(data_ov002_0211004c);
    } else {
        this->ChangeState(data_ov002_021105a4);
    }
    return 1;

skip:
    if (mHasWings == 0) {
        this->ChangeState(data_ov002_021101b4);
        return 1;
    }
    if (mIsAirborne == 0) {
        this->func_ov002_020c2f64();
        this->ChangeState(data_ov002_021105bc);
        this->SetAnim(0x43, 0x40000000, 0x1000, 0);
        mVertSpeed = 0;
        return 1;
    }

    if (this->IsAnim(0x4a) != 0) {
        if (mVertSpeed > 0x20000) {
            mVertAccel = -0x3400;
            if (mVertSpeed >= 0) {
                if ((data_0209f498[gActivePlayerSlot].held & 2) == 0) {
                    mVertAccel = -0x8000;
                }
            } else {
                mVertAccel = -0x2000;
                this->mHorzSpeed = this->mHorzSpeed + 0xe00;
            }
            if (this->FinishedAnim() != 0) {
                mVertAccel = 0;
                mTerminalVelocity = -0x4b000;
                this->SetAnim(0x49, 0, 0x1000, 0);
            }
        } else {
            mVertAccel = 0;
            mTerminalVelocity = -0x4b000;
            func_ov002_020df8f0(c);
        }
    } else {
        func_ov002_020df8f0(c);
    }

    Vector3_16f vec16;
    Vector3 vec32;
#define spC vec16.x
#define spE vec16.y
#define sp10 vec16.z
#define sp14 vec32.x
#define sp18 vec32.y
#define sp1C vec32.z
    {
        s16 n = FX_SinCosTable_[((u16)mPrevAngleX >> 4) * 2 + 1];
        s32 r0 = (s16)-n;
        s16 t = FX_SinCosTable_[((u16)mAngleY >> 4) * 2];
        spC = (s16)FX_MUL(r0, t);
        spE = FX_SinCosTable_[((u16)mAngleX >> 4) * 2];
        s16 t2 = FX_SinCosTable_[((u16)mAngleY >> 4) * 2 + 1];
        sp10 = (s16)FX_MUL(r0, t2);
    }

    if (mPrevVertSpeed >= 0x28000) {
        mParticle1 = Particle::System::New(mParticle1, 0xd1, mPosX, mPosY, mPosZ,
                                           (const Vector3_16f *)(&vec16), 0);
    }

    if (mPrevVertSpeed >= 0x30000) {
        if (mPrevAngleX > 0x800) {
            if (mStateArg == 0) {
                Sound::PlayBank0(0xb9, mCamSpacePos);
                func_ov002_020e25f0(2);
                mStateArg++;
            }
            {
                Matrix4x3 *bone =
                    &mModels.mBodyModels[this->GetBodyModelID(this->param1 & 0xff, 0)]
                         ->data.transforms[14];
                dExtAnmModel_c *m2 =
                    mModels.mBodyModels[this->GetBodyModelID(this->param1 & 0xff, 0)];
                MTX_MultVec43((Vector3 *)&bone->m[9], &m2->mat4x3, &vec32);
                Vec3_MulScalarInPlace((Vector3 *)&vec32.x, 0x8000);
                mParticle2 = Particle::System::New(mParticle2, 0xd0, sp14, sp18, sp1C,
                                                   (const Vector3_16f *)(&vec16), 0);
            }
            {
                Matrix4x3 *bone =
                    &mModels.mBodyModels[this->GetBodyModelID(this->param1 & 0xff, 0)]
                         ->data.transforms[11];
                dExtAnmModel_c *m2 =
                    mModels.mBodyModels[this->GetBodyModelID(this->param1 & 0xff, 0)];
                MTX_MultVec43((Vector3 *)&bone->m[9], &m2->mat4x3, &vec32);
                Vec3_MulScalarInPlace((Vector3 *)&vec32.x, 0x8000);
                mParticle3 = Particle::System::New(mParticle3, 0xd0, sp14, sp18, sp1C,
                                                   (const Vector3_16f *)(&vec16), 0);
            }
        }
    } else {
        mStateArg = 0;
    }

    {
        s32 v = mPrevVertSpeed;
        s32 r1 = (v / 0x1000) * 3;
        if (r1 >= 0x100) {
            r1 = 0x100;
        }
        mLoopingSoundHandle = func_02012194(mLoopingSoundHandle, 0, 0x104, 3, r1, &mCamSpacePos, 0);
    }
    AdvanceAnims();
    return 1;
}

#undef spC
#undef spE
#undef sp10
#undef sp14
#undef sp18
#undef sp1C
#undef c

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Fly_Init() {
    extern int gCamera[];
    mAngleXSpeed = 0;
    mAngleYSpeed = 0;
    mPrevAngleX = 0;
    mPrevAngleZ = 0;
    mPrevAngleY = mAngleY;
    ReleaseHeldActor();
    if (mStateStep == 0) {
        mVertSpeed = 0x5d000;
        this->SetAnim(0x4a, 0x40000000, 0x1000, 0);
        mTerminalVelocity = -0x4000;
    } else {
        mVertAccel = 0;
        this->SetAnim(0x49, 0, 0x1000, 0);
    }
    ((::dCamera_c *)gCamera[0])->func_0200d6f0(mPlayerNo);
    return 1;
}

void daPly_c::func_ov002_020e032c() {
    extern unsigned char gGameMode;
    unsigned char st;
    if (mHasWings == 0)
        return;
    mHasWings = 0;
    st = gGameMode;
    if ((int)(st == 1) == 0) {
        if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) == 0)
            return;
        if ((int)(st == 2) != 0) return;
    }
    EndPowerupMusic(0x33);
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

void daPly_c::InitWingFeathers(bool b_) {
    extern unsigned char gGameMode[];
    int b = b_;

    int t0 = (gGameMode[0] == 1);
    if (!t0) {
        if (param1 != 0) return;
    }
    func_ov002_020bdd2c();
    mHasWings = 1;
    mPowerupTimer = 0x708;
    {
        int t1 = (gGameMode[0] == 1);
        if (!t1)
            StartPowerupMusic(0x33);
        else
            StartPowerupMusic(0x4e);
    }
    if (b == 0) return;
    func_ov002_020c43c4(1);
}

int daPly_c::func_ov002_020e0478() {
    return IsState(data_ov002_02110214) != 0;
}

/* func_ov002_020e04a4 at 0x020e04a4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */

void daPly_c::func_ov002_020e04a4() {
    if (data_0209f498[gActivePlayerSlot].mag < 0x600)
        return;
    func_ov002_020bf88c();
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Land_Main() {
    extern u16 data_0209f49e[];
    extern u16 data_0209f49c[];
    extern s16 data_0209f4a0[];
    if (func_ov002_020c0434()) {
        func_ov002_020c0364(3);
        func_ov002_020c06fc(0x4000);
        return 1;
    }

    func_ov002_020e04a4();
    u16 temp_r3 = mStateFlags;

    if ((u16)(temp_r3 & 0x40) == 0) {
        if ((data_0209f498[gActivePlayerSlot].pressed & 2) || (u16)(temp_r3 & 0x100) != 0) {
            mStateFlags &= ~0x100;
            if (func_ov002_020e3078(&data_ov002_0211055c) != 0 &&
                (data_0209f498[gActivePlayerSlot].held & 0x400) && mHorzSpeed >= 0) {
                this->ChangeState(data_ov002_0211055c);
                return 1;
            }
            if (this->IsAnim(0x4e) != 0) {
                mAngleY = mPrevAngleY;
            }
            int v = (mHeldObj != 0);
            if (v != 0) {
                mJumpComboTimer = 0;
            }
            this->ChangeState(data_ov002_0211019c);
            return 1;
        }
        if (func_ov002_020d5c6c() != 0) {
            return 1;
        }
        if (data_0209f498[gActivePlayerSlot].pressed & 1) {
            return func_ov002_020dde74();
        }
    } else {
        mStateFlags &= ~0x100;
    }

    ApproachLinear(mHorzSpeed, func_ov002_020bf224(ScaleByCharFactor(0x20000), 0), 0x4000);

    if (func_ov002_020e3078(&data_ov002_0211052c) != 0) {
        mHorzSpeed = 0;
    }

    if (mStateTimer != 0) {
        goto tail;
    }

    if ((data_0209f498[gActivePlayerSlot].mag != 0) || (this->FinishedAnim() != 0)) {
        if (this->IsAnim(0x1b) != 0) {
            this->SetAnim(0x2e, 0x40000000, 0x1000, 0);
            goto tail;
        }
        if (this->IsAnim(0x4e) != 0) {
            mAngleY = (s16)(mAngleY + 0x8000);
        }
        if ((data_0209f498[gActivePlayerSlot].mag != 0) &&
            AngleDiff(mAngleY, mPrevAngleY) >= 0x4000) {
            if (AngleDiff(mDesiredAngleY, mAngleY) >= 0x4000) {
                mAngleY = mPrevAngleY;
            } else {
                mHorzSpeed = 0 - mHorzSpeed;
            }
        }
        mPrevAngleY = mAngleY;
        this->ChangeState(data_ov002_0211013c);
        if (this->IsState(data_ov002_0211013c) != 0 && mStateStep != 0 &&
            mHorzSpeed >= ScaleByCharFactor(0x1c000)) {
            mSpineRot[2] = 0x4000;
        }
        return 1;
    }

    if (func_ov002_020e3078(&data_ov002_0211019c) == 0) {
        mHorzSpeed = 0;
    }

tail:
    AdvanceAnims();
    return 1;
}

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
#pragma push                // without it, the later pop does not compile
#pragma opt_common_subs off // without it, St_Land_Init shrinks 0x1d8 to 0x1d4

int daPly_c::St_Land_Init() {
    extern char data_0209f4a0[];
    char* c = (char*)this;
    daPly_c::State *r4;

    this->func_ov002_020c2f64();
    r4 = mPrevState;

    if (mJumpedFromQuicksand == 0) {
        int v = mSurfaceType;
        if (v >= 6 && v <= 9) {
            int s = mSinkDepth;
            if (s < 0x1e000) {
                mSinkDepth = 0x1e000;
            }
        }
    }

    int r1;
    if (r4 == &data_ov002_0211019c) {
        r1 = func_ov002_020e0a64();
    } else if (r4 == &data_ov002_021101b4) {
        r1 = 0x55;
    } else if (r4 == &data_ov002_021101e4) {
        r1 = 0x4e;
    } else if (r4 == &data_ov002_0211052c) {
        mPrevAngleY = mAngleY;
        mHorzSpeed = 0;
        r1 = 0x2b;
    } else if (r4 == &data_ov002_02110724 || r4 == &data_ov002_0211007c) {
        r1 = 0x51;
    } else if (r4 == &data_ov002_0211055c) {
        r1 = 0x1b;
    } else if (r4 == &data_ov002_02110454) {
        r1 = 0x60;
    } else {
        r1 = 0x52;
    }

    this->SetAnim(r1, 0x40000000, 0x1000, 0);

    int idx = gActivePlayerSlot;
    short sval = *(short*)((char*)&data_0209f4a0 + idx * 0x18);
    if (sval == 0 && (IsAnim(0x2b) || IsAnim(0x4c))) {
        func_ov002_020e25d4(c);
    }
    mStateStep = 0;
    if (r4 == &data_ov002_0211019c && mJumpComboStage == 2) {
        mStateStep = 1;
    }

    mStateTimer = 2;
    if (func_ov002_020e2c84() != 2 && mIsInShallowWater == 0) {
        this->func_ov002_020bf9d4();
    }

    return 1;
}

#pragma pop // without it, opt_common_subs stays off and later functions change

int daPly_c::func_ov002_020e0a64() {
    if (IsAnim(0x54) == 0)
        mJumpComboTimer = 8;
    if (mIsMega != 0)
        return 0x9b;
    return data_ov002_0210a5b4[mJumpComboStage];
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_SmallLaunchUp_Main() {

  if(mIsAirborne==0){
    this->ChangeState(data_ov002_02110424);
  }
  AdvanceAnims();
  return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_SmallLaunchUp_Init() {
    mJumpedFromQuicksand = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x56, 0, 0x1000, 0);
    mVertSpeed = 0x2a000;
    mHorzSpeed = 0;
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    return 1;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_SlopeJump_Main() {
    extern unsigned short data_0209f49e[];
    ApproachLinear(mHorzSpeed, 0, 0x800);
    if (mIsAirborne == 0) {
        this->ChangeState(data_ov002_0211013c);
    }
    if (*(unsigned short *)((char *)data_0209f49e + gActivePlayerSlot * 0x18) & 1) {
        mPrevAngleY = mAngleY;
        if (mIsMega == 0) {
            if (param1 == 3) {
                this->ChangeState(data_ov002_02110034);
            } else {
                this->ChangeState(data_ov002_021105bc);
            }
        }
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_SlopeJump_Init() {
    mJumpedFromQuicksand = 0;
    if (func_ov002_020e2b6c())
        return 1;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x53, 0x40000000, 0x1000, 0);
    mVertSpeed = 0x2a000;
    func_ov002_020e2ad0();
    if (mHorzSpeed > 0) {
        mHorzSpeed = FX_MUL(mHorzSpeed, 0xc00);
    }
    func_ov002_020e25f0(0);
    return 1;
}

int daPly_c::func_ov002_020e0ccc(void *st) {
    int b;
    if (st == 0)
        return 0;
    b = (int)(((dActor_c *)st)->actorID == 0xc1);
    if (b == 0)
        return 0;
    ChangeState(data_ov002_02110484);
    return 1;
}

int daPly_c::St_CrazedCrate_Cleanup() {
    mdCcAcPos_c.flags &= ~0x20;
    mJumpComboStage = 0;
    return 1;
}

int daPly_c::St_CrazedCrate_Main() {
    typedef long long s64;
    func_ov002_020e28d4(0x1000, 0x1800);

    if (mClsnFlags & 2) {
        int atan = cstd::atan2(mWallNormalX, this->mWallNormalZ);
        int diff = AngleDiff(atan, this->mAngleY);
        if (diff >= 0x6000) {
            int t = (s16)(2 * atan - this->mAngleY);
            t += 0x8000;
            mAngleY = t;
            int half = 0x800;
            int *xpos = (int *)(int)(&mPosX);
            int* zpos;
            mPrevAngleY = mAngleY;
            *xpos +=
                (int)(((s64)mHorzSpeed * FX_SinCosTable_[(((int)(u16)mPrevAngleY >> 4) << 1) + 1] +
                       half) >>
                      12);
            zpos = (int *)(int)(&mPosZ);
            *zpos += (int)(((s64)mHorzSpeed * FX_SinCosTable_[((int)(u16)mPrevAngleY >> 4) << 1] +
                            half) >>
                           12);
        }
    }

    mAngleY = mPrevAngleY;

    if (mIsAirborne == 0) {
        if (mJumpComboStage < 2) {
            (*(u8 *)(int)(&mJumpComboStage))++;
            func_ov002_020e0f38(this->mJumpComboStage);
        } else {
            if (param1 == 3) {
                if (mObjInMouth != 0) {
                    ((fBase_c *)mObjInMouth)->MarkForDestruction();
                }
                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                this->SpawnCoins(pos, 5, 0xf000, 0);
                func_ov002_020d718c();
            } else {
                ReleaseHeldActor();
            }
            this->ChangeState(data_ov002_0211031c);
            this->SetAnim(0x43, 0x40000000, 0x1000, 0);
        }
    }

    int v98 = mHorzSpeed;
    int va8 = mVertSpeed;
    int atan2 = cstd::atan2(v98 >> 8, va8 >> 8);
    mAngleX = (s16)(atan2 - 0x4000);

    return 1;
}

void daPly_c::func_ov002_020e0f38(int a) {
    mVertSpeed = data_ov002_020ff17c[a];
    mHorzSpeed = data_ov002_020ff188[a];
    if (a != 0)
        LandingDust(true);
    if (a != 2)
        func_0201267c(0x6c, &mCamSpacePos);
    else
        func_0201267c(0x6d, &mCamSpacePos);
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* daPly_c::St_CrazedCrate_Init() at 0x020e0fac (ov002), size 0x74
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */

int daPly_c::St_CrazedCrate_Init() {
  mJumpedFromQuicksand = 0;
  mIsInAirState = 1;
  mIsFallScreaming = 0;
  mJumpComboStage = 0;
  mIsAirborne = 1;
  mLandSoundPlayed = 0;
  this->SetAnim(0x43, 0x40000000, 0x1000, 0);
  func_ov002_020e0f38(mJumpComboStage);
  mPrevAngleY = mAngleY;
  mdCcAcPos_c.flags |= 0x20;
  return 1;
}

/* recovered: named members + shared header */
void daPly_c::SpinBounce(Fix12i f) {
    if (this->IsState(data_ov002_021105a4))
        return;
    if (this->GetHealth() == 0)
        return;
    this->mVertSpeed = f;
    Sound::PlayCharVoice(this->mCharacter, 0x28, this->mCamSpacePos);
    Sound::PlayBank0(0xb6, this->mCamSpacePos);
    this->mStatePhase = 1;
    this->ChangeState(data_ov002_02110454);
}

extern "C" {

int func_ov002_020e10a8(daPly_c *c) {
    return c->IsState(data_ov002_02110454) != 0;
}
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Spin_Cleanup() {
    mdCcAcPos_c.flags &= ~0x20;
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Spin-jump, per frame. Landing hands off to another state after snapping the
 * facing angle back to mPrevAngleY. The global bit at data_0209f49c selects
 * between two spin rates and two terminal velocities.
 */

int daPly_c::St_Spin_Main() {
    extern u16 data_0209f49c[];
    func_ov002_020e28d4(0x1000, 0x1000);
    if (mIsAirborne == 0) {
        mAngleY = mPrevAngleY;
        this->ChangeState(data_ov002_02110424);
    }
    if (*(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18) & 2) {
        mTerminalVelocity = -0x9000;
        mAngleY += 0x2000;
    } else {
        mTerminalVelocity = -0xc000;
        mAngleY += 0x1800;
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Spin_Init() {
    struct dCamera_c;
    extern int *gCamera;
    mJumpedFromQuicksand = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x5f, 0, 0x1000, 0);
    mTerminalVelocity = -0x10000;
    if (mStatePhase) {
        mVertSpeed = 0x50000;
        func_ov002_020e25f0(2);
    }
    int *p = (int *)&mdCcAcPos_c.flags;
    int old = *p;
    int **cam_ptr_ptr = &gCamera;
    *p = old | 0x20;
    unsigned char pid = mPlayerNo;
    ((::dCamera_c *)*cam_ptr_ptr)->SetSpinState(pid);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_LongJump_Main() {
    func_ov002_020e28d4(0x1800, 0x800);
    if (mIsAirborne == 0) {
        this->ChangeState(data_ov002_02110424);
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Three guards get first refusal on the long jump; if any takes it, this
 * returns without entering. Otherwise: airborne flags, the jump animation,
 * an upward kick, then horizontal speed scaled by 1.5 (0x1800 in 12.4 fixed
 * point, with the usual +0x800 round-to-nearest) and clamped to 0x3c000.
 */

int daPly_c::St_LongJump_Init() {
    mJumpedFromQuicksand = 0;
    if (func_ov002_020e2be4()) {
        return 1;
    }
    if (func_ov002_020e2ba8()) {
        return 1;
    }
    if (func_ov002_020e2b6c()) {
        return 1;
    }
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x1a, 0x40000000, 0x1000, 0);
    mVertSpeed = 0x1e000;
    func_ov002_020e2ad0();
    mVertAccel = -0x2000;
    mHorzSpeed = FX_MUL(mHorzSpeed, 0x1800);
    if (mHorzSpeed >= 0x3c000) {
        mHorzSpeed = 0x3c000;
    }
    /* mCamSpacePosX/Y/Z are three consecutive words; dActor_c.h declares them
       individually rather than as one Vector3. */
    if (mIsMetal == 0) {
        Sound::PlayBank0((u32)mGroundSoundType + 0x30, mCamSpacePos);
    } else {
        Sound::PlayBank0(0xa0, mCamSpacePos);
    }
    Sound::PlayCharVoice(mCharacter, 4, mCamSpacePos);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_SideFlip_Main() {
    extern int func_ov002_020eeca8(dBgCh_Actr * a, dActor_c * b);
    extern u16 data_0209f49e[];
    extern u16 data_0209f49c[];
    func_ov002_020eeca8(&mMeshClsn, this);

    if (IsState(data_ov002_02110694)) {
        if (mVertSpeed < 0) {
            mHorzSpeed = 0x12000;
        }
        func_ov002_020e28d4(0, 0x800);
    } else {
        func_ov002_020e28d4(0x1800, 0x800);
    }

    if (mIsAirborne == 0) {
        this->ChangeState(data_ov002_02110424);
    } else {
        if (!IsState(data_ov002_021101fc)) {
            u16 r0v = *(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18);
            if (r0v & 0x400) {
                if (IsState(data_ov002_0211052c)) {
                    mPrevAngleY = mAngleY;
                }
            }
            if (func_ov002_020e2664()) {
                return 1;
            }
        } else {
            if (this->FinishedAnim()) {
                this->SetAnim(0x54, 0x40000000, 0x1000, 0);
            }
        }

        mVertAccel = -0x4000;

        if (mVertSpeed >= 0) {
            u16 r1v = *(u16*)((char*)data_0209f49c + gActivePlayerSlot * 0x18);
            if ((r1v & 2) != 0 || IsState(data_ov002_021101e4) || IsState(data_ov002_021101fc) ||
                IsState(data_ov002_0211052c)) {
                mVertAccel = -0x3400;
            }
        } else {
            if (param1 == 1 && IsState(data_ov002_0211052c)) {
                mStatePhase = 0;
                this->ChangeState(data_ov002_02110454);
                return 1;
            }
        }

        if (IsState(data_ov002_021101e4)) {
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
            u32 w = anim.currFrame;
            if ((u16)(w >> 12) == 0x10) {
                Sound::PlayBank0(0xf, mCamSpacePos);
            }
        }
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_WaterJump_Init() {
    mJumpedFromQuicksand = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x53, 0x40000000, 0x1000, 0);
    mVertSpeed = 0x2a000;
    mHorzSpeed = 0x12000;
    Sound::PlayBank0(0x18, mCamSpacePos);
    func_ov002_020e25f0(0);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_HeadstandJump_Init() {
    mJumpedFromQuicksand = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x1c, 0x40000000, 0x1000, 0);
    mVertSpeed = 0x3e000;
    mHorzSpeed = 0x18000;
    Sound::PlayCharVoice(mCharacter, 0xc, mCamSpacePos);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_WallJump_Main() {
    extern int func_ov002_020eeca8(dBgCh_Actr *, dActor_c *);
    extern unsigned short data_0209f49e[];
    extern void (daPly_c::*data_ov002_0211073c[])();
    func_ov002_020eeca8(&mMeshClsn, this);
    func_ov002_020e28d4(0x1800, 0x800);
    if (mIsAirborne == 0) {
        this->ChangeState(data_ov002_02110424);
    } else {
        if (*(unsigned short *)((char *)data_0209f49e + gActivePlayerSlot * 0x18) & 0x400) {
            if (this->IsState(data_ov002_0211052c)) {
                mPrevAngleY = mAngleY;
            }
        }
        if (func_ov002_020e2664())
            return 1;
        (this->*data_ov002_0211073c[param1])();
    }
    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * ov002, 0x020e17f8, size 0xb0. Entering the wall jump: clear the airborne
 * bookkeeping, start anim 0x28, set the launch speeds, TURN AROUND, and shout.
 *
 * The turn is the whole move. mAngleY += 0x8000 is exactly half a revolution
 * on a 16-bit angle, and mPrevAngleY is snapped to the new value in the same
 * breath so nothing interpolates through the 180 degrees -- the player faces
 * away from the wall instantly rather than spinning.
 *
 * Every offset the placeholder body carried is a named field of daPly_c or of
 * its dActor_c base, so the shadow `struct daPly_c` it used to declare is gone.
 * The vertical speed is chosen by param1: character 2 gets a fixed 0x2aa00,
 * everyone else goes through the ov002 worker with 0x3e000.
 *
 * `x = x + k` AND `x += k` ARE NOT THE SAME FUNCTION HERE, and that one line
 * is the whole difference between this file matching and not. On the s16
 * member the compound form makes mwcc materialise &mAngleY into a register --
 * `add r,r,#0x8e` then `ldrsh r,[r]` and later `strh r,[r]` -- one instruction
 * more than the ROM, which addresses the field directly off the object both
 * times (`ldrsh r,[r,#0x8e]` / `strh r,[r,#0x8e]`). Written out longhand it
 * matches. Nothing else in the function was ever wrong: the draft was 44 of 45
 * instructions correct and had been sitting unenrolled for it.
 */
int daPly_c::St_WallJump_Init() {
    mJumpedFromQuicksand = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x28, 0x40000000, 0x1000, 0);
    if (param1 == 2) {
        mVertSpeed = 0x2aa00;
    } else {
        func_ov002_020bf2d8(0x3e000);
    }
    mHorzSpeed = 0x18000;
    mAngleY = mAngleY + 0x8000;
    mPrevAngleY = mAngleY;
    mStateWork = 1;
    mStateStep = 0;
    Sound::PlayCharVoice(mCharacter, 0xc, mCamSpacePos);
    return 1;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_BackFlip_Init() {
    mJumpedFromQuicksand = 0;
    if (func_ov002_020e2be4())
        return 1;
    if (func_ov002_020e2ba8())
        return 1;
    if (func_ov002_020e2b6c())
        return 1;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    this->SetAnim(0x2a, 0x40000000, 0x1000, 0);
    mVertSpeed = 0x3e000;
    if (param1 == 1)
        mVertSpeed = 0x4c000;
    func_ov002_020e2ad0();
    mHorzSpeed = 0x10000;
    mPrevAngleY = mAngleY + 0x8000;
    func_ov002_020e25f0(0);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_SlideKickRecover_Init() {
    mJumpedFromQuicksand = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    ReleaseHeldActor();
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    mVertSpeed = 0x20000;
    this->SetAnim(0x56, 0x40000000, 0x1000, 0);
    func_ov002_020e25f0(0);
    Sound::PlayBank0(0xf, mCamSpacePos);
    return 1;
}

/* recovered: named members + shared header, real C++ method */

int daPly_c::St_SideFlip_Init() {
    mJumpedFromQuicksand = 0;
    if (func_ov002_020e2be4())
        return 1;
    if (func_ov002_020e2ba8())
        return 1;
    if (func_ov002_020e2b6c())
        return 1;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    mHorzSpeed = 0x8000;
    mVertSpeed = 0x3e000;
    func_ov002_020e2ad0();
    this->SetAnim(0x4d, 0x40000000, 0x1000, 0);
    mPrevAngleY = mDesiredAngleY;
    mAngleY = mDesiredAngleY + 0x8000;
    func_ov002_020e25f0(0);
    return 1;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Fall_Main() {
    func_ov002_020e28d4(0x1800, 0x800);
    if (mIsAirborne == 0) {
        this->ChangeState(data_ov002_02110424);
    } else if (func_ov002_020e2664()) {
        return 1;
    }
    AdvanceAnims();
    if (mHorzSpeed >= ScaleByCharFactor(0x28000))
        mHorzSpeed = ScaleByCharFactor(0x28000);
    return 1;
}

/* recovered: named members + shared header, real C++ method */
int daPly_c::St_Fall_Init() {
    mJumpedFromQuicksand = 0;
    mIsInAirState = 1;
    mIsFallScreaming = 0;
    mJumpComboStage = 0;
    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    if (this->IsAnim(0x41)) {
        this->SetAnim(0x44, 0x40000000, 0x1000, 0);
        mStateStep = 1;
    } else {
        this->SetAnim(0x54, 0x40000000, 0x1000, 0);
        mStateStep = 0;
    }
    mPeakY = mPosY;
    return 1;
}

void daPly_c::func_ov002_020e1c20() {
    extern unsigned char data_0209f49c[];
    if ((mClsnFlags) & 0xc) {
        ChangeState(data_ov002_021101b4);
        return;
    }

    if ((mStateStep) != 0) {
        if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
            if ((mClsnFlags) & 4) {
                mVertSpeed = -0x20000;
            }
            mLoopingSoundHandle =
                func_02012120(mLoopingSoundHandle, mCharacter, 0x18, &mCamSpacePos, 0);
            mHorzSpeed = FX_MUL(mHorzSpeed, 0xf00);
            if ((mVertSpeed) < 0) {
                int *p = (&mVertSpeed);
                *p = *p + 0xc00;
            } else {
                int *p = (&mVertSpeed);
                *p = *p + 0x1000;
            }
            if ((mVertSpeed) < 0x11000) {
                return;
            }
        }
        mStateStep = 0;
        mStateWork = 0;
        SetAnim(0x54, 0x40000000, 0x1000, 0);
        return;
    }

    mVertAccel = func_ov002_020bf27c(-0x4000);
    if ((mVertSpeed) >= 0) {
        mVertAccel = -0x8000;
        if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
            mVertAccel = func_ov002_020bf27c(-0x3000);
        }
        return;
    }

    if ((mVertSpeed) >= -0x8000) {
        return;
    }

    if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
        if ((mStateWork) == 1) {
            mStateStep = 1;
            mVertAccel = 0;
            SetAnim(0x3f, 0, 0x1000, 0);
            dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
            anim.speed = 0x4000;
        }
    }
}

extern "C" {

void func_ov002_020e1e70(char *c) {
    extern unsigned char data_0209f49c[];
    daPly_c *player = (daPly_c *)c;
    player->mVertAccel = -0x4000;
    player->mTerminalVelocity = -0x4b000;
    if ((player->mVertSpeed) >= 0) {
        player->mVertAccel = -0x8000;
        if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
            player->mVertAccel = -0x3400;
        }
    }
    if ((player->mVertSpeed) >= 0) {
        return;
    }
    if ((*((unsigned short *)(&data_0209f49c[gActivePlayerSlot * 0x18]))) & 2) {
        ((daPly_c *)c)->SetAnim(0x3f, 0, 0x1000, 0);
        dExtFrameCtrl_c &anim =
            *player->mModels.mBodyModels[((daPly_c *)c)->GetBodyModelID(player->param1 & 0xff, 0)];
        anim.speed = 0x4000;
        player->mLoopingSoundHandle = func_02012120(player->mLoopingSoundHandle, player->mCharacter,
                                                    0x18, &player->mCamSpacePos, 0);
        if ((player->mHasWings) != 0) {
            player->mVertAccel = player->func_ov002_020bf27c(-0x4000) >> 1;
            player->mTerminalVelocity = -0x25800;
        } else {
            player->mVertAccel = player->func_ov002_020bf27c(-0x4000);
        }
    } else if (((daPly_c *)c)->IsAnim(0x3f) != 0) {
        ((daPly_c *)c)->SetAnim(0x54, 0, 0x1000, 0);
    }
}
}

extern "C" {

void func_ov002_020e200c(char* c){
    extern unsigned char data_0209f49c[];
    daPly_c *player = (daPly_c *)c;
    player->mVertAccel = player->func_ov002_020bf27c(-0x4000);
    player->mTerminalVelocity = -0x4b000;
    if (player->mVertSpeed >= 0) {
        player->mVertAccel = -0x8000;
        if (*(unsigned short *)&data_0209f49c[gActivePlayerSlot * 0x18] & 2)
            player->mVertAccel = -0x3400;
    }
    if (player->mVertSpeed >= 0)
        return;
    if (player->mJumpComboStage == 1)
        ((daPly_c *)c)->SetAnim(0x50, 0x40000000, 0x1000, 0);
    if ((*(unsigned short *)&data_0209f49c[gActivePlayerSlot * 0x18] & 2) == 0)
        return;
    if (player->mHasWings == 0)
        return;
    player->mVertAccel = -0x2000;
    player->mTerminalVelocity = -0x25800;
}
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::St_Jump_Main() {
    extern int func_ov002_020eeca8(dBgCh_Actr *, dActor_c *);
    extern u16 data_0209f49e[];
    extern void (daPly_c::*data_ov002_0211073c[])();
    func_ov002_020eeca8(&mMeshClsn, this);
    func_ov002_020e28d4(0xd00, 0x800);

    if (mIsAirborne == 0) {
        if (mStateTimer != 0) {
            mStateFlags |= 0x100;
        }
        this->ChangeState(data_ov002_02110424);
    } else {
        {
            int off = gActivePlayerSlot * 0x18;
            u8 *p = (u8 *)((char *)data_0209f4ab + off);
            if (*p != 0) {
                *p = 5;
            }
            if (*(u16 *)((char *)data_0209f49e + off) & 2) {
                mStateTimer = 5;
            }
        }

        if (func_ov002_020e2664())
            return 1;

        if (mJumpComboStage == 2) {
            mVertAccel = -0x4000;
            mTerminalVelocity = -0x4b000;
            if (mVertSpeed >= 0) {
                mVertAccel = -0x3400;
            }

            {
                dExtFrameCtrl_c &anim = *mModels.mBodyModels[GetBodyModelID(param1 & 0xff, 0)];
                u32 w = anim.currFrame;
                u16 t = (u16)(w >> 12);
                if (t == 4 || t == 0x18 || t == 0x2c) {
                    Sound::PlayBank0(0xf, mCamSpacePos);
                }
            }
        } else {
            int idx = param1;
            if (mIsMega != 0) {
                idx = 0;
            }
            (this->*data_ov002_0211073c[idx])();
        }
    }

    AdvanceAnims();
    return 1;
}

/* recovered: named members + shared header, real C++ method
 *
 * Entering the jump state. Two guards get first refusal. Then the triple-jump
 * combo stage advances (mod 3) if the combo timer is still running, and is
 * reset by any of: sinking deeper than 0x1000, carrying something, being mega,
 * or -- at stage 2 without wings -- not running fast enough. Wings at stage 2
 * divert to a different state entirely; otherwise the per-stage launch speed
 * and animation are looked up by stage, speed is scaled by 0.8, quartered on a
 * sharp turn, and zeroed if a ledge check fires.
 */
/* launch speed, by combo stage */
/* jump animation, by combo stage */
/* the winged-jump state */
int daPly_c::St_Jump_Init() {
    int t;

    mJumpedFromQuicksand = 0;
    if (func_ov002_020e2be4() != 0)
        return 1;
    if (func_ov002_020e2b6c() != 0)
        return 1;

    mIsInAirState = 1;
    mIsFallScreaming = 0;
    if (mJumpComboTimer == 0) {
        mJumpComboStage = 0;
    } else {
        mJumpComboStage = (mJumpComboStage + 1) % 3;
    }

    // if/else around the combo clear grows this function 0x314 -> 0x318
    if (mSinkDepth > 0x1000)
        goto clear;
    t = (mHeldObj != 0);
    if (t == false)
        goto skipclear;
clear:
    mJumpComboStage = 0;
skipclear:

    if (mIsMega != 0)
        mJumpComboStage = 0;

    if (mJumpComboStage == 2 && mHasWings == 0) {
        if (mHorzSpeed <= ScaleByCharFactor(0x14000))
            mJumpComboStage = 0;
    }

    mIsAirborne = 1;
    mLandSoundPlayed = 0;
    if (mHasWings == 1 && mJumpComboStage == 2) {
        mStateStep = 0;
        this->ChangeState(data_ov002_02110214);
    } else {
        int v = data_ov002_0210a59c[mJumpComboStage] + (mHorzSpeed >> 2);
        if (mIsMega != 0) {
            func_ov002_020bf2d8(FX_MUL(v, 0x1400));
        } else {
            func_ov002_020bf2d8(v);
        }
        func_ov002_020e2ad0();
        if (mIsMega == 0) {
            this->SetAnim(data_ov002_0210a5a8[mJumpComboStage], 0x40000000, 0x1000, 0);
        } else {
            this->SetAnim(0x9c, 0x40000000, 0x1000, 0);
        }
        if (mHorzSpeed > ScaleByCharFactor(0x64000)) {
            mHorzSpeed = ScaleByCharFactor(0x64000);
        }
    }

    mStateWork = 0;
    if (param1 == 3) {
        if (mJumpComboStage != 2)
            mStateWork = 1;
    }

    mStateStep = 0;
    mStateArg = 0;
    unk_6cc = 0;
    func_ov002_020e25f0(mJumpComboStage);

    mHorzSpeed = FX_MUL(mHorzSpeed, 0xccc);

    if (AngleDiff(mPrevAngleY, mAngleY) >= 0x4000) {
        mHorzSpeed >>= 2;
    }

    if (func_ov002_020c19d0(0x64, 0x32) != 0) {
        mStateFlags |= 0x200;
        mHorzSpeed = 0;
    }
    return 1;
}

/* recovered: shared common types */

extern "C" void func_ov002_020e25d4(char *c)
{
    daPly_c *player = (daPly_c *)c;
    Sound::PlayCharVoice(player->mCharacter, 0x10, player->mCamSpacePos);
}

extern "C" {

inline int inline_fn(int *arg0) {
    return RandomIntInternal(arg0);
}
}

void daPly_c::func_ov002_020e25f0(int a) {
    extern int data_ov002_0210e160[];
    if ((mIsMetal) == 0) {
        Sound::PlayBank0((mGroundSoundType) + 0x30, mCamSpacePos);
    } else {
        Sound::PlayBank0(0xa0, mCamSpacePos);
    }
    int idx = (a * 2) + (inline_fn(data_ov002_0210e160) & 1);
    Sound::PlayCharVoice(mCharacter, data_ov002_020ff284[idx], mCamSpacePos);
}

int daPly_c::func_ov002_020e2664() {
    extern u16 data_0209f49e[];
    u16 flags;
    dActor_c *ptr;
    int b1;
    int b2;

    if (func_ov002_020d5c6c())
        return 1;

    flags = *(u16*)((char*)data_0209f49e + gActivePlayerSlot * 0x18);
    if (flags & 0x400) {
        if (IsState(data_ov002_021101e4)) {
            mAngleY = mAngleY + 0x8000;
        }
        if (func_ov002_020d674c()) {
            ChangeState(data_ov002_0211004c);
        } else {
            ChangeState(data_ov002_021105a4);
        }
        return 1;
    }

    // return 0 instead of the shared label shrinks this function 0x270 -> 0x26c
    if (!(flags & 1)) goto ret0;

    ptr = mHeldObj;
    b1 = !!ptr;
    if (b1 != false) {
        b2 = ptr->mFlags;
        b2 = ((b2 & 0x200) != 0);
        if (b2 == false) {
            ChangeState(data_ov002_021105d4);
        }
        return 1;
    }

    if (IsState(data_ov002_0211019c) && mJumpComboStage != 2) {
        if (func_ov002_020dde74())
            return 1;
        goto ret0;
    }

    if (IsState(data_ov002_021101b4) || IsState(data_ov002_02110724) ||
        IsState(data_ov002_021101fc) || IsState(data_ov002_0211052c) ||
        IsState(data_ov002_0211055c) || IsState(data_ov002_02110694))
        return 0;

    if (IsState(data_ov002_021101e4)) {
        mAngleY = mAngleY + 0x8000;
    }
    mPrevAngleY = mAngleY;

    if (mIsMega == 0) {
        if (param1 == 3) {
            ChangeState(data_ov002_02110034);
        } else {
            ChangeState(data_ov002_021105bc);
        }
    } else {
        mPunchKickStep = 2;
        ChangeState(data_ov002_02110574);
    }

    mAngleY = mPrevAngleY;
    return 1;

ret0:
    return 0;
}

void daPly_c::func_ov002_020e28d4(int a, int b) {
    extern s16 data_0209f4a0[];
    int r4;

    if (param1 == 1)
        a <<= 1;

    if (*(s16*)((char*)data_0209f4a0 + gActivePlayerSlot * 0x18) != 0) {
        if (mClsnFlags & 2) {
            int t = cstd::atan2(mWallNormalX, mWallNormalZ);
            if (AngleDiff((s16)(t + 0x8000), mPrevAngleY) < 0x2000) {
                if (mHorzSpeed >= 0x3000)
                    mHorzSpeed = 0x3000;
            }
        }
        if (IsState(data_ov002_0211055c)) {
            r4 = ScaleByCharFactor(0x3c000);
            s16 e = AngleDiff(mDesiredAngleY, mAngleY);
            e = e + 0x4000;
            s16 tv = FX_SinCosTable_[((u16)e >> 4) * 2];
            r4 = FX_MUL(r4, tv);
            if (r4 < 0)
                r4 = -r4;
            if (r4 < 0xa000)
                r4 = 0xa000;
        } else {
            r4 = func_ov002_020bf224(ScaleByCharFactor(0x21000), ScaleByCharFactor(0xa000));
        }

        s16 r7 = mDesiredAngleY;
        if (mHorzSpeed != 0) {
            if (AngleDiff(mPrevAngleY, r7) > 0x6000) {
                r7 = r7 + 0x8000;
                r4 = -r4;
            }
            ApproachAngle(&mPrevAngleY, r7, 0x10, 0x100, 0);
        } else {
            mPrevAngleY = r7;
        }
    } else {
        r4 = 0;
        a = b;
    }

    ApproachLinear(mHorzSpeed, r4, a);

    if (AngleDiff(mPrevAngleY, mAngleY) < 0x4000)
        return;

    if (mHorzSpeed >= 0x10000)
        mHorzSpeed -= 0x2000;
}

void daPly_c::func_ov002_020e2ad0() {
    if (mSinkDepth <= 0x1000)
        return;
    if (mQuicksandJumpTimer == 0) {
        mVertSpeed = FX_MUL(mVertSpeed, 0x900);
        mHorzSpeed >>= 1;
        mJumpedFromQuicksand = 1;
    } else {
        if (mSinkDepth < 0xb000)
            mSinkDepth = 0xb000;
    }
}

int daPly_c::func_ov002_020e2b6c() {
    if (mSinkDepth < 0xb000)
        return 0;
    ChangeState(data_ov002_021104cc);
    return 1;
}

int daPly_c::func_ov002_020e2ba8() {
    if (mIsMega == 0)
        return 0;
    ChangeState(data_ov002_0211019c);
    return 1;
}

int daPly_c::func_ov002_020e2be4() {
    int thr;
    if (func_ov002_020c04ac())
        return 0;
    switch (func_ov002_020c031c()) {
    case 4:
    case 5:
        thr = 0xf74;
        break;
    case 3:
        thr = 0xf04;
        break;
    default:
        thr = 0xddb;
        break;
    case 1:
    case 2:
        thr = 0xddb;
        break;
    }
    if (mFloorNormalY > thr)
        return 0;
    ChangeState(data_ov002_021103f4);
    return 1;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

int daPly_c::func_ov002_020e2c84() {
    extern dCamera_c *gCamera;
    int diff;

    diff = mPeakY - mPosY;
    func_ov002_020e301c();

    if (func_ov002_020e3078(&data_ov002_02110454) != 0 ||
        func_ov002_020e3078(&data_ov002_02110484) != 0 || mIsMega != 0)
        return 0;

    if (TryEnterBigFallLanding() != 0)
        return 2;

    if (diff > 0xbb8000) {
        mStateStep = 2;
        mHurtDamage = 0;
        ChangeState(data_ov002_02110094);

        if (func_ov002_020d91e0(0x400, 1) != 0) {
            {
                u8 *p = &mStateStep;
                *p &= 1;
            }
            ChangeState(data_ov002_0211010c);
            return 1;
        }

        gCamera->func_0200d8c8(&mPos, 0x7d0000);
        mIsFallScreaming = 0;
        Sound::PlayCharVoice(mCharacter, 7, mCamSpacePos);
    } else if (diff > 0x47e000) {
        if (func_ov002_020c0cbc() == 0) {
            if (func_ov002_020d91e0(0x200, 1) != 0) {
                if (IsState(data_ov002_02110094) != 0) {
                    {
                        u8 *p = &mStateStep;
                        *p &= 1;
                    }
                    ChangeState(data_ov002_0211010c);
                } else {
                    func_ov002_020c5dec(2);
                }
                return 1;
            }

            func_ov002_020db8bc(1);
            mVertSpeed = 0;
            gCamera->func_0200d8c8(&mPos, 0x7d0000);
            mIsFallScreaming = 0;
            Sound::PlayCharVoice(mCharacter, 7, mCamSpacePos);
        }
    }

    mPeakY = mPosY;
    return 0;
}

int daPly_c::TryEnterBigFallLanding() {
    signed char c = gCurrentSublevel;

    // return 0 at each check grows this function 0x17c -> 0x188
    if (c == 0x13 || c == 0xa || c == 0x10) {
        /* proceed */
    } else if (c != 0x22) {
        goto ret0;
    }

    int val = mSurfaceType;
    if (val == 1 || val == 0xe) {
        return 0;
    }

    if (!mMeshClsn.IsOnGround()) {
        return 0;
    }

    dBgPi *cr = mMeshClsn.GetFloorResult();
    if (cr->GetClsnID() != -1) {
        if (dActor_c::FindWithID(cr->GetClsnID()) != 0) {
            return 0;
        }
    }

    int diff = mPeakY - mPosY;
    if (diff <= 0x3e8000) {
        goto ret0;
    }
    if (mFloorNormalY < 0xddb) {
        goto ret0;
    }

    mStateStep = 0;
    if (IsState(data_ov002_021105bc)) {
        mStateStep = 1;
    } else if (IsState(data_ov002_021105a4)) {
        mStateStep = 2;
    } else {
        if (func_ov002_020e3078(&data_ov002_021101e4)) {
            mAngleY = mPrevAngleY;
        }
    }

    mStateArg = 0;
    if (c == 0x10) {
        mStateArg = 1;
    } else if (c == 0x22) {
        mStateArg = 2;
    }

    ChangeState(data_ov002_0211040c);
    return 1;

ret0:
    return 0;
}

void daPly_c::func_ov002_020e301c() {
    extern dCamera_c *gCamera;
    if (param1 != 2)
        return;
    if (mPeakY - mPosY < 0x190000)
        return;
    gCamera->func_0200d8c8(&mPos, 0x320000);
}

struct Player3078 { char p[0x400]; };
int daPly_c::func_ov002_020e3078(void *s) {
    return mPrevState == s;
}

/* recovered: real C++ method, named members + shared header */
/* Five instructions, and every one of them is in the source now:
     ldr r0, [r0, #0x370]   -> mState
     cmp r0, r1             -> == &state
     moveq r0, #1 / movne r0, #0
     bx lr
   The moveq/movne pair is the compiler normalising to a truth value, which is
   what makes bool the honest return type rather than int -- contrast the
   dBgCh_Actr accessors, which return a raw mask straight out of an `and`.

   The parameter is a reference, so the caller passes an address and this
   compares pointer identity: two distinct State objects with identical
   contents are NOT the same state. That is the whole semantic. */
bool daPly_c::IsState(daPly_c::State &state) {
    return mState == &state;
}

/* recovered: named members + shared header, real C++ method,
 * declarations from a shared header
 *
 * The parameter is a REFERENCE -- the ROM name says RNS_5StateE -- so it is
 * taken as one and bound to a local pointer, leaving every expression in the
 * body exactly as it was.
 */
/* recovered: named members + shared header */

int daPly_c::ChangeState(daPly_c::State &state) {
    extern void *gCamera;
    daPly_c::State *newState = &state;
    mRequestedState = newState;

    {
        daPly_c::State *cur;
        int ok;
        /* The null-checks below stay as raw offset arithmetic on purpose.
           Spelling either as `*(void**)&cur->mCleanup == 0` costs 4 words:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise
           the whole 8-byte pmf first. Reading one to CALL it is free -- the
           calls below use the named members. Measured both ways. */
        cur = mState;
        if (cur == 0)
            ok = 1;
        else if (*(int *)((char *)cur + 0x10) == 0)
            ok = 1;
        else
            ok = (this->*(cur->mCleanup))();
        if (!ok) return 0;
    }

    if (mState == &data_ov002_0211022c
        && (unsigned short)(mStateFlags & 0x400) == 0
        && mIsNoControl != 0
        && newState != &data_ov002_0211013c
        && newState != &data_ov002_0211067c
        && newState != &data_ov002_021106ac
        && newState != &data_ov002_0211022c)
    {
        return 0;
    }

    {
        int b = (mHeldObj != 0);
        if (b) {
        if (newState == &data_ov002_0211013c) {
            mStateStep = 1;
            newState = &data_ov002_02110364;
        }
        }
    }

    mLoopingSoundHandle = 0;
    mParticle3 = 0;
    mParticle2 = mParticle3;
    mParticle1 = mParticle2;

    mPrevState = mState;
    mState = newState;

    mHideShadow = 0;
    mIsBodyClsnEnabled = 1;
    mSkipGroundUpdate = 0;

    mModelOffsetZ = 0;
    mModelOffsetY = mModelOffsetZ;

    mIsInAirState = 0;
    mTongueHitDist = 0;

    mAngleZ = 0;
    mAngleX = mAngleZ;

    mIsControlDisabled = 0;
    mDetectWater = 0;
    mAttackClashed = 0;

    mStateFlags &= ~0x200;

    {
        int v1 = 0x4000;
        int v2 = 0x4b000;
        v1 = -v1;
        v2 = -v2;
        mVertAccel = v1;
        mTerminalVelocity = v2;
    }

    mStateTimer = 0;

    func_ov002_020d4540();

    mIsTakingDamage = 0;
    func_ov002_020c9e18();

    func_ov002_020e6780();

    if (gCamera != 0) {
        ((::dCamera_c *)gCamera)->func_0200d81c(mPlayerNo);
    }

    {
        daPly_c::State *s = mState;
        if (*(int*)s == 0) return 1;
        return (this->*(s->mInit))();
    }
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* _020bdef0, _020bdd9c, _020e032c and __cxa_vec_delete come from decl_common.h
   above; redeclaring one here with `char *` where that header says `void *`
   is not a redeclaration but an attempt to overload a C-linkage name, which
   mwccarm rejects outright. func_ov002_020bdd2c is a daPly_c member now and
   comes from daPly_c.h. */

int daPly_c::CleanupResources() {
    enum { false_, true_ };
    struct VB { virtual void v0(); virtual void v1(); };
    void UnloadSilverStarAndNumber(void);
    void UnloadKeyModels(int n);
    extern u8 gGameMode;
    extern SharedFilePtr data_ov002_0210e3d8;
    extern SharedFilePtr data_ov002_0210e750;
    extern SharedFilePtr data_ov002_0210eb88;
    int i;
    u32 b;

    func_ov002_020bdd2c();
    func_ov002_020bdef0();
    func_ov002_020bdd9c();
    func_ov002_020e032c();
    for (i = 0; i < 4; i++) {
        int j;
        VB *p = (VB *)mModels.mBodyModels[i];
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
        p = (VB *)mModels.mHeadModels[i];
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
        j = i + 4;
        p = (VB *)mModels.mHeadModels[j];
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
        {
            int *q = mModels.mBodyTexParams[i];
            if (q != 0)
                ::operator delete[]((void *)q);
            q = mModels.mHeadTexParams[i];
            if (q != 0)
                ::operator delete[]((void *)q);
            q = mModels.mHeadTexParams[j];
            if (q != 0)
                ::operator delete[]((void *)q);
        }
    }
    {
        VB *p = (VB *)mModels.mBodyModels[4];
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
        p = (VB *)mModels.mSleepBubbleModel;
        if (p != 0) {
            if (p != 0)
                p->v1();
        }
    }
    {
        if (mTrailPos != 0)
            delete[] mTrailPos;
        if (mTrailFlags != 0)
            ::operator delete[](mTrailFlags);
        if (mHeldObjQueue != 0)
            ::operator delete[]((void *)mHeldObjQueue);
    }
    ((data_ov002_020ff480[mCharFileBase + (param1 & 3)]))->Release();
    b = gGameMode;
    b = b == 1;
    if (b != false_)
        func_ov002_020bebd4();
    data_ov002_0210ec50.Release();
    data_ov002_0210ebb8.Release();
    data_ov002_0210ec60.Release();
    data_ov002_0210e818.Release();
    data_ov002_0210e690.Release();
    data_ov002_0210eb20.Release();
    data_ov002_0210e758.Release();
    data_ov002_0210ec38.Release();
    data_ov002_0210eb00.Release();
    data_ov002_0210e790.Release();
    data_ov002_0210e7a8.Release();
    data_ov002_0210e7f0.Release();
    data_ov002_0210e798.Release();
    data_ov002_0210e4f0.Release();
    data_ov002_0210e3d8.Release();
    data_ov002_0210eb88.Release();
    data_ov002_0210e6c0.Release();
    data_ov002_0210eb98.Release();
    data_ov002_0210e4c0.Release();
    data_ov002_0210e500.Release();
    data_ov002_0210e1c8.Release();
    data_ov002_0210e770.Release();
    data_ov002_0210e230.Release();
    data_ov002_0210e288.Release();
    data_ov002_0210e478.Release();
    data_ov002_0210e9d0.Release();
    data_ov002_0210ea10.Release();
    data_ov002_0210e538.Release();
    data_ov002_0210eac8.Release();
    data_ov002_0210e898.Release();
    data_ov002_0210e9e8.Release();
    data_ov002_0210eba8.Release();
    data_ov002_0210eb70.Release();
    data_ov002_0210eaf0.Release();
    data_ov002_0210e640.Release();
    data_ov002_0210e670.Release();
    data_ov002_0210e680.Release();
    data_ov002_0210e438.Release();
    data_ov002_0210e3b0.Release();
    data_ov002_0210e3c8.Release();
    data_ov002_0210e408.Release();
    data_ov002_0210e400.Release();
    data_ov002_0210ecb8.Release();
    data_ov002_0210e8d0.Release();
    data_ov002_0210ebd8.Release();
    b = gGameMode;
    b = b == 1;
    if (b == false_) {
        data_ov002_0210d9a8.Release();
        data_ov002_02110aa4.Release();
        data_ov002_0210da38.Release();
        data_ov002_0210e750.Release();
        data_ov002_0210e588.Release();
        data_ov002_0210e7e8.Release();
        data_ov002_0210e450.Release();
        data_ov002_0210e788.Release();
        data_ov002_0210e1e8.Release();
        data_ov002_0210e1e0.Release();
        data_ov002_0210e4d0.Release();
        data_ov002_0210e4e8.Release();
        data_ov002_0210e1d0.Release();
        data_ov002_0210e800.Release();
        data_ov002_0210e4b8.Release();
        data_ov002_0210e270.Release();
        data_ov002_0210e458.Release();
        data_ov002_0210ea70.Release();
        data_ov002_0210e540.Release();
        data_ov002_0210e8e8.Release();
        data_ov002_0210eca8.Release();
        data_ov002_0210ec98.Release();
        data_ov002_0210e600.Release();
        data_ov002_0210ea88.Release();
        data_ov002_0210e910.Release();
        data_ov002_0210e460.Release();
        data_ov002_0210e2f0.Release();
        data_ov002_0210e958.Release();
        data_ov002_0210eb38.Release();
        data_ov002_0210e660.Release();
        data_ov002_0210ea30.Release();
        data_ov002_0210e3e8.Release();
        data_ov002_0210eb90.Release();
        data_ov002_0210e6b8.Release();
        data_ov002_0210e620.Release();
        data_ov002_0210e6a0.Release();
        data_ov002_0210eb58.Release();
        data_ov002_0210e3e0.Release();
        data_ov002_0210eb10.Release();
        data_ov002_0210e708.Release();
        data_ov002_0210ebc8.Release();
        data_ov002_0210e6e0.Release();
        data_ov002_0210e430.Release();
        data_ov002_0210ec40.Release();
        data_ov002_0210e738.Release();
        data_ov002_0210e250.Release();
        data_ov002_0210ec10.Release();
        data_ov002_0210ec00.Release();
        data_ov002_0210e728.Release();
    }
    if (mLoadedResourceFlags & 1)
        UnloadSilverStarAndNumber();
    if (mLoadedResourceFlags & 2)
        data_ov002_0210da40.Release();
    if (mLoadedResourceFlags & 4)
        data_ov002_0210d9a0.Release();
    if (mLoadedResourceFlags & 8)
        data_ov002_0210d9c0.Release();
    if (mLoadedResourceFlags & 0x10)
        UnloadKeyModels(mKeyModelId);
    b = gGameMode;
    b = b == 1;
    if (b == false_) {
        if (mLoadedResourceFlags & 0x20) {
            data_ov002_0210e6f8.Release();
            data_ov002_0210e7b0.Release();
            data_ov002_0210e780.Release();
        }
    } else {
        data_ov002_0210e7b0.Release();
    }
    if (mLoadedResourceFlags & 0x40) {
        u32 idx = mHatCharacter;
        if (idx == param1)
            idx = mPrevCharacter;
        ((data_ov002_020ff480[mCharFileBase + idx]))->Release();
    }
    return 1;
}

/* recovered: named members + shared header, real C++ method */
/* daPly_c::OnPendingDestroy() at 0x020e3a04 (ov002) -- vtable slot 12.
 * Empty override; daPly_c does nothing when marked for destruction.
 */
void daPly_c::OnPendingDestroy() {
    this;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */

int daPly_c::Render() {
    struct VObj {
        virtual void d00();
        virtual void d04();
        virtual void d08();
        virtual void d0c();
        virtual void d10();
        virtual void m14(void* a);
        virtual void m18(void* a, void* b);
    };
    extern u8 gGameMode;
    extern int data_020a0e68;
    int t = (gGameMode == 1);
    if (t != false) {
        if (data_0209fc5c[mPlayerNo] == 0)
            return 1;
    }
    if (this->IsState(data_ov002_021100c4)) {
        if (mInvincibleTimer & 2)
            return 1;
    } else {
        if (mInvincibleTimer & 1)
            return 1;
    }
    if (mOpacity == 0)
        return 1;
    if (!(mFlags & 0x10)) {
        dExtAnm2Model_c *bodyMdl;
        int i;
        bodyMdl = mModels.mBodyModels[this->GetBodyModelID(mBodyModelId, 1)];
        if (mIsBalloon == 0) {
            bodyMdl->ApplyOpacity(mOpacity, 0);
            {
                int t2 = (gGameMode == 1);
                if (t2 != false) {
                    if (param1 == 3) {
                        int *p = (int *)(int)&bodyMdl->data;
                        BMD_File *hdr = (BMD_File *)p[0];
                        BMD_Material *rec = (BMD_Material *)p[1];
                        u32 i2;
                        for (i2 = 0; i2 < hdr->numMaterials; i2++) {
                            rec->texImageParam = mVsYoshiMaterialParam;
                            rec++;
                        }
                    }
                }
            }
            bodyMdl->dExtSimpleModel_c::Render(&mScale);
            if (param1 == 1) {
                mModels.mTexSeqPlayer[0].Update(mModels.mBodyModels[1]->data);
                mModels.mTexSeqPlayer[0].currFrame = mPlayerTexFrame << 12;
            }
            func_ov002_020e3e00(bodyMdl, &mScale, mOpacity);
            if (mDrawnHasWings != 0) {
                mModels.mAnmModel2.ApplyOpacity(mOpacity, 0);
                if (mBodyModelId == 3) {
                    Matrix4x3_FromTranslation((Matrix4x3 *)&data_020a0e68, -0x1b33, -0x666, 0);
                    Matrix4x3_ApplyInPlaceToRotationZ((Matrix4x3 *)&data_020a0e68, (short)0x4000);
                    MTX_Concat43((const Matrix4x3 *)&data_020a0e68, &bodyMdl->data.transforms[8],
                                 (Matrix4x3 *)&data_020a0e68);
                    ((VObj *)((char *)&mModels.mAnmModel2))->m18(&data_020a0e68, &mScale);
                } else {
                    ((VObj *)((char *)&mModels.mAnmModel2))
                        ->m18(&bodyMdl->data.transforms[15], &mScale);
                }
            }
        } else {
            mModels.mAnmModel1.ApplyOpacity(mOpacity, 0);
            ((VObj *)((char *)&mModels.mAnmModel1))->m14((char *)&mScaleX);
        }
        i = func_ov002_020becf4(mBodyModelId, 1);
        {
            dExtSimpleModel_c *mdl4 = mModels.mHeadModels[i];
            if (mdl4 != 0 && i != 9 && i != 8) {
                mdl4->ApplyOpacity(mOpacity, 0);
                {
                    int t3 = (gGameMode == 1);
                    if (t3 != false) {
                        if (param1 == 3) {
                            int *p = (int *)(int)&mModels.mHeadModels[i]->data;
                            BMD_File *hdr = (BMD_File *)p[0];
                            BMD_Material *rec = (BMD_Material *)p[1];
                            u32 i2;
                            for (i2 = 0; i2 < hdr->numMaterials; i2++) {
                                rec->texImageParam = mVsYoshiMaterialParam;
                                rec++;
                            }
                        }
                    }
                }
                if (i == 3) {
                    mModels.mHeadModels[i]->dExtSimpleModel_c::Render(&mScale);
                } else {
                    Matrix4x3 *dst = mModels.mHeadModels[i]->data.transforms;
                    Matrix4x3 *src = &bodyMdl->data.transforms[15];
                    *dst = *src;
                    ((VObj *)(char *)mModels.mHeadModels[i])->m14((char *)&mScaleX);
                }
                if (param1 == 1) {
                    mModels.mTexSeqPlayer[1].Update(mModels.mHeadModels[1]->data);
                    mModels.mTexSeqPlayer[1].currFrame = mPlayerTexFrame << 12;
                }
                if (func_ov002_020bea7c() == 0) {
                    mModels.mTexSeqBody[mBodyModelId].Update(mModels.mHeadModels[i]->data);
                }
                func_ov002_020e3e00((dExtSimpleModel_c *)(char *)mModels.mHeadModels[i], &mScale,
                                    mOpacity);
                if (func_ov002_020d225c()) {
                    ((VObj *)mModels.mSleepBubbleModel)->m14((char *)&mSleepBubbleScaleX);
                }
            }
        }
    }
    return 1;
}

/* recovered: shared common types */
//
/* decl_common.h is deliberately NOT included. This file declares its callees
   concretely -- `const int *`, `unsigned int`, `dExtSimpleModel_c *` -- while the header
   declares the same symbols with generic `void*`/`int`. Under `extern "C"`
   those are not compatible redeclarations but illegal overloads, so the file
   did not compile AT ALL (errors on the MTX_Concat33, ClearMaterialFlags and
   func_ov002_020e3e00 lines) for as long as the include was here -- which is
   why no gate ever noticed: an uncompilable file is dropped by all of them.
   Only two declarations were ever needed from the header; they are restated
   below under their real names. */
/* Were spelled `func_02111b64` / `func_02111b6c` -- invented names that
       exist in no symbols.txt anywhere in config/. ov002's own relocs record
       both sites as `kind:load` to an address claimed by 17 and 13 modules
       respectively (config/arm9/overlays/ov002/relocs.txt:6676-6677). A `load`
       cannot answer a function, which rules out ov012/ov025/ov045/ov052; of
       the data claimants that remain, ov055 alone declares BOTH addresses
       unambiguous `kind:bss`. ov017 and ov033 are clean at 0x02111b6c but
       ambiguous at 0x02111b64; every other candidate is ambiguous at both. */

extern "C" void func_ov002_020e3e00(dExtSimpleModel_c *obj, Vector3 *vec, unsigned int opacity) {
    extern int data_020a0e68[12];
    void func_0203c178(int *dst, int a, int b, int c);
    void MTX_Concat33(int *dst, const int *a, const int *b);
    int ip = (gCurrentSublevel == 0x2f) ? 1 : 0;

    if (!ip) {
        if (gCurrentSublevel != 5)
            return;
        if (gCurrentAreaId != 3)
            return;
    }

    if (ip) {
        int ok;
        if (data_ov055_02111b6c == 0 || (data_ov055_02111b64 & 0x20000) != 0)
            ok = 1;
        else
            ok = 0;
        if (!ok) return;
    }

    Matrix4x3 tmp;
    Matrix4x3 *src = &obj->mat4x3;
    tmp = *src;

    int r0;
    if (ip)
        r0 = 0;
    else
        r0 = 0x1086000;
    int t = r0 >> 3;
    t <<= 1;
    int diff = t - src->m[9];
    src->m[9] = diff;

    func_0203c178(data_020a0e68, -0x1000, 0x1000, 0x1000);
    MTX_Concat33((int *)src, data_020a0e68, (int *)src);

    obj->ClearMaterialFlags(0x80);
    obj->SetMaterialFlags(0x40);
    obj->dExtSimpleModel_c::Render(vec);
    obj->ApplyOpacity(opacity, 0);
    obj->SetMaterialFlags(0x80);
    obj->ClearMaterialFlags(0x40);

    *src = tmp;
}

int daPly_c::func_ov002_020e3f90() {
    extern Matrix4x3 data_020a0e68;
    struct S {
        short angs[3];
        volatile int v[3];
        int tgt[3];
    } s; // without volatile, the function shrinks 0x3e4 to 0x39c
    int b;
    int t;
    int t1;
    int t2;
    daPly_c *p;
    unsigned int n;
    char* m;

    if (mGrabbedByActor == 0)
        return 0;
    if (mGrabbedByActor->mCarryMatrix == 0)
        return 0;

    b = 0;
    s.v[0] = 0;
    s.v[1] = 0;
    s.v[2] = 0;
    data_020a0e68 = **(Matrix4x3 **)&((dActor_c *)*(char **)&mGrabbedByActor)->mCarryMatrix;
    p = (daPly_c *)mGrabbedByActor;
    if ((p)->actorID == 0xbf)
        b = 1;
    if (b != 0) {
        t = (unsigned int)(p->mHeldObj != 0);
        if (t == 0) return 0;
        t = (unsigned int)(((int)mFlags & 0x4000) != 0);
        if (t == 0) return 0;
        if ((unsigned int)(((int)p->mCharFileBase >> 2) - 0x6c) <= 1) {
            s.tgt[0] = -0x3000;
            s.tgt[1] = -0x1000;
            s.tgt[2] = -0x4000;
            s.angs[0] = -0x2000;
            s.angs[1] = -0x1000;
            s.angs[2] = -0x1000;
        } else {
            s.tgt[0] = 0;
            s.tgt[1] = -0x3000;
            s.tgt[2] = -0x4000;
            s.angs[0] = 0x1000;
            s.angs[1] = 0x7000;
            s.angs[2] = 0;
        }
        ApproachLinear(mStateTargetX, s.tgt[0], 0x400);
        ApproachLinear(mStateTargetY, s.tgt[1], 0x400);
        ApproachLinear(mStateTargetZ, s.tgt[2], 0x400);
        ApproachAngle(&mAngleX, s.angs[0], 0x10, 0x1000, 0x100);
        ApproachAngle(&mAngleY, s.angs[1], 0x10, 0x1000, 0x100);
        ApproachAngle(&mAngleZ, s.angs[2], 0x10, 0x1000, 0x100);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, mStateTargetX, mStateTargetY,
                                            mStateTargetZ);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    } else {
        t = (unsigned int)((p)->actorID == 0xbd);
        if (t != 0) {
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x5800, -0x6800, -0x2000);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x3c71, 0x38e, -0x4e38);
        } else {
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x5800, 0x5000, 0xb000);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, -0x600, -0x2600, 0);
        }
    }
    mShadowMatrix = data_020a0e68;
    t = (&data_020a0e68)->m[9];
    t1 = ((volatile Matrix4x3 *)&data_020a0e68)->m[10]; // without volatile, 7 words differ
    t2 = ((volatile Matrix4x3 *)&data_020a0e68)->m[11]; // without volatile, 10 words differ
    s.v[1] = t1;
    s.v[2] = t2;
    s.v[0] = t;
    mPosX = t << 3;
    mPosY = s.v[1] << 3;
    mPosZ = s.v[2] << 3;
    mPrevAngleY = mGrabbedByActor->mAngleY;
    n = GetBodyModelID(param1 & 0xff, 0);
    m = (char *)mModels.mBodyModels[n];
    ((dExtSimpleModel_c *)m)->mat4x3 = data_020a0e68;
    mModels.mAnmModel2.mat4x3 = data_020a0e68;
    n = func_ov002_020becf4(param1 & 0xff, 0);
    if (n != 9 && n != 8) {
        m = (char *)mModels.mHeadModels[n];
        if (m != 0) {
            ((dExtSimpleModel_c *)m)->mat4x3 = data_020a0e68;
        }
    }
    return 1;
}

void daPly_c::func_ov002_020e4374(int *p1, int *p2) {
    int v;
    int m;
    *p1 = (mModelOffsetY + (mPosY - mSinkDepth) + 0xf000) - mGroundY;
    if (mIsBalloon != 0)
        *p1 -= 0x70000;
    if (*p1 <= 0x1000) *p1 = 0x1000;
    m = 0x64000;
    if (mIsMega != 0) {
        m = mScaleY * 0x64;
    } else if (mIsBalloon != 0) {
        m = 0xc8000;
    }
    v = FX_MUL(*p1, 0x180);
    *p2 = m - v;
    if (*p2 < 0xa000) *p2 = 0xa000;
    *p1 += 0x28000;
}

/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

void daPly_c::func_ov002_020e444c() {
    extern struct Matrix4x3 data_020a0e68;
    int sp8, spC;
    int b;

    if (func_ov002_020e3f90() == 0) {
        int y = mModelOffsetY + (mPosY - mSinkDepth) + 0xf000;
        if (mIsBalloon != 0)
            y -= 0x70000;
        Matrix4x3_FromTranslation(&data_020a0e68, mPosX >> 3, y >> 3, mPosZ >> 3);
        mShadowMatrix = data_020a0e68;

        {
            int out[3];
            int src[3];
            int offsetZ = mModelOffsetZ;
            src[1] = mModelOffsetY - mSinkDepth;
            src[2] = offsetZ;
            src[0] = 0;
            Vec3_RotateYAndTranslate((Vector3 *)out, (const Vector3 *)&mPosX, mAngleY,
                                     (const Vector3 *)src);
            Matrix4x3_FromTranslation(&data_020a0e68, out[0] >> 3, out[1] >> 3, out[2] >> 3);
        }

        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);

        if (mIsBalloon == 0) {
            int bodyID = GetBodyModelID(param1 & 0xff, 0);
            mModels.mBodyModels[bodyID]->mat4x3 = data_020a0e68;
            mModels.mAnmModel2.mat4x3 = data_020a0e68;
        } else {
            mModels.mAnmModel1.mat4x3 = data_020a0e68;
        }

        {
            int r0 = func_ov002_020becf4(param1 & 0xff, 0);
            if (r0 != 9 && r0 != 8) {
                dExtSimpleModel_c *q = mModels.mHeadModels[r0];
                if (q != 0) {
                    q->mat4x3 = data_020a0e68;
                }
            }
        }

        if (func_ov002_020d225c() != 0) {
            int z = FX_MUL(mSleepBubbleScaleZ, 0x2400) + 0xc000;
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x7000, z);
            mModels.mSleepBubbleModel->mat4x3 = data_020a0e68;
        }
    }

    func_ov002_020e4374(&sp8, &spC);

    if ((mOpacity | mIsVanish) == 0)
        mHideShadow = 1;

    if (mMeshClsn.IsOnGround() != 0) {
        dBgPi *fr = mMeshClsn.GetFloorResult();
        if (fr->GetClsnID() != -1) {
            void *a = dActor_c::FindWithID(fr->GetClsnID());
            if (a != 0) {
                b = (int)(((dActor_c *)a)->actorID == 0xa1);
                if (b != 0) {
                    sp8 += 0x3c000;
                }
            }
        }
    }

    if (mHideShadow != 0)
        return;

    DropShadowRadHeight(mShadowModel, mShadowMatrix, spC, sp8, 0xf);
}

// local extern: include-free shard, the call passes the object as an untyped pointer so it cannot use the header member

void daPly_c::func_ov002_020e4768() {
    struct VTable4 {
        virtual void v0();
        virtual void v1();
        virtual void v2();
        virtual void v3();
        virtual void DoThing(int arg);
    };
    extern Matrix4x3 data_020a0e68;
    unsigned int mid;
    dExtAnm2Model_c *comp;
    int idx;

    mid = GetBodyModelID(param1 & 0xff, 0);
    comp = mModels.mBodyModels[mid];
    func_020167a4((struct dExtAnmModel_c *)(void *)comp);

    func_ov002_020e640c();

    mid = GetBodyModelID(param1 & 0xff, 0);
    comp = mModels.mBodyModels[mid];
    comp->data.UpdateVertsUsingBones();

    idx = func_ov002_020becf4(param1 & 0xff, 0);

    if (idx == 3) {
        unsigned int m2 = GetBodyModelID(param1 & 0xff, 0);
        int a1 = (int)&mModels.mBodyModels[m2]->data.transforms[15];
        a1 = idx ? a1 : a1;
        ((VTable4 **)mModels.mHeadModels)[idx]->DoThing(a1);
    }

    {
        int hasFlag = (mHeldObj != 0);
        if (hasFlag) {
            mid = GetBodyModelID(param1 & 0xff, 0);
            comp = mModels.mBodyModels[mid];
            data_020a0e68 = comp->mat4x3;

            mid = GetBodyModelID(param1 & 0xff, 0);
            comp = mModels.mBodyModels[mid];
            MTX_Concat43(&comp->data.transforms[14], &data_020a0e68, &data_020a0e68);

            mHeldObjMatrix = data_020a0e68;
            mCarryMatrix = &mHeldObjMatrix;
        }
    }

    mid = GetBodyModelID(param1 & 0xff, 0);
    comp = mModels.mBodyModels[mid];
    data_020a0e68 = comp->mat4x3;

    mid = GetBodyModelID(param1 & 0xff, 0);
    comp = mModels.mBodyModels[mid];
    MTX_Concat43(&comp->data.transforms[15], &data_020a0e68, &data_020a0e68);

    mHeadMatrix = data_020a0e68;
}

extern "C" {

int func_ov002_020e496c(daPly_c *c) {
    daPly_c *player = c;
    int id = (c)->GetBodyModelID(player->mBodyModelId, 1);
    return ((int *)player->mModels.mBodyModels)[id];
}
}

int daPly_c::func_ov002_020e4990() {
    int id;
    int b;
    id = GetBodyModelID(param1 & 0xff, 0);
    func_0201628c(((int *)mModels.mBodyModels)[id], 8);
    b = mHeldObj ? 1 : 0;
    if (b == 0)
        return 0;
    if (IsAnim(0x30) || IsAnim(0x2f) || IsAnim(0x33) || IsAnim(0x18) || IsAnim(0x8a) ||
        IsAnim(0x87) || IsAnim(0x8b) || IsAnim(0x6d) || IsAnim(0x6f) || IsAnim(0x6c) ||
        IsAnim(0x91) || IsAnim(0x8e) || IsAnim(0x90) || IsAnim(0x8f) || IsAnim(0x43) ||
        IsAnim(0x5b) || IsAnim(0x58) || IsAnim(0x81) || IsAnim(0x80) || IsAnim(0xaf) ||
        IsAnim(0xb0))
        return 0;
    id = GetBodyModelID(param1 & 0xff, 0);
    func_020162a4(((int *)mModels.mBodyModels)[id], 8);
    return 1;
}

void daPly_c::func_ov002_020e4bb8() {
    u8 save_6fd;
    u8 save_6f9;
    u8 save_6fb;
    u8 save_6ff;

    func_ov002_020be3b0();

    save_6fd = mIsBalloon;
    save_6f9 = mIsMetal;
    save_6fb = mIsVanish;
    save_6ff = mHasWings;

    mPowerupBlinkOn = 0;
    if (mPowerupTimer < 0x3f) {
        mPowerupBlinkOn = (u8)(mPowerupTimer & 1);
        if (mPowerupBlinkOn == 0) {
            if (mHasWings == 1 || mIsVanish != 0) {
                mIsVanish = 0;
                mHasWings = mIsVanish;
            }
            if (mIsMetal != 0) {
                u32 v;
                u32 id;
                dExtAnm2Model_c *dst;
                dExtAnm2Model_c *src;
                SharedFilePtr *poolEntry;
                char* p;

                mIsMetal = 0;
                v = param1;
                id = GetBodyModelID((u8)v, 0);
                dst = mModels.mBodyModels[id];
                src = mModels.mBodyModels[4];
                poolEntry = data_ov002_020ff480[mCharFileBase + v];
                p = (char *)poolEntry->filePtr;
                dst->Copy(*src, p, 0);
            }
        }
    }

    if (mBalloonTimer < 0x3f) {
        if (mIsBalloon != 0) {
            mIsBalloon = (u8)(mBalloonTimer & 1);
            mHideShadow = 0;
            if (mIsBalloon == 0) {
                mOpacity = 0;
            } else {
                mOpacity = 0x1f;
            }
        }
    }

    func_ov002_020e444c();
    func_ov002_020e4990();
    func_ov002_020e4768();

    mBodyModelId = (u8)param1;
    mDrawnIsBalloon = mIsBalloon;
    mDrawnIsMetal = mIsMetal;
    mPlayerTexFrame = mIsVanish;
    mDrawnHasWings = mHasWings;
    mDrawnHasNoCap = (u8)HasNoCap();

    mIsBalloon = save_6fd;
    mIsMetal = save_6f9;
    mIsVanish = save_6fb;
    mHasWings = save_6ff;
}

/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
/* local extern: the call passes an untyped this, so it cannot use the header method. */
#define LAU(p) (p)

int daPly_c::Behavior() {
    struct C3;
    typedef int (C3::*PMF)();
    struct G_ee90 {
        char pad[0x26c];
        s32 flag26c;
    };
    extern u8 gGameMode;
    extern G_ee90 data_0209ee90;
    extern s16 data_0209f4a6;
    s32 r0;
    s32 scale;
    s32 r2;
    s32 temp;
    s32 mul;
    s32 fallFlag;
    Vector3 v0;
    Vector3 v1;

    if (gGameMode == 1)
        r0 = 1;
    else
        r0 = 0;
    if (r0 != 0 && data_0209fc5c[mPlayerNo] == 0)
        return 1;

    scale = 0x1000;
    r2 = scale;
    if (mIsMega != 0) {
        scale = 0x3000;
        r2 = 0x4200;
    } else if (mIsBalloon != 0) {
        scale = 0x1400;
        r2 = 0x2000;
    }
    temp = FX_MUL(r2, 0x32000);
    mMeshClsn.SetRadius(temp);
    mMeshClsn.SetHeight(temp);
    mdCcAcPos_c.radius = scale * 0x28;
    mul = 0x96;
    if (IsState(data_ov002_021104e4)
        || IsState(data_ov002_02110514)
        || IsState(data_ov002_02110334)
        || IsState(data_ov002_0211031c)
        || IsState(data_ov002_021105bc)
        || IsState(data_ov002_0211061c)
        || IsState(data_ov002_02110634)) {
        mul = 0x5a;
    }
    mdCcAcPos_c.height = mul * scale;

    if (mIsMega != 0)
        mdCcAcPos_c.flags |= 0x10;
    else
        mdCcAcPos_c.flags &= ~0x10;

    // duplicating the player-slot store grows this function 0x868 -> 0x874
    if (data_0209fc68 == 0) {
        if (gRunningKuppaScript != 0)
            r0 = 1;
        else
            r0 = 0;
        if (r0 == 0) {
            gActivePlayerSlot = 0;
            goto after_player_slot;
        }
    }
    gActivePlayerSlot = mPlayerNo;
after_player_slot:

    if (func_ov002_020c4188() != 0)
        return 1;

    DecIfAbove0_Short(&mStateTimer);
    DecIfAbove0_Short(&mStateWaitTimer);
    DecIfAbove0_Short(&mInvincibleTimer);
    DecIfAbove0_Short(&mJumpComboTimer);
    DecIfAbove0_Short(&mPunchKickCooldown);
    DecIfAbove0_Short(&mStickGraceTimer);
    DecIfAbove0_Short(&mCrouchTimer);
    DecIfAbove0_Short(&mScaleAnimTimer);
    DecIfAbove0_Short(&mHoldHeavyTimer);
    DecIfAbove0_Short(&mClimbCooldown);
    DecIfAbove0_Short(&mWalkTimer);
    DecIfAbove0_Short(&mWaterRunTimer);
    DecIfAbove0_Short(&mQuicksandRunTimer);
    DecIfAbove0_Short(&mQuicksandJumpTimer);
    DecIfAbove0_Short(&mTeleportTimer);

    if (mTeleportTimer == 1)
        gShowTouchScreenArrows = 0;

    if (mIsControlDisabled == 0) {
        DecIfAbove0_Short(&mBalloonTimer);
        if (DecIfAbove0_Short(&mFireTimer) == 0)
            func_ov002_020d80d0();
        if (DecIfAbove0_Short(&mPowerupTimer) == 0) {
            func_ov002_020e032c();
            func_ov002_020bdef0();
            func_ov002_020bdd9c();
        }
        if (DecIfAbove0_Short(&mMegaTimer) == 0)
            func_ov002_020bdd2c();

        if (mPrevAreaId != mAreaId) {
            mPrevAreaId = mAreaId;
            mMouthHoldTimer = 0;
        }
        if (DecIfAbove0_Short(&mMouthHoldTimer) == 0 && mObjInMouth != 0)
            func_ov002_020d6790();
    }

    {
        s16 ang = mDesiredAngleY;
        s32 stride = 0x18;
        mPrevDesiredAngleY = ang;
        mDesiredAngleY = *(s16 *)((char *)&data_0209f4a6 + gActivePlayerSlot * stride);
        if (data_0209fc68 == 0) {
            s16 add = GetAngleToCamera(0);
            s16 *p = (s16 *)LAU((char *)&mDesiredAngleY);
            *p = (s16)(*p + add);
        } else {
            s16 add = GetAngleToCamera(mPlayerNo);
            s16 *p = (s16 *)LAU((char *)&mDesiredAngleY);
            *p = (s16)(*p + add);
        }
    }

    func_ov002_020c2e78();
    func_ov002_020ca940();
    func_ov002_020d8158();

    {
        daPly_c::State *st = mState;
        if (*(void **)&st->mMain != 0)
            (this->*(st->mMain))();
    }

    if ((u16)(mStateFlags & 0x80) != 0) {
        this->ChangeState(data_ov002_021104fc);
        mStateFlags &= ~0x80;
    }

    if (mIsNoControl != 0)
        fallFlag = 1;
    else
        fallFlag = 0;
    if (gCurrentSublevel == 0x1f && IsState(data_ov002_021104b4) != 0)
        fallFlag = 0;

    if (mIsAirborne == 0 || fallFlag != 0 || mPeakY < mPosY)
        mPeakY = mPosY;

    if (mIsAirborne == 0 && mIsFallScreaming != 0) {
        Sound::PlayCharVoice(mCharacter, 0x2e, mCamSpacePos);
        mIsFallScreaming = 0;
    }
    if (mIsAirborne != 0 && mIsFallScreaming == 0
        && IsState(data_ov002_02110454) == 0
        && (mPeakY - mPosY) > 0x47e000) {
        mIsFallScreaming = 1;
        Sound::PlayCharVoice(mCharacter, 0x1d, mCamSpacePos);
    }

    if (mIsInShallowWater != 0 && mIsUnderwater == 0 && mIsAirborne == 0)
        func_ov002_020ce8bc(mHorzSpeed);

    mPreClsnAngleY = mAngleY;
    mStateFlags &= ~0x440;

    if (mHasWings != 0)
        mModels.mAnmModel2.Advance();

    mPreClsnPosX = mPosX;
    mPreClsnPosY = mPosY;
    mPreClsnPosZ = mPosZ;
    func_ov002_020bf36c(&mdCcAcPos_c);
    func_ov002_020bf13c();

    {
        s32 *p = (s32 *)LAU((int)&v0);
        s32 z = 0;
        p[0] = z;
        p[1] = z;
        {
            daPly_c::State *st = &data_ov002_0210ffec;
            p[2] = z;
            if (IsState(*st) != 0) {
                if (mIsMega != 0) {
                    s32 t = 0x12c000;
                    v0.y = -t;
                } else {
                    s32 t = 0x64000;
                    v0.y = -t;
                }
            } else if (IsState(data_ov002_021106f4) != 0) {
                v0.y = 0x64000;
            }
        }
    }
    v1.x = v0.x;
    v1.y = v0.y;
    v1.z = v0.z;
    mdCcAcPos_c.SetPosRelativeToActor(v1);
    func_ov002_020c2db8();

    if (mIsInShallowWater != 0)
        mGroundSoundType = 3;

    if (data_0209ee90.flag26c != 0) {
        data_0209ee90.flag26c = 0;
        this->ChangeState(data_ov002_021101b4);
        mStateStep = 0;
        this->ChangeState(*reinterpret_cast<daPly_c::State *>(&data_ov002_0210ffec));
    }

    UpdateScaleAnim();
    func_ov002_020d869c();

    {
        u16 t = mInvincibleTimer;
        if (t != 0) {
            if (t == 1)
                mdCcAcPos_c.vulnFlags |= 0x10000000;
            else
                mdCcAcPos_c.vulnFlags &= ~0x10000000;
        }
    }

    mdCcAcPos_c.Clear();
    if (mIsBodyClsnEnabled != 0)
        mdCcAcPos_c.Update();

    if (data_0209fc68 == 0)
        gActivePlayerSlot = 0;
    else
        gActivePlayerSlot = gLocalPlayerId;

    func_ov002_020e4bb8();

    {
        s32 r2 = mTongueHitDist;
        if (r2 != 0) {
            u8 ch = mUseAltBodyModel;
            Matrix4x3 *t4 = &mModels.mHeadModels[(ch << 2) + 3]->data.transforms[3];
            if (r2 < 0)
                r2 = -r2;
            t4->m[11] = r2 >> 3;
        }
    }

    data_0209f254 = 0;
    if (param1 == 3) {
        void *p = mHeldObjQueue[0];
        if (p != 0)
            data_0209f254 = ((daYegg_c *)p)->mParamHigh;
    }

    return 1;
}

#undef LAU

/* recovered: named members + shared header, real C++ method */
// Match: daPly_c::InitResources (ov002:0x020e558c).
// The 0x14-byte block allocated for mHeldObjQueue is zeroed in place with
// mwccarm's __builtin__clear intrinsic: that is the compiler's inline CLEAR
// (5 x 4 strb post-inc, subs/bne) aimed at the heap pointer itself. Value-
// init, placement new, struct assignment and memset all go through a stack
// temporary or an out-of-line __clear/memset call instead.
// Also: pin gGameMode in local `d` so ==2 keeps r1 bool (preserves r0);
// decl order td,tx,tz + load order for dBgCh_Gnd pos regs.

s32 daPly_c::InitResources() {
    int SublevelToLevel(int s);
    extern void func_02035644(int *p, int a);
    void LoadSilverStarAndNumber(void);
    void func_02013a00(void);
    int StartWithFarCamera(void);
    /* local extern: `new Vector3[50]` elides both structor args to 0 because
       types.h's ctor/dtor are inline-empty; the ROM passes them, so the
       __cxa_vec_new call is spelled by hand. Measured under the pin: mwccarm
       elides each pointer independently when that structor's empty inline
       body is visible where the new-expression is parsed; a body defined
       after it is passed (and locals still inline it). types.h defines both
       in the class, so only a shared-header change could pass them here, and
       __cxa_vec_ctor (member arrays) never elides. The two callees cannot be renamed
       to readable aliases: enroll.candidates() enrolls the defining file for
       every symbols.txt row at its address, so a second name at a compiled
       function's address fails its isolate (one emitted def only), and
       _ZN7Vector3D1Ev's emitted name is compiler-mangled besides. */
    extern u8 gGameMode;
    extern s8 data_02092114;
    extern u8 gPrevReturnState;
    /* The helpers below take void or char pointers, and this file's match
       depends on declaration and load order documented in the header comment
       above. One alias keeps every one of those decisions untouched. */
    void *self = this;
    char* c = (char*)self;
    Vector3 pos;
    unsigned int a, b;
    u8 n8;
    u8 sub;
    int t;
    int changed;
    int i; unsigned int u;
    u8 f254;
    u8 cur;
    int td, tx, tz;

    a = param1;
    mPlayerNo = (a >> 6) & 3;
    n8 = a >> 8;
    b = param1;
    sub = (b >> 3) & 7;
    mCharacter = b & 7;
    pad_6da[0] = sub;
    changed = 0;
    t = (unsigned int)(gGameMode == 1);
    if (t == 0) {
        if (SublevelToLevel(gCurrentSublevel) == 0x1d) {
            data_0209f254 = 0;
            cur = mCharacter;
            if (sub != cur) {
                data_02092114 = -1;
                sub = cur;
                data_02092128[mPlayerNo] = mCharacter;
                changed = 1;
            }
        }
        if (data_02092114 >= 0) sub = mCharacter;
    }
    param1 = sub;
    {
        unsigned int d = gGameMode;
        t = (unsigned int)(d == 2);
        // structured form grows this function 0x3bc -> 0x3c4
        if (t != 0)
            goto Ld0;
        t = (unsigned int)(d == 1);
    }
    if (t != 0) goto Lac;
    if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) != 0)
        goto Lac;
    t = (unsigned int)(gRunningKuppaScript != 0);
    if (t == 0) goto Ld0;
Lac:
    if (mPlayerNo == gLocalPlayerId) {
        PlayCharacterSound();
        func_ov002_020e6350();
    }
Ld0:
    func_ov002_020e5948();
    if (changed != 0)
        ((daPly_c *)(c))->func_ov002_020beabc();
    mTrailPos = (Vector3 *)__cxa_vec_new(0x32, 0xc, 8, (void (*)(void *))func_0203d384,
                                         (void (*)(void *))_ZN7Vector3D1Ev);
    mTrailFlags = (u8 *)::operator new[](0x32);
    mHeldObjQueue = (dActor_c **)::operator new[](5 * sizeof(dActor_c *));
    __builtin__clear((void *)mHeldObjQueue, 5 * sizeof(dActor_c *));
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov002_020e63a4();
    mMeshClsn.Init((dActor_c *)c, 0x32000, 0x32000, &mPrevAngle, &mAngle);
    func_02035644((int *)&mMeshClsn, 0x28000);
    mTerminalVelocity = -0x4b000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mSpawnAngleY = mAngleY;
    mGroundY = 0x80000000;
    mWaterSurfaceY = 0x80000000;
    mToxicSurfaceY = 0x80000000;
    mOpacity = 0x1f;
    mFloorPathID = 0xff;
    *(s16*)&mPrevAreaId = mAreaId;
    f254 = data_0209f254;
    if (f254 != 0) {
        if (param1 == 3) {
            mEggParams = f254;
            func_ov002_020d6368();
            if ((f254 & 0x80) != 0) {
                if ((mLoadedResourceFlags & 1) == 0) {
                    LoadSilverStarAndNumber();
                    *(u8*)((int)(&mLoadedResourceFlags)) |= 1;
                }
            }
        }
    }
    data_0209f254 = 0;
    if (gPrevReturnState == 1) {
        if (SaveData::HasPlayerLostCap() != 0)
            func_02013a00();
    }
    ((daPly_c *)(c))->func_ov002_020c7dd0(n8);
    dBgCh_Gnd rc;
    tz = mPosZ;
    tx = mPosX;
    td = data_0209212c;
    pos.x = tx;
    pos.y = td;
    pos.z = tz;
    rc.mProbeHeight = td * 2;
    rc.SetObjAndPos(pos, (dActor_c *)c);
    rc.StartDetectingWater();
    if (gCurrentSublevel == 0x1d) {
        gReturnSublevel = 2;
        gReturnEntranceId = 0xe;
    }
    if (StartWithFarCamera() != 0) mUseFarCamera = 1;
    mPeakY = mPosY;
    return 1;
}

/* defined in src/game/actors/d_a_obj1up_logo.cpp */

void daPly_c::func_ov002_020e5948() {
    extern s8 gNumStageStars[];
    extern u8 gGameMode;
    extern SharedFilePtr data_ov002_0210e3d8;
    extern SharedFilePtr data_ov002_0210eb88;
    extern SharedFilePtr data_ov002_0210e750;
    daPly_c *player = this;
    s32 i;
    s32 j;
    s32 bodyId;
    void* ma;
    void* p;
    s32 n;
    s32 k;
    s32 outer;
    dExtAnmTexPat_c *texseq;
    SharedFilePtr *entry;

    dExtFrameCtrl_c::LoadFile(data_ov002_0210e798);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210e4f0);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210e3d8);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210e6c0);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210eb98);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210eb88);
    dExtFrameCtrl_c::LoadFile(data_ov002_0210e4c0);

    if (gNumStageStars[player->mPlayerNo] != 0) {
        LoadSilverStarAndNumber();
        player->mLoadedResourceFlags |= 1;
    }

    {
        s32 b = player->param1;
        s32 a = player->mCharacter;
        if (a != b) {
        switch (b) {
        case 0:
            dExtSimpleModel_c::LoadFile(data_ov002_0210da40);
            player->mLoadedResourceFlags |= 2;
            break;
        case 1:
            dExtSimpleModel_c::LoadFile(data_ov002_0210d9a0);
            player->mLoadedResourceFlags |= 4;
            break;
        case 2:
            dExtSimpleModel_c::LoadFile(data_ov002_0210d9c0);
            player->mLoadedResourceFlags |= 8;
            break;
        }
    }
    }

    dExtAnmTexPat_c::LoadFile(data_ov002_0210e500);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e1c8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e770);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e230);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e288);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e478);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e9d0);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210ea10);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e538);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210eac8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e898);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e9e8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210eba8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210eb70);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210eaf0);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e640);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e670);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e680);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e438);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e3b0);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e3c8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e408);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e400);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210ecb8);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210e8d0);
    dExtAnmTexPat_c::LoadFile(data_ov002_0210ebd8);

    {
        s32 mode;
        mode = gGameMode;
        mode = (mode == 1);
        if (mode == 0) {
            dExtFrameCtrl_c::LoadFile(data_ov002_0210e750);
            dExtSimpleModel_c::LoadFile(data_ov002_0210d9a8);
            dExtSimpleModel_c::LoadFile(data_ov002_02110aa4);
            dExtFrameCtrl_c::LoadFile(data_ov002_0210e588);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e7e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e450);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e788);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e1e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e1e0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e4d0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e4e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e1d0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e800);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e4b8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e270);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e458);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ea70);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e540);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e8e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eca8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ec98);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e600);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ea88);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e910);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e460);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e2f0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e958);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eb38);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e660);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ea30);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e3e8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eb90);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e6b8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e620);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e6a0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eb58);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e3e0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210eb10);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e708);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ebc8);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e6e0);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e430);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ec40);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e738);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e250);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ec10);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210ec00);
            dExtAnmTexPat_c::LoadFile(data_ov002_0210e728);
        }
    }

    {
        s32 mode;
        mode = gGameMode;
        mode = (mode == 1);
        if (mode == 0) {
            player->mCharFileBase = 0;
        } else {
            player->mCharFileBase = 0x44;
            func_ov002_020bec2c();
        }
    }

    dExtFrameCtrl_c::LoadFile(*data_ov002_020ff480[(s32)player->mCharFileBase]);

    i = 0;
    do {
        player->mModels.mBodyModels[i] = new dExtAnm2Model_c;
        player->mModels.mBodyModels[i]->SetFile(
            dExtSimpleModel_c::LoadFile(*data_ov002_0210a68c[i]), 1, 1);
        player->mModels.mBodyModels[i]->Func_020162C4(
            (BCA_File *)data_ov002_020ff480[i + 0xC4]->filePtr, 0, 0x1000, 0);
        player->mModels.mBodyTexParams[i] = func_ov002_020e6bb0(player->mModels.mBodyModels[i]);
        i++;
    } while (i < 4);

    func_ov002_020e6780();

    i = 0;
    do {
        if (i == 3) {
            player->mModels.mHeadModels[i] = new dExtAnmModel_c;
            player->mModels.mHeadModels[i]->SetFile(
                dExtSimpleModel_c::LoadFile(*data_ov002_0210a69c[i]), 1, 1);
            ((dExtAnmModel_c *)player->mModels.mHeadModels[i])
                ->SetAnim((BCA_File *)data_ov002_0210eb88.filePtr, 0, 0x1000, 0);
        } else {
            player->mModels.mHeadModels[i] = new dExtSimpleModel_c;
            player->mModels.mHeadModels[i]->SetFile(
                dExtSimpleModel_c::LoadFile(*data_ov002_0210a69c[i]), 1, 1);
        }
        player->mModels.mHeadTexParams[i] = func_ov002_020e6bb0(player->mModels.mHeadModels[i]);
        i++;
    } while (i < 4);

    j = 0;
    do {
        p = new dExtSimpleModel_c;
        {
            s32 idx = j + 4;
            player->mModels.mHeadModels[idx] = (dExtSimpleModel_c *)p;
            player->mModels.mHeadModels[idx]->SetFile(
                dExtSimpleModel_c::LoadFile(*data_ov002_0210a6ac[j]), 1, 1);
            player->mModels.mHeadTexParams[idx] =
                func_ov002_020e6bb0(player->mModels.mHeadModels[idx]);
        }
        j++;
    } while (j < 4);

    {
        s32 mode;
        mode = gGameMode;
        mode = (mode == 1);
        if (mode == 0) {
            player->mModels.mSleepBubbleModel = new dExtSimpleModel_c;
            player->mModels.mSleepBubbleModel->SetFile(
                dExtSimpleModel_c::LoadFile(data_ov002_0210da38), 1, -1);

            if (gCurrentSublevel != 2 && gCurrentSublevel != 5 && gCurrentSublevel != 4) {
                player->mLoadedResourceFlags |= 0x20;
                player->mModels.mBodyModels[4] = new dExtAnm2Model_c;
                player->mModels.mBodyModels[4]->SetFile(
                    dExtSimpleModel_c::LoadFile(data_ov002_0210e780), 1, -1);
                player->mModels.mBodyModels[4]->Func_020162C4(
                    (BCA_File *)data_ov002_020ff480[0xC6]->filePtr, 0, 0x1000, 0);
                player->mModels.mAnmModel1.SetFile(dExtSimpleModel_c::LoadFile(data_ov002_0210e6f8),
                                                   1, 1);
                player->mModels.mAnmModel1.SetAnim((BCA_File *)data_ov002_0210e750.filePtr, 0,
                                                   0x1000, 0);
                player->mModels.mAnmModel2.SetFile(dExtSimpleModel_c::LoadFile(data_ov002_0210e7b0),
                                                   1, 1);
                player->mModels.mAnmModel2.SetAnim((BCA_File *)data_ov002_0210e4c0.filePtr, 0,
                                                   0x1000, 0);
            }
        } else {
            player->mModels.mAnmModel2.SetFile(dExtSimpleModel_c::LoadFile(data_ov002_0210e7b0), 1,
                                               1);
            player->mModels.mAnmModel2.SetAnim((BCA_File *)data_ov002_0210e4c0.filePtr, 0, 0x1000,
                                               0);
        }
    }

    player->mShadowModel.InitCylinder();

    bodyId = GetBodyModelID(3, 0);
    ma = player->mModels.mBodyModels[bodyId];
    player->mVsYoshiMaterialParam =
        (player->mPlayerNo << 1) + ((dExtAnmModel_c *)ma)->data.materials->texImageParam;

    n = 0;
    {
        s32 mode;
        mode = gGameMode;
        mode = (mode == 1);
        if (mode != 0)
            n = 0x11;
    }

    if (n < 0x19) {
        outer = n * 4;
        do {
            k = 0;
            texseq = player->mModels.mTexSeqBody;
            do {
                entry = data_ov002_020ff2f0[outer + k];
                dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210a69c[k]->filePtr,
                                         *(BTP_File *)entry->filePtr);
                if (n == 0x18) {
                    texseq->SetFile(*(BTP_File *)entry->filePtr, 0, 0x1000, 0);
                }
                k++;
                texseq++;
            } while (k < 4);
            n++;
            outer += 4;
        } while (n < 0x19);
    }

    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210ebb8.filePtr,
                             *(BTP_File *)data_ov002_0210e8d0.filePtr);
    player->mModels.mTexSeqPlayer[0].SetFile(*(BTP_File *)data_ov002_0210e8d0.filePtr, 0, 0x1000,
                                             0);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210eb20.filePtr,
                             *(BTP_File *)data_ov002_0210ebd8.filePtr);
    player->mModels.mTexSeqPlayer[1].SetFile(*(BTP_File *)data_ov002_0210ebd8.filePtr, 0, 0x1000,
                                             0);
}

void daPly_c::PlayCharacterSound() {
    func_02011f7c((void *)data_ov002_020ff0f0[mCharacter & 3]);
}

void daPly_c::func_ov002_020e6350() {
    extern unsigned char gGameMode;
    int b = (gGameMode == 1);
    if (b != 0)
        return;
    func_02011ee4((void *)data_ov002_020ff0f4[param1 & 3]);
}

void daPly_c::func_ov002_020e63a4() {
    int v[3];
    v[0] = data_ov002_0210f92c[0];
    v[1] = data_ov002_0210f92c[1];
    v[2] = data_ov002_0210f92c[2];
    mdCcAcPos_c.Init(this, *(Vector3 *)v, 0x28000, 0x96000, 0x400008, 0x4f3ffe0);
}

/* One runtime bone of a body model: ModelComponents::bones points at
   0x34-byte records, not the file's 0x40-byte BMD_Bone. Same shape as
   d_a_cnn.cpp's CnnBone, except that the rotations are unsigned: the ROM
   reads them with ldrh. func_ov002_020e640c writes the head (bone 15) and
   spine (bone 8) rotations, reads bone 1's translation as root motion, and
   resets every other bone's translation from the file bone's +0x24..+0x2c. */
struct PlayerBone {
    u8 pad_00[0x1a];
    u16 rotX;   /* 0x1a */
    u16 rotY;   /* 0x1c */
    u16 rotZ;   /* 0x1e */
    s32 transX; /* 0x20 */
    s32 transY; /* 0x24 */
    s32 transZ; /* 0x28 */
    u8 pad_2c[0x8];
};
#ifndef SM64DS_PLATFORM_PC
typedef char PlayerBone_size_must_be_0x34[sizeof(PlayerBone) == 0x34 ? 1 : -1];
#endif

/* One of the file's 0x40-byte bone records (BMD_File::bones). Its rest-pose
   translation sits at +0x24; func_ov002_020e640c copies it into the runtime
   PlayerBone. d_ent_obj.cpp's VsBoneFileRecord reads the same three words in
   UnknownVsPlayer::func_ov075_02114b60. */
struct PlayerFileBone {
    u8 pad_00[0x24];
    s32 transX; /* 0x24 */
    s32 transY; /* 0x28 */
    s32 transZ; /* 0x2c */
    u8 pad_30[0x10];
};
typedef char PlayerFileBone_size_must_be_0x40[sizeof(PlayerFileBone) == 0x40 ? 1 : -1];

void daPly_c::func_ov002_020e640c() {
    char *md;
    char *mp;
    char *r4p;
    int id;
    Vector3 d;

    id = GetBodyModelID(param1 & 0xff, 0);
    {
        int *p = (int *)(int)&mModels.mBodyModels[id]->data;
        md = (char *)p[2];
        mp = (char *)p[0];
    }
    r4p = (char *)((BMD_File *)mp)->bones;

    ((PlayerBone *)md)[8].rotY += mSpineRot[1];
    ((PlayerBone *)md)[8].rotZ += mSpineRot[2];

    {
        int b = mHeadRotMode;
        if (b == 0) {
        } else if (b == 1) {
            ((PlayerBone *)md)[15].rotX += *(u16 *)mHeadRot;
            ((PlayerBone *)md)[15].rotY += (u16)mHeadRot[1];
            ((PlayerBone *)md)[15].rotZ += (u16)mHeadRot[2];
        } else if (b == 2) {
            ((PlayerBone *)md)[15].rotX = *mHeadRot;
            ((PlayerBone *)md)[15].rotY = mHeadRot[1];
            ((PlayerBone *)md)[15].rotZ = mHeadRot[2];
        } else if (b == 3) {
            ((PlayerBone *)md)[15].rotZ = (s16)(*mHeadRot + 0x4000);
            ((PlayerBone *)md)[15].rotY = mHeadRot[1];
            ((PlayerBone *)md)[15].rotX = mHeadRot[2];
            ((PlayerBone *)md)[8].rotZ = *mSpineRot;
            ((PlayerBone *)md)[8].rotX = mSpineRot[1];
            ((PlayerBone *)md)[8].rotY = mSpineRot[2];
        } else if (b == 4) {
            ApproachLinear2(mHeadRot[0], (s16)(((PlayerBone *)md)[15].rotZ - 0x4000), 0x400);
            ApproachLinear2(mHeadRot[1], ((PlayerBone *)md)[15].rotY, 0x400);
            ApproachLinear2(mHeadRot[2], ((PlayerBone *)md)[15].rotX, 0x400);
            ((PlayerBone *)md)[15].rotZ = (s16)(*mHeadRot + 0x4000);
            ((PlayerBone *)md)[15].rotY = mHeadRot[1];
            ((PlayerBone *)md)[15].rotX = mHeadRot[2];
        }
    }

    mHeadBoneRot[0] = ((PlayerBone *)md)[15].rotX;
    mHeadBoneRot[1] = ((PlayerBone *)md)[15].rotY;
    mHeadBoneRot[2] = ((PlayerBone *)md)[15].rotZ;

    ((PlayerBone *)md)->transX = ((PlayerFileBone *)r4p)->transX;
    ((PlayerBone *)md)->transY = ((PlayerFileBone *)r4p)->transY;
    ((PlayerBone *)md)->transZ = ((PlayerFileBone *)r4p)->transZ;

    {
        int b = mRootMotionMode;
        if (b != 0) {
            if ((b & 1) == 1) {
                mStateTargetX = ((PlayerBone *)md)[1].transX;
                if (mRootMotionMode < 3)
                    mStateTargetY = 0;
                else
                    mStateTargetY = ((PlayerBone *)md)[1].transY;
                mStateTargetZ = ((PlayerBone *)md)[1].transZ;
                mStateAnchorX = mStateTargetX;
                mStateAnchorY = mStateTargetY;
                mStateAnchorZ = mStateTargetZ;
                mRootMotionMode++;
            } else {
                d.x = (((PlayerBone *)md)[1].transX - mStateTargetX) << 3;
                if (mRootMotionMode < 3)
                    d.y = 0;
                else
                    d.y = (((PlayerBone *)md)[1].transY - mStateTargetY) << 3;
                d.z = (((PlayerBone *)md)[1].transZ - mStateTargetZ) << 3;
                Vec3_RotateYAndTranslate(&mPos, &mPos, mAngleY, &d);
                mStateTargetX = ((PlayerBone *)md)[1].transX;
                if (mRootMotionMode >= 3)
                    mStateTargetY = ((PlayerBone *)md)[1].transY;
                mStateTargetZ = ((PlayerBone *)md)[1].transZ;
            }
            ((PlayerBone *)md)[1].transX = 0;
            if (mRootMotionMode >= 3)
                ((PlayerBone *)md)[1].transY = 0;
            ((PlayerBone *)md)[1].transZ = 0;
        }
    }

    md += 0x68;
    r4p += 0x80;
    {
        s16 i = 2;
        if (((BMD_File *)mp)->numBones > 2) {
            do {
                ((PlayerBone *)md)->transX = ((PlayerFileBone *)r4p)->transX;
                ((PlayerBone *)md)->transY = ((PlayerFileBone *)r4p)->transY;
                ((PlayerBone *)md)->transZ = ((PlayerFileBone *)r4p)->transZ;
                i = (s16)(i + 1);
                md += 0x34;
                r4p += 0x40;
            } while ((u32)i < ((BMD_File *)mp)->numBones);
        }
    }
}

void daPly_c::func_ov002_020e6780() {
    mSpineRot[0] = 0;
    mSpineRot[1] = 0;
    mSpineRot[2] = 0;
    mHeadRot[0] = 0;
    mHeadRot[1] = 0;
    mHeadRot[2] = 0;
    mHeadRotMode = 0;
}

/* Out-of-line ~daPly_c is the key function. defer_codegen on for this
 * definition only: under defer_codegen off the variants come out D1, D0, D2,
 * and the cartridge order is D0 then D1. With deferred codegen they come out
 * D2, D0, D1, and the following function stays after them. The vtable's
 * typeinfo and type name are compared against the cartridge's records. D2
 * is not in the cartridge. */
#pragma defer_codegen on

daPly_c::~daPly_c() {}

#pragma defer_codegen off

/* Every member is built by the compiler. mModels' implicit constructor is
 * inlined with its own `this` (daPly_c + 0xdc), which is the second base
 * register the retail body carries. C2 is not in the cartridge. */
daPly_c::daPly_c() {}

int daPly_c::OnYoshiTryEat() {
    this;
    return 1;
}

extern "C" {

extern unsigned short data_ov002_0210a760[];
void func_ov002_020e69c0(void *unused)
{
    (void)unused;
    unsigned short *dst = data_ov002_0210a760;
    unsigned short a = *dst;
    unsigned short b = dst[1];
    /* byte +4 kept: &dst[2] adds a word (measured). */
    unsigned short *src = (unsigned short *)((char *)dst + 4);
    int i;
    for (i = 2; i < 0x20; i++) {
        *dst++ = *src++;
    }
    dst[0] = a;
    dst[1] = b;
}

/* Blends one RGB555 channel: to * t plus the already-scaled from term. */
static inline int Color_Lerp(int to, int t, int fromScaled) {
    return (to * t) + fromScaled;
}

/* Blends RGB555 colour from toward to by the fx12 weight t (0..0x1000).
 * The first argument is not read. */
int Color_Interp(int unused, int from, int to, int t) {
    int r;
    int g;
    int b;
    int s;
    unsigned int out;
    if (t >= 0x1000) {
        return to;
    }
    if (t <= 0) {
        return from;
    }
    s = 0x1000 - t;
    r = (from & 0x1f) * s;
    g = (from & 0x3e0) * s;
    b = (from & 0x7c00) * s;
    r = Color_Lerp(to & 0x1f, t, r);
    g = Color_Lerp(to & 0x3e0, t, g);
    b = Color_Lerp(to & 0x7c00, t, b);
    out = ((unsigned int)((r | g) | b)) << 4;
    return out >> 0x10;
}
}

/* A 32-entry RGB555 ramp and the blended copy func_ov002_020e6a78 builds
 * behind it. */
struct ColorRamp {
    unsigned short base[0x20];
    unsigned short blended[0x20]; /* 0x40 */
};
extern "C" {

/* Blends the 32-entry ramp toward data_ov002_0210a760 (which
 * func_ov002_020e69c0 rotates) by blend, into the ramp's second half,
 * and hands that half to G3X_SetToonTable. */
void func_ov002_020e6a78(void *ramp, int blend) {
    unsigned short *src = ((ColorRamp *)ramp)->base;
    unsigned short *target = data_ov002_0210a760;
    unsigned short *dst = ((ColorRamp *)ramp)->blended;
    int i;
    for (i = 0; i < 0x20; i++) {
        *dst++ = Color_Interp((int)ramp, *src++, *target++, blend);
    }
    G3X_SetToonTable(((ColorRamp *)ramp)->blended);
}

/* Fills both halves of the ramp with the grey ramp (i, i, i) and clears
 * DISP3DCNT bits 1, 12 and 13. */
unsigned short *func_ov002_020e6ad4(unsigned short *ramp) {
    unsigned short *dst = ramp;
    unsigned short *copy = ((ColorRamp *)ramp)->blended;
    int i = 0;
    do {
        unsigned short grey = (i * 0x400) | (i | (i * 0x20));
        *dst++ = grey;
        *copy++ = grey;
        i++;
    } while (i < 0x20);
    *(unsigned short *)0x4000060 &= ~0x3002;
    return ramp;
}
}

/* What the three material helpers below read from a model: its resource
 * at +0x08 (material count at +0x24) and its runtime materials at +0x0c,
 * 0x30 bytes each with TEXIMAGE_PARAM at +0x1c. */
struct ModelMaterialRes {
    char pad[0x24];
    unsigned int numMaterials; /* 0x24 */
};

struct ModelMaterial {
    char pad[0x1c];
    unsigned int texImageParam; /* 0x1c */
    char pad2[0x10];
};
extern "C" {

/* Zeroes every material's TEXIMAGE_PARAM (the untextured cap flash). */
void func_ov002_020e6b3c(void *model) {
    void **p = (void **)((char *)model + 8);
    ModelMaterialRes *res = (ModelMaterialRes *)p[0];
    unsigned int n = res->numMaterials;
    ModelMaterial *mat = (ModelMaterial *)p[1];
    unsigned int i;
    for (i = 0; i < n; i++, mat++)
        mat->texImageParam = 0;
}

/* Writes the words func_ov002_020e6bb0 saved back into the materials. */
void func_ov002_020e6b74(void *model, int *saved) {
    void **p = (void **)((char *)model + 8);
    ModelMaterialRes *res = (ModelMaterialRes *)p[0];
    unsigned int n = res->numMaterials;
    ModelMaterial *dst = (ModelMaterial *)p[1];
    unsigned int i;
    for (i = 0; i < n; i++, dst++, saved++)
        dst->texImageParam = *saved;
}

/* Allocates one word per material and saves each TEXIMAGE_PARAM into it. */
int *func_ov002_020e6bb0(void *model) {
    void **pp;
    ModelMaterialRes *res;
    unsigned int count;
    int *out;
    int *p;
    ModelMaterial *src;
    unsigned int i;

    pp = (void **)((long long)(int)((char *)model + 8));
    res = (ModelMaterialRes *)pp[0];
    count = res->numMaterials;
    out = (int *)::operator new[](count << 2);

    p = out;
    src = (ModelMaterial *)pp[1];
    for (i = 0; i < count; i++, src++)
        *p++ = src->texImageParam;
    return out;
}
}

/* Static initializer __sinit_ov002_021019d0, placed in .init by define_section.
 * Still written by hand: the objects it builds keep their ROM-owned .bss
 * homes, so a compiler-emitted initializer has nothing to define. */
/* Zero-filled .bss owned by this TU (0x0210e14c..0x02110924).
 * daPly_c::ST_WAIT is the St_Wait state at 0x02110154.
 * daPly_c::ST_OWL is the St_Owl state at 0x02110244.
 * Outside .init: the section pragma would place these objects in .init.
 * No constructor and no initializer, so __sinit_ov002_021019d0 stays put. */
extern "C" {

int data_ov002_0210e160;
SharedFilePtr data_ov002_0210e168;
SharedFilePtr data_ov002_0210e170;
SharedFilePtr data_ov002_0210e178;
SharedFilePtr data_ov002_0210e180;
SharedFilePtr data_ov002_0210e188;
SharedFilePtr data_ov002_0210e190;
SharedFilePtr data_ov002_0210e198;
SharedFilePtr data_ov002_0210e1a0;
SharedFilePtr data_ov002_0210e1a8;
SharedFilePtr data_ov002_0210e1b0;
SharedFilePtr data_ov002_0210e1b8;
SharedFilePtr data_ov002_0210e1c0;
SharedFilePtr data_ov002_0210e1c8;
SharedFilePtr data_ov002_0210e1d0;
SharedFilePtr data_ov002_0210e1d8;
SharedFilePtr data_ov002_0210e1e0;
SharedFilePtr data_ov002_0210e1e8;
SharedFilePtr data_ov002_0210e1f0;
SharedFilePtr data_ov002_0210e1f8;
SharedFilePtr data_ov002_0210e200;
SharedFilePtr data_ov002_0210e208;
SharedFilePtr data_ov002_0210e210;
SharedFilePtr data_ov002_0210e218;
SharedFilePtr data_ov002_0210e220;
SharedFilePtr data_ov002_0210e228;
SharedFilePtr data_ov002_0210e230;
SharedFilePtr data_ov002_0210e238;
SharedFilePtr data_ov002_0210e240;
SharedFilePtr data_ov002_0210e248;
SharedFilePtr data_ov002_0210e250;
SharedFilePtr data_ov002_0210e258;
SharedFilePtr data_ov002_0210e260;
SharedFilePtr data_ov002_0210e268;
SharedFilePtr data_ov002_0210e270;
SharedFilePtr data_ov002_0210e278;
SharedFilePtr data_ov002_0210e280;
SharedFilePtr data_ov002_0210e288;
SharedFilePtr data_ov002_0210e290;
SharedFilePtr data_ov002_0210e298;
SharedFilePtr data_ov002_0210e2a0;
SharedFilePtr data_ov002_0210e2a8;
SharedFilePtr data_ov002_0210e2b0;
SharedFilePtr data_ov002_0210e2b8;
SharedFilePtr data_ov002_0210e2c0;
SharedFilePtr data_ov002_0210e2c8;
SharedFilePtr data_ov002_0210e2d0;
SharedFilePtr data_ov002_0210e2d8;
SharedFilePtr data_ov002_0210e2e0;
SharedFilePtr data_ov002_0210e2e8;
SharedFilePtr data_ov002_0210e2f0;
SharedFilePtr data_ov002_0210e2f8;
SharedFilePtr data_ov002_0210e300;
SharedFilePtr data_ov002_0210e308;
SharedFilePtr data_ov002_0210e310;
SharedFilePtr data_ov002_0210e318;
SharedFilePtr data_ov002_0210e320;
SharedFilePtr data_ov002_0210e328;
SharedFilePtr data_ov002_0210e330;
SharedFilePtr data_ov002_0210e338;
SharedFilePtr data_ov002_0210e340;
SharedFilePtr data_ov002_0210e348;
SharedFilePtr data_ov002_0210e350;
SharedFilePtr data_ov002_0210e358;
SharedFilePtr data_ov002_0210e360;
SharedFilePtr data_ov002_0210e368;
SharedFilePtr data_ov002_0210e370;
SharedFilePtr data_ov002_0210e378;
SharedFilePtr data_ov002_0210e380;
SharedFilePtr data_ov002_0210e388;
SharedFilePtr data_ov002_0210e390;
SharedFilePtr data_ov002_0210e398;
SharedFilePtr data_ov002_0210e3a0;
SharedFilePtr data_ov002_0210e3a8;
SharedFilePtr data_ov002_0210e3b0;
SharedFilePtr data_ov002_0210e3b8;
SharedFilePtr data_ov002_0210e3c0;
SharedFilePtr data_ov002_0210e3c8;
SharedFilePtr data_ov002_0210e3d0;
SharedFilePtr data_ov002_0210e3d8;
SharedFilePtr data_ov002_0210e3e0;
SharedFilePtr data_ov002_0210e3e8;
SharedFilePtr data_ov002_0210e3f0;
SharedFilePtr data_ov002_0210e3f8;
SharedFilePtr data_ov002_0210e400;
SharedFilePtr data_ov002_0210e408;
SharedFilePtr data_ov002_0210e410;
SharedFilePtr data_ov002_0210e418;
SharedFilePtr data_ov002_0210e420;
SharedFilePtr data_ov002_0210e428;
SharedFilePtr data_ov002_0210e430;
SharedFilePtr data_ov002_0210e438;
SharedFilePtr data_ov002_0210e440;
SharedFilePtr data_ov002_0210e448;
SharedFilePtr data_ov002_0210e450;
SharedFilePtr data_ov002_0210e458;
SharedFilePtr data_ov002_0210e460;
SharedFilePtr data_ov002_0210e468;
SharedFilePtr data_ov002_0210e470;
SharedFilePtr data_ov002_0210e478;
SharedFilePtr data_ov002_0210e480;
SharedFilePtr data_ov002_0210e488;
SharedFilePtr data_ov002_0210e490;
SharedFilePtr data_ov002_0210e498;
SharedFilePtr data_ov002_0210e4a0;
SharedFilePtr data_ov002_0210e4a8;
SharedFilePtr data_ov002_0210e4b0;
SharedFilePtr data_ov002_0210e4b8;
SharedFilePtr data_ov002_0210e4c0;
SharedFilePtr data_ov002_0210e4c8;
SharedFilePtr data_ov002_0210e4d0;
SharedFilePtr data_ov002_0210e4d8;
SharedFilePtr data_ov002_0210e4e0;
SharedFilePtr data_ov002_0210e4e8;
SharedFilePtr data_ov002_0210e4f0;
SharedFilePtr data_ov002_0210e4f8;
SharedFilePtr data_ov002_0210e500;
SharedFilePtr data_ov002_0210e508;
SharedFilePtr data_ov002_0210e510;
SharedFilePtr data_ov002_0210e518;
SharedFilePtr data_ov002_0210e520;
SharedFilePtr data_ov002_0210e528;
SharedFilePtr data_ov002_0210e530;
SharedFilePtr data_ov002_0210e538;
SharedFilePtr data_ov002_0210e540;
SharedFilePtr data_ov002_0210e548;
SharedFilePtr data_ov002_0210e550;
SharedFilePtr data_ov002_0210e558;
SharedFilePtr data_ov002_0210e560;
SharedFilePtr data_ov002_0210e568;
SharedFilePtr data_ov002_0210e570;
SharedFilePtr data_ov002_0210e578;
SharedFilePtr data_ov002_0210e580;
SharedFilePtr data_ov002_0210e588;
SharedFilePtr data_ov002_0210e590;
SharedFilePtr data_ov002_0210e598;
SharedFilePtr data_ov002_0210e5a0;
SharedFilePtr data_ov002_0210e5a8;
SharedFilePtr data_ov002_0210e5b0;
SharedFilePtr data_ov002_0210e5b8;
SharedFilePtr data_ov002_0210e5c0;
SharedFilePtr data_ov002_0210e5c8;
SharedFilePtr data_ov002_0210e5d0;
SharedFilePtr data_ov002_0210e5d8;
SharedFilePtr data_ov002_0210e5e0;
SharedFilePtr data_ov002_0210e5e8;
SharedFilePtr data_ov002_0210e5f0;
SharedFilePtr data_ov002_0210e5f8;
SharedFilePtr data_ov002_0210e600;
SharedFilePtr data_ov002_0210e608;
SharedFilePtr data_ov002_0210e610;
SharedFilePtr data_ov002_0210e618;
SharedFilePtr data_ov002_0210e620;
SharedFilePtr data_ov002_0210e628;
SharedFilePtr data_ov002_0210e630;
SharedFilePtr data_ov002_0210e638;
SharedFilePtr data_ov002_0210e640;
SharedFilePtr data_ov002_0210e648;
SharedFilePtr data_ov002_0210e650;
SharedFilePtr data_ov002_0210e658;
SharedFilePtr data_ov002_0210e660;
SharedFilePtr data_ov002_0210e668;
SharedFilePtr data_ov002_0210e670;
SharedFilePtr data_ov002_0210e678;
SharedFilePtr data_ov002_0210e680;
SharedFilePtr data_ov002_0210e688;
SharedFilePtr data_ov002_0210e690;
SharedFilePtr data_ov002_0210e698;
SharedFilePtr data_ov002_0210e6a0;
SharedFilePtr data_ov002_0210e6a8;
SharedFilePtr data_ov002_0210e6b0;
SharedFilePtr data_ov002_0210e6b8;
SharedFilePtr data_ov002_0210e6c0;
SharedFilePtr data_ov002_0210e6c8;
SharedFilePtr data_ov002_0210e6d0;
SharedFilePtr data_ov002_0210e6d8;
SharedFilePtr data_ov002_0210e6e0;
SharedFilePtr data_ov002_0210e6e8;
SharedFilePtr data_ov002_0210e6f0;
SharedFilePtr data_ov002_0210e6f8;
SharedFilePtr data_ov002_0210e700;
SharedFilePtr data_ov002_0210e708;
SharedFilePtr data_ov002_0210e710;
SharedFilePtr data_ov002_0210e718;
SharedFilePtr data_ov002_0210e720;
SharedFilePtr data_ov002_0210e728;
SharedFilePtr data_ov002_0210e730;
SharedFilePtr data_ov002_0210e738;
SharedFilePtr data_ov002_0210e740;
SharedFilePtr data_ov002_0210e748;
SharedFilePtr data_ov002_0210e750;
SharedFilePtr data_ov002_0210e758;
SharedFilePtr data_ov002_0210e760;
SharedFilePtr data_ov002_0210e768;
SharedFilePtr data_ov002_0210e770;
SharedFilePtr data_ov002_0210e778;
SharedFilePtr data_ov002_0210e780;
SharedFilePtr data_ov002_0210e788;
SharedFilePtr data_ov002_0210e790;
SharedFilePtr data_ov002_0210e798;
SharedFilePtr data_ov002_0210e7a0;
SharedFilePtr data_ov002_0210e7a8;
SharedFilePtr data_ov002_0210e7b0;
SharedFilePtr data_ov002_0210e7c0;
SharedFilePtr data_ov002_0210e7c8;
SharedFilePtr data_ov002_0210e7d0;
SharedFilePtr data_ov002_0210e7d8;
SharedFilePtr data_ov002_0210e7e0;
SharedFilePtr data_ov002_0210e7e8;
SharedFilePtr data_ov002_0210e7f0;
SharedFilePtr data_ov002_0210e7f8;
SharedFilePtr data_ov002_0210e800;
SharedFilePtr data_ov002_0210e808;
SharedFilePtr data_ov002_0210e810;
SharedFilePtr data_ov002_0210e818;
SharedFilePtr data_ov002_0210e820;
SharedFilePtr data_ov002_0210e828;
SharedFilePtr data_ov002_0210e830;
SharedFilePtr data_ov002_0210e838;
SharedFilePtr data_ov002_0210e840;
SharedFilePtr data_ov002_0210e848;
SharedFilePtr data_ov002_0210e850;
SharedFilePtr data_ov002_0210e858;
SharedFilePtr data_ov002_0210e860;
SharedFilePtr data_ov002_0210e868;
SharedFilePtr data_ov002_0210e870;
SharedFilePtr data_ov002_0210e878;
SharedFilePtr data_ov002_0210e880;
SharedFilePtr data_ov002_0210e888;
SharedFilePtr data_ov002_0210e890;
SharedFilePtr data_ov002_0210e898;
SharedFilePtr data_ov002_0210e8a0;
SharedFilePtr data_ov002_0210e8a8;
SharedFilePtr data_ov002_0210e8b0;
SharedFilePtr data_ov002_0210e8b8;
SharedFilePtr data_ov002_0210e8c0;
SharedFilePtr data_ov002_0210e8c8;
SharedFilePtr data_ov002_0210e8d0;
SharedFilePtr data_ov002_0210e8d8;
SharedFilePtr data_ov002_0210e8e0;
SharedFilePtr data_ov002_0210e8e8;
SharedFilePtr data_ov002_0210e8f0;
SharedFilePtr data_ov002_0210e8f8;
SharedFilePtr data_ov002_0210e900;
SharedFilePtr data_ov002_0210e908;
SharedFilePtr data_ov002_0210e910;
SharedFilePtr data_ov002_0210e918;
SharedFilePtr data_ov002_0210e920;
SharedFilePtr data_ov002_0210e928;
SharedFilePtr data_ov002_0210e930;
SharedFilePtr data_ov002_0210e938;
SharedFilePtr data_ov002_0210e940;
SharedFilePtr data_ov002_0210e948;
SharedFilePtr data_ov002_0210e950;
SharedFilePtr data_ov002_0210e958;
SharedFilePtr data_ov002_0210e960;
SharedFilePtr data_ov002_0210e968;
SharedFilePtr data_ov002_0210e970;
SharedFilePtr data_ov002_0210e978;
SharedFilePtr data_ov002_0210e980;
SharedFilePtr data_ov002_0210e988;
SharedFilePtr data_ov002_0210e990;
SharedFilePtr data_ov002_0210e998;
SharedFilePtr data_ov002_0210e9a0;
SharedFilePtr data_ov002_0210e9a8;
SharedFilePtr data_ov002_0210e9b0;
SharedFilePtr data_ov002_0210e9b8;
SharedFilePtr data_ov002_0210e9c0;
SharedFilePtr data_ov002_0210e9c8;
SharedFilePtr data_ov002_0210e9d0;
SharedFilePtr data_ov002_0210e9d8;
SharedFilePtr data_ov002_0210e9e0;
SharedFilePtr data_ov002_0210e9e8;
SharedFilePtr data_ov002_0210e9f0;
SharedFilePtr data_ov002_0210e9f8;
SharedFilePtr data_ov002_0210ea00;
SharedFilePtr data_ov002_0210ea08;
SharedFilePtr data_ov002_0210ea10;
SharedFilePtr data_ov002_0210ea18;
SharedFilePtr data_ov002_0210ea20;
SharedFilePtr data_ov002_0210ea28;
SharedFilePtr data_ov002_0210ea30;
SharedFilePtr data_ov002_0210ea38;
SharedFilePtr data_ov002_0210ea40;
SharedFilePtr data_ov002_0210ea48;
SharedFilePtr data_ov002_0210ea50;
SharedFilePtr data_ov002_0210ea58;
SharedFilePtr data_ov002_0210ea60;
SharedFilePtr data_ov002_0210ea68;
SharedFilePtr data_ov002_0210ea70;
SharedFilePtr data_ov002_0210ea78;
SharedFilePtr data_ov002_0210ea80;
SharedFilePtr data_ov002_0210ea88;
SharedFilePtr data_ov002_0210ea90;
SharedFilePtr data_ov002_0210ea98;
SharedFilePtr data_ov002_0210eaa0;
SharedFilePtr data_ov002_0210eaa8;
SharedFilePtr data_ov002_0210eab0;
SharedFilePtr data_ov002_0210eab8;
SharedFilePtr data_ov002_0210eac0;
SharedFilePtr data_ov002_0210eac8;
SharedFilePtr data_ov002_0210ead0;
SharedFilePtr data_ov002_0210ead8;
SharedFilePtr data_ov002_0210eae0;
SharedFilePtr data_ov002_0210eae8;
SharedFilePtr data_ov002_0210eaf0;
SharedFilePtr data_ov002_0210eaf8;
SharedFilePtr data_ov002_0210eb00;
SharedFilePtr data_ov002_0210eb08;
SharedFilePtr data_ov002_0210eb10;
SharedFilePtr data_ov002_0210eb18;
SharedFilePtr data_ov002_0210eb20;
SharedFilePtr data_ov002_0210eb28;
SharedFilePtr data_ov002_0210eb30;
SharedFilePtr data_ov002_0210eb38;
SharedFilePtr data_ov002_0210eb40;
SharedFilePtr data_ov002_0210eb48;
SharedFilePtr data_ov002_0210eb50;
SharedFilePtr data_ov002_0210eb58;
SharedFilePtr data_ov002_0210eb60;
SharedFilePtr data_ov002_0210eb68;
SharedFilePtr data_ov002_0210eb70;
SharedFilePtr data_ov002_0210eb78;
SharedFilePtr data_ov002_0210eb80;
SharedFilePtr data_ov002_0210eb88;
SharedFilePtr data_ov002_0210eb90;
SharedFilePtr data_ov002_0210eb98;
SharedFilePtr data_ov002_0210eba0;
SharedFilePtr data_ov002_0210eba8;
SharedFilePtr data_ov002_0210ebb0;
SharedFilePtr data_ov002_0210ebb8;
SharedFilePtr data_ov002_0210ebc0;
SharedFilePtr data_ov002_0210ebc8;
SharedFilePtr data_ov002_0210ebd0;
SharedFilePtr data_ov002_0210ebd8;
SharedFilePtr data_ov002_0210ebe0;
SharedFilePtr data_ov002_0210ebe8;
SharedFilePtr data_ov002_0210ebf0;
SharedFilePtr data_ov002_0210ebf8;
SharedFilePtr data_ov002_0210ec00;
SharedFilePtr data_ov002_0210ec08;
SharedFilePtr data_ov002_0210ec10;
SharedFilePtr data_ov002_0210ec18;
SharedFilePtr data_ov002_0210ec20;
SharedFilePtr data_ov002_0210ec28;
SharedFilePtr data_ov002_0210ec30;
SharedFilePtr data_ov002_0210ec38;
SharedFilePtr data_ov002_0210ec40;
SharedFilePtr data_ov002_0210ec48;
SharedFilePtr data_ov002_0210ec50;
SharedFilePtr data_ov002_0210ec58;
SharedFilePtr data_ov002_0210ec60;
SharedFilePtr data_ov002_0210ec68;
SharedFilePtr data_ov002_0210ec70;
SharedFilePtr data_ov002_0210ec78;
SharedFilePtr data_ov002_0210ec80;
SharedFilePtr data_ov002_0210ec88;
SharedFilePtr data_ov002_0210ec90;
SharedFilePtr data_ov002_0210ec98;
SharedFilePtr data_ov002_0210eca0;
SharedFilePtr data_ov002_0210eca8;
SharedFilePtr data_ov002_0210ecb0;
SharedFilePtr data_ov002_0210ecb8;
SharedFilePtr data_ov002_0210ecc0;
SharedFilePtr data_ov002_0210ecc8;
SharedFilePtr data_ov002_0210ecd0;
SharedFilePtr data_ov002_0210ecd8;
SharedFilePtr data_ov002_0210ece0;
SharedFilePtr data_ov002_0210ece8;
SharedFilePtr data_ov002_0210ecf0;
SharedFilePtr data_ov002_0210ecf8;
SharedFilePtr data_ov002_0210ed00;
SharedFilePtr data_ov002_0210ed08;
SharedFilePtr data_ov002_0210ed10;
SharedFilePtr data_ov002_0210ed18;
SharedFilePtr data_ov002_0210ed20;
SharedFilePtr data_ov002_0210ed28;
SharedFilePtr data_ov002_0210ed30;
SharedFilePtr data_ov002_0210ed38;
SharedFilePtr data_ov002_0210ed40;
SharedFilePtr data_ov002_0210ed48;
SharedFilePtr data_ov002_0210ed50;
SharedFilePtr data_ov002_0210ed58;
SharedFilePtr data_ov002_0210ed60;
void *data_ov002_0210ed68[3];
void *data_ov002_0210ed74[3];
void *data_ov002_0210ed80[3];
void *data_ov002_0210ed8c[3];
void *data_ov002_0210ed98[3];
void *data_ov002_0210eda4[3];
void *data_ov002_0210edb0[3];
void *data_ov002_0210edbc[3];
void *data_ov002_0210edc8[3];
void *data_ov002_0210edd4[3];
void *data_ov002_0210ede0[3];
void *data_ov002_0210edec[3];
void *data_ov002_0210edf8[3];
void *data_ov002_0210ee04[3];
void *data_ov002_0210ee10[3];
void *data_ov002_0210ee1c[3];
void *data_ov002_0210ee28[3];
void *data_ov002_0210ee34[3];
void *data_ov002_0210ee40[3];
void *data_ov002_0210ee4c[3];
void *data_ov002_0210ee58[3];
void *data_ov002_0210ee64[3];
void *data_ov002_0210ee70[3];
void *data_ov002_0210ee7c[3];
void *data_ov002_0210ee88[3];
void *data_ov002_0210ee94[3];
void *data_ov002_0210eea0[3];
void *data_ov002_0210eeac[3];
void *data_ov002_0210eeb8[3];
void *data_ov002_0210eec4[3];
void *data_ov002_0210eed0[3];
void *data_ov002_0210eedc[3];
void *data_ov002_0210eee8[3];
void *data_ov002_0210eef4[3];
void *data_ov002_0210ef00[3];
void *data_ov002_0210ef0c[3];
void *data_ov002_0210ef18[3];
void *data_ov002_0210ef24[3];
void *data_ov002_0210ef30[3];
void *data_ov002_0210ef3c[3];
void *data_ov002_0210ef48[3];
void *data_ov002_0210ef54[3];
void *data_ov002_0210ef60[3];
void *data_ov002_0210ef6c[3];
void *data_ov002_0210ef78[3];
void *data_ov002_0210ef84[3];
void *data_ov002_0210ef90[3];
void *data_ov002_0210ef9c[3];
void *data_ov002_0210efa8[3];
void *data_ov002_0210efb4[3];
void *data_ov002_0210efc0[3];
void *data_ov002_0210efcc[3];
void *data_ov002_0210efd8[3];
void *data_ov002_0210efe4[3];
void *data_ov002_0210eff0[3];
void *data_ov002_0210effc[3];
void *data_ov002_0210f008[3];
void *data_ov002_0210f014[3];
void *data_ov002_0210f020[3];
void *data_ov002_0210f02c[3];
void *data_ov002_0210f038[3];
void *data_ov002_0210f044[3];
void *data_ov002_0210f050[3];
void *data_ov002_0210f05c[3];
void *data_ov002_0210f068[3];
void *data_ov002_0210f074[3];
void *data_ov002_0210f080[3];
void *data_ov002_0210f08c[3];
void *data_ov002_0210f098[3];
void *data_ov002_0210f0a4[3];
void *data_ov002_0210f0b0[3];
void *data_ov002_0210f0bc[3];
void *data_ov002_0210f0c8[3];
void *data_ov002_0210f0d4[3];
void *data_ov002_0210f0e0[3];
void *data_ov002_0210f0ec[3];
void *data_ov002_0210f0f8[3];
void *data_ov002_0210f104[3];
void *data_ov002_0210f110[3];
void *data_ov002_0210f11c[3];
void *data_ov002_0210f128[3];
void *data_ov002_0210f134[3];
void *data_ov002_0210f140[3];
void *data_ov002_0210f14c[3];
void *data_ov002_0210f158[3];
void *data_ov002_0210f164[3];
void *data_ov002_0210f170[3];
void *data_ov002_0210f17c[3];
void *data_ov002_0210f188[3];
void *data_ov002_0210f194[3];
void *data_ov002_0210f1a0[3];
void *data_ov002_0210f1b8[3];
void *data_ov002_0210f1c4[3];
void *data_ov002_0210f1d0[3];
void *data_ov002_0210f1dc[3];
void *data_ov002_0210f1e8[3];
void *data_ov002_0210f1f4[3];
void *data_ov002_0210f200[3];
void *data_ov002_0210f20c[3];
void *data_ov002_0210f218[3];
void *data_ov002_0210f230[3];
void *data_ov002_0210f23c[3];
void *data_ov002_0210f248[3];
void *data_ov002_0210f254[3];
void *data_ov002_0210f260[3];
void *data_ov002_0210f26c[3];
void *data_ov002_0210f278[3];
void *data_ov002_0210f284[3];
void *data_ov002_0210f290[3];
void *data_ov002_0210f29c[3];
void *data_ov002_0210f2a8[3];
void *data_ov002_0210f2b4[3];
void *data_ov002_0210f2c0[3];
void *data_ov002_0210f2cc[3];
void *data_ov002_0210f2d8[3];
void *data_ov002_0210f2e4[3];
void *data_ov002_0210f2f0[3];
void *data_ov002_0210f2fc[3];
void *data_ov002_0210f314[3];
void *data_ov002_0210f320[3];
void *data_ov002_0210f32c[3];
void *data_ov002_0210f338[3];
void *data_ov002_0210f35c[3];
void *data_ov002_0210f368[3];
void *data_ov002_0210f374[3];
void *data_ov002_0210f380[3];
void *data_ov002_0210f38c[3];
void *data_ov002_0210f398[3];
void *data_ov002_0210f3a4[3];
void *data_ov002_0210f3bc[3];
void *data_ov002_0210f3c8[3];
void *data_ov002_0210f3d4[3];
void *data_ov002_0210f3e0[3];
void *data_ov002_0210f3ec[3];
void *data_ov002_0210f3f8[3];
void *data_ov002_0210f404[3];
void *data_ov002_0210f410[3];
void *data_ov002_0210f41c[3];
void *data_ov002_0210f428[3];
void *data_ov002_0210f434[3];
void *data_ov002_0210f440[3];
void *data_ov002_0210f44c[3];
void *data_ov002_0210f458[3];
void *data_ov002_0210f464[3];
void *data_ov002_0210f470[3];
void *data_ov002_0210f47c[3];
void *data_ov002_0210f488[3];
void *data_ov002_0210f494[3];
void *data_ov002_0210f4a0[3];
void *data_ov002_0210f4ac[3];
void *data_ov002_0210f4b8[3];
void *data_ov002_0210f4c4[3];
void *data_ov002_0210f4d0[3];
void *data_ov002_0210f4dc[3];
void *data_ov002_0210f4e8[3];
void *data_ov002_0210f4f4[3];
void *data_ov002_0210f500[3];
void *data_ov002_0210f50c[3];
void *data_ov002_0210f518[3];
void *data_ov002_0210f524[3];
void *data_ov002_0210f530[3];
void *data_ov002_0210f53c[3];
void *data_ov002_0210f548[3];
void *data_ov002_0210f554[3];
void *data_ov002_0210f560[3];
void *data_ov002_0210f56c[3];
void *data_ov002_0210f578[3];
void *data_ov002_0210f584[3];
void *data_ov002_0210f590[3];
void *data_ov002_0210f59c[3];
void *data_ov002_0210f5a8[3];
void *data_ov002_0210f5b4[3];
void *data_ov002_0210f5c0[3];
void *data_ov002_0210f5cc[3];
void *data_ov002_0210f5d8[3];
void *data_ov002_0210f5e4[3];
void *data_ov002_0210f5f0[3];
void *data_ov002_0210f5fc[3];
void *data_ov002_0210f608[3];
void *data_ov002_0210f614[3];
void *data_ov002_0210f620[3];
void *data_ov002_0210f62c[3];
void *data_ov002_0210f638[3];
void *data_ov002_0210f644[3];
void *data_ov002_0210f650[3];
void *data_ov002_0210f65c[3];
void *data_ov002_0210f668[3];
void *data_ov002_0210f674[3];
void *data_ov002_0210f680[3];
void *data_ov002_0210f68c[3];
void *data_ov002_0210f698[3];
void *data_ov002_0210f6a4[3];
void *data_ov002_0210f6b0[3];
void *data_ov002_0210f6bc[3];
void *data_ov002_0210f6c8[3];
void *data_ov002_0210f6d4[3];
void *data_ov002_0210f6e0[3];
void *data_ov002_0210f6ec[3];
void *data_ov002_0210f6f8[3];
void *data_ov002_0210f704[3];
void *data_ov002_0210f710[3];
void *data_ov002_0210f71c[3];
void *data_ov002_0210f728[3];
void *data_ov002_0210f734[3];
void *data_ov002_0210f740[3];
void *data_ov002_0210f74c[3];
void *data_ov002_0210f758[3];
void *data_ov002_0210f764[3];
void *data_ov002_0210f770[3];
void *data_ov002_0210f77c[3];
void *data_ov002_0210f788[3];
void *data_ov002_0210f794[3];
void *data_ov002_0210f7a0[3];
void *data_ov002_0210f7ac[3];
void *data_ov002_0210f7b8[3];
void *data_ov002_0210f7c4[3];
void *data_ov002_0210f7d0[3];
void *data_ov002_0210f7dc[3];
void *data_ov002_0210f7e8[3];
void *data_ov002_0210f800[3];
void *data_ov002_0210f80c[3];
void *data_ov002_0210f818[3];
void *data_ov002_0210f830[3];
void *data_ov002_0210f83c[3];
void *data_ov002_0210f848[3];
void *data_ov002_0210f854[3];
void *data_ov002_0210f860[3];
void *data_ov002_0210f86c[3];
void *data_ov002_0210f878[3];
void *data_ov002_0210f884[3];
void *data_ov002_0210f890[3];
void *data_ov002_0210f8a8[3];
void *data_ov002_0210f8b4[3];
void *data_ov002_0210f8c0[3];
void *data_ov002_0210f8d8[3];
void *data_ov002_0210f8e4[3];
void *data_ov002_0210f8f0[3];
void *data_ov002_0210f8fc[3];
void *data_ov002_0210f908[3];
void *data_ov002_0210f914[3];
void *data_ov002_0210f920[3];
int data_ov002_0210f92c[3];
void *data_ov002_0210f938[3];
void *data_ov002_0210f944[3];
void *data_ov002_0210f950[3];
void *data_ov002_0210f95c[3];
void *data_ov002_0210f968[3];
void *data_ov002_0210f974[3];
void *data_ov002_0210f980[3];
void *data_ov002_0210f98c[3];
void *data_ov002_0210f998[3];
void *data_ov002_0210f9a4[3];
void *data_ov002_0210f9b0[3];
void *data_ov002_0210f9bc[3];
void *data_ov002_0210f9c8[3];
void *data_ov002_0210f9d4[3];
void *data_ov002_0210f9e0[3];
void *data_ov002_0210f9ec[3];
void *data_ov002_0210f9f8[3];
void *data_ov002_0210fa04[3];
void *data_ov002_0210fa10[3];
void *data_ov002_0210fa1c[3];
void *data_ov002_0210fa28[3];
void *data_ov002_0210fa34[3];
void *data_ov002_0210fa40[3];
void *data_ov002_0210fa4c[3];
void *data_ov002_0210fa58[3];
void *data_ov002_0210fa64[3];
void *data_ov002_0210fa70[3];
void *data_ov002_0210fa7c[3];
void *data_ov002_0210fa88[3];
void *data_ov002_0210fa94[3];
void *data_ov002_0210faa0[3];
void *data_ov002_0210faac[3];
void *data_ov002_0210fab8[3];
void *data_ov002_0210fac4[3];
void *data_ov002_0210fad0[3];
void *data_ov002_0210fadc[3];
void *data_ov002_0210fae8[3];
void *data_ov002_0210faf4[3];
void *data_ov002_0210fb00[3];
void *data_ov002_0210fb0c[3];
void *data_ov002_0210fb18[3];
void *data_ov002_0210fb24[3];
void *data_ov002_0210fb30[3];
void *data_ov002_0210fb3c[3];
void *data_ov002_0210fb48[3];
void *data_ov002_0210fb54[3];
void *data_ov002_0210fb60[3];
void *data_ov002_0210fb6c[3];
void *data_ov002_0210fb78[3];
void *data_ov002_0210fb84[3];
void *data_ov002_0210fb90[3];
void *data_ov002_0210fb9c[3];
void *data_ov002_0210fba8[3];
void *data_ov002_0210fbb4[3];
void *data_ov002_0210fbc0[3];
void *data_ov002_0210fbcc[3];
void *data_ov002_0210fbd8[3];
void *data_ov002_0210fbe4[3];
void *data_ov002_0210fbf0[3];
void *data_ov002_0210fbfc[3];
void *data_ov002_0210fc08[3];
void *data_ov002_0210fc14[3];
void *data_ov002_0210fc20[3];
void *data_ov002_0210fc2c[3];
void *data_ov002_0210fc38[3];
void *data_ov002_0210fc44[3];
void *data_ov002_0210fc50[3];
void *data_ov002_0210fc5c[3];
void *data_ov002_0210fc68[3];
void *data_ov002_0210fc74[3];
void *data_ov002_0210fc80[3];
void *data_ov002_0210fc8c[3];
void *data_ov002_0210fc98[3];
void *data_ov002_0210fca4[3];
void *data_ov002_0210fcb0[3];
void *data_ov002_0210fcbc[3];
void *data_ov002_0210fcc8[3];
void *data_ov002_0210fcd4[3];
void *data_ov002_0210fce0[3];
void *data_ov002_0210fcec[3];
void *data_ov002_0210fcf8[3];
void *data_ov002_0210fd04[3];
void *data_ov002_0210fd10[3];
void *data_ov002_0210fd1c[3];
void *data_ov002_0210fd28[3];
void *data_ov002_0210fd34[3];
void *data_ov002_0210fd40[3];
void *data_ov002_0210fd4c[3];
void *data_ov002_0210fd58[3];
void *data_ov002_0210fd64[3];
void *data_ov002_0210fd70[3];
void *data_ov002_0210fd7c[3];
void *data_ov002_0210fd88[3];
void *data_ov002_0210fd94[3];
void *data_ov002_0210fda0[3];
void *data_ov002_0210fdac[3];
void *data_ov002_0210fdb8[3];
void *data_ov002_0210fdc4[3];
void *data_ov002_0210fdd0[3];
void *data_ov002_0210fddc[3];
void *data_ov002_0210fde8[3];
void *data_ov002_0210fdf4[3];
void *data_ov002_0210fe00[3];
void *data_ov002_0210fe0c[3];
void *data_ov002_0210fe18[3];
void *data_ov002_0210fe24[3];
void *data_ov002_0210fe30[3];
void *data_ov002_0210fe3c[3];
void *data_ov002_0210fe48[3];
void *data_ov002_0210fe54[3];
void *data_ov002_0210fe60[3];
void *data_ov002_0210fe6c[3];
void *data_ov002_0210fe78[3];
void *data_ov002_0210fe84[3];
void *data_ov002_0210fe90[3];
void *data_ov002_0210fe9c[3];
void *data_ov002_0210fea8[3];
void *data_ov002_0210feb4[3];
void *data_ov002_0210fec0[3];
void *data_ov002_0210fecc[3];
void *data_ov002_0210fed8[3];
void *data_ov002_0210fee4[3];
void *data_ov002_0210fef0[3];
void *data_ov002_0210fefc[3];
void *data_ov002_0210ff08[3];
void *data_ov002_0210ff14[3];
void *data_ov002_0210ff20[3];
void *data_ov002_0210ff2c[3];
void *data_ov002_0210ff38[3];
void *data_ov002_0210ff44[3];
void *data_ov002_0210ff50[3];
void *data_ov002_0210ff5c[3];
void *data_ov002_0210ff68[3];
void *data_ov002_0210ff74[3];
void *data_ov002_0210ff80[3];
void *data_ov002_0210ff8c[3];
void *data_ov002_0210ff98[3];
void *data_ov002_0210ffa4[3];
void *data_ov002_0210ffb0[3];
void *data_ov002_0210ffbc[3];
void *data_ov002_0210ffc8[3];
void *data_ov002_0210ffd4[3];
void *data_ov002_0210ffe0[3];
daPly_c::State data_ov002_0210ffec;
daPly_c::State data_ov002_02110004;
daPly_c::State data_ov002_0211001c;
daPly_c::State data_ov002_02110034;
daPly_c::State data_ov002_0211004c;
daPly_c::State data_ov002_02110064;
daPly_c::State data_ov002_0211007c;
daPly_c::State data_ov002_02110094;
daPly_c::State data_ov002_021100ac;
daPly_c::State data_ov002_021100c4;
daPly_c::State data_ov002_021100dc;
daPly_c::State data_ov002_021100f4;
daPly_c::State data_ov002_0211010c;
daPly_c::State data_ov002_02110124;
daPly_c::State data_ov002_0211013c;
}
/* St_Wait. C++ linkage, so the object is _ZN7daPly_c7ST_WAITE. */
daPly_c::State daPly_c::ST_WAIT;
extern "C" {
daPly_c::State data_ov002_0211016c;
daPly_c::State data_ov002_02110184;
daPly_c::State data_ov002_0211019c;
daPly_c::State data_ov002_021101b4;
daPly_c::State data_ov002_021101cc;
daPly_c::State data_ov002_021101e4;
daPly_c::State data_ov002_021101fc;
daPly_c::State data_ov002_02110214;
daPly_c::State data_ov002_0211022c;
}
/* St_Owl. C++ linkage, so the object is _ZN7daPly_c6ST_OWLE. */
daPly_c::State daPly_c::ST_OWL;
extern "C" {
daPly_c::State data_ov002_0211025c;
daPly_c::State data_ov002_02110274;
daPly_c::State data_ov002_0211028c;
daPly_c::State data_ov002_021102a4;
daPly_c::State data_ov002_021102bc;
daPly_c::State data_ov002_021102d4;
daPly_c::State data_ov002_021102ec;
daPly_c::State data_ov002_02110304;
daPly_c::State data_ov002_0211031c;
daPly_c::State data_ov002_02110334;
daPly_c::State data_ov002_0211034c;
daPly_c::State data_ov002_02110364;
daPly_c::State data_ov002_0211037c;
daPly_c::State data_ov002_02110394;
daPly_c::State data_ov002_021103ac;
daPly_c::State data_ov002_021103c4;
daPly_c::State data_ov002_021103dc;
daPly_c::State data_ov002_021103f4;
daPly_c::State data_ov002_0211040c;
daPly_c::State data_ov002_02110424;
daPly_c::State data_ov002_0211043c;
daPly_c::State data_ov002_02110454;
daPly_c::State data_ov002_0211046c;
daPly_c::State data_ov002_02110484;
daPly_c::State data_ov002_0211049c;
daPly_c::State data_ov002_021104b4;
daPly_c::State data_ov002_021104cc;
daPly_c::State data_ov002_021104e4;
daPly_c::State data_ov002_021104fc;
daPly_c::State data_ov002_02110514;
daPly_c::State data_ov002_0211052c;
daPly_c::State data_ov002_02110544;
daPly_c::State data_ov002_0211055c;
daPly_c::State data_ov002_02110574;
daPly_c::State data_ov002_0211058c;
daPly_c::State data_ov002_021105a4;
daPly_c::State data_ov002_021105bc;
daPly_c::State data_ov002_021105d4;
daPly_c::State data_ov002_021105ec;
daPly_c::State data_ov002_02110604;
daPly_c::State data_ov002_0211061c;
daPly_c::State data_ov002_02110634;
daPly_c::State data_ov002_0211064c;
daPly_c::State data_ov002_02110664;
daPly_c::State data_ov002_0211067c;
daPly_c::State data_ov002_02110694;
daPly_c::State data_ov002_021106ac;
daPly_c::State data_ov002_021106c4;
daPly_c::State data_ov002_021106dc;
daPly_c::State data_ov002_021106f4;
daPly_c::State data_ov002_0211070c;
daPly_c::State data_ov002_02110724;
void (daPly_c::*data_ov002_0211073c[4])();
void (daPly_c::*data_ov002_0211075c[7])();
ScriptCommand data_ov002_02110794[14];
unsigned short data_ov002_02110804[64];
/* A PlayerStateFn[20] definition is not trivial: mwcc emits an empty sinit
 * and a .ctor word for it. The bytes are 160 zero-filled BSS bytes. The
 * stores below cast this object back to that view. */
unsigned char data_ov002_02110884[0xa0];
unsigned int data_ov002_0210e158;
AnimScriptStep data_ov002_0210e7b8[1];
}

#pragma define_section initsec ".init"                                                             \
                               ".init"                                                             \
                               ".init" far_absolute RX // __sinit would leave .init
#pragma section initsec begin                          // without it, __sinit would leave .init
/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov002_021019d0_AssetId {
    ASSET_B_MARIO_ALL_BMD = 0x04be,
    ASSET_B_MARIO_BODY_BMD = 0x04bf,
    ASSET_B_MARIO_HEAD_CAP_BMD = 0x04c0,
    ASSET_B_MARIO_START_BCA = 0x04c1,
    ASSET_COLD_WAIT_BCA = 0x04c2,
    ASSET_COLD_WAIT_END_BCA = 0x04c3,
    ASSET_COLD_WAIT_START_BCA = 0x04c4,
    ASSET_DAMAGE_START_BCA = 0x04c5,
    ASSET_DAMAGE_WAIT_BCA = 0x04c6,
    ASSET_DOOR_PULL_BCA = 0x04c7,
    ASSET_DOOR_PUSH_BCA = 0x04c8,
    ASSET_DROWN_DIE_BCA = 0x04c9,
    ASSET_ED_CAP_OFF_BCA = 0x04ca,
    ASSET_ED_CAP_OFF_WAIT_BCA = 0x04cb,
    ASSET_ED_FLY_BCA = 0x04cc,
    ASSET_ED_FLY_START_BCA = 0x04cd,
    ASSET_ED_KISS_TO_POSE_BCA = 0x04ce,
    ASSET_ED_KISS_WAIT_BCA = 0x04cf,
    ASSET_ED_LOOK_LEFT_BCA = 0x04d0,
    ASSET_ED_LOOK_LEFT_END_BCA = 0x04d1,
    ASSET_ED_LOOK_LEFT_WAIT_BCA = 0x04d2,
    ASSET_ED_NOD_BCA = 0x04d3,
    ASSET_ED_PUTUP_WAIT_BCA = 0x04d4,
    ASSET_ED_WAIT_BCA = 0x04d5,
    ASSET_ED_WAVE_BCA = 0x04d6,
    ASSET_E_DOWN_BCA = 0x04d7,
    ASSET_FALL_DIE_BCA = 0x04d8,
    ASSET_FALL_SAND_BCA = 0x04d9,
    ASSET_FIRST_CHANGE_BCA = 0x04da,
    ASSET_GAS_DIE_BCA = 0x04db,
    ASSET_GAS_WAIT_BCA = 0x04dc,
    ASSET_H_CARRY_START_BCA = 0x04dd,
    ASSET_H_CARRY_THROW_BCA = 0x04de,
    ASSET_H_CARRY_WAIT_BCA = 0x04df,
    ASSET_H_CARRY_WALK_BCA = 0x04e0,
    ASSET_KEY_GET_BCA = 0x04e1,
    ASSET_KEY_OPEN_BCA = 0x04e2,
    ASSET_LIE_END_BCA = 0x04e3,
    ASSET_LIE_WAIT_BCA = 0x04e4,
    ASSET_L_ED_AMAZED_BCA = 0x04e5,
    ASSET_L_ED_CAP_OFF_BCA = 0x04e6,
    ASSET_L_ED_CAP_OFF_WAIT_BCA = 0x04e7,
    ASSET_L_ED_LOOKUP_WAIT_BCA = 0x04e8,
    ASSET_L_ED_LOOK_MARIO_BCA = 0x04e9,
    ASSET_L_ED_LOOK_M_WAIT_BCA = 0x04ea,
    ASSET_L_ED_WAIT_BCA = 0x04eb,
    ASSET_L_ENTRY_WAIT_BCA = 0x04ec,
    ASSET_L_ENTRY_WAIT_END_BCA = 0x04ed,
    ASSET_L_FIRST_CHANGE_BCA = 0x04ee,
    ASSET_L_LIE_END_BCA = 0x04ef,
    ASSET_L_LIE_WAIT_BCA = 0x04f0,
    ASSET_L_OP_LOOK_WARIO_BCA = 0x04f1,
    ASSET_L_OP_NOD_BCA = 0x04f2,
    ASSET_L_OP_TURN_BCA = 0x04f3,
    ASSET_L_SLEEP_END_BCA = 0x04f4,
    ASSET_L_SLEEP_START_BCA = 0x04f5,
    ASSET_L_SLEEP_TO_LIE_BCA = 0x04f6,
    ASSET_L_SLEEP_WAIT_BCA = 0x04f7,
    ASSET_L_TX_DAMAGE_START_BTP = 0x04f8,
    ASSET_L_TX_DAMAGE_WAIT_BTP = 0x04f9,
    ASSET_L_TX_ED_AMAZED_BTP = 0x04fa,
    ASSET_L_TX_GAS_WAIT_BTP = 0x04fb,
    ASSET_L_TX_LIE_END_BTP = 0x04fc,
    ASSET_L_TX_LIE_WAIT_BTP = 0x04fd,
    ASSET_L_TX_RETURN_DOWN_BTP = 0x04fe,
    ASSET_L_TX_SLEEP_END_BTP = 0x04ff,
    ASSET_L_TX_SLEEP_START_BTP = 0x0500,
    ASSET_L_TX_SLEEP_TO_LIE_BTP = 0x0501,
    ASSET_L_TX_SLEEP_WAIT_BTP = 0x0502,
    ASSET_M_ENTRY_WAIT_BCA = 0x0503,
    ASSET_M_ENTRY_WAIT_END_BCA = 0x0504,
    ASSET_N_CARRY_LOST_BCA = 0x0505,
    ASSET_N_CARRY_POSE_BCA = 0x0506,
    ASSET_N_CARRY_PUT_BCA = 0x0507,
    ASSET_N_CARRY_START_BCA = 0x0508,
    ASSET_N_CARRY_WAIT_BCA = 0x0509,
    ASSET_N_CARRY_WALK_BCA = 0x050a,
    ASSET_OP_GO_BCA = 0x050b,
    ASSET_OP_WAIT_BCA = 0x050c,
    ASSET_PADDLE_1_BCA = 0x050d,
    ASSET_PADDLE_2_BCA = 0x050e,
    ASSET_PADDLE_CARRY_BCA = 0x050f,
    ASSET_PADDLE_END_BCA = 0x0510,
    ASSET_PUTON_T_CAP_BCA = 0x0511,
    ASSET_P_KOOPA_HOLD_BCA = 0x0512,
    ASSET_P_KOOPA_H_WAIT_BCA = 0x0513,
    ASSET_P_KOOPA_THROW_BCA = 0x0514,
    ASSET_RETURN_DOWN_BCA = 0x0515,
    ASSET_RETURN_NOCAP_BCA = 0x0516,
    ASSET_RETURN_STAR_BCA = 0x0517,
    ASSET_SCALE_UP_BCA = 0x0518,
    ASSET_SDROP_HEAD_BCA = 0x0519,
    ASSET_SDROP_HEAD_END_BCA = 0x051a,
    ASSET_SDROP_HEAD_WAIT_BCA = 0x051b,
    ASSET_SDROP_HIP_BCA = 0x051c,
    ASSET_SDROP_HIP_END_BCA = 0x051d,
    ASSET_SDROP_HIP_WAIT_BCA = 0x051e,
    ASSET_SDROP_LEG_BCA = 0x051f,
    ASSET_SDROP_LEG_END_BCA = 0x0520,
    ASSET_SDROP_LEG_WAIT_BCA = 0x0521,
    ASSET_SDROP_WAIT_BCA = 0x0522,
    ASSET_SDROP_WALK_BCA = 0x0523,
    ASSET_SHF_DIE_BCA = 0x0524,
    ASSET_SHOOT_BCA = 0x0525,
    ASSET_SHOOT_TO_FLY_BCA = 0x0526,
    ASSET_SLEEP_END_BCA = 0x0527,
    ASSET_SLEEP_START_BCA = 0x0528,
    ASSET_SLEEP_TO_LIE_BCA = 0x0529,
    ASSET_SLEEP_WAIT_BCA = 0x052a,
    ASSET_STAR_OPEN_BCA = 0x052b,
    ASSET_SU_HIPAT_BCA = 0x052c,
    ASSET_SU_HIPED_BCA = 0x052d,
    ASSET_SU_HIPSR_BCA = 0x052e,
    ASSET_SU_JUMP_BCA = 0x052f,
    ASSET_SU_JUMP_END_BCA = 0x0530,
    ASSET_SU_RUN_BCA = 0x0531,
    ASSET_SU_WAIT_BCA = 0x0532,
    ASSET_SU_WALK_BCA = 0x0533,
    ASSET_SU_WALK_END_BCA = 0x0534,
    ASSET_SU_WALK_START_BCA = 0x0535,
    ASSET_SWIM_BCA = 0x0536,
    ASSET_SWIM_ATTACK_BCA = 0x0537,
    ASSET_SWIM_CARRY_BCA = 0x0538,
    ASSET_SWIM_DAMAGE_BCA = 0x0539,
    ASSET_SWIM_DIE_BCA = 0x053a,
    ASSET_SWIM_IN_BCA = 0x053b,
    ASSET_SWIM_STAR_GET_BCA = 0x053c,
    ASSET_SWIM_THROW_BCA = 0x053d,
    ASSET_SWIM_WAIT_BCA = 0x053e,
    ASSET_S_DIE_BCA = 0x053f,
    ASSET_TX_DAMAGE_START_BTP = 0x0540,
    ASSET_TX_DAMAGE_WAIT_BTP = 0x0541,
    ASSET_TX_ED_KISS_TO_POSE_BTP = 0x0542,
    ASSET_TX_ED_KISS_WAIT_BTP = 0x0543,
    ASSET_TX_GAS_WAIT_BTP = 0x0544,
    ASSET_TX_LIE_END_BTP = 0x0545,
    ASSET_TX_LIE_WAIT_BTP = 0x0546,
    ASSET_TX_RETURN_DOWN_BTP = 0x0547,
    ASSET_TX_SLEEP_END_BTP = 0x0548,
    ASSET_TX_SLEEP_START_BTP = 0x0549,
    ASSET_TX_SLEEP_TO_LIE_BTP = 0x054a,
    ASSET_TX_SLEEP_WAIT_BTP = 0x054b,
    ASSET_WARIO_METAL_MODEL_BMD = 0x054c,
    ASSET_W_ED_AMAZED_WAIR_BCA = 0x054d,
    ASSET_W_ED_CAP_OFF_BCA = 0x054e,
    ASSET_W_ED_CAP_OFF_WAIT_BCA = 0x054f,
    ASSET_W_ED_LOOKUP_WAIT_BCA = 0x0550,
    ASSET_W_ED_WAIT_BCA = 0x0551,
    ASSET_W_ED_WAVE_BCA = 0x0552,
    ASSET_W_ENTRY_WAIT_BCA = 0x0553,
    ASSET_W_ENTRY_WAIT_END_BCA = 0x0554,
    ASSET_W_FIRST_CHANGE_BCA = 0x0555,
    ASSET_W_KEY_GET_BCA = 0x0556,
    ASSET_W_LIE_END_BCA = 0x0557,
    ASSET_W_LIE_WAIT_BCA = 0x0558,
    ASSET_W_OP_LOOK_LUIGI_BCA = 0x0559,
    ASSET_W_OP_NOD_BCA = 0x055a,
    ASSET_W_OP_POINT_BCA = 0x055b,
    ASSET_W_OP_TURN_BCA = 0x055c,
    ASSET_W_SLEEP_END_BCA = 0x055d,
    ASSET_W_SLEEP_START_BCA = 0x055e,
    ASSET_W_SLEEP_TO_LIE_BCA = 0x055f,
    ASSET_W_SLEEP_WAIT_BCA = 0x0560,
    ASSET_W_TX_DAMAGE_START_BTP = 0x0561,
    ASSET_W_TX_DAMAGE_WAIT_BTP = 0x0562,
    ASSET_W_TX_ED_AMAZED_BTP = 0x0563,
    ASSET_W_TX_GAS_WAIT_BTP = 0x0564,
    ASSET_W_TX_LIE_END_BTP = 0x0565,
    ASSET_W_TX_LIE_WAIT_BTP = 0x0566,
    ASSET_W_TX_RETURN_DOWN_BTP = 0x0567,
    ASSET_W_TX_SLEEP_END_BTP = 0x0568,
    ASSET_W_TX_SLEEP_START_BTP = 0x0569,
    ASSET_W_TX_SLEEP_TO_LIE_BTP = 0x056a,
    ASSET_W_TX_SLEEP_WAIT_BTP = 0x056b,
    ASSET_Y_ED_LOOKUP_WAIT_BCA = 0x056c,
    ASSET_Y_ED_LOOK_CAM_BCA = 0x056d,
    ASSET_Y_ED_SHY_TO_WAIT_BCA = 0x056e,
    ASSET_Y_ED_SHY_WAIT_BCA = 0x056f,
    ASSET_Y_ED_TURN_BCA = 0x0570,
    ASSET_Y_ED_WAIT_BCA = 0x0571,
    ASSET_Y_EGG_RUN_BCA = 0x0572,
    ASSET_Y_EGG_WAIT_BCA = 0x0573,
    ASSET_Y_LIE_END_BCA = 0x0574,
    ASSET_Y_LIE_WAIT_BCA = 0x0575,
    ASSET_Y_OP_LIE_TO_SIT_BCA = 0x0576,
    ASSET_Y_OP_SIT_TO_STAND_BCA = 0x0577,
    ASSET_Y_OP_SIT_WAIT_BCA = 0x0578,
    ASSET_Y_OP_STAND_WAIT_BCA = 0x0579,
    ASSET_Y_SLEEP_END_BCA = 0x057a,
    ASSET_Y_SLEEP_START_BCA = 0x057b,
    ASSET_Y_SLEEP_TO_LIE_BCA = 0x057c,
    ASSET_Y_SLEEP_WAIT_BCA = 0x057d,
    ASSET_Y_TX_DAMAGE_START_BTP = 0x057e,
    ASSET_Y_TX_DAMAGE_WAIT_BTP = 0x057f,
    ASSET_Y_TX_GAS_WAIT_BTP = 0x0580,
    ASSET_Y_TX_LIE_END_BTP = 0x0581,
    ASSET_Y_TX_LIE_WAIT_BTP = 0x0582,
    ASSET_Y_TX_RETURN_DOWN_BTP = 0x0583,
    ASSET_Y_TX_SLEEP_END_BTP = 0x0584,
    ASSET_Y_TX_SLEEP_START_BTP = 0x0585,
    ASSET_Y_TX_SLEEP_TO_LIE_BTP = 0x0586
};

extern "C" {

void func_0203b9b4(int *p, int v);
void __register_global_object(void *object, void *destructor, void **node);
}

void *func_02017acc(void *self, unsigned int fileID);
int func_02017ab4(int self);
int SharedFilePtr_Destruct_Anim(int self);
void *SharedFilePtr_Construct_TexSeq(void *self, unsigned int fileID);
int SharedFilePtr_Destruct_TexSeq(int self);
extern "C" {

void func_0201973c(void);
unsigned short *func_ov002_020e6ad4(unsigned short *);

/* The random-number seed RandomIntInternal draws from. */

/* Model, animation and texture-sequence file handles, by address. */

/* __register_global_object chain nodes, one per handle. */
extern void *data_ov002_0210f8d8[3];
extern void *data_ov002_0210f8e4[3];
extern void *data_ov002_0210f8f0[3];
extern void *data_ov002_0210f8fc[3];
extern void *data_ov002_0210f908[3];
extern void *data_ov002_0210f914[3];
extern void *data_ov002_0210f920[3];
extern void *data_ov002_0210f938[3];
extern void *data_ov002_0210f944[3];
extern void *data_ov002_0210f950[3];
extern void *data_ov002_0210f95c[3];
extern void *data_ov002_0210f968[3];
extern void *data_ov002_0210f974[3];
extern void *data_ov002_0210f980[3];
extern void *data_ov002_0210f98c[3];
extern void *data_ov002_0210f998[3];
extern void *data_ov002_0210f9a4[3];
extern void *data_ov002_0210f9b0[3];
extern void *data_ov002_0210f9bc[3];
extern void *data_ov002_0210f9c8[3];
extern void *data_ov002_0210f9d4[3];
extern void *data_ov002_0210f9e0[3];
extern void *data_ov002_0210f9ec[3];
extern void *data_ov002_0210f9f8[3];
extern void *data_ov002_0210fa04[3];
extern void *data_ov002_0210fa10[3];
extern void *data_ov002_0210fa1c[3];
extern void *data_ov002_0210fa28[3];
extern void *data_ov002_0210fa34[3];
extern void *data_ov002_0210fa40[3];
extern void *data_ov002_0210fa4c[3];
extern void *data_ov002_0210fa58[3];
extern void *data_ov002_0210fa64[3];
extern void *data_ov002_0210fa70[3];
extern void *data_ov002_0210fa7c[3];
extern void *data_ov002_0210fa88[3];
extern void *data_ov002_0210fa94[3];
extern void *data_ov002_0210faa0[3];
extern void *data_ov002_0210faac[3];
extern void *data_ov002_0210fab8[3];
extern void *data_ov002_0210fac4[3];
extern void *data_ov002_0210fad0[3];
extern void *data_ov002_0210fadc[3];
extern void *data_ov002_0210fae8[3];
extern void *data_ov002_0210faf4[3];
extern void *data_ov002_0210fb00[3];
extern void *data_ov002_0210fb0c[3];
extern void *data_ov002_0210fb18[3];
extern void *data_ov002_0210fb24[3];
extern void *data_ov002_0210fb30[3];
extern void *data_ov002_0210fb3c[3];
extern void *data_ov002_0210fb48[3];
extern void *data_ov002_0210fb54[3];
extern void *data_ov002_0210fb60[3];
extern void *data_ov002_0210fb6c[3];
extern void *data_ov002_0210fb78[3];
extern void *data_ov002_0210fb84[3];
extern void *data_ov002_0210fb90[3];
extern void *data_ov002_0210fb9c[3];
extern void *data_ov002_0210fba8[3];
extern void *data_ov002_0210fbb4[3];
extern void *data_ov002_0210fbc0[3];
extern void *data_ov002_0210fbcc[3];
extern void *data_ov002_0210fbd8[3];
extern void *data_ov002_0210fbe4[3];
extern void *data_ov002_0210fbf0[3];
extern void *data_ov002_0210fbfc[3];
extern void *data_ov002_0210fc08[3];
extern void *data_ov002_0210fc14[3];
extern void *data_ov002_0210fc20[3];
extern void *data_ov002_0210fc2c[3];
extern void *data_ov002_0210fc38[3];
extern void *data_ov002_0210fc44[3];
extern void *data_ov002_0210fc50[3];
extern void *data_ov002_0210fc5c[3];
extern void *data_ov002_0210fc68[3];
extern void *data_ov002_0210fc74[3];
extern void *data_ov002_0210fc80[3];
extern void *data_ov002_0210fc8c[3];
extern void *data_ov002_0210fc98[3];
extern void *data_ov002_0210fca4[3];
extern void *data_ov002_0210fcb0[3];
extern void *data_ov002_0210fcbc[3];
extern void *data_ov002_0210fcc8[3];
extern void *data_ov002_0210fcd4[3];
extern void *data_ov002_0210fce0[3];
extern void *data_ov002_0210fcec[3];
extern void *data_ov002_0210fcf8[3];
extern void *data_ov002_0210fd04[3];
extern void *data_ov002_0210fd10[3];
extern void *data_ov002_0210fd1c[3];
extern void *data_ov002_0210fd28[3];
extern void *data_ov002_0210fd34[3];
extern void *data_ov002_0210fd40[3];
extern void *data_ov002_0210fd4c[3];
extern void *data_ov002_0210fd58[3];
extern void *data_ov002_0210fd64[3];
extern void *data_ov002_0210fd70[3];
extern void *data_ov002_0210fd7c[3];
extern void *data_ov002_0210fd88[3];
extern void *data_ov002_0210fd94[3];
extern void *data_ov002_0210fda0[3];
extern void *data_ov002_0210fdac[3];
extern void *data_ov002_0210fdb8[3];
extern void *data_ov002_0210fdc4[3];
extern void *data_ov002_0210fdd0[3];
extern void *data_ov002_0210fddc[3];
extern void *data_ov002_0210fde8[3];
extern void *data_ov002_0210fdf4[3];
extern void *data_ov002_0210fe00[3];
extern void *data_ov002_0210fe0c[3];
extern void *data_ov002_0210fe18[3];
extern void *data_ov002_0210fe24[3];
extern void *data_ov002_0210fe30[3];
extern void *data_ov002_0210fe3c[3];
extern void *data_ov002_0210fe48[3];
extern void *data_ov002_0210fe54[3];
extern void *data_ov002_0210fe60[3];
extern void *data_ov002_0210fe6c[3];
extern void *data_ov002_0210fe78[3];
extern void *data_ov002_0210fe84[3];
extern void *data_ov002_0210fe90[3];
extern void *data_ov002_0210fe9c[3];
extern void *data_ov002_0210fea8[3];
extern void *data_ov002_0210feb4[3];
extern void *data_ov002_0210fec0[3];
extern void *data_ov002_0210fecc[3];
extern void *data_ov002_0210fed8[3];
extern void *data_ov002_0210fee4[3];
extern void *data_ov002_0210fef0[3];
extern void *data_ov002_0210fefc[3];
extern void *data_ov002_0210ff08[3];
extern void *data_ov002_0210ff14[3];
extern void *data_ov002_0210ff20[3];
extern void *data_ov002_0210ff2c[3];
extern void *data_ov002_0210ff38[3];
extern void *data_ov002_0210ff44[3];
extern void *data_ov002_0210ff50[3];
extern void *data_ov002_0210ff5c[3];
extern void *data_ov002_0210ff68[3];
extern void *data_ov002_0210ff74[3];
extern void *data_ov002_0210ff80[3];
extern void *data_ov002_0210ff8c[3];
extern void *data_ov002_0210ff98[3];
extern void *data_ov002_0210ffa4[3];
extern void *data_ov002_0210ffb0[3];
extern void *data_ov002_0210ffbc[3];
extern void *data_ov002_0210ffc8[3];
extern void *data_ov002_0210ffd4[3];
extern void *data_ov002_0210ffe0[3];

/* Pointer-to-member constants in ov002 .data, copied into the states below. */
extern int (daPly_c::*data_ov002_02109dd4)();
extern int (daPly_c::*data_ov002_02109ddc)();
extern int (daPly_c::*data_ov002_02109de4)();
extern int (daPly_c::*data_ov002_02109dec)();
extern int (daPly_c::*data_ov002_02109df4)();
extern int (daPly_c::*data_ov002_02109e04)();
extern int (daPly_c::*data_ov002_02109e0c)();
extern int (daPly_c::*data_ov002_02109e14)();
extern int (daPly_c::*data_ov002_02109e1c)();
extern int (daPly_c::*data_ov002_02109e24)();
extern int (daPly_c::*data_ov002_02109e34)();
extern int (daPly_c::*data_ov002_02109e3c)();
extern int (daPly_c::*data_ov002_02109e44)();
extern int (daPly_c::*data_ov002_02109e4c)();
extern int (daPly_c::*data_ov002_02109e54)();
extern int (daPly_c::*data_ov002_02109e5c)();
extern int (daPly_c::*data_ov002_02109e64)();
extern int (daPly_c::*data_ov002_02109e6c)();
extern int (daPly_c::*data_ov002_02109e74)();
extern int (daPly_c::*data_ov002_02109e7c)();
extern int (daPly_c::*data_ov002_02109e84)();
extern int (daPly_c::*data_ov002_02109e8c)();
extern int (daPly_c::*data_ov002_02109e94)();
extern int (daPly_c::*data_ov002_02109e9c)();
extern int (daPly_c::*data_ov002_02109ea4)();
extern int (daPly_c::*data_ov002_02109eac)();
extern int (daPly_c::*data_ov002_02109eb4)();
extern int (daPly_c::*data_ov002_02109ebc)();
extern int (daPly_c::*data_ov002_02109ec4)();
extern int (daPly_c::*data_ov002_02109ecc)();
extern int (daPly_c::*data_ov002_02109ed4)();
extern int (daPly_c::*data_ov002_02109edc)();
extern int (daPly_c::*data_ov002_02109ee4)();
extern int (daPly_c::*data_ov002_02109eec)();
extern int (daPly_c::*data_ov002_02109ef4)();
extern int (daPly_c::*data_ov002_02109efc)();
extern int (daPly_c::*data_ov002_02109f04)();
extern int (daPly_c::*data_ov002_02109f14)();
extern int (daPly_c::*data_ov002_02109f1c)();
extern int (daPly_c::*data_ov002_02109f24)();
extern int (daPly_c::*data_ov002_02109f2c)();
extern int (daPly_c::*data_ov002_02109f34)();
extern int (daPly_c::*data_ov002_02109f3c)();
extern int (daPly_c::*data_ov002_02109f4c)();
extern int (daPly_c::*data_ov002_02109f5c)();
extern int (daPly_c::*data_ov002_02109f64)();
extern int (daPly_c::*data_ov002_02109f6c)();
extern int (daPly_c::*data_ov002_02109f74)();
extern int (daPly_c::*data_ov002_02109f7c)();
extern int (daPly_c::*data_ov002_02109f84)();
extern int (daPly_c::*data_ov002_02109f8c)();
extern int (daPly_c::*data_ov002_02109f94)();
extern int (daPly_c::*data_ov002_02109f9c)();
extern int (daPly_c::*data_ov002_02109fa4)();
extern int (daPly_c::*data_ov002_02109fac)();
extern int (daPly_c::*data_ov002_02109fb4)();
extern int (daPly_c::*data_ov002_02109fbc)();
extern int (daPly_c::*data_ov002_02109fc4)();
extern int (daPly_c::*data_ov002_02109fcc)();
extern int (daPly_c::*data_ov002_02109fd4)();
extern int (daPly_c::*data_ov002_02109fdc)();
extern int (daPly_c::*data_ov002_02109fec)();
extern int (daPly_c::*data_ov002_02109ffc)();
extern int (daPly_c::*data_ov002_0210a004)();
extern int (daPly_c::*data_ov002_0210a00c)();
extern int (daPly_c::*data_ov002_0210a014)();
extern int (daPly_c::*data_ov002_0210a01c)();
extern int (daPly_c::*data_ov002_0210a024)();
extern int (daPly_c::*data_ov002_0210a02c)();
extern int (daPly_c::*data_ov002_0210a034)();
extern int (daPly_c::*data_ov002_0210a03c)();
extern int (daPly_c::*data_ov002_0210a044)();
extern int (daPly_c::*data_ov002_0210a04c)();
extern int (daPly_c::*data_ov002_0210a05c)();
extern int (daPly_c::*data_ov002_0210a06c)();
extern int (daPly_c::*data_ov002_0210a08c)();
extern int (daPly_c::*data_ov002_0210a09c)();
extern int (daPly_c::*data_ov002_0210a0a4)();
extern int (daPly_c::*data_ov002_0210a0ac)();
extern int (daPly_c::*data_ov002_0210a0bc)();
extern int (daPly_c::*data_ov002_0210a0c4)();
extern int (daPly_c::*data_ov002_0210a0cc)();
extern int (daPly_c::*data_ov002_0210a0d4)();
extern int (daPly_c::*data_ov002_0210a0e4)();
extern int (daPly_c::*data_ov002_0210a0ec)();
extern int (daPly_c::*data_ov002_0210a0f4)();
extern int (daPly_c::*data_ov002_0210a0fc)();
extern int (daPly_c::*data_ov002_0210a104)();
extern int (daPly_c::*data_ov002_0210a114)();
extern int (daPly_c::*data_ov002_0210a11c)();
extern int (daPly_c::*data_ov002_0210a13c)();
extern int (daPly_c::*data_ov002_0210a15c)();
extern int (daPly_c::*data_ov002_0210a164)();
extern int (daPly_c::*data_ov002_0210a16c)();
extern int (daPly_c::*data_ov002_0210a17c)();
extern int (daPly_c::*data_ov002_0210a184)();
extern int (daPly_c::*data_ov002_0210a18c)();
extern int (daPly_c::*data_ov002_0210a194)();
extern int (daPly_c::*data_ov002_0210a19c)();
extern int (daPly_c::*data_ov002_0210a1a4)();
extern int (daPly_c::*data_ov002_0210a1ac)();
extern int (daPly_c::*data_ov002_0210a1bc)();
extern int (daPly_c::*data_ov002_0210a1c4)();
extern int (daPly_c::*data_ov002_0210a1dc)();
extern int (daPly_c::*data_ov002_0210a1e4)();
extern int (daPly_c::*data_ov002_0210a1f4)();
extern int (daPly_c::*data_ov002_0210a204)();
extern int (daPly_c::*data_ov002_0210a20c)();
extern int (daPly_c::*data_ov002_0210a214)();
extern int (daPly_c::*data_ov002_0210a21c)();
extern int (daPly_c::*data_ov002_0210a224)();
extern int (daPly_c::*data_ov002_0210a234)();
extern int (daPly_c::*data_ov002_0210a24c)();
extern int (daPly_c::*data_ov002_0210a254)();
extern int (daPly_c::*data_ov002_0210a25c)();
extern int (daPly_c::*data_ov002_0210a26c)();
extern int (daPly_c::*data_ov002_0210a27c)();
extern int (daPly_c::*data_ov002_0210a284)();
extern int (daPly_c::*data_ov002_0210a28c)();
extern int (daPly_c::*data_ov002_0210a294)();
extern int (daPly_c::*data_ov002_0210a2a4)();
extern int (daPly_c::*data_ov002_0210a2ac)();
extern int (daPly_c::*data_ov002_0210a2b4)();
extern int (daPly_c::*data_ov002_0210a2bc)();
extern int (daPly_c::*data_ov002_0210a2c4)();
extern int (daPly_c::*data_ov002_0210a2cc)();
extern int (daPly_c::*data_ov002_0210a2d4)();
extern int (daPly_c::*data_ov002_0210a2dc)();
extern int (daPly_c::*data_ov002_0210a2e4)();
extern int (daPly_c::*data_ov002_0210a2ec)();
extern int (daPly_c::*data_ov002_0210a2f4)();
extern int (daPly_c::*data_ov002_0210a2fc)();
extern int (daPly_c::*data_ov002_0210a304)();
extern int (daPly_c::*data_ov002_0210a314)();
extern int (daPly_c::*data_ov002_0210a31c)();
extern int (daPly_c::*data_ov002_0210a324)();
extern int (daPly_c::*data_ov002_0210a32c)();
extern int (daPly_c::*data_ov002_0210a33c)();
extern int (daPly_c::*data_ov002_0210a344)();
extern PlayerFreeCharPmf data_ov002_02109dfc;
extern PlayerFreeCharPmf data_ov002_02109f0c;
extern void (daPly_c::*data_ov002_02109f54)();
extern void (daPly_c::*data_ov002_0210a074)();
extern PlayerFreeCharPmf data_ov002_0210a10c;
extern void (daPly_c::*data_ov002_0210a12c)();
extern void (daPly_c::*data_ov002_0210a134)();
extern void (daPly_c::*data_ov002_0210a144)();
extern void (daPly_c::*data_ov002_0210a154)();
extern void (daPly_c::*data_ov002_0210a1b4)();
extern void (daPly_c::*data_ov002_0210a1cc)();
extern void (daPly_c::*data_ov002_0210a1ec)();
extern void (daPly_c::*data_ov002_0210a1fc)();
extern void (daPly_c::*data_ov002_0210a22c)();
extern PlayerFreeCharPmf data_ov002_0210a23c;
extern PlayerFreeCharPmf data_ov002_0210a244;
extern PlayerFreeCharPmf data_ov002_0210a264;
extern void (daPly_c::*data_ov002_0210a274)();
extern void (daPly_c::*data_ov002_0210a29c)();
extern void (daPly_c::*data_ov002_0210a30c)();
extern PlayerFreeVoidPmf data_ov002_0210a334;

/* daPly_c handler tables; the second is indexed by mNoCtrlKind, the third by mStateWork.
 * The objects below 0x02110884 are defined in the zero-filled block. C linkage
 * keeps the ROM names. data_ov002_02110884 is defined with them. */
extern "C" {
extern void (daPly_c::*data_ov002_0211073c[4])();
extern unsigned char data_ov002_02110884[0xa0];
extern void (daPly_c::*data_ov002_0211075c[7])();
/* The two 32-entry palette ramps func_ov002_020e69c0 and func_ov002_020e6a78 work on. */
extern unsigned short data_ov002_02110804[];
}

#pragma section initsec end
extern "C" {
/* The state records and handler tables are initialized once, from the
 * handlers they name.  Keeping this in an inline .text function lets mwccarm
 * emit the pointer-to-member records in .data rather than in .init. */
static inline void PlayerInitStates() {
    data_ov002_0211013c.mInit = &daPly_c::St_Walk_Init;
    data_ov002_0211013c.mMain = &daPly_c::St_Walk_Main;
    data_ov002_0211013c.mCleanup = 0;
    daPly_c::ST_WAIT.mInit = &daPly_c::St_Wait_Init;
    daPly_c::ST_WAIT.mMain = &daPly_c::St_Wait_Main;
    daPly_c::ST_WAIT.mCleanup = &daPly_c::St_Wait_Cleanup;
    data_ov002_02110184.mCleanup = 0;
    data_ov002_02110184.mInit = &daPly_c::St_TurnAround_Init;
    data_ov002_02110184.mMain = &daPly_c::St_TurnAround_Main;
    data_ov002_0211019c.mInit = &daPly_c::St_Jump_Init;
    data_ov002_0211019c.mCleanup = 0;
    data_ov002_0211019c.mMain = &daPly_c::St_Jump_Main;
    data_ov002_021101b4.mCleanup = 0;
    data_ov002_021101b4.mInit = &daPly_c::St_Fall_Init;
    data_ov002_021101b4.mMain = &daPly_c::St_Fall_Main;
    data_ov002_021101e4.mInit = &daPly_c::St_SideFlip_Init;
    data_ov002_021101e4.mCleanup = 0;
    data_ov002_021101e4.mMain = &daPly_c::St_SideFlip_Main;
    data_ov002_021101fc.mCleanup = 0;
    data_ov002_021101fc.mInit = &daPly_c::St_SlideKickRecover_Init;
    data_ov002_021101fc.mMain = &daPly_c::St_SideFlip_Main;
    data_ov002_02110214.mInit = &daPly_c::St_Fly_Init;
    data_ov002_02110214.mCleanup = 0;
    data_ov002_02110214.mMain = &daPly_c::St_Fly_Main;
    daPly_c::ST_OWL.mInit = &daPly_c::St_Owl_Init;
    daPly_c::ST_OWL.mMain = &daPly_c::St_Owl_Main;
    daPly_c::ST_OWL.mCleanup = &daPly_c::St_Owl_Cleanup;
    data_ov002_02110274.mCleanup = 0;
    data_ov002_02110274.mInit = &daPly_c::St_WindCarry_Init;
    data_ov002_02110274.mMain = &daPly_c::St_WindCarry_Main;
    data_ov002_0211028c.mInit = &daPly_c::St_Balloon_Init;
    data_ov002_0211028c.mMain = &daPly_c::St_Balloon_Main;
    data_ov002_0211028c.mCleanup = &daPly_c::St_Balloon_Cleanup;
    data_ov002_021102bc.mCleanup = 0;
    data_ov002_021102bc.mInit = &daPly_c::St_TornadoSpin_Init;
    data_ov002_021102bc.mMain = &daPly_c::St_TornadoSpin_Main;
    data_ov002_021102d4.mInit = &daPly_c::St_Cannon_Init;
    data_ov002_021102d4.mMain = &daPly_c::St_Cannon_Main;
    data_ov002_021102d4.mCleanup = &daPly_c::St_Cannon_Cleanup;
    data_ov002_02110304.mInit = &daPly_c::St_Shell_Init;
    data_ov002_02110304.mMain = &daPly_c::St_Shell_Main;
    data_ov002_02110304.mCleanup = &daPly_c::St_Shell_Cleanup;
    data_ov002_0211031c.mCleanup = 0;
    data_ov002_0211031c.mInit = &daPly_c::St_StomachSlide_Init;
    data_ov002_0211031c.mMain = &daPly_c::St_StomachSlide_Main;
    data_ov002_02110334.mInit = &daPly_c::St_ButtSlide_Init;
    data_ov002_02110334.mCleanup = 0;
    data_ov002_02110334.mMain = &daPly_c::St_ButtSlide_Main;
    data_ov002_02110364.mInit = &daPly_c::St_HoldLight_Init;
    data_ov002_02110364.mMain = &daPly_c::St_HoldLight_Main;
    data_ov002_02110364.mCleanup = &daPly_c::St_HoldLight_Cleanup;
    data_ov002_02110394.mCleanup = 0;
    data_ov002_02110394.mInit = &daPly_c::St_HoldHeavy_Init;
    data_ov002_02110394.mMain = &daPly_c::St_HoldHeavy_Main;
    data_ov002_021103ac.mInit = &daPly_c::St_WallSlide_Init;
    data_ov002_021103ac.mCleanup = 0;
    data_ov002_021103ac.mMain = &daPly_c::St_WallSlide_Main;
    data_ov002_021103dc.mCleanup = 0;
    data_ov002_021103dc.mInit = &daPly_c::St_WallJump_Init;
    data_ov002_021103dc.mMain = &daPly_c::St_WallJump_Main;
    data_ov002_021103f4.mInit = &daPly_c::St_SlopeJump_Init;
    data_ov002_021103f4.mCleanup = 0;
    data_ov002_021103f4.mMain = &daPly_c::St_SlopeJump_Main;
    data_ov002_02110424.mCleanup = 0;
    data_ov002_02110424.mInit = &daPly_c::St_Land_Init;
    data_ov002_02110424.mMain = &daPly_c::St_Land_Main;
    data_ov002_0211043c.mInit = &daPly_c::St_OnWall_Init;
    data_ov002_0211043c.mCleanup = 0;
    data_ov002_0211043c.mMain = &daPly_c::St_OnWall_Main;
    data_ov002_02110454.mInit = &daPly_c::St_Spin_Init;
    data_ov002_02110454.mMain = &daPly_c::St_Spin_Main;
    data_ov002_02110454.mCleanup = &daPly_c::St_Spin_Cleanup;
    data_ov002_02110484.mInit = &daPly_c::St_CrazedCrate_Init;
    data_ov002_02110484.mMain = &daPly_c::St_CrazedCrate_Main;
    data_ov002_02110484.mCleanup = &daPly_c::St_CrazedCrate_Cleanup;
    data_ov002_0211049c.mCleanup = 0;
    data_ov002_0211049c.mInit = &daPly_c::St_WaitQuicksand_Init;
    data_ov002_0211049c.mMain = &daPly_c::St_WaitQuicksand_Main;
    data_ov002_021104cc.mInit = &daPly_c::St_JumpQuicksand_Init;
    data_ov002_021104cc.mCleanup = 0;
    data_ov002_021104cc.mMain = &daPly_c::St_JumpQuicksand_Main;
    data_ov002_021104e4.mCleanup = 0;
    data_ov002_021104e4.mInit = &daPly_c::St_Crouch_Init;
    data_ov002_021104e4.mMain = &daPly_c::St_Crouch_Main;
    data_ov002_02110514.mInit = &daPly_c::St_Crawl_Init;
    data_ov002_02110514.mCleanup = 0;
    data_ov002_02110514.mMain = &daPly_c::St_Crawl_Main;
    data_ov002_0211052c.mCleanup = 0;
    data_ov002_0211052c.mInit = &daPly_c::St_BackFlip_Init;
    data_ov002_0211052c.mMain = &daPly_c::St_SideFlip_Main;
    data_ov002_0211055c.mInit = &daPly_c::St_LongJump_Init;
    data_ov002_0211055c.mCleanup = 0;
    data_ov002_0211055c.mMain = &daPly_c::St_LongJump_Main;
    data_ov002_02110574.mCleanup = 0;
    data_ov002_02110574.mInit = &daPly_c::St_PunchKick_Init;
    data_ov002_02110574.mMain = &daPly_c::St_PunchKick_Main;
    data_ov002_021105a4.mInit = &daPly_c::St_GroundPound_Init;
    data_ov002_021105a4.mMain = &daPly_c::St_GroundPound_Main;
    data_ov002_021105a4.mCleanup = &daPly_c::St_GroundPound_Cleanup;
    data_ov002_021105bc.mCleanup = 0;
    data_ov002_021105bc.mInit = &daPly_c::St_Dive_Init;
    data_ov002_021105bc.mMain = &daPly_c::St_StomachSlide_Main;
    data_ov002_021105d4.mInit = &daPly_c::St_Throw_Init;
    data_ov002_021105d4.mCleanup = 0;
    data_ov002_021105d4.mMain = &daPly_c::St_Throw_Main;
    data_ov002_021105ec.mInit = &daPly_c::St_GrabBowserTail_Init;
    data_ov002_021105ec.mMain = &daPly_c::St_GrabBowserTail_Main;
    data_ov002_021105ec.mCleanup = &daPly_c::St_GrabBowserTail_Cleanup;
    data_ov002_02110604.mInit = &daPly_c::St_SwingPlayer_Init;
    data_ov002_02110604.mMain = &daPly_c::St_SwingPlayer_Main;
    data_ov002_02110604.mCleanup = &daPly_c::St_SwingPlayer_Cleanup;
    data_ov002_0211061c.mCleanup = 0;
    data_ov002_0211061c.mInit = &daPly_c::St_SweepKick_Init;
    data_ov002_0211061c.mMain = &daPly_c::St_SweepKick_Main;
    data_ov002_02110634.mInit = &daPly_c::St_SlideKick_Init;
    data_ov002_02110634.mCleanup = 0;
    data_ov002_02110634.mMain = &daPly_c::St_SlideKick_Main;
    data_ov002_0211064c.mInit = &daPly_c::St_CameraZoom_Init;
    data_ov002_0211064c.mMain = &daPly_c::St_CameraZoom_Main;
    data_ov002_0211064c.mCleanup = &daPly_c::St_CameraZoom_Cleanup;
    data_ov002_02110664.mInit = &daPly_c::St_Null_Init;
    data_ov002_02110664.mMain = &daPly_c::St_Null_Main;
    data_ov002_02110664.mCleanup = &daPly_c::St_Null_Cleanup;
    data_ov002_0211067c.mInit = &daPly_c::St_Swim_Init;
    data_ov002_0211067c.mMain = &daPly_c::St_Swim_Main;
    data_ov002_0211067c.mCleanup = &daPly_c::St_Swim_Cleanup;
    data_ov002_02110694.mCleanup = 0;
    data_ov002_02110694.mInit = &daPly_c::St_WaterJump_Init;
    data_ov002_02110694.mMain = &daPly_c::St_SideFlip_Main;
    data_ov002_021106ac.mInit = &daPly_c::St_MetalWaterGround_Init;
    data_ov002_021106ac.mMain = &daPly_c::St_MetalWaterGround_Main;
    data_ov002_021106ac.mCleanup = &daPly_c::St_Swim_Cleanup;
    data_ov002_021106c4.mInit = &daPly_c::St_MetalWaterWater_Init;
    data_ov002_021106c4.mMain = &daPly_c::St_MetalWaterWater_Main;
    data_ov002_021106c4.mCleanup = &daPly_c::St_Swim_Cleanup;
    data_ov002_021106dc.mInit = &daPly_c::St_Climb_Init;
    data_ov002_021106dc.mMain = &daPly_c::St_Climb_Main;
    data_ov002_021106dc.mCleanup = &daPly_c::St_Climb_Cleanup;
    data_ov002_021106f4.mInit = &daPly_c::St_Headstand_Init;
    data_ov002_021106f4.mMain = &daPly_c::St_Headstand_Main;
    data_ov002_021106f4.mCleanup = &daPly_c::St_Climb_Cleanup;
    data_ov002_0211070c.mCleanup = 0;
    data_ov002_0211070c.mInit = &daPly_c::St_WallJump_Init;
    data_ov002_0211070c.mMain = &daPly_c::St_WallJump_Main;
    data_ov002_02110724.mInit = &daPly_c::St_HeadstandJump_Init;
    data_ov002_02110724.mCleanup = 0;
    data_ov002_02110724.mMain = &daPly_c::St_SideFlip_Main;
    data_ov002_0210ffec.mInit = &daPly_c::St_LedgeHang_Init;
    data_ov002_0210ffec.mMain = &daPly_c::St_LedgeHang_Main;
    data_ov002_0210ffec.mCleanup = &daPly_c::St_LedgeHang_Cleanup;
    data_ov002_02110004.mCleanup = 0;
    data_ov002_02110004.mInit = &daPly_c::St_LedgeGrab_Init;
    data_ov002_02110004.mMain = &daPly_c::St_LedgeGrab_Main;
    data_ov002_0211001c.mInit = &daPly_c::St_CeilingGrate_Init;
    data_ov002_0211001c.mCleanup = 0;
    data_ov002_0211001c.mMain = &daPly_c::St_CeilingGrate_Main;
    data_ov002_02110034.mInit = &daPly_c::St_YoshiPower_Init;
    data_ov002_02110034.mMain = &daPly_c::St_YoshiPower_Main;
    data_ov002_02110034.mCleanup = &daPly_c::St_YoshiPower_Cleanup;
    data_ov002_0211004c.mCleanup = 0;
    data_ov002_0211004c.mInit = &daPly_c::St_Swallow_Init;
    data_ov002_0211004c.mMain = &daPly_c::St_Swallow_Main;
    data_ov002_02110064.mInit = &daPly_c::St_InYoshiMouth_Init;
    data_ov002_02110064.mMain = &daPly_c::St_InYoshiMouth_Main;
    data_ov002_02110064.mCleanup = &daPly_c::St_InYoshiMouth_Cleanup;
    data_ov002_0211007c.mCleanup = 0;
    data_ov002_0211007c.mInit = &daPly_c::St_SmallLaunchUp_Init;
    data_ov002_0211007c.mMain = &daPly_c::St_SmallLaunchUp_Main;
    data_ov002_02110094.mInit = &daPly_c::St_Hurt_Init;
    data_ov002_02110094.mMain = &daPly_c::St_Hurt_Main;
    data_ov002_02110094.mCleanup = &daPly_c::St_Hurt_Cleanup;
    data_ov002_021100ac.mCleanup = 0;
    data_ov002_021100ac.mInit = &daPly_c::St_HurtWater_Init;
    data_ov002_021100ac.mMain = &daPly_c::St_HurtWater_Main;
    data_ov002_021100c4.mInit = &daPly_c::St_Electrocute_Init;
    data_ov002_021100c4.mCleanup = 0;
    data_ov002_021100c4.mMain = &daPly_c::St_Electrocute_Main;
    data_ov002_021100dc.mCleanup = 0;
    data_ov002_021100dc.mInit = &daPly_c::St_BurnFire_Init;
    data_ov002_021100dc.mMain = &daPly_c::St_BurnFire_Main;
    data_ov002_021100f4.mInit = &daPly_c::St_BurnLava_Init;
    data_ov002_021100f4.mCleanup = 0;
    data_ov002_021100f4.mMain = &daPly_c::St_BurnLava_Main;
    data_ov002_0211010c.mCleanup = 0;
    data_ov002_0211010c.mInit = &daPly_c::St_DeadHit_Init;
    data_ov002_0211010c.mMain = &daPly_c::St_DeadHit_Main;
    data_ov002_02110124.mInit = &daPly_c::St_DeadPit_Init;
    data_ov002_02110124.mCleanup = 0;
    data_ov002_02110124.mMain = &daPly_c::St_DeadPit_Main;
    data_ov002_0211016c.mInit = &daPly_c::St_Grabbed_Init;
    data_ov002_0211016c.mMain = &daPly_c::St_Grabbed_Main;
    data_ov002_0211016c.mCleanup = &daPly_c::St_Grabbed_Cleanup;
    data_ov002_021101cc.mInit = &daPly_c::St_Thrown_Init;
    data_ov002_021101cc.mMain = &daPly_c::St_Thrown_Main;
    data_ov002_021101cc.mCleanup = &daPly_c::St_Thrown_Cleanup;
    data_ov002_0211022c.mInit = &daPly_c::St_NoControl_Init;
    data_ov002_0211022c.mMain = &daPly_c::St_NoControl_Main;
    data_ov002_0211022c.mCleanup = &daPly_c::St_NoControl_Cleanup;
    data_ov002_0211025c.mCleanup = 0;
    data_ov002_0211025c.mInit = &daPly_c::St_DebugFly_Init;
    data_ov002_0211025c.mMain = &daPly_c::St_DebugFly_Main;
    data_ov002_021102a4.mInit = &daPly_c::St_Teleport_Init;
    data_ov002_021102a4.mCleanup = 0;
    data_ov002_021102a4.mMain = &daPly_c::St_Teleport_Main;
    data_ov002_021102ec.mInit = &daPly_c::St_Squish_Init;
    data_ov002_021102ec.mMain = &daPly_c::St_Squish_Main;
    data_ov002_021102ec.mCleanup = &daPly_c::St_Squish_Cleanup;
    data_ov002_0211034c.mInit = &daPly_c::St_DizzyStars_Init;
    data_ov002_0211034c.mMain = &daPly_c::St_DizzyStars_Main;
    data_ov002_0211034c.mCleanup = &daPly_c::St_DizzyStars_Cleanup;
    data_ov002_0211037c.mCleanup = 0;
    data_ov002_0211037c.mInit = &daPly_c::St_Bonk_Init;
    data_ov002_0211037c.mMain = &daPly_c::St_Bonk_Main;
    data_ov002_021103c4.mInit = &daPly_c::St_BowserEarthquake_Init;
    data_ov002_021103c4.mCleanup = 0;
    data_ov002_021103c4.mMain = &daPly_c::St_BowserEarthquake_Main;
    data_ov002_0211040c.mCleanup = 0;
    data_ov002_0211040c.mInit = &daPly_c::St_StuckInGround_Init;
    data_ov002_0211040c.mMain = &daPly_c::St_StuckInGround_Main;
    data_ov002_0211046c.mInit = &daPly_c::St_Talk_Init;
    data_ov002_0211046c.mMain = &daPly_c::St_Talk_Main;
    data_ov002_0211046c.mCleanup = &daPly_c::St_Talk_Cleanup;
    data_ov002_021104b4.mInit = &daPly_c::St_LevelEnter_Init;
    data_ov002_021104b4.mMain = &daPly_c::St_LevelEnter_Main;
    data_ov002_021104b4.mCleanup = &daPly_c::St_LevelEnter_Cleanup;
    data_ov002_021104fc.mCleanup = 0;
    data_ov002_021104fc.mInit = &daPly_c::St_Respawn_Init;
    data_ov002_021104fc.mMain = &daPly_c::St_Respawn_Main;
    data_ov002_02110544.mInit = &daPly_c::St_OpeningWakeUp_Init;
    data_ov002_02110544.mCleanup = 0;
    data_ov002_02110544.mMain = &daPly_c::St_OpeningWakeUp_Main;
    data_ov002_0211058c.mInit = &daPly_c::St_EndingFly_Init;
    data_ov002_0211058c.mCleanup = 0;
    data_ov002_0211058c.mMain = &daPly_c::St_EndingFly_Main;
    func_ov002_020e6ad4(data_ov002_02110804);
    data_ov002_0211073c[0] = data_ov002_02109f0c.pmf;
    data_ov002_0211073c[1] = data_ov002_0210a47c.pmf;
    data_ov002_0211073c[2] = data_ov002_0210a10c.pmf;
    data_ov002_0211073c[3] = &daPly_c::func_ov002_020e1c20;
    ((PlayerStateFn *)data_ov002_02110884)[0].voidPmf = data_ov002_02109dfc.pmf;
    ((PlayerStateFn *)data_ov002_02110884)[1].voidPmf = data_ov002_0210a264.pmf;
    ((PlayerStateFn *)data_ov002_02110884)[2].voidPmf = data_ov002_0210a244.pmf;
    ((PlayerStateFn *)data_ov002_02110884)[3].voidPmf = data_ov002_0210a23c.pmf;
    ((PlayerStateFn *)data_ov002_02110884)[4].voidPmf = &daPly_c::func_ov002_020c9d68;
    ((PlayerStateFn *)data_ov002_02110884)[5].intPmf = &daPly_c::func_ov002_020c9d1c;
    ((PlayerStateFn *)data_ov002_02110884)[6].voidPmf = &daPly_c::func_ov002_020c9cc0;
    ((PlayerStateFn *)data_ov002_02110884)[7].voidPmf = &daPly_c::func_ov002_020c9c4c;
    ((PlayerStateFn *)data_ov002_02110884)[8].intPmf = &daPly_c::func_ov002_020c9c00;
    ((PlayerStateFn *)data_ov002_02110884)[9].voidPmf = &daPly_c::func_ov002_020c9b7c;
    ((PlayerStateFn *)data_ov002_02110884)[10].voidPmf = &daPly_c::func_ov002_020c9b5c;
    ((PlayerStateFn *)data_ov002_02110884)[11].voidPmf = &daPly_c::func_ov002_020c9ac0;
    ((PlayerStateFn *)data_ov002_02110884)[12].intPmf = &daPly_c::func_ov002_020c9b10;
    ((PlayerStateFn *)data_ov002_02110884)[13].voidPmf = &daPly_c::func_ov002_020c9a04;
    ((PlayerStateFn *)data_ov002_02110884)[14].voidPmf = &daPly_c::func_ov002_020c9998;
    ((PlayerStateFn *)data_ov002_02110884)[15].voidPmf = &daPly_c::func_ov002_020c990c;
    ((PlayerStateFn *)data_ov002_02110884)[16].voidPmf = &daPly_c::func_ov002_020c98a4;
    ((PlayerStateFn *)data_ov002_02110884)[17].voidPmf = &daPly_c::func_ov002_020c9840;
    ((PlayerStateFn *)data_ov002_02110884)[18].voidPmf = &daPly_c::func_ov002_020c97f8;
    ((PlayerStateFn *)data_ov002_02110884)[19].voidPmf = &daPly_c::func_ov002_020c97e0;
    data_ov002_0211075c[0] = &daPly_c::func_ov002_020c75f0;
    data_ov002_0211075c[1] = &daPly_c::func_ov002_020c7350;
    data_ov002_0211075c[2] = &daPly_c::func_ov002_020c71e0;
    data_ov002_0211075c[3] = &daPly_c::func_ov002_020c7194;
    data_ov002_0211075c[4] = data_ov002_0210a334.pmf;
    data_ov002_0211075c[5] = &daPly_c::func_ov002_020c70ac;
    data_ov002_0211075c[6] = &daPly_c::func_ov002_020c6fe4;
}
}

#pragma section initsec begin
extern "C" {
void __sinit_ov002_021019d0(void) {
    func_0203b9b4(&data_ov002_0210e160, 0x4d2);
    __register_global_object(&data_ov002_0210e160, (void *)func_0201973c, data_ov002_0210f9b0);
    func_02017acc(&data_ov002_0210e6f8, ASSET_B_MARIO_ALL_BMD);
    __register_global_object(&data_ov002_0210e6f8, (void *)func_02017ab4, data_ov002_0210f9c8);
    func_02017acc(&data_ov002_0210e648, ASSET_B_MARIO_BODY_BMD);
    __register_global_object(&data_ov002_0210e648, (void *)func_02017ab4, data_ov002_0210f9d4);
    func_02017acc(&data_ov002_0210eae8, ASSET_B_MARIO_HEAD_CAP_BMD);
    __register_global_object(&data_ov002_0210eae8, (void *)func_02017ab4, data_ov002_0210f9e0);
    func_02017acc(&data_ov002_0210eb20, 0x807e);
    __register_global_object(&data_ov002_0210eb20, (void *)func_02017ab4, data_ov002_0210f9f8);
    func_02017acc(&data_ov002_0210eb00, 0x807f);
    __register_global_object(&data_ov002_0210eb00, (void *)func_02017ab4, data_ov002_0210fa04);
    func_02017acc(&data_ov002_0210ebb8, 0x8080);
    __register_global_object(&data_ov002_0210ebb8, (void *)func_02017ab4, data_ov002_0210fa1c);
    func_02017acc(&data_ov002_0210e690, 0x8081);
    __register_global_object(&data_ov002_0210e690, (void *)func_02017ab4, data_ov002_0210fa28);
    func_02017acc(&data_ov002_0210ec38, 0x8082);
    __register_global_object(&data_ov002_0210ec38, (void *)func_02017ab4, data_ov002_0210fa40);
    func_02017acc(&data_ov002_0210ec50, 0x8083);
    __register_global_object(&data_ov002_0210ec50, (void *)func_02017ab4, data_ov002_0210fa4c);
    func_02017acc(&data_ov002_0210e758, 0x80b8);
    __register_global_object(&data_ov002_0210e758, (void *)func_02017ab4, data_ov002_0210fa64);
    func_02017acc(&data_ov002_0210e790, 0x80b9);
    __register_global_object(&data_ov002_0210e790, (void *)func_02017ab4, data_ov002_0210fa70);
    func_02017acc(&data_ov002_0210e780, ASSET_WARIO_METAL_MODEL_BMD);
    __register_global_object(&data_ov002_0210e780, (void *)func_02017ab4, data_ov002_0210fa94);
    func_02017acc(&data_ov002_0210ec60, 0x80ba);
    __register_global_object(&data_ov002_0210ec60, (void *)func_02017ab4, data_ov002_0210faa0);
    func_02017acc(&data_ov002_0210e7b0, 0x80bd);
    __register_global_object(&data_ov002_0210e7b0, (void *)func_02017ab4, data_ov002_0210fab8);
    func_02017acc(&data_ov002_0210e3a0, 0x80c0);
    __register_global_object(&data_ov002_0210e3a0, (void *)func_02017ab4, data_ov002_0210fac4);
    func_02017acc(&data_ov002_0210e7d8, 0x80c1);
    __register_global_object(&data_ov002_0210e7d8, (void *)func_02017ab4, data_ov002_0210fadc);
    func_02017acc(&data_ov002_0210e7a8, 0x80c2);
    __register_global_object(&data_ov002_0210e7a8, (void *)func_02017ab4, data_ov002_0210fae8);
    func_02017acc(&data_ov002_0210e7f0, 0x80c3);
    __register_global_object(&data_ov002_0210e7f0, (void *)func_02017ab4, data_ov002_0210fb00);
    func_02017acc(&data_ov002_0210e818, 0x80c4);
    __register_global_object(&data_ov002_0210e818, (void *)func_02017ab4, data_ov002_0210fb0c);
    data_ov002_0210e490.Construct(0x801d);
    __register_global_object(&data_ov002_0210e490, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb24);
    data_ov002_0210e838.Construct(0x801e);
    __register_global_object(&data_ov002_0210e838, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb30);
    data_ov002_0210e848.Construct(0x801f);
    __register_global_object(&data_ov002_0210e848, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb48);
    data_ov002_0210e2c0.Construct(ASSET_L_ED_AMAZED_BCA);
    __register_global_object(&data_ov002_0210e2c0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb54);
    data_ov002_0210e860.Construct(ASSET_L_ED_CAP_OFF_BCA);
    __register_global_object(&data_ov002_0210e860, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb6c);
    data_ov002_0210e4f8.Construct(ASSET_L_ED_CAP_OFF_WAIT_BCA);
    __register_global_object(&data_ov002_0210e4f8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb78);
    data_ov002_0210e888.Construct(ASSET_L_ED_LOOK_M_WAIT_BCA);
    __register_global_object(&data_ov002_0210e888, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb90);
    data_ov002_0210e868.Construct(ASSET_L_ED_LOOK_MARIO_BCA);
    __register_global_object(&data_ov002_0210e868, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb9c);
    data_ov002_0210e7c0.Construct(ASSET_L_ED_LOOKUP_WAIT_BCA);
    __register_global_object(&data_ov002_0210e7c0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fbb4);
    data_ov002_0210e8b0.Construct(ASSET_L_ED_WAIT_BCA);
    __register_global_object(&data_ov002_0210e8b0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fbc0);
    data_ov002_0210e8b8.Construct(ASSET_L_ENTRY_WAIT_BCA);
    __register_global_object(&data_ov002_0210e8b8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fbe4);
    data_ov002_0210e518.Construct(ASSET_L_ENTRY_WAIT_END_BCA);
    __register_global_object(&data_ov002_0210e518, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fbf0);
    data_ov002_0210e8d8.Construct(ASSET_L_FIRST_CHANGE_BCA);
    __register_global_object(&data_ov002_0210e8d8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fc08);
    data_ov002_0210e8f0.Construct(ASSET_L_LIE_END_BCA);
    __register_global_object(&data_ov002_0210e8f0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fc14);
    data_ov002_0210e900.Construct(ASSET_L_LIE_WAIT_BCA);
    __register_global_object(&data_ov002_0210e900, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fc2c);
    data_ov002_0210e548.Construct(ASSET_L_OP_LOOK_WARIO_BCA);
    __register_global_object(&data_ov002_0210e548, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fc38);
    data_ov002_0210e530.Construct(ASSET_L_OP_NOD_BCA);
    __register_global_object(&data_ov002_0210e530, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fc50);
    data_ov002_0210e528.Construct(ASSET_L_OP_TURN_BCA);
    __register_global_object(&data_ov002_0210e528, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f380);
    data_ov002_0210e4b0.Construct(0x8020);
    __register_global_object(&data_ov002_0210e4b0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fc68);
    data_ov002_0210e550.Construct(ASSET_L_SLEEP_END_BCA);
    __register_global_object(&data_ov002_0210e550, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fc74);
    data_ov002_0210e748.Construct(ASSET_L_SLEEP_START_BCA);
    __register_global_object(&data_ov002_0210e748, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fc8c);
    data_ov002_0210e5c0.Construct(ASSET_L_SLEEP_TO_LIE_BCA);
    __register_global_object(&data_ov002_0210e5c0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fc98);
    data_ov002_0210e998.Construct(ASSET_L_SLEEP_WAIT_BCA);
    __register_global_object(&data_ov002_0210e998, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fcb0);
    data_ov002_0210eaa0.Construct(0x8029);
    __register_global_object(&data_ov002_0210eaa0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fcbc);
    data_ov002_0210ea50.Construct(0x802a);
    __register_global_object(&data_ov002_0210ea50, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fcd4);
    data_ov002_0210e8c8.Construct(ASSET_M_ENTRY_WAIT_BCA);
    __register_global_object(&data_ov002_0210e8c8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fce0);
    data_ov002_0210e778.Construct(ASSET_M_ENTRY_WAIT_END_BCA);
    __register_global_object(&data_ov002_0210e778, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fcf8);
    data_ov002_0210ebe0.Construct(0x802b);
    __register_global_object(&data_ov002_0210ebe0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fd04);
    data_ov002_0210e4f0.Construct(0x802c);
    __register_global_object(&data_ov002_0210e4f0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fd1c);
    data_ov002_0210e4c8.Construct(0x802d);
    __register_global_object(&data_ov002_0210e4c8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fd28);
    data_ov002_0210eb30.Construct(0x802e);
    __register_global_object(&data_ov002_0210eb30, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fd40);
    data_ov002_0210e510.Construct(0x802f);
    __register_global_object(&data_ov002_0210e510, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fd4c);
    data_ov002_0210e970.Construct(ASSET_W_ED_AMAZED_WAIR_BCA);
    __register_global_object(&data_ov002_0210e970, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fd64);
    data_ov002_0210e558.Construct(ASSET_W_ED_CAP_OFF_BCA);
    __register_global_object(&data_ov002_0210e558, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fd70);
    data_ov002_0210e9c0.Construct(ASSET_W_ED_CAP_OFF_WAIT_BCA);
    __register_global_object(&data_ov002_0210e9c0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fd88);
    data_ov002_0210ea20.Construct(ASSET_W_ED_LOOKUP_WAIT_BCA);
    __register_global_object(&data_ov002_0210ea20, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fd94);
    data_ov002_0210e5b0.Construct(ASSET_W_ED_WAIT_BCA);
    __register_global_object(&data_ov002_0210e5b0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fdac);
    data_ov002_0210e5d0.Construct(ASSET_W_ED_WAVE_BCA);
    __register_global_object(&data_ov002_0210e5d0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fdb8);
    data_ov002_0210e5f0.Construct(ASSET_W_ENTRY_WAIT_BCA);
    __register_global_object(&data_ov002_0210e5f0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fdd0);
    data_ov002_0210e9a0.Construct(ASSET_W_ENTRY_WAIT_END_BCA);
    __register_global_object(&data_ov002_0210e9a0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fddc);
    data_ov002_0210e520.Construct(ASSET_W_FIRST_CHANGE_BCA);
    __register_global_object(&data_ov002_0210e520, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fdf4);
    data_ov002_0210eab8.Construct(0x8030);
    __register_global_object(&data_ov002_0210eab8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fe00);
    data_ov002_0210ead0.Construct(0x8031);
    __register_global_object(&data_ov002_0210ead0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fe18);
    data_ov002_0210eac0.Construct(0x8032);
    __register_global_object(&data_ov002_0210eac0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fe24);
    data_ov002_0210e9f0.Construct(0x8033);
    __register_global_object(&data_ov002_0210e9f0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fe3c);
    data_ov002_0210eae0.Construct(ASSET_W_KEY_GET_BCA);
    __register_global_object(&data_ov002_0210eae0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fe48);
    data_ov002_0210ea08.Construct(ASSET_W_LIE_END_BCA);
    __register_global_object(&data_ov002_0210ea08, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fe60);
    data_ov002_0210e990.Construct(ASSET_W_LIE_WAIT_BCA);
    __register_global_object(&data_ov002_0210e990, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fe6c);
    data_ov002_0210eaf8.Construct(ASSET_W_OP_LOOK_LUIGI_BCA);
    __register_global_object(&data_ov002_0210eaf8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fe84);
    data_ov002_0210e9b8.Construct(ASSET_W_OP_NOD_BCA);
    __register_global_object(&data_ov002_0210e9b8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fe90);
    data_ov002_0210ecb0.Construct(ASSET_W_OP_POINT_BCA);
    __register_global_object(&data_ov002_0210ecb0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fea8);
    data_ov002_0210eb08.Construct(ASSET_W_OP_TURN_BCA);
    __register_global_object(&data_ov002_0210eb08, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210feb4);
    data_ov002_0210e668.Construct(0x8034);
    __register_global_object(&data_ov002_0210e668, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fecc);
    data_ov002_0210e650.Construct(ASSET_W_SLEEP_END_BCA);
    __register_global_object(&data_ov002_0210e650, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fed8);
    data_ov002_0210eb18.Construct(ASSET_W_SLEEP_START_BCA);
    __register_global_object(&data_ov002_0210eb18, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fef0);
    data_ov002_0210e658.Construct(ASSET_W_SLEEP_TO_LIE_BCA);
    __register_global_object(&data_ov002_0210e658, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fefc);
    data_ov002_0210e678.Construct(ASSET_W_SLEEP_WAIT_BCA);
    __register_global_object(&data_ov002_0210e678, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ff14);
    data_ov002_0210eb28.Construct(0x803b);
    __register_global_object(&data_ov002_0210eb28, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ff20);
    data_ov002_0210e638.Construct(0x803c);
    __register_global_object(&data_ov002_0210e638, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ff38);
    data_ov002_0210e688.Construct(0x803d);
    __register_global_object(&data_ov002_0210e688, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ff44);
    data_ov002_0210eb40.Construct(0x803e);
    __register_global_object(&data_ov002_0210eb40, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ff5c);
    data_ov002_0210e630.Construct(0x803f);
    __register_global_object(&data_ov002_0210e630, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ff68);
    data_ov002_0210e930.Construct(0x8040);
    __register_global_object(&data_ov002_0210e930, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ff80);
    data_ov002_0210eb50.Construct(ASSET_Y_ED_LOOK_CAM_BCA);
    __register_global_object(&data_ov002_0210eb50, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ff8c);
    data_ov002_0210e698.Construct(ASSET_Y_ED_LOOKUP_WAIT_BCA);
    __register_global_object(&data_ov002_0210e698, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ffa4);
    data_ov002_0210eb60.Construct(ASSET_Y_ED_SHY_TO_WAIT_BCA);
    __register_global_object(&data_ov002_0210eb60, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ffb0);
    data_ov002_0210eb68.Construct(ASSET_Y_ED_SHY_WAIT_BCA);
    __register_global_object(&data_ov002_0210eb68, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ffc8);
    data_ov002_0210e6a8.Construct(ASSET_Y_ED_TURN_BCA);
    __register_global_object(&data_ov002_0210e6a8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ffd4);
    data_ov002_0210e618.Construct(ASSET_Y_ED_WAIT_BCA);
    __register_global_object(&data_ov002_0210e618, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ed68);
    data_ov002_0210e6b0.Construct(ASSET_Y_EGG_RUN_BCA);
    __register_global_object(&data_ov002_0210e6b0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ed74);
    data_ov002_0210eb78.Construct(ASSET_Y_EGG_WAIT_BCA);
    __register_global_object(&data_ov002_0210eb78, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ed8c);
    data_ov002_0210e3d0.Construct(0x8041);
    __register_global_object(&data_ov002_0210e3d0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ed98);
    data_ov002_0210eb80.Construct(0x8042);
    __register_global_object(&data_ov002_0210eb80, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210edb0);
    data_ov002_0210eb88.Construct(0x8043);
    __register_global_object(&data_ov002_0210eb88, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210edbc);
    data_ov002_0210e6c0.Construct(0x8044);
    __register_global_object(&data_ov002_0210e6c0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210edd4);
    data_ov002_0210e3d8.Construct(0x8045);
    __register_global_object(&data_ov002_0210e3d8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ede0);
    data_ov002_0210eb98.Construct(0x8046);
    __register_global_object(&data_ov002_0210eb98, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210edf8);
    data_ov002_0210e6c8.Construct(0x8047);
    __register_global_object(&data_ov002_0210e6c8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ee04);
    data_ov002_0210eba0.Construct(ASSET_Y_LIE_END_BCA);
    __register_global_object(&data_ov002_0210eba0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ee1c);
    data_ov002_0210e6d0.Construct(ASSET_Y_LIE_WAIT_BCA);
    __register_global_object(&data_ov002_0210e6d0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ee28);
    data_ov002_0210ebb0.Construct(ASSET_Y_OP_LIE_TO_SIT_BCA);
    __register_global_object(&data_ov002_0210ebb0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ee40);
    data_ov002_0210e3f0.Construct(ASSET_Y_OP_SIT_TO_STAND_BCA);
    __register_global_object(&data_ov002_0210e3f0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ee4c);
    data_ov002_0210e6d8.Construct(ASSET_Y_OP_SIT_WAIT_BCA);
    __register_global_object(&data_ov002_0210e6d8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ee64);
    data_ov002_0210ebc0.Construct(ASSET_Y_OP_STAND_WAIT_BCA);
    __register_global_object(&data_ov002_0210ebc0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ee70);
    data_ov002_0210e3f8.Construct(0x8048);
    __register_global_object(&data_ov002_0210e3f8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ee88);
    data_ov002_0210e6e8.Construct(ASSET_Y_SLEEP_END_BCA);
    __register_global_object(&data_ov002_0210e6e8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ee94);
    data_ov002_0210ebd0.Construct(ASSET_Y_SLEEP_START_BCA);
    __register_global_object(&data_ov002_0210ebd0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210eeac);
    data_ov002_0210e6f0.Construct(ASSET_Y_SLEEP_TO_LIE_BCA);
    __register_global_object(&data_ov002_0210e6f0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fbcc);
    data_ov002_0210e3c0.Construct(ASSET_Y_SLEEP_WAIT_BCA);
    __register_global_object(&data_ov002_0210e3c0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210eec4);
    data_ov002_0210e628.Construct(0x8419);
    __register_global_object(&data_ov002_0210e628, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210eed0);
    data_ov002_0210e700.Construct(0x841a);
    __register_global_object(&data_ov002_0210e700, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210eee8);
    data_ov002_0210ebe8.Construct(0x841b);
    __register_global_object(&data_ov002_0210ebe8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210eef4);
    data_ov002_0210ebf0.Construct(0x841c);
    __register_global_object(&data_ov002_0210ebf0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ef0c);
    data_ov002_0210e710.Construct(0x841d);
    __register_global_object(&data_ov002_0210e710, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ef18);
    data_ov002_0210e3a8.Construct(0x841e);
    __register_global_object(&data_ov002_0210e3a8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ef30);
    data_ov002_0210ebf8.Construct(0x841f);
    __register_global_object(&data_ov002_0210ebf8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ef3c);
    data_ov002_0210e718.Construct(0x8420);
    __register_global_object(&data_ov002_0210e718, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ef54);
    data_ov002_0210ec08.Construct(0x8421);
    __register_global_object(&data_ov002_0210ec08, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ef60);
    data_ov002_0210e720.Construct(0x8422);
    __register_global_object(&data_ov002_0210e720, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ef78);
    data_ov002_0210ec20.Construct(0x8423);
    __register_global_object(&data_ov002_0210ec20, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ef84);
    data_ov002_0210e410.Construct(0x8424);
    __register_global_object(&data_ov002_0210e410, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210ef9c);
    data_ov002_0210e260.Construct(0x8425);
    __register_global_object(&data_ov002_0210e260, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fa88);
    data_ov002_0210e730.Construct(0x8426);
    __register_global_object(&data_ov002_0210e730, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210efb4);
    data_ov002_0210ec30.Construct(0x8050);
    __register_global_object(&data_ov002_0210ec30, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210efc0);
    data_ov002_0210e418.Construct(0x8051);
    __register_global_object(&data_ov002_0210e418, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210efd8);
    data_ov002_0210e740.Construct(0x8052);
    __register_global_object(&data_ov002_0210e740, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210efe4);
    data_ov002_0210ec48.Construct(0x8053);
    __register_global_object(&data_ov002_0210ec48, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210effc);
    data_ov002_0210e420.Construct(0x8054);
    __register_global_object(&data_ov002_0210e420, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f008);
    data_ov002_0210e750.Construct(ASSET_B_MARIO_START_BCA);
    __register_global_object(&data_ov002_0210e750, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f020);
    data_ov002_0210e760.Construct(0x8055);
    __register_global_object(&data_ov002_0210e760, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f02c);
    data_ov002_0210e428.Construct(0x8056);
    __register_global_object(&data_ov002_0210e428, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f044);
    data_ov002_0210e258.Construct(0x8057);
    __register_global_object(&data_ov002_0210e258, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f050);
    data_ov002_0210e268.Construct(0x8058);
    __register_global_object(&data_ov002_0210e268, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f068);
    data_ov002_0210e440.Construct(0x8059);
    __register_global_object(&data_ov002_0210e440, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f074);
    data_ov002_0210e9e0.Construct(0x805a);
    __register_global_object(&data_ov002_0210e9e0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f08c);
    data_ov002_0210e240.Construct(0x805b);
    __register_global_object(&data_ov002_0210e240, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f098);
    data_ov002_0210e448.Construct(0x805c);
    __register_global_object(&data_ov002_0210e448, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f0b0);
    data_ov002_0210e798.Construct(0x805d);
    __register_global_object(&data_ov002_0210e798, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f0bc);
    data_ov002_0210e7a0.Construct(0x805e);
    __register_global_object(&data_ov002_0210e7a0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f0d4);
    data_ov002_0210e278.Construct(0x805f);
    __register_global_object(&data_ov002_0210e278, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f0e0);
    data_ov002_0210e238.Construct(0x8060);
    __register_global_object(&data_ov002_0210e238, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f0f8);
    data_ov002_0210e248.Construct(0x8061);
    __register_global_object(&data_ov002_0210e248, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f104);
    data_ov002_0210e468.Construct(ASSET_COLD_WAIT_BCA);
    __register_global_object(&data_ov002_0210e468, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f11c);
    data_ov002_0210e470.Construct(ASSET_COLD_WAIT_END_BCA);
    __register_global_object(&data_ov002_0210e470, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f128);
    data_ov002_0210e280.Construct(ASSET_COLD_WAIT_START_BCA);
    __register_global_object(&data_ov002_0210e280, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f140);
    data_ov002_0210e7d0.Construct(0x8062);
    __register_global_object(&data_ov002_0210e7d0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f14c);
    data_ov002_0210e290.Construct(0x8063);
    __register_global_object(&data_ov002_0210e290, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f164);
    data_ov002_0210e480.Construct(0x8064);
    __register_global_object(&data_ov002_0210e480, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f170);
    data_ov002_0210e488.Construct(ASSET_DAMAGE_START_BCA);
    __register_global_object(&data_ov002_0210e488, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f188);
    data_ov002_0210e228.Construct(ASSET_DAMAGE_WAIT_BCA);
    __register_global_object(&data_ov002_0210e228, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f194);
    data_ov002_0210e498.Construct(ASSET_DOOR_PULL_BCA);
    __register_global_object(&data_ov002_0210e498, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f1b8);
    data_ov002_0210e4a0.Construct(ASSET_DOOR_PUSH_BCA);
    __register_global_object(&data_ov002_0210e4a0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f1c4);
    data_ov002_0210e808.Construct(ASSET_DROWN_DIE_BCA);
    __register_global_object(&data_ov002_0210e808, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f1dc);
    data_ov002_0210e298.Construct(ASSET_E_DOWN_BCA);
    __register_global_object(&data_ov002_0210e298, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f1e8);
    data_ov002_0210e810.Construct(0x8065);
    __register_global_object(&data_ov002_0210e810, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f200);
    data_ov002_0210e2a0.Construct(ASSET_ED_CAP_OFF_BCA);
    __register_global_object(&data_ov002_0210e2a0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f20c);
    data_ov002_0210e820.Construct(ASSET_ED_CAP_OFF_WAIT_BCA);
    __register_global_object(&data_ov002_0210e820, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f230);
    data_ov002_0210e2a8.Construct(ASSET_ED_FLY_BCA);
    __register_global_object(&data_ov002_0210e2a8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f23c);
    data_ov002_0210e1d8.Construct(ASSET_ED_FLY_START_BCA);
    __register_global_object(&data_ov002_0210e1d8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f254);
    data_ov002_0210e2b0.Construct(ASSET_ED_KISS_TO_POSE_BCA);
    __register_global_object(&data_ov002_0210e2b0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f260);
    data_ov002_0210e840.Construct(ASSET_ED_KISS_WAIT_BCA);
    __register_global_object(&data_ov002_0210e840, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f278);
    data_ov002_0210e2b8.Construct(ASSET_ED_LOOK_LEFT_BCA);
    __register_global_object(&data_ov002_0210e2b8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f284);
    data_ov002_0210e850.Construct(ASSET_ED_LOOK_LEFT_END_BCA);
    __register_global_object(&data_ov002_0210e850, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f29c);
    data_ov002_0210e4e0.Construct(ASSET_ED_LOOK_LEFT_WAIT_BCA);
    __register_global_object(&data_ov002_0210e4e0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f2a8);
    data_ov002_0210e858.Construct(ASSET_ED_NOD_BCA);
    __register_global_object(&data_ov002_0210e858, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f2c0);
    data_ov002_0210e2c8.Construct(ASSET_ED_PUTUP_WAIT_BCA);
    __register_global_object(&data_ov002_0210e2c8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f2cc);
    data_ov002_0210e948.Construct(ASSET_ED_WAIT_BCA);
    __register_global_object(&data_ov002_0210e948, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f2e4);
    data_ov002_0210e2d0.Construct(ASSET_ED_WAVE_BCA);
    __register_global_object(&data_ov002_0210e2d0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f2f0);
    data_ov002_0210e870.Construct(0x8066);
    __register_global_object(&data_ov002_0210e870, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f314);
    data_ov002_0210e2d8.Construct(0x8067);
    __register_global_object(&data_ov002_0210e2d8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f320);
    data_ov002_0210e880.Construct(ASSET_FALL_DIE_BCA);
    __register_global_object(&data_ov002_0210e880, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f32c);
    data_ov002_0210e508.Construct(ASSET_FALL_SAND_BCA);
    __register_global_object(&data_ov002_0210e508, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f338);
    data_ov002_0210e2e0.Construct(0x8068);
    __register_global_object(&data_ov002_0210e2e0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f5e4);
    data_ov002_0210e890.Construct(ASSET_FIRST_CHANGE_BCA);
    __register_global_object(&data_ov002_0210e890, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f35c);
    data_ov002_0210e2e8.Construct(0x8069);
    __register_global_object(&data_ov002_0210e2e8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f368);
    data_ov002_0210e1f0.Construct(0x806a);
    __register_global_object(&data_ov002_0210e1f0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f374);
    data_ov002_0210e1b8.Construct(ASSET_GAS_DIE_BCA);
    __register_global_object(&data_ov002_0210e1b8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f38c);
    data_ov002_0210e8a0.Construct(ASSET_GAS_WAIT_BCA);
    __register_global_object(&data_ov002_0210e8a0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f398);
    data_ov002_0210e2f8.Construct(ASSET_H_CARRY_START_BCA);
    __register_global_object(&data_ov002_0210e2f8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f3a4);
    data_ov002_0210e8a8.Construct(ASSET_H_CARRY_THROW_BCA);
    __register_global_object(&data_ov002_0210e8a8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f3bc);
    data_ov002_0210e300.Construct(ASSET_H_CARRY_WAIT_BCA);
    __register_global_object(&data_ov002_0210e300, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f3c8);
    data_ov002_0210e8c0.Construct(ASSET_H_CARRY_WALK_BCA);
    __register_global_object(&data_ov002_0210e8c0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f3d4);
    data_ov002_0210e308.Construct(0x806b);
    __register_global_object(&data_ov002_0210e308, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f3e0);
    data_ov002_0210e1a8.Construct(0x806c);
    __register_global_object(&data_ov002_0210e1a8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f3ec);
    data_ov002_0210e310.Construct(0x806d);
    __register_global_object(&data_ov002_0210e310, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f3f8);
    data_ov002_0210e8e0.Construct(0x806e);
    __register_global_object(&data_ov002_0210e8e0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f404);
    data_ov002_0210e318.Construct(0x806f);
    __register_global_object(&data_ov002_0210e318, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f410);
    data_ov002_0210e8f8.Construct(0x8070);
    __register_global_object(&data_ov002_0210e8f8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f41c);
    data_ov002_0210e320.Construct(0x8071);
    __register_global_object(&data_ov002_0210e320, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f428);
    data_ov002_0210e908.Construct(0x8072);
    __register_global_object(&data_ov002_0210e908, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f434);
    data_ov002_0210e328.Construct(0x8073);
    __register_global_object(&data_ov002_0210e328, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f440);
    data_ov002_0210e1a0.Construct(0x8074);
    __register_global_object(&data_ov002_0210e1a0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f44c);
    data_ov002_0210e330.Construct(0x8075);
    __register_global_object(&data_ov002_0210e330, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f458);
    data_ov002_0210e918.Construct(0x8076);
    __register_global_object(&data_ov002_0210e918, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f464);
    data_ov002_0210e560.Construct(0x8077);
    __register_global_object(&data_ov002_0210e560, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f470);
    data_ov002_0210e338.Construct(0x8078);
    __register_global_object(&data_ov002_0210e338, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f47c);
    data_ov002_0210e928.Construct(0x8079);
    __register_global_object(&data_ov002_0210e928, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f488);
    data_ov002_0210e568.Construct(ASSET_KEY_GET_BCA);
    __register_global_object(&data_ov002_0210e568, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f494);
    data_ov002_0210e938.Construct(ASSET_KEY_OPEN_BCA);
    __register_global_object(&data_ov002_0210e938, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f4a0);
    data_ov002_0210e940.Construct(0x807a);
    __register_global_object(&data_ov002_0210e940, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f4ac);
    data_ov002_0210e340.Construct(0x807b);
    __register_global_object(&data_ov002_0210e340, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f4b8);
    data_ov002_0210e1f8.Construct(ASSET_LIE_END_BCA);
    __register_global_object(&data_ov002_0210e1f8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f4c4);
    data_ov002_0210e950.Construct(ASSET_LIE_WAIT_BCA);
    __register_global_object(&data_ov002_0210e950, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f4d0);
    data_ov002_0210e570.Construct(0x807c);
    __register_global_object(&data_ov002_0210e570, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f4dc);
    data_ov002_0210e960.Construct(0x807d);
    __register_global_object(&data_ov002_0210e960, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f4e8);
    data_ov002_0210e968.Construct(0x8084);
    __register_global_object(&data_ov002_0210e968, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f4f4);
    data_ov002_0210e578.Construct(0x8085);
    __register_global_object(&data_ov002_0210e578, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f500);
    data_ov002_0210e208.Construct(0x8086);
    __register_global_object(&data_ov002_0210e208, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f50c);
    data_ov002_0210e978.Construct(0x8087);
    __register_global_object(&data_ov002_0210e978, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f518);
    data_ov002_0210e580.Construct(0x8088);
    __register_global_object(&data_ov002_0210e580, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f524);
    data_ov002_0210e980.Construct(ASSET_N_CARRY_LOST_BCA);
    __register_global_object(&data_ov002_0210e980, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f530);
    data_ov002_0210e588.Construct(ASSET_N_CARRY_POSE_BCA);
    __register_global_object(&data_ov002_0210e588, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f53c);
    data_ov002_0210e988.Construct(ASSET_N_CARRY_PUT_BCA);
    __register_global_object(&data_ov002_0210e988, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f548);
    data_ov002_0210e348.Construct(ASSET_N_CARRY_START_BCA);
    __register_global_object(&data_ov002_0210e348, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f554);
    data_ov002_0210e178.Construct(ASSET_N_CARRY_WAIT_BCA);
    __register_global_object(&data_ov002_0210e178, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f560);
    data_ov002_0210e200.Construct(ASSET_N_CARRY_WALK_BCA);
    __register_global_object(&data_ov002_0210e200, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f56c);
    data_ov002_0210e590.Construct(ASSET_OP_GO_BCA);
    __register_global_object(&data_ov002_0210e590, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f578);
    data_ov002_0210e9a8.Construct(ASSET_OP_WAIT_BCA);
    __register_global_object(&data_ov002_0210e9a8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f584);
    data_ov002_0210e9b0.Construct(ASSET_P_KOOPA_H_WAIT_BCA);
    __register_global_object(&data_ov002_0210e9b0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f590);
    data_ov002_0210e598.Construct(ASSET_P_KOOPA_HOLD_BCA);
    __register_global_object(&data_ov002_0210e598, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f59c);
    data_ov002_0210e188.Construct(ASSET_P_KOOPA_THROW_BCA);
    __register_global_object(&data_ov002_0210e188, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f5a8);
    data_ov002_0210e5a0.Construct(ASSET_PADDLE_1_BCA);
    __register_global_object(&data_ov002_0210e5a0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f5b4);
    data_ov002_0210e9c8.Construct(ASSET_PADDLE_2_BCA);
    __register_global_object(&data_ov002_0210e9c8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f5c0);
    data_ov002_0210e350.Construct(ASSET_PADDLE_CARRY_BCA);
    __register_global_object(&data_ov002_0210e350, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f5cc);
    data_ov002_0210e5a8.Construct(ASSET_PADDLE_END_BCA);
    __register_global_object(&data_ov002_0210e5a8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f5d8);
    data_ov002_0210e9d8.Construct(0x8089);
    __register_global_object(&data_ov002_0210e9d8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f5f0);
    data_ov002_0210e358.Construct(0x808a);
    __register_global_object(&data_ov002_0210e358, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f5fc);
    data_ov002_0210e170.Construct(ASSET_PUTON_T_CAP_BCA);
    __register_global_object(&data_ov002_0210e170, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f608);
    data_ov002_0210e9f8.Construct(ASSET_RETURN_DOWN_BCA);
    __register_global_object(&data_ov002_0210e9f8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f614);
    data_ov002_0210e360.Construct(ASSET_RETURN_NOCAP_BCA);
    __register_global_object(&data_ov002_0210e360, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f620);
    data_ov002_0210e5b8.Construct(ASSET_RETURN_STAR_BCA);
    __register_global_object(&data_ov002_0210e5b8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f62c);
    data_ov002_0210ea00.Construct(0x808b);
    __register_global_object(&data_ov002_0210ea00, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f638);
    data_ov002_0210e368.Construct(0x808c);
    __register_global_object(&data_ov002_0210e368, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f644);
    data_ov002_0210e370.Construct(0x808d);
    __register_global_object(&data_ov002_0210e370, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f650);
    data_ov002_0210ea18.Construct(0x808e);
    __register_global_object(&data_ov002_0210ea18, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f65c);
    data_ov002_0210e210.Construct(0x808f);
    __register_global_object(&data_ov002_0210e210, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f668);
    data_ov002_0210e5c8.Construct(0x8090);
    __register_global_object(&data_ov002_0210e5c8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f674);
    data_ov002_0210ea28.Construct(0x8091);
    __register_global_object(&data_ov002_0210ea28, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f680);
    data_ov002_0210e378.Construct(ASSET_S_DIE_BCA);
    __register_global_object(&data_ov002_0210e378, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f68c);
    data_ov002_0210e190.Construct(ASSET_SCALE_UP_BCA);
    __register_global_object(&data_ov002_0210e190, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f698);
    data_ov002_0210ea38.Construct(0x8092);
    __register_global_object(&data_ov002_0210ea38, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f6a4);
    data_ov002_0210e380.Construct(ASSET_SDROP_HEAD_BCA);
    __register_global_object(&data_ov002_0210e380, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f6b0);
    data_ov002_0210ea40.Construct(ASSET_SDROP_HEAD_END_BCA);
    __register_global_object(&data_ov002_0210ea40, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f6bc);
    data_ov002_0210ea48.Construct(ASSET_SDROP_HEAD_WAIT_BCA);
    __register_global_object(&data_ov002_0210ea48, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f6c8);
    data_ov002_0210e5d8.Construct(ASSET_SDROP_HIP_BCA);
    __register_global_object(&data_ov002_0210e5d8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f6d4);
    data_ov002_0210e218.Construct(ASSET_SDROP_HIP_END_BCA);
    __register_global_object(&data_ov002_0210e218, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f6e0);
    data_ov002_0210ea58.Construct(ASSET_SDROP_HIP_WAIT_BCA);
    __register_global_object(&data_ov002_0210ea58, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f6ec);
    data_ov002_0210e5e0.Construct(ASSET_SDROP_LEG_BCA);
    __register_global_object(&data_ov002_0210e5e0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f6f8);
    data_ov002_0210ea60.Construct(ASSET_SDROP_LEG_END_BCA);
    __register_global_object(&data_ov002_0210ea60, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f704);
    data_ov002_0210e5e8.Construct(ASSET_SDROP_LEG_WAIT_BCA);
    __register_global_object(&data_ov002_0210e5e8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f710);
    data_ov002_0210ea68.Construct(ASSET_SDROP_WAIT_BCA);
    __register_global_object(&data_ov002_0210ea68, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f71c);
    data_ov002_0210e388.Construct(ASSET_SDROP_WALK_BCA);
    __register_global_object(&data_ov002_0210e388, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f728);
    data_ov002_0210e198.Construct(0x8093);
    __register_global_object(&data_ov002_0210e198, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f734);
    data_ov002_0210ea78.Construct(0x8094);
    __register_global_object(&data_ov002_0210ea78, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f740);
    data_ov002_0210e390.Construct(ASSET_SHF_DIE_BCA);
    __register_global_object(&data_ov002_0210e390, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f74c);
    data_ov002_0210e5f8.Construct(0x8095);
    __register_global_object(&data_ov002_0210e5f8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f758);
    data_ov002_0210ea80.Construct(ASSET_SHOOT_BCA);
    __register_global_object(&data_ov002_0210ea80, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f764);
    data_ov002_0210e398.Construct(ASSET_SHOOT_TO_FLY_BCA);
    __register_global_object(&data_ov002_0210e398, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f770);
    data_ov002_0210ea90.Construct(0x8096);
    __register_global_object(&data_ov002_0210ea90, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f77c);
    data_ov002_0210ea98.Construct(ASSET_SLEEP_END_BCA);
    __register_global_object(&data_ov002_0210ea98, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f788);
    data_ov002_0210e608.Construct(ASSET_SLEEP_START_BCA);
    __register_global_object(&data_ov002_0210e608, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f794);
    data_ov002_0210eaa8.Construct(ASSET_SLEEP_TO_LIE_BCA);
    __register_global_object(&data_ov002_0210eaa8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f7a0);
    data_ov002_0210eab0.Construct(ASSET_SLEEP_WAIT_BCA);
    __register_global_object(&data_ov002_0210eab0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f7ac);
    data_ov002_0210e610.Construct(0x8097);
    __register_global_object(&data_ov002_0210e610, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f7b8);
    data_ov002_0210e220.Construct(0x8098);
    __register_global_object(&data_ov002_0210e220, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f7c4);
    data_ov002_0210e168.Construct(0x8099);
    __register_global_object(&data_ov002_0210e168, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f7d0);
    data_ov002_0210e180.Construct(0x809a);
    __register_global_object(&data_ov002_0210e180, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f7dc);
    data_ov002_0210e3b8.Construct(0x809b);
    __register_global_object(&data_ov002_0210e3b8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f7e8);
    data_ov002_0210e1b0.Construct(0x809c);
    __register_global_object(&data_ov002_0210e1b0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f800);
    data_ov002_0210e1c0.Construct(0x809d);
    __register_global_object(&data_ov002_0210e1c0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f80c);
    data_ov002_0210ed58.Construct(0x809e);
    __register_global_object(&data_ov002_0210ed58, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f818);
    data_ov002_0210ed60.Construct(0x809f);
    __register_global_object(&data_ov002_0210ed60, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f830);
    data_ov002_0210ed50.Construct(0x80a0);
    __register_global_object(&data_ov002_0210ed50, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f83c);
    data_ov002_0210ed48.Construct(0x80a1);
    __register_global_object(&data_ov002_0210ed48, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f848);
    data_ov002_0210ed40.Construct(0x80a2);
    __register_global_object(&data_ov002_0210ed40, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f854);
    data_ov002_0210ed38.Construct(0x80a3);
    __register_global_object(&data_ov002_0210ed38, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f860);
    data_ov002_0210ed30.Construct(ASSET_STAR_OPEN_BCA);
    __register_global_object(&data_ov002_0210ed30, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f86c);
    data_ov002_0210ed28.Construct(0x80a4);
    __register_global_object(&data_ov002_0210ed28, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f878);
    data_ov002_0210ed20.Construct(ASSET_SU_HIPAT_BCA);
    __register_global_object(&data_ov002_0210ed20, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f884);
    data_ov002_0210ed18.Construct(ASSET_SU_HIPED_BCA);
    __register_global_object(&data_ov002_0210ed18, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f890);
    data_ov002_0210ed10.Construct(ASSET_SU_HIPSR_BCA);
    __register_global_object(&data_ov002_0210ed10, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f8a8);
    data_ov002_0210ed08.Construct(ASSET_SU_JUMP_BCA);
    __register_global_object(&data_ov002_0210ed08, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f8b4);
    data_ov002_0210ecf8.Construct(ASSET_SU_JUMP_END_BCA);
    __register_global_object(&data_ov002_0210ecf8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f8c0);
    data_ov002_0210ecf0.Construct(ASSET_SU_RUN_BCA);
    __register_global_object(&data_ov002_0210ecf0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f8d8);
    data_ov002_0210ece8.Construct(ASSET_SU_WAIT_BCA);
    __register_global_object(&data_ov002_0210ece8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f8e4);
    data_ov002_0210ece0.Construct(ASSET_SU_WALK_BCA);
    __register_global_object(&data_ov002_0210ece0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f8f0);
    data_ov002_0210ecd8.Construct(ASSET_SU_WALK_END_BCA);
    __register_global_object(&data_ov002_0210ecd8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f8fc);
    data_ov002_0210ecd0.Construct(ASSET_SU_WALK_START_BCA);
    __register_global_object(&data_ov002_0210ecd0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f908);
    data_ov002_0210ecc8.Construct(ASSET_SWIM_BCA);
    __register_global_object(&data_ov002_0210ecc8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f914);
    data_ov002_0210ecc0.Construct(ASSET_SWIM_ATTACK_BCA);
    __register_global_object(&data_ov002_0210ecc0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f920);
    data_ov002_0210e768.Construct(ASSET_SWIM_CARRY_BCA);
    __register_global_object(&data_ov002_0210e768, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f938);
    data_ov002_0210eca0.Construct(ASSET_SWIM_DAMAGE_BCA);
    __register_global_object(&data_ov002_0210eca0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f944);
    data_ov002_0210ec90.Construct(ASSET_SWIM_DIE_BCA);
    __register_global_object(&data_ov002_0210ec90, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f950);
    data_ov002_0210ec88.Construct(ASSET_SWIM_IN_BCA);
    __register_global_object(&data_ov002_0210ec88, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f95c);
    data_ov002_0210ec80.Construct(ASSET_SWIM_STAR_GET_BCA);
    __register_global_object(&data_ov002_0210ec80, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f968);
    data_ov002_0210ec78.Construct(ASSET_SWIM_THROW_BCA);
    __register_global_object(&data_ov002_0210ec78, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f974);
    data_ov002_0210ec70.Construct(ASSET_SWIM_WAIT_BCA);
    __register_global_object(&data_ov002_0210ec70, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f980);
    data_ov002_0210ec68.Construct(0x80a5);
    __register_global_object(&data_ov002_0210ec68, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f98c);
    data_ov002_0210e4d8.Construct(0x80a6);
    __register_global_object(&data_ov002_0210e4d8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f998);
    data_ov002_0210e920.Construct(0x80a7);
    __register_global_object(&data_ov002_0210e920, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f9a4);
    data_ov002_0210e7c8.Construct(0x80a8);
    __register_global_object(&data_ov002_0210e7c8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f9bc);
    data_ov002_0210ead8.Construct(0x80a9);
    __register_global_object(&data_ov002_0210ead8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210f9ec);
    data_ov002_0210eb48.Construct(0x80aa);
    __register_global_object(&data_ov002_0210eb48, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fa10);
    data_ov002_0210ec18.Construct(0x80ab);
    __register_global_object(&data_ov002_0210ec18, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fa34);
    data_ov002_0210ec58.Construct(0x80ac);
    __register_global_object(&data_ov002_0210ec58, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fa58);
    data_ov002_0210ed00.Construct(0x80b3);
    __register_global_object(&data_ov002_0210ed00, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fa7c);
    data_ov002_0210ec28.Construct(0x80b4);
    __register_global_object(&data_ov002_0210ec28, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210faac);
    data_ov002_0210e7e0.Construct(0x80b5);
    __register_global_object(&data_ov002_0210e7e0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fad0);
    data_ov002_0210e7f8.Construct(0x80b6);
    __register_global_object(&data_ov002_0210e7f8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210faf4);
    data_ov002_0210e4a8.Construct(0x80b7);
    __register_global_object(&data_ov002_0210e4a8, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb18);
    data_ov002_0210e828.Construct(0x80bb);
    __register_global_object(&data_ov002_0210e828, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb3c);
    data_ov002_0210e4c0.Construct(0x80bc);
    __register_global_object(&data_ov002_0210e4c0, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb60);
    data_ov002_0210e878.Construct(0x80be);
    __register_global_object(&data_ov002_0210e878, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fb84);
    data_ov002_0210e830.Construct(0x80bf);
    __register_global_object(&data_ov002_0210e830, (void *)SharedFilePtr_Destruct_Anim,
                             data_ov002_0210fba8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e538, 0x8021);
    __register_global_object(&data_ov002_0210e538, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fbd8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e8d0, 0x8022);
    __register_global_object(&data_ov002_0210e8d0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fbfc);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e8e8, ASSET_L_TX_DAMAGE_START_BTP);
    __register_global_object(&data_ov002_0210e8e8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fc20);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e540, ASSET_L_TX_DAMAGE_WAIT_BTP);
    __register_global_object(&data_ov002_0210e540, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fc44);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e958, ASSET_L_TX_ED_AMAZED_BTP);
    __register_global_object(&data_ov002_0210e958, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fc5c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e898, 0x8023);
    __register_global_object(&data_ov002_0210e898, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fc80);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e9e8, 0x8024);
    __register_global_object(&data_ov002_0210e9e8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fca4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ea70, ASSET_L_TX_GAS_WAIT_BTP);
    __register_global_object(&data_ov002_0210ea70, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fcc8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ebd8, 0x8025);
    __register_global_object(&data_ov002_0210ebd8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fcec);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eac8, 0x8026);
    __register_global_object(&data_ov002_0210eac8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fd10);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e460, ASSET_L_TX_LIE_END_BTP);
    __register_global_object(&data_ov002_0210e460, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fd34);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e910, ASSET_L_TX_LIE_WAIT_BTP);
    __register_global_object(&data_ov002_0210e910, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fd58);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e2f0, ASSET_L_TX_RETURN_DOWN_BTP);
    __register_global_object(&data_ov002_0210e2f0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fd7c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ea10, 0x8027);
    __register_global_object(&data_ov002_0210ea10, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fda0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e600, ASSET_L_TX_SLEEP_END_BTP);
    __register_global_object(&data_ov002_0210e600, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fdc4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ec98, ASSET_L_TX_SLEEP_START_BTP);
    __register_global_object(&data_ov002_0210ec98, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fde8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ea88, ASSET_L_TX_SLEEP_TO_LIE_BTP);
    __register_global_object(&data_ov002_0210ea88, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fe0c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eca8, ASSET_L_TX_SLEEP_WAIT_BTP);
    __register_global_object(&data_ov002_0210eca8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fe30);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e9d0, 0x8028);
    __register_global_object(&data_ov002_0210e9d0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fe54);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eaf0, 0x8035);
    __register_global_object(&data_ov002_0210eaf0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fe78);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ea30, ASSET_W_TX_DAMAGE_START_BTP);
    __register_global_object(&data_ov002_0210ea30, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fe9c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e660, ASSET_W_TX_DAMAGE_WAIT_BTP);
    __register_global_object(&data_ov002_0210e660, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fec0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb10, ASSET_W_TX_ED_AMAZED_BTP);
    __register_global_object(&data_ov002_0210eb10, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210fee4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e670, 0x8036);
    __register_global_object(&data_ov002_0210e670, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ff08);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e680, 0x8037);
    __register_global_object(&data_ov002_0210e680, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ff2c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb38, ASSET_W_TX_GAS_WAIT_BTP);
    __register_global_object(&data_ov002_0210eb38, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ff50);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e640, 0x8038);
    __register_global_object(&data_ov002_0210e640, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ff74);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb58, ASSET_W_TX_LIE_END_BTP);
    __register_global_object(&data_ov002_0210eb58, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ff98);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e6a0, ASSET_W_TX_LIE_WAIT_BTP);
    __register_global_object(&data_ov002_0210e6a0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ffbc);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e3e0, ASSET_W_TX_RETURN_DOWN_BTP);
    __register_global_object(&data_ov002_0210e3e0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ffe0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb70, 0x8039);
    __register_global_object(&data_ov002_0210eb70, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ed80);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e6b8, ASSET_W_TX_SLEEP_END_BTP);
    __register_global_object(&data_ov002_0210e6b8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210eda4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eb90, ASSET_W_TX_SLEEP_START_BTP);
    __register_global_object(&data_ov002_0210eb90, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210edc8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e620, ASSET_W_TX_SLEEP_TO_LIE_BTP);
    __register_global_object(&data_ov002_0210e620, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210edec);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e3e8, ASSET_W_TX_SLEEP_WAIT_BTP);
    __register_global_object(&data_ov002_0210e3e8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ee10);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210eba8, 0x803a);
    __register_global_object(&data_ov002_0210eba8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ee34);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e3c8, 0x8049);
    __register_global_object(&data_ov002_0210e3c8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ee58);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e6e0, ASSET_Y_TX_DAMAGE_START_BTP);
    __register_global_object(&data_ov002_0210e6e0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ee7c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ebc8, ASSET_Y_TX_DAMAGE_WAIT_BTP);
    __register_global_object(&data_ov002_0210ebc8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210eea0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e400, 0x804a);
    __register_global_object(&data_ov002_0210e400, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210eeb8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ecb8, 0x804b);
    __register_global_object(&data_ov002_0210ecb8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210eedc);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e708, ASSET_Y_TX_GAS_WAIT_BTP);
    __register_global_object(&data_ov002_0210e708, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ef00);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e408, 0x804c);
    __register_global_object(&data_ov002_0210e408, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ef24);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ec00, ASSET_Y_TX_LIE_END_BTP);
    __register_global_object(&data_ov002_0210ec00, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ef48);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ec10, ASSET_Y_TX_LIE_WAIT_BTP);
    __register_global_object(&data_ov002_0210ec10, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ef6c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e728, ASSET_Y_TX_RETURN_DOWN_BTP);
    __register_global_object(&data_ov002_0210e728, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210ef90);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e3b0, 0x804d);
    __register_global_object(&data_ov002_0210e3b0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210efa8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e738, ASSET_Y_TX_SLEEP_END_BTP);
    __register_global_object(&data_ov002_0210e738, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210efcc);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210ec40, ASSET_Y_TX_SLEEP_START_BTP);
    __register_global_object(&data_ov002_0210ec40, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210eff0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e250, ASSET_Y_TX_SLEEP_TO_LIE_BTP);
    __register_global_object(&data_ov002_0210e250, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f014);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e430, 0x804e);
    __register_global_object(&data_ov002_0210e430, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f038);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e438, 0x804f);
    __register_global_object(&data_ov002_0210e438, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f05c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e770, 0x80ad);
    __register_global_object(&data_ov002_0210e770, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f080);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e788, ASSET_TX_DAMAGE_START_BTP);
    __register_global_object(&data_ov002_0210e788, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f0a4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e450, ASSET_TX_DAMAGE_WAIT_BTP);
    __register_global_object(&data_ov002_0210e450, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f0c8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e270, ASSET_TX_ED_KISS_TO_POSE_BTP);
    __register_global_object(&data_ov002_0210e270, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f0ec);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e458, ASSET_TX_ED_KISS_WAIT_BTP);
    __register_global_object(&data_ov002_0210e458, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f110);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e288, 0x80ae);
    __register_global_object(&data_ov002_0210e288, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f134);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e478, 0x80af);
    __register_global_object(&data_ov002_0210e478, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f158);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e7e8, ASSET_TX_GAS_WAIT_BTP);
    __register_global_object(&data_ov002_0210e7e8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f17c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e230, 0x80b0);
    __register_global_object(&data_ov002_0210e230, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f1a0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e800, ASSET_TX_LIE_END_BTP);
    __register_global_object(&data_ov002_0210e800, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f1d0);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e1d0, ASSET_TX_LIE_WAIT_BTP);
    __register_global_object(&data_ov002_0210e1d0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f1f4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e4b8, ASSET_TX_RETURN_DOWN_BTP);
    __register_global_object(&data_ov002_0210e4b8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f218);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e1c8, 0x80b1);
    __register_global_object(&data_ov002_0210e1c8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f248);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e4d0, ASSET_TX_SLEEP_END_BTP);
    __register_global_object(&data_ov002_0210e4d0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f26c);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e1e0, ASSET_TX_SLEEP_START_BTP);
    __register_global_object(&data_ov002_0210e1e0, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f290);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e4e8, ASSET_TX_SLEEP_TO_LIE_BTP);
    __register_global_object(&data_ov002_0210e4e8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f2b4);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e1e8, ASSET_TX_SLEEP_WAIT_BTP);
    __register_global_object(&data_ov002_0210e1e8, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f2d8);
    SharedFilePtr_Construct_TexSeq(&data_ov002_0210e500, 0x80b2);
    __register_global_object(&data_ov002_0210e500, (void *)SharedFilePtr_Destruct_TexSeq,
                             data_ov002_0210f2fc);
    PlayerInitStates();
}
}
}

#pragma section initsec end

/* Retail constructor-table word .p__sinit_ov002_021019d0 at 0x02108110.
 * pinit is the section mwcc names .ctor. The initializer above is a C
 * function, so the compiler does not emit its own .p__sinit_ word for
 * this file. This pointer is that word. */
#pragma section pinit begin
static void (*p__sinit_ov002_021019d0)(void) = __sinit_ov002_021019d0;
#pragma section pinit end
