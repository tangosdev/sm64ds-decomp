//cpp
/* daObjMarioCap_c -- the lost Mario cap, ov002 (overlay_actors CAP(269);
 * RTTI ov002:0x021095ac names 15daObjMarioCap_c; profile
 * g_profile_OBJ_MARIO_CAP). ov002 is mixed (Yoshi egg, switches, stars, push
 * block, player, ...); this is the cap, not those.
 *
 * A cap is a pickup for one character (mModelIndex 0..2). Its mType, from the
 * low byte of param1, picks one of nine states (below) and a collider size,
 * and decides whether the cap registers a dCapIcon_c. Depending on type it
 * sits still, falls and slides down slopes, plays an animation next to a
 * player, or is a vanish / metal power-up; a player touching it (or Yoshi's
 * egg path, OnTurnIntoEgg) puts it into the taken state, where it follows the
 * player until its animation ends. A cap in the sliding state blinks through the last
 * half of its timer and is destroyed when the timer ends (unless bit 0x20000 or 0x40000
 * of mFlags, the Yoshi-mouth states, is set).
 *
 * The whole cap unit of ov002 .text, 0x020b6f18..0x020b8bf0, 31 functions:
 * the D1/D0 pair, the state helpers and methods, InitResources and the
 * registry factory daObjMarioCap_c_classInit (`return new`, through the
 * class's leaf operator new). OnYoshiTryEat is the key function -- the first
 * out-of-line virtual daObjMarioCap_c declares after the inline destructor in
 * daObjMarioCap_c.h -- so the compiler owns retail's D1/D0 pair and the
 * complete RTTI/vtable group, and no D2 is retained.
 *
 * STATES. InitResources' switch hands EnterState one of nine
 * {enter, per-frame} records (data_ov002_0210df04 .. df84, filled in by
 * __sinit_ov002_02101064 from 8-byte member-pointer constants).
 * EnterState stores the record in mStateEntry and runs its enter
 * function; Behavior then runs the per-frame one.
 *
 *   record  enter          per-frame   mType            what the per-frame function does
 *   df64    InitWait       Wait        0                no override matrix and the player has not
 *                                                       lost the cap: once on the ground, spawns
 *                                                       OBJ_MARIO_CAP with 0x12 ORed into param1 and
 *                                                       removes itself a frame later. Otherwise:
 *                                                       touch check, and PlayerLoseCap + removal
 *                                                       below the kill height (data_02092138)
 *   df84    InitTimedWait  TimedWait   1                200-frame timer; removed when it runs out,
 *                                                       on touching the ground, below the kill
 *                                                       height, or off screen (mFlags & 8)
 *   df04    InitTouchWait  TouchWait   2                touch check only
 *   df24    InitTimedWait2 TimedWait2  3                same removal rules as df84
 *   df34    InitSlide      Slide       4..9, 11,        the sliding / blinking state (types 4, 6, 8,
 *                                      16..18           16, 17 only run the touch check; param1
 *                                                       type 14 is rewritten to 4 by InitResources,
 *                                                       so no handler sees mType 14)
 *   df54    InitTaken      Taken       19, and any      the taken state: follows the player, plays
 *                                      type once        the pickup animation, removes the cap when
 *                                      touched or       it ends; hands the hat over itself only on
 *                                      turned into      the mTakenStep 1 path (see the function)
 *                                      an egg
 *   df74    InitAnim       Anim        10, 15, 20..22   plays the type's animation, tracking the
 *                                                       player for types 10 and 15; removed when it
 *                                                       ends (types 10, 15, 22)
 *   df14    InitFall       Fall        12               enter zeroes mVertAccel; the per-frame body
 *                                                       only does anything while mVertAccel is
 *                                                       nonzero; below the kill height it calls
 *                                                       PlayerLoseCap and removes the cap
 *   df44    InitDormant    Dormant     13               enter clears mBlinkHidden; per-frame does
 *                                                       nothing
 *
 * Fix12 reads throughout: 0x1000 is 1.0, and positions / sizes shown as "N
 * units" are the raw value >> 12. Angles: 0x10000 is a full turn.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *
 *   The functions are written in REVERSE ROM order, because mwccarm 2004/b56
 *   emits one .text section per function in the reverse of source order.
 *
 *   common.h comes first so Matrix4x3 is the flat s32[12] spelling; math/
 *   Matrix.h through ModelAnim.h would scalarize the copies in
 *   UpdateMatrix.
 *
 *   The PMF stand-ins (CapStateSelf / CapStateRec / CapEnterSelf). A PMF on
 *   the real dEnemyBase_c makes mwccarm ICE rather than give a diagnostic.
 *
 *   The (long long)(int) 20.12 multiplies in Slide and the
 *   (unsigned long long)&vulnFlags or-into-vulnFlags in InitSlide
 *   are the matching forms; plain member addressing does not match.
 *
 *   CheckTouch's angle copy keeps `a = b ? a : a` through a V16.
 *
 *   Reading the player's angles / position as plain members (`closest->mAngleX`)
 *   misses in InitAnim (0x19c vs 0x1a0 bytes) and Taken
 *   (0x224 vs 0x21c); taking `&player->mAngleX` / `&player->mPosX` and indexing
 *   the pointer matches. mStateTimer is s16 in dEnemyBase_c but the helpers
 *   read it unsigned, hence CAP_TIMER.
 *
 * WHY SOME CALLS ARE SPELLED AS MANGLED SYMBOLS:
 *   dActor_c::SetRanges is not on the header (notes/mwccarm-codegen.md 6az).
 *   ModelAnim::SetAnim / DropShadowRadHeight / ReflectAngle take Fix12<int>
 *   by value, which has no implicit int conversion (notes/mwccarm-codegen.md
 *   6az). dBgCh_Actr::GetFloorResult / GetWallResult are not declared.
 *
 * Known limits:
 *   InitResources keeps two `(u32)param1` read-side casts; spelled plainly,
 *   2004/b56 materialises the param1 address once for each read-modify-write
 *   and the body grows from 0x4c8 to 0x4d0 (see the comments at both sites).
 *   It also keeps the dCcAc_c::Init / dBgCh_Actr::Init calls mangled: both
 *   take Fix12<int> by value.
 *   The data_ov002_0210de* / 0210df* handles have no recovered names in
 *   symbols.txt, so none are coined; the sixteen-byte state records are only
 *   described above.
 *   Unrecovered meaning: most mType values (only 4, 6..9 and 19 are named in
 *   the header), what the animations behind the 0210de* handles contain (the
 *   file ids 0x8012 / 0x8013 and 0x476..0x480 are in __sinit_ov002_02101064), what func_ov002_020f030c's result (unused here) is, and the
 *   fields unk_403 and unk_408, which are written and never read in this file.
 *   mStateEntry stays an s32 because the member-pointer record is spelled by
 *   the file-local CapStateRec / CapEnterSelf stand-ins.
 *   The SharedFilePtr header has no fields; CleanupResources still casts the
 *   AnimRec tables.
 *   dBgCh_Gnd stays a 0x50 stack blob: its C1/D1 only run on the airborne
 *   path (UpdateMatrix).
 *   The +0xc8 override-matrix pointer lives in dActor_c's pad_0c5, a header
 *   this class does not own (CAP_OVERRIDE_MATRIX); this file never sets it.
 */

#include "common.h"
#include "daObjMarioCap_c.h"
#include "decl_common.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "SaveData.h"
#include "Sound.h"
#include "SurfaceInfo.h"
#include "dBgCh_Gnd.h"

/* --------------------------------------------------------------------------
 * Local shapes the merged members need.
 * ------------------------------------------------------------------------ */

/* The animation descriptor pairs these tables hold: a header word and the
 * BCA file pointer read at +4. Spelled as a struct because three members
 * reach the second word by name and two more take the address of the whole
 * record. SharedFilePtr.h commits to no fields, so CleanupResources still
 * casts these to SharedFilePtr*. */
struct AnimRec { void *f0; void *file; };

/* Behavior walks a pointer-to-member stashed at +0x3bc. NOT the real
 * dEnemyBase_c: a PMF on a non-polymorphic single-base class is laid out
 * differently from one on the real class, so the stand-in shape here is
 * codegen, not decoration. Letting the PMF bind to the real dEnemyBase_c
 * makes mwccarm abort with an internal compiler error rather than a
 * diagnostic. */
struct CapStateSelf { char pad[0x800]; };
typedef void (CapStateSelf::*CapStatePmf)();
struct CapStateRec { char pad[8]; CapStatePmf fn; };

/* EnterState's own view of the same slot, with the class shaped so
 * the member pointer it stores and immediately calls is laid out the way the
 * ROM's bytes read it. */
struct CapEnterSelf;
typedef int (CapEnterSelf::*CapEnterPmf)();
struct CapEnterSelf { char pad[0x3bc]; CapEnterPmf *pp; };

/* Bit view of dCapIcon_c::mFlags. f1 is bit 1, the bit Behavior watches (the
 * one dCapIcon_c's GetCapState tests); f0 is the cap-bank bit. */
struct Flags3eb {
    u8 f0 : 1;
    u8 f1 : 1;
};

/* dEnemyBase_c declares mStateTimer as an s16, but the helpers below read and
 * write it as an unsigned halfword (ldrh/strh); a signed read is ldrsh. */
#define CAP_TIMER(cap) (*(u16 *)&(cap)->mStateTimer)

/* An s16 angle read as unsigned: the index into the sin/cos table is
 * (angle >> 4) * 2, one {sin, cos} pair of s16 per 16 angle units. */
#define CAP_ANGLE(a) (*(u16 *)&(a))

/* A Matrix4x3 pointer at +0xc8, inside dActor_c's pad_0c5 (a header this class
 * does not own). The actor carrying the cap sets it (daMky_c stores its
 * mCapMtx there for the cap it spawns); this file only tests it and clears it. While it is non-null,
 * UpdateMatrix copies it into the model matrix instead of building one,
 * and the ordinary pickup in CheckTouch is skipped unless mType is 0
 * (the vanish and metal touches above it still fire). */
#define CAP_OVERRIDE_MATRIX(cap) (*(Matrix4x3 **)((char *)(cap) + 0xc8))

/* Actor IDs (symbols/actor_debug_names.tsv). */
enum {
    kActorPlayer = 0xbf,        /* PLAYER */
    kActorMarioCap = 0x10d,     /* OBJ_MARIO_CAP: this class */
    kActor1UpLogo = 0x14b       /* OBJ_1UPLOGO */
};

/* Sound bank 3: the 1-Up chime (da1up_c plays it for the 1-up mushroom too). */
enum { kSfx1Up = 0x6e };

/* Sizes in fix12 (0x1000 = 1.0 unit). */
enum {
    kClsnSizeSmall = 0x1e000,   /* 30 units: dCcAc_c radius and height at Init */
    kClsnSizeLarge = 0x32000,   /* 50 units: collider radius and height of the df34 types */
    kClipFar = 0x1000000        /* 4096 units: SetRanges clip and far distances */
};

/* --------------------------------------------------------------------------
 * ROM symbols. Mangled spellings live inside extern "C" so the C++ front end
 * does not mangle them a second time.
 * ------------------------------------------------------------------------ */
extern "C" {

/* -- other modules -- */
void  func_02013a88(void);
short func_02010844(void *self, void *v, short angle);
void  func_020167a4(void *p);
int   func_02037e58(void *p);
void  func_ov002_020f030c(int x);

unsigned short DecIfAbove0_Short(unsigned short *p);

int   Vec3_HorzLen(void *v);
void  Vec3_MulScalarInPlace(void *v, int s);
void  Vec3_Add(void *out, void *a, void *b);
void  Vec3_Asr(int *out, void *v, int shift);

void  Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
void  Matrix4x3_FromRotationY(void *m, int angle);

void  _Z11UpdateAngleRssis(void *p, short target, int step, short limit);
void  _Z14ApproachLinearRiii(int *x, int target, int step);

short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
int   _ZN4cstd4fdivEii(int a, int b);


void *_ZNK10dBgCh_Actr13GetWallResultEv(void *self);
char *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);

/* dCcAc_c::Init and dBgCh_Actr::Init take Fix12<int> by value, which has no
   implicit int conversion (notes/mwccarm-codegen.md 6az). */
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *thiz, void *actor, s32 f1, s32 f2, u32 a, u32 b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, s32 f1, s32 f2, void *v, void *w);

void *_ZN9dBgCh_GndC1Ev(dBgCh_Gnd *ray);
void  _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *ray);

/* SetRanges carries Fix12<int> by value; dActor_c.h deliberately omits it
   (notes/mwccarm-codegen.md 6az). A call is unaffected. */
void  _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int offsetY, int radius,
                                                int clipDistance, int farDistance);
void  _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
          void *self, void *shadow, void *matrix, int radius, int depth, int opacity);
short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int nx, int nz, short ang);
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int flags,
                                                  int speed, unsigned int startFrame);

/* -- data -- */
extern Matrix4x3      data_020a0e68;
extern short          data_02082214[];
extern int            data_02092138;
extern u8             data_0209f2d8[];

extern struct AnimRec data_ov002_0210de00, data_ov002_0210de08;
extern struct AnimRec data_ov002_0210de10, data_ov002_0210de18;
extern struct AnimRec data_ov002_0210de20, data_ov002_0210de28;
extern struct AnimRec data_ov002_0210de30, data_ov002_0210de38;
extern struct AnimRec data_ov002_0210de40, data_ov002_0210de48;
extern struct AnimRec data_ov002_0210de50, data_ov002_0210de58;
extern struct AnimRec data_ov002_0210de60;

/* data_ov002_0210df04..df84 are declared by decl_common.h as `char`; only
   df54 is missing there. Keep the family's spelling. */
extern char           data_ov002_0210df54;

extern struct AnimRec *data_ov002_020ff0a0[];
extern struct AnimRec *data_ov002_020ff0b8[];
extern int            *data_ov002_020ff0c4[];

}

// @symbol daObjMarioCap_c_classInit
extern "C" daObjMarioCap_c *daObjMarioCap_c_classInit(void)
{
    return new daObjMarioCap_c();
}

/* Decodes param1 (low byte mType, bits 8..11 mModelIndex, bits 12..15
 * mIconKind), loads the animation files for the type, sets up the model, the
 * two colliders and the gravity, then enters the type's state and registers
 * the cap-icon unless mIconKind is 0xff. Returns 0 (failure) if the character
 * index is 3 or more or the model does not load. */
// @symbol _ZN15daObjMarioCap_c13InitResourcesEv
int daObjMarioCap_c::InitResources()
{
    int flag;   /* 1 only for type 17: the cap-bank bit passed to the icon */
    unsigned char v;

    mType = param1 & 0xff;
    mModelIndex = (param1 >> 8) & 0xf;
    mIconKind = (param1 >> 0xc) & 0xf;

    if (mType == 0xff)
        mType = 0;

    if (mModelIndex >= 3)
        return 0;

    /* Types 17 and 4 accept icon kinds 0..2; every other type only 0..1. */
    if (mType == 0x11 || mType == 4) {
        if (mIconKind > 2)
            mIconKind = 0;
    } else {
        if (mIconKind > 1)
            mIconKind = 0;
    }

    /* dExtFrameCtrl_c files this type will use; CleanupResources releases the same
       set. Type 15 and types 20..22 load their own sets, every other type the
       pair at the bottom. */
    switch (mType) {
    case 0xf:
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de50);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de60);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de48);
        break;
    case 0x14:
    case 0x15:
    case 0x16:
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de28);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de08);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de20);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de40);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de10);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de00);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de58);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de18);
        break;
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    default:
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de30);
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)&data_ov002_0210de38);
        break;
    }

    if (mModelAnim.SetFile(
            (BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov002_020ff0ac[mModelIndex]),
            1, -1) == 0)
        return 0;

    mShadowModel.InitCylinder();

    mScaleX = 0x1000;           /* 1.0 */
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    flag = 0;

    /* dCcAc_c collider: radius and height 30 units, flags 0x800002, vulnFlags 0.
       dBgCh_Actr level-collision: radius 30 units, height 22 units. */
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, kClsnSizeSmall, kClsnSizeSmall, 0x800002, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, kClsnSizeSmall, 0x16000, 0, 0);

    /* Remember where the cap was placed. */
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;

    mVertAccel = -0x1000;       /* gravity: -1 unit a frame each frame */
    mTerminalVelocity = -0x1e000;     /* -30 units a frame */

    /* Per type: the state record to enter (see the banner), the icon kind and
       the collider size. 0x8000 in vulnFlags is the bit dCc_c.h labels yoshi
       tongue (a best-effort reading there). */
    switch (mType) {
    default:
        break;
    case 0:
        *(s32 *)(((long long)((char *)&mdCcAc_c.vulnFlags))) |= 0x8000;
        mIconKind = 4;
        EnterState(&data_ov002_0210df64);
        break;
    case 1:
        mIconKind = 0xff;
        EnterState(&data_ov002_0210df84);
        break;
    case 2:
        *(s32 *)(((long long)((char *)&mdCcAc_c.vulnFlags))) |= 0x8000;
        mIconKind = 4;
        EnterState(&data_ov002_0210df04);
        break;
    case 3:
        mIconKind = 0xff;
        EnterState(&data_ov002_0210df24);
        break;
    case TYPE_START_TAKEN:
        mIconKind = 0xff;
        mPlayer = ClosestPlayer();
        EnterState(&data_ov002_0210df54);
        mTakenStep = 1;
        break;
    case 20:
    case 21:
    case 22:
        /* These three run the render-matrix update once here before entering
           the animation state they share with 10 and 15. */
        UpdateMatrix();
        /* fallthrough */
    case 10:
    case 15:
        mIconKind = 0xff;
        EnterState(&data_ov002_0210df74);
        break;
    case 12:
        mIconKind = 4;
        EnterState(&data_ov002_0210df14);
        break;
    case 13:
        mIconKind = 4;
        EnterState(&data_ov002_0210df44);
        break;
    case 14:
        mIconKind = 2;
        mdCcAc_c.radius = kClsnSizeLarge;
        mdCcAc_c.height = kClsnSizeLarge;
        /* Spelt plainly (`param1 = param1 - 0xa;`), both sides of this
           assignment are the same expression, and 2004/b56 value-numbers
           them together and materialises the address once (`add r3, r5, #8`
           at +0x37c, then `ldr r0, [r3]` and `str r2, [r3]`), where the ROM
           folds the offset into both accesses. A redundant cast on the read
           side is enough to make the two sides textually different and
           reach the folded form -- no `volatile` needed, so tools/tiers.py
           never reads this as a codegen trick. Same residue and same lever
           as daDoor_c::InitResources (src/actors/daDoor_c.cpp) and the second
           site below. */
        param1 = (u32)param1 - 0xa;   /* low byte 14 becomes 4, matching mType below */
        mType = TYPE_RESPAWNING;
        EnterState(&data_ov002_0210df34);
        break;
    case 17:
        mdCcAc_c.radius = kClsnSizeLarge;
        mdCcAc_c.height = kClsnSizeLarge;
        flag = 1;
        EnterState(&data_ov002_0210df34);
        break;
    case TYPE_VANISH_LUIGI_A:
    case TYPE_VANISH_LUIGI_B:
    case TYPE_METAL_WARIO_A:
    case TYPE_METAL_WARIO_B:
        mIconKind = 0xff;
        mdCcAc_c.radius = kClsnSizeLarge;
        mdCcAc_c.height = kClsnSizeLarge;
        EnterState(&data_ov002_0210df34);
        break;
    case 5:
    case 11:
    case 18:
        mAreaId = -1;           /* not bound to an area */
        /* fallthrough */
    case 16:
        mIconKind = 3;
        /* fallthrough */
    case TYPE_RESPAWNING:
        mdCcAc_c.radius = kClsnSizeLarge;
        mdCcAc_c.height = kClsnSizeLarge;
        EnterState(&data_ov002_0210df34);
        break;
    }

    mModelAnim.speed = 0x1000;  /* animations play at 1.0x */

    /* Register with the cap-icon list for this character; flag is the
       cap-bank bit. */
    if (mIconKind != 0xff) {
        if (flag != 0)
            v = 1;
        else
            v = 0;
        mCapIcon.func_ov001_020ab228((char *)this, mModelIndex & 0xff, mIconKind, v);
    }

    /* Keep only the type and character bits (0..11) of param1. This is the
       second materialised param1 read-modify-write, at +0x448; see the first
       one in case 14 for the mechanism. Measured: with both casts the
       candidate is 0x4c8 and 0 of 306 words differ; with neither it is 0x4d0,
       and over the shared prefix 98 of 308 differ. */
    param1 = (u32)param1 & 0xfff;
    return 1;
}

/* Per frame, for a cap with an icon: waits (dormant) while bit 1 of the
 * icon's flags is clear; when it comes on, the cap pops in -- scale reset to 0,
 * thrown up at 15 units a frame (mVertSpeed 0xf000), a puff of dust -- and
 * scales up while it falls: the scale chases a target that starts at 2.0 and eases
 * down to 1.0, so it overshoots to about 1.25 before settling at 1.0. Then: the current
 * state's per-frame function, the render-matrix update, the model animation
 * and bone update, UpdateYoshiEat (which ends the frame early when it returns
 * nonzero), the generic move + collide step (skipped for the types listed at
 * the site), and the collider refresh. Always returns 1. */
// @symbol _ZN15daObjMarioCap_c8BehaviorEv
int daObjMarioCap_c::Behavior()
{
    if (mIconKind != 0xff) {
        if (((Flags3eb *)&mCapIcon.mFlags)->f1 == 0) {
            mDormant = 1;
        } else if (mDormant == 1) {
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            mDormant = 0;
            mPopScale = 0x2000;     /* 2.0 */
            mPopIn = 1;
            mVertSpeed = 0xf000;    /* 15 units a frame, upward */
            SmallPoofDust();
        }
    }

    if (mDormant == 1) {
        return 1;
    }

    /* Pop-in: the target eases from 2.0 to 1.0 by 0.125 a frame, the scale
       follows it by 0.25 a frame, and the cap moves and collides meanwhile. */
    if (mIconKind != 0xff && mPopIn != 0) {
        _Z14ApproachLinearRiii(&mPopScale, 0x1000, 0x200);
        _Z14ApproachLinearRiii(&mScaleX, mPopScale, 0x400);
        mScaleZ = mScaleX;
        mScaleY = mScaleZ;
        UpdatePos(&mdCcAc_c);
        UpdateWMClsn(mWithMeshClsn, 0);
        if (mWithMeshClsn.IsOnGround() != 0) {
            if (mScaleX == 0x1000) {
                mPopIn = 0;
            }
        }
    }

    /* Run the current state's per-frame function (the second member pointer
       of the record in mStateEntry), if it has one. */
    {
        CapStateRec *q = *(CapStateRec **)&mStateEntry;
        if (q->fn != 0) {
            (((CapStateSelf *)this)->*(q->fn))();
        }
    }

    UpdateMatrix();
    mModelAnim.Advance();

    if (mModelAnim.file != 0) {
        func_020167a4(&mModelAnim);
        mModelAnim.UpdateVerts();
    }

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        return 1;
    }

    /* Types 4, 6, 8, 10, 12, 13, 15, 17, 19, 20, 21 and 22 skip the generic
       move + collide step. */
    {
        int v = mType;
        if (v != 4 && v != 0x11 && v != 6 && v != 8 && v != 0xc && v != 0xa
            && v != 0x13 && v != 0xf && v != 0x14 && v != 0x15 && v != 0x16 && v != 0xd) {
            UpdatePos(&mdCcAc_c);
            UpdateWMClsn(mWithMeshClsn, 0);
        }
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* Draws the model unless: mFlags bit 0x40000 is set (a Yoshi-mouth bit), the
 * cap is dormant, or its scale is under 0.0625 (0x100). Types 4..9, 11, 16, 17
 * and 18 are also skipped on the frames mBlinkHidden has bit 0 set, except in
 * the taken state. Always returns 1. */
// @symbol _ZN15daObjMarioCap_c6RenderEv
int daObjMarioCap_c::Render()
{
    int b = (mFlags & 0x40000) ? 1 : 0;
    if (b) return 1;
    if (mDormant == 1 || mScaleX < 0x100) return 1;

    int t = mType;
    if (t == 4 || t == 17 || t == 5 || t == 18 || t == 16 || t == 11 || (unsigned)(t - 6) <= 3) {
        if (!(mBlinkHidden & 1) || mStateEntry == (int)&data_ov002_0210df54) {
            mModelAnim.Model::Render((const Vector3 *)&mScaleX);
        }
    } else {
        mModelAnim.Model::Render((const Vector3 *)&mScaleX);
    }
    return 1;
}

/* Takes the cap off the cap-icon list, if it was on it. */
// @symbol _ZN15daObjMarioCap_c16OnPendingDestroyEv
void daObjMarioCap_c::OnPendingDestroy()
{
    if (mIconKind == 0xff)
        return;
    mCapIcon.Unlink();
}

/* Releases the character's model and the animation files InitResources
 * loaded for this type, in the same grouping. Always returns 1. */
// @symbol _ZN15daObjMarioCap_c16CleanupResourcesEv
int daObjMarioCap_c::CleanupResources()
{
  int i = mModelIndex;
  if (i >= 3) return 1;
  ((SharedFilePtr *)(data_ov002_020ff0ac[i]))->Release();
  switch (mType) {
  case 0xf:
    ((SharedFilePtr *)(&data_ov002_0210de50))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de60))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de48))->Release();
    break;
  case 0x14:
  case 0x15:
  case 0x16:
    ((SharedFilePtr *)(&data_ov002_0210de28))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de08))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de20))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de40))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de10))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de00))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de58))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de18))->Release();
    break;
  default:
    ((SharedFilePtr *)(&data_ov002_0210de30))->Release();
    ((SharedFilePtr *)(&data_ov002_0210de38))->Release();
    break;
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/*                                                                            */
/* The key function: the first out-of-line virtual this class declares after   */
/* the inline destructor, so the vtable and RTTI group land in this TU.        */
/* -------------------------------------------------------------------------- */
/* Returns 0 for type 2 and 4 for every other type. The other OnYoshiTryEat
   overrides on this tree use 0 for "refuse the bite"; what 4 selects is not
   recovered here. */
// @symbol _ZN15daObjMarioCap_c13OnYoshiTryEatEv
s32 daObjMarioCap_c::OnYoshiTryEat() {
  if (mType == 2) return 0;
  return 4;
}

/* Slot 19. Gives the player the hat the way CheckTouch does
 * (SetNoControlState(8) then SetNewHatCharacter, but with 1 where the touch
 * passes 0 as the second argument) and starts the 0x8012 animation, then binds
 * the cap to that player and enters the taken state. */
// @symbol _ZN15daObjMarioCap_c13OnTurnIntoEggER6Player
void daObjMarioCap_c::OnTurnIntoEgg(Player &player)
{
    if (player.SetNoControlState(8, -1, 0) == 1) {
        player.SetNewHatCharacter(mModelIndex & 0xff, 1, 0);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov002_0210de30.file,
                                                    0x40000000, 0x1000, 0);
        mAnimStarted = 1;
    }
    mPlayer = &player;
    EnterState(&data_ov002_0210df54);
}

/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Render-matrix update, once a frame. Copies the movement angles into the    */
/* render angles (all types but 5, 7, 9, 11, 16, 18, 20, 21, 22). If an       */
/* override matrix is set, copies it into the model's matrix and stops.       */
/* Otherwise (types 6 and 7 first call ApplyOpacity(0, 0)) builds             */
/* translation (position / 8) * rotation (the render angles) in the scratch   */
/* matrix and copies that in. Then it places the drop shadow, except in the   */
/* taken, animation (df74) and idle (df44) states or while the scale is under */
/* 0.3125 (0x500): the floor height is the cap's own Y on the ground, or the  */
/* result of a downward probe from 40 units above the cap (that start height  */
/* if the probe hits nothing) when airborne. The shadow has radius 80 units,  */
/* depth 50 units, opacity 0xf, and sits 10 units lower for characters 0 and */
/* 1.                                                                         */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjMarioCap_c12UpdateMatrixEv
void daObjMarioCap_c::UpdateMatrix()
{
    int probe[3];
    int v[3];
    char ray[0x50];
    int m = mType;
    if (m != 5 && m != 0x12 && m != 0x10 && m != 0xb && m != 7 && m != 9 &&
        m != 0x14 && m != 0x15 && m != 0x16) {
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
    }

    if (CAP_OVERRIDE_MATRIX(this) != 0) {
        mModelAnim.mat4x3 = *CAP_OVERRIDE_MATRIX(this);
        return;
    }

    if ((unsigned int)(mType - 6) <= 1)
        mModelAnim.ApplyOpacity(0, 0);

    Vec3_Asr(v, &mPosX, 3);     /* position / 8: the model matrix's translation scale */
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX,
                                           mAngleY, mAngleZ);
    mModelAnim.mat4x3 = data_020a0e68;

    {
        char *s = *(char **)&mStateEntry;
        if (s == &data_ov002_0210df54)
            return;
        if (s == &data_ov002_0210df74)
            return;
        if (s == &data_ov002_0210df44)
            return;
    }

    if (mScaleX < 0x500)
        return;

    {
        int y = mPosY;
        int off;
        if (mWithMeshClsn.IsOnGround() == 0) {
            probe[0] = mPosX;
            probe[1] = mPosY;
            probe[2] = mPosZ;
            probe[1] = probe[1] + 0x28000;
            _ZN9dBgCh_GndC1Ev((dBgCh_Gnd *)ray);
            ((dBgCh_Gnd *)ray)->SetObjAndPos(*(Vector3 *)probe, 0);
            y = probe[1];
            if (((dBgCh_Gnd *)ray)->DetectClsn() != 0)
                y = ((dBgCh_Gnd *)ray)->clsnY;
            _ZN9dBgCh_GndD1Ev((dBgCh_Gnd *)ray);
        }

        off = 0;
        {
            int t = mModelIndex;
            if (t == 0)
                goto neg;
            if (t == 1) {
            neg:
                off = -10;
            }
        }
        Matrix4x3_FromRotationY(&mShadowMat, mAngleY);
        /* m[9..11] is the translation row. */
        mShadowMat.m[9] = mPosX >> 3;
        mShadowMat.m[10] = (y + (off << 12)) >> 3;
        mShadowMat.m[11] = mPosZ >> 3;
        _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
            this, &mShadowModel, &mShadowMat, 0x50000, 0x32000, 0xf);
    }
}

/* Enter a state: stores the record in mStateEntry and calls its first member
 * pointer (the enter function) on the cap, returning its result, or returns 0
 * if that member pointer is null. */
// @symbol _ZN15daObjMarioCap_c10EnterStateEPv
int daObjMarioCap_c::EnterState(void *rec)
{
    CapEnterSelf *c = (CapEnterSelf *)this;
    c->pp = (CapEnterPmf *)rec;
    CapEnterPmf *q = c->pp;
    if (*q == 0) return 0;
    return (c->**q)();
}

/* df64's enter function: nothing to do. */
// @symbol _ZN15daObjMarioCap_c8InitWaitEv
int daObjMarioCap_c::InitWait()
{
    return 1;
}

/* df64's per-frame function (type 0). While there is no override matrix and
 * the player has not lost the cap: once on the ground with the timer at 0,
 * spawns OBJ_MARIO_CAP with 0x12 ORed into param1 at the cap's position and
 * sets the timer to 3; the timer counts down and the cap is removed when it
 * reads 1, the frame after the spawn. Otherwise: if there is no override
 * matrix, widens the clip ranges (clip offset and radius 50 units, clip and
 * far distance 4096 units); runs the touch check; and below the kill height
 * (STAR_CAP_MIN_POS_Y, data_02092138) marks the cap lost for the player
 * (SaveData::PlayerLoseCap) and removes it. The spawn arguments pass the
 * cap's own area ID. */
// @symbol _ZN15daObjMarioCap_c4WaitEv
int daObjMarioCap_c::Wait() {
    if (CAP_OVERRIDE_MATRIX(this) == 0 && !SaveData::HasPlayerLostCap()) {
        if (CAP_TIMER(this) == 0 && mWithMeshClsn.IsOnGround()) {
            if (dActor_c::Spawn(kActorMarioCap, param1 | 0x12, *(const Vector3 *)&mPosX, (const Vector3_16 *)0, mAreaId, -1)) {
                CAP_TIMER(this) = 3;
            }
        }
        if (DecIfAbove0_Short(&CAP_TIMER(this)) == 1) {
            MarkForDestruction();
        }
        return 1;
    }
    if (CAP_OVERRIDE_MATRIX(this) == 0) {
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0x32000, 0x32000, kClipFar, kClipFar);
    }
    CheckTouch();
    if (data_02092138 > mPosY) {
        SaveData::PlayerLoseCap();
        MarkForDestruction();
    }
    return 1;
}

/* df84's enter function (type 1): 200-frame timer. */
// @symbol _ZN15daObjMarioCap_c13InitTimedWaitEv
int daObjMarioCap_c::InitTimedWait()
{
    mStateTimer = 200;
    return 1;
}

/* df84's per-frame function (type 1): removes the cap when it is below the
 * kill height, on the ground, out of timer, or off screen (mFlags & 8).
 * Returns 1. */
// @symbol _ZN15daObjMarioCap_c9TimedWaitEv
int daObjMarioCap_c::TimedWait()
{
    if (data_02092138 > mPosY
        || mWithMeshClsn.IsOnGround()
        || !DecIfAbove0_Short((unsigned short*)&mStateTimer)
        || (mFlags & 8))
    {
        MarkForDestruction();
        return 1;
    }
    return 1;
}

/* df04's enter function (type 2): nothing to do. */
// @symbol _ZN15daObjMarioCap_c13InitTouchWaitEv
int daObjMarioCap_c::InitTouchWait()
{
    return 1;
}

/* df04's per-frame function (type 2): the touch check, unless an override
 * matrix is set. */
// @symbol _ZN15daObjMarioCap_c9TouchWaitEv
int daObjMarioCap_c::TouchWait()
{
    if (CAP_OVERRIDE_MATRIX(this) == 0)
        CheckTouch();
    return 1;
}

/* df24's enter function (type 3): 200-frame timer. */
// @symbol _ZN15daObjMarioCap_c14InitTimedWait2Ev
int daObjMarioCap_c::InitTimedWait2()
{
    mStateTimer = 200;
    return 1;
}

/* df24's per-frame function (type 3): the same removal rules as
 * TimedWait. Returns 1. */
// @symbol _ZN15daObjMarioCap_c10TimedWait2Ev
int daObjMarioCap_c::TimedWait2()
{
    if (data_02092138 > mPosY
        || mWithMeshClsn.IsOnGround()
        || !DecIfAbove0_Short((unsigned short*)&mStateTimer)
        || (mFlags & 8))
    {
        MarkForDestruction();
        return 1;
    }
    return 1;
}

/* df14's enter function (type 12): switches gravity off. */
// @symbol _ZN15daObjMarioCap_c8InitFallEv
int daObjMarioCap_c::InitFall()
{
    mVertAccel = 0;
    return 1;
}

/* df14's per-frame function (type 12). While mVertAccel is nonzero (enter
 * zeroes it, so not from this file's states): moves and collides, sets the
 * 0x8000 vulnFlags bit, and on landing stops its horizontal speed, runs the
 * touch check and widens the clip ranges. Below the kill height it marks the
 * cap lost for the player (SaveData::PlayerLoseCap) and removes it. */
// @symbol _ZN15daObjMarioCap_c4FallEv
int daObjMarioCap_c::Fall() {
  if (mVertAccel != 0) {
    UpdatePos(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    mdCcAc_c.vulnFlags |= 0x8000;
    if (mWithMeshClsn.IsOnGround()) {
      mHorzSpeed = 0;
      CheckTouch();
      _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0x32000, 0x32000, kClipFar, kClipFar);
    }
  }
  if (data_02092138 > mPosY) {
    SaveData::PlayerLoseCap();
    MarkForDestruction();
  }
  return 1;
}

/* df34's enter function (types 4..9, 11, 16..18). Loads the countdown
 * for the types that use one -- types 5, 7 and 9: 210 frames in game mode 1
 * (CURRENT_GAMEMODE, the mode dMeter_c draws its VS icons for), 120 frames in
 * other modes; type 18: 300 frames; type 11: 180 frames -- and keeps a copy in
 * mStartTimer for the blink. Then sets the 0x8000 vulnFlags bit for every type.
 * Returns 1. */
// @symbol _ZN15daObjMarioCap_c9InitSlideEv
int daObjMarioCap_c::InitSlide()
{
    int state;
    int* p;
    int val;
    int ret;

    state = mType;
    if (state == 5 || state == 7 || state == 9) {
        CAP_TIMER(this) = 0xd2;
        if ((int)(data_0209f2d8[0] == 1) == 0) CAP_TIMER(this) = 0x78;
        mStartTimer = CAP_TIMER(this);
    }

    state = mType;
    if (state == 0x12) {
        CAP_TIMER(this) = 0x12c;
        mStartTimer = CAP_TIMER(this);
    }

    state = mType;
    if (state == 0xb) {
        CAP_TIMER(this) = 0xb4;
        mStartTimer = CAP_TIMER(this);
    }

    /* (unsigned long long) is the MATCH form; mdCcAc_c.vulnFlags |= DIFFs. */
    p = (int *)((unsigned long long)&mdCcAc_c.vulnFlags);
    val = *p;
    ret = 1;
    val |= 0x8000;
    *p = val;
    return ret;
}

/* df34's per-frame function. Types 4, 6, 8, 16 and 17 just run the touch
 * check. The others count mStateTimer down; nothing more happens once it
 * reads 0, and when the decrement leaves it at 1 the cap is removed unless
 * mFlags has a Yoshi-mouth bit (0x20000 or 0x40000) set. While airborne the
 * cap only saves mHorzSpeed in unk_408. On the ground it runs the touch check
 * and drives the blink (mBlinkHidden) from the low bits of the timer: bit 2
 * while the timer is under half of mStartTimer, bit 1 once it is under a
 * quarter. Type 18 then stops (speed 0). The rest slide: the floor surface
 * (func_02037e58 on the floor result's surface info, which sits 4 bytes into the result) picks a push of
 * 0x5000..0xa000 (5..10 units a frame; func_ov002_020f02c8, which maps the
 * surface value to it), aimed along mSlopeAngle and scaled by the horizontal
 * length of the floor normal. That is added to the cap's own velocity
 * (mHorzSpeed along mPrevAngleY) and the sum becomes the new heading and
 * speed, capped at 0xf000 (15 units a frame). mVertSpeed is set from the floor
 * normal and the words at 0x0a4 / 0x0ac, with 0x8000 (8 units a frame) of
 * extra downward bias, and mAngleX / mAngleZ move toward the floor's tilt
 * along and across mAngleY (UpdateAngle with arguments 4 and 0x1000). Returns 1. */
// @symbol _ZN15daObjMarioCap_c5SlideEv
int daObjMarioCap_c::Slide()
{
    struct Vector3 ownVel;
    struct Vector3 slopeVel;
    struct Vector3 out;
    int st;
    char *fr;
    int surface;
    int slopePush;
    int spd;
    short ang;
    short pitch;
    short roll;
    int b;
    int j;
    int s;
    int co;

    st = mType;
    if (st == 4 || st == 0x11 || st == 6 || st == 8 || st == 0x10) {
        CheckTouch();
        return 1;
    }
    if (DecIfAbove0_Short((unsigned short *)&mStateTimer) == 0)
        return 1;
    if (CAP_TIMER(this) == 1) {
        b = (int)((mFlags & 0x60000) != 0);
        if (b == 0) {
            MarkForDestruction();
            return 1;
        }
    }
    if (mWithMeshClsn.IsOnGround() == 0) {
        unk_408 = mHorzSpeed;
        return 1;
    }
    CheckTouch();
    if (CAP_TIMER(this) < mStartTimer >> 1) {
        mBlinkHidden = (CAP_TIMER(this) & 4) >> 2;
        if (CAP_TIMER(this) < mStartTimer >> 2)
            mBlinkHidden = (CAP_TIMER(this) & 2) >> 1;
    }
    if (mType == 0x12) {
        mHorzSpeed = 0;
        return 1;
    }
    fr = _ZNK10dBgCh_Actr14GetFloorResultEv(&mWithMeshClsn);
    ((SurfaceInfo *)(fr + 4))->CopyNormalTo(*(Vector3 *)&mFloorNormalX);
    surface = func_02037e58(fr + 4);
    mSlopeAngle = _ZN4cstd5atan2E5Fix12IiES1_(mFloorNormalX, mFloorNormalZ);
    slopePush = func_ov002_020f02c8(surface);
    func_ov002_020f030c(surface);
    spd = mHorzSpeed;
    j = (CAP_ANGLE(mPrevAngleY) >> 4) * 2;
    s = data_02082214[j];
    co = data_02082214[j + 1];
    /* Velocity in the plane: speed times {sin, cos} of the angle, fix12 multiply
       rounded to nearest. (long long)(int) is the MATCH form; a plain 32-bit
       mul DIFFs. */
    ownVel.x = (int)(((long long)spd * s + 0x800) >> 12);
    ownVel.y = 0;
    ownVel.z = (int)(((long long)spd * co + 0x800) >> 12);
    j = (CAP_ANGLE(mSlopeAngle) >> 4) * 2;
    s = data_02082214[j];
    co = data_02082214[j + 1];
    slopeVel.x = (int)(((long long)slopePush * s + 0x800) >> 12);
    slopeVel.y = 0;
    slopeVel.z = (int)(((long long)slopePush * co + 0x800) >> 12);
    Vec3_MulScalarInPlace(&slopeVel, Vec3_HorzLen(&mFloorNormalX));
    Vec3_Add(&out, &ownVel, &slopeVel);
    ang = _ZN4cstd5atan2E5Fix12IiES1_(out.x, out.z);
    mHorzSpeed = Vec3_HorzLen(&out);
    if (mHorzSpeed > 0xf000)
        mHorzSpeed = 0xf000;
    mPrevAngleY = ang;
    mVertSpeed = -(_ZN4cstd4fdivEii(
        (int)(((long long)mFloorNormalX * unk_0a4 + 0x800) >> 12)
      + (int)(((long long)mFloorNormalZ * unk_0ac + 0x800) >> 12),
        mFloorNormalY) + 0x8000);
    pitch = func_02010844(this, &mFloorNormalX, mAngleY);
    roll = func_02010844(this, &mFloorNormalX, mAngleY - 0x4000);
    _Z11UpdateAngleRssis(&mAngleX, pitch, 4, 0x1000);
    _Z11UpdateAngleRssis(&mAngleZ, roll, 4, 0x1000);
    return 1;
}

/* df54's enter function: the taken state. Zeroes the velocity words and
 * mVertAccel, clears mTakenStep, mBlinkHidden and the override matrix, sets
 * unk_403, drops the Yoshi-mouth bit 0x40000 from mFlags and the 0x8000 bit
 * from vulnFlags, and binds the nearest player if none is bound yet. In
 * modes other than 1 (CURRENT_GAMEMODE), when that player's mCharacter is 3
 * and mModelIndex equals the player's param1, plays the 1-up sound, gives one
 * life and spawns the 1-up logo actor 100 units above the cap. Finally
 * starts the 150-frame removal timer and resets the scale to 1.0. Returns 1. */
// @symbol _ZN15daObjMarioCap_c9InitTakenEv
int daObjMarioCap_c::InitTaken()
{
    unk_0a4 = 0;
    mVertSpeed = 0;
    unk_0ac = 0;
    mVertAccel = 0;
    mTakenStep = 0;
    mBlinkHidden = 0;
    CAP_OVERRIDE_MATRIX(this) = 0;
    unk_403 = 1;

    /* (int)self + off is the MATCH form; mFlags &= / vulnFlags &= CSE. */
    *(u32 *)(((int)&mFlags)) &= ~0x40000u;
    *(u32 *)(((int)&mdCcAc_c.vulnFlags)) &= ~0x8000u;

    if (mPlayer == 0) {
        mPlayer = ClosestPlayer();
    }

    {
        unsigned b = (data_0209f2d8[0] == 1);
        if (b == 0) {
            Player *p = mPlayer;
            if (p->mCharacter == 3) {
                if (mModelIndex == (int)p->param1) {
                    struct Vector3 v;
                    Sound::PlayBank3(kSfx1Up, *(Vector3 *)&mCamSpacePosX);
                    GiveLives(1);
                    v.x = mPosX;
                    v.y = mPosY;
                    v.z = mPosZ;
                    v.y += 0x64000;
                    dActor_c::Spawn(kActor1UpLogo, 8, v, (Vector3_16 *)0, mAreaId, -1);
                }
            }
        }
    }

    mStateTimer = 0x96;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return 1;
}

/* df54's per-frame function: the taken state. Removes the cap when the
 * timer set by the enter function runs out; binds the nearest player if there
 * is none yet. Otherwise it copies the player's angles and position every
 * frame. mTakenStep 1: once Player::SetNoControlState(0xf) succeeds, hands
 * the hat over (SetNewHatCharacter), plays the animation data_ov002_0210de38
 * and moves to step 2; step 2: removes the cap when that animation has
 * finished. With mTakenStep 0: plays data_ov002_0210de30 when
 * Player::Unk_020c9e5c(8) says 1, and when it has finished removes the cap,
 * first spawning a new OBJ_MARIO_CAP at mHomePos if mType is 4. Returns 1. */
// @symbol _ZN15daObjMarioCap_c5TakenEv
int daObjMarioCap_c::Taken() {
    if (DecIfAbove0_Short((unsigned short *)&mStateTimer) == 0) {
        MarkForDestruction();
        return 1;
    }

    if (mPlayer == 0) {
        mPlayer = ClosestPlayer();
        return 1;
    }

    CAP_OVERRIDE_MATRIX(this) = 0;

    {
        s16 *q = &mPlayer->mAngleX;
        mPrevAngleX = q[0];
        mPrevAngleY = q[1];
        mPrevAngleZ = q[2];
    }
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;

    {
        s32 *q = &mPlayer->mPosX;
        mPosX = q[0];
        mPosY = q[1];
        mPosZ = q[2];
    }

    switch (mTakenStep) {
    case 1:
        if (mPlayer->SetNoControlState(0xf, -1, 0) == 1) {
            mPlayer->SetNewHatCharacter(mModelIndex & 0xff, 0, 0);
            func_02013a88();
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov002_0210de38.file, 0x40000000, 0x1000, 0);
            mTakenStep = 2;
        }
        break;
    case 2:
        if (mModelAnim.Finished() != 0) {
            MarkForDestruction();
        }
        break;
    }

    if (mTakenStep != 0) {
        return 1;
    }

    if (mAnimStarted == 0) {
        if (mPlayer->Unk_020c9e5c(8) == 1) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov002_0210de30.file, 0x40000000, 0x1000, 0);
            mAnimStarted = 1;
        }
    }

    if (mAnimStarted == 1) {
        if (mModelAnim.Finished() != 0) {
            if (mType == 4) {
                dActor_c::Spawn(kActorMarioCap, param1, *(const Vector3 *)&mHomePosX, (const Vector3_16 *)0, mAreaId, -1);
            }
            MarkForDestruction();
        }
    }

    return 1;
}

/* df74's enter function: the animation state. Clears mBlinkHidden and
 * mAnimStarted. Types 10 and 15 bind the nearest player (if any) and take
 * over its angles. Then starts the animation for the type: 10, 15, 20, 21 or
 * 22 each play their own (the animation tables are unrecovered; 15 and 20
 * index them by mModelIndex). Returns 1. */
// @symbol _ZN15daObjMarioCap_c8InitAnimEv
int daObjMarioCap_c::InitAnim()
{
    /* Reading the angles through closest->mAngleX / &mModelAnim methods
       changes the size; the pointer-to-first-member form below is the one
       that matches. */
    int state;

    mBlinkHidden = 0;
    mAnimStarted = 0;

    state = mType;
    if (state == 0xa || state == 0xf) {
        Player* closest = ClosestPlayer();
        if (closest != 0) {
            s16* src = &closest->mAngleX;
            mPrevAngleX = src[0];
            mPrevAngleY = src[1];
            mPrevAngleZ = src[2];
            mAngleX = mPrevAngleX;
            mAngleY = mPrevAngleY;
            mAngleZ = mPrevAngleZ;
            mPlayer = closest;
        }
    }

    switch (mType) {
    case 0xa:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &mModelAnim, data_ov002_0210de30.file, 0x40000000, 0x1000, 0);
        break;
    case 0xf:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &mModelAnim, data_ov002_020ff0a0[mModelIndex]->file, 0x40000000, 0x1000, 0);
        break;
    case 0x14:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &mModelAnim, data_ov002_020ff0b8[mModelIndex]->file, 0x40000000, 0x1000, 0);
        break;
    case 0x15:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &mModelAnim, data_ov002_0210de58.file, 0x40000000, 0x1000, 0);
        break;
    case 0x16:
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
            &mModelAnim, data_ov002_0210de18.file, 0x40000000, 0x1000, 0);
        break;
    }

    return 1;
}

/* df74's per-frame function. Types 10 and 15 copy the bound player's angles
 * and position onto the cap every frame. Once the animation has finished,
 * types 10, 15 and 22 remove the cap; type 21 does nothing more; type 20
 * starts its second animation (data_ov002_020ff0c4 entry [1]) once, setting
 * mAnimStarted. Returns 1. */
// @symbol _ZN15daObjMarioCap_c4AnimEv
int daObjMarioCap_c::Anim()
{
    short* sp;
    int* ip;

    if (mType == 0xa || mType == 0xf) {
        if (mPlayer != 0) {
            sp = &mPlayer->mAngleX;
            mPrevAngleX = sp[0];
            mPrevAngleY = sp[1];
            mPrevAngleZ = sp[2];
            mAngleX = mPrevAngleX;
            mAngleY = mPrevAngleY;
            mAngleZ = mPrevAngleZ;

            ip = &mPlayer->mPosX;
            mPosX = ip[0];
            mPosY = ip[1];
            mPosZ = ip[2];
        }
    }

    if (mModelAnim.Finished() != 0) {
        switch (mType) {
        case 0xa:
        case 0xf:
        case 0x16:
            MarkForDestruction();
            break;

        case 0x15:
            break;

        case 0x14:
            if (mAnimStarted == 0) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
                    &mModelAnim,
                    (void*)data_ov002_020ff0c4[mModelIndex][1],
                    0, 0x1000, 0);
                mAnimStarted = 1;
            }
            break;
        }
    }

    return 1;
}

/* df44's enter function: clears mBlinkHidden. Returns 1. */
// @symbol _ZN15daObjMarioCap_c11InitDormantEv
int daObjMarioCap_c::InitDormant()
{
    mBlinkHidden = 0;
    return 1;
}

/* df44's per-frame function: does nothing. Returns 1. */
// @symbol _ZN15daObjMarioCap_c7DormantEv
int daObjMarioCap_c::Dormant()
{
    return 1;
}

/* The touch check shared by the states that let the player pick the cap up.
 * If the cap is against a wall, reflects mPrevAngleY off the wall normal.
 * Then it proceeds only if the collider's other owner is a Player (actor ID
 * kActorPlayer), hitFlags bit 0x8000 is clear, the player is not collecting
 * a cap, and it is not a player with param1 3 that has an object in its
 * mouth. Types 6/7 then turn that player into vanish-Luigi and types 8/9
 * into metal-Wario, removing the cap; with an override matrix set, any other
 * nonzero type is ignored; if the save says the cap was lost, the cap goes
 * to the taken state with mTakenStep 1; otherwise, once
 * Player::SetNoControlState(8) succeeds, it hands over the hat
 * (SetNewHatCharacter(mModelIndex)), copies the player's angles and enters
 * the taken state. */
// @symbol _ZN15daObjMarioCap_c10CheckTouchEv
void daObjMarioCap_c::CheckTouch()
{
    struct V16 { u16 x, y, z; } v;
    int normal[3];
    int state;

    if (mWithMeshClsn.IsOnWall() != 0) {
        void* wr = _ZNK10dBgCh_Actr13GetWallResultEv(&mWithMeshClsn);
        ((SurfaceInfo *)((char*)wr + 4))->CopyNormalTo(*(Vector3 *)&normal[0]);
        mPrevAngleY = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
            this, normal[0], normal[2], mPrevAngleY);
    }

    if (mdCcAc_c.otherOwner == 0) return;

    mPlayer = (Player *)dActor_c::FindWithID(mdCcAc_c.otherOwner);
    if (mPlayer == 0) return;

    {
        int t = (mPlayer->actorID == kActorPlayer);
        if (t == false) return;
    }

    if ((mdCcAc_c.hitFlags & 0x8000) != 0) return;
    if (mPlayer->IsCollectingCap() != 0) return;

    if (mPlayer->param1 == 3) {
        if (mPlayer->mObjInMouth != 0) return;
    }

    state = mType;
    if ((unsigned)(state - 6) <= 1) {
        mPlayer->InitVanishLuigi();
        MarkForDestruction();
        return;
    }
    if ((unsigned)(state - 8) <= 1) {
        mPlayer->InitMetalWario();
        MarkForDestruction();
        return;
    }

    if (CAP_OVERRIDE_MATRIX(this) != 0) {
        if (state != 0) return;
    }

    if (SaveData::HasPlayerLostCap() != 0) {
        EnterState(&data_ov002_0210df54);
        mTakenStep = 1;
        return;
    }

    if (mPlayer->SetNoControlState(8, -1, 0) == 0) return;

    mPlayer->SetNewHatCharacter(
        mModelIndex & 0xff, 0, 0);

    {
        /* `a = b ? a : a` through a V16 is the MATCH form. */
        Player* found = mPlayer;
        int a = *(u16*)&found->mAngleX;
        int b = *(u16*)&found->mAngleY;
        a = b ? a : a;
        v.x = a;
        v.y = b;
        v.z = *(u16*)&found->mAngleZ;
        mPrevAngleX = *(s16*)&v.x;
        mPrevAngleY = *(s16*)&v.y;
        mPrevAngleZ = *(s16*)&v.z;
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
    }

    EnterState(&data_ov002_0210df54);
}
