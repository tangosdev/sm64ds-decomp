//cpp
/* Reconstructed translation unit.
 * ov085/daMip_c  (32 functions)
 *
 * MIP the rabbit. The class identity is the cartridge's own: ov085 0x021300bc
 * holds the length-prefixed string "7daMip_c", 0x021300c8 is the
 * __si_class_type_info record that names it, and 0x021300f8 is its 31-slot
 * vtable. The tree called this class `Rabbit`; that name is coined, appears
 * nowhere in the image, and is retired by this change.
 *
 * FUNCTION ORDER IS ROM-ASCENDING. This TU disables deferred code generation so
 * CodeWarrior emits each definition where it stands. That is also what puts the
 * destructor pair out in the cartridge's own order: ov085 has D1 at 0x0212a6d4
 * BELOW D0 at 0x0212a724 and no D2 at all, and an out-of-line destructor under
 * DEFERRED codegen comes out D2, D0, D1.
 *
 * Assembled from these legacy one-function sources (ROM address order).  They lived
 * directly under src/ and no longer exist; the manifest's per-function
 * legacy_source rows keep their full paths:
 *   [0]  0x0212a6d4  _ZN6RabbitD1Ev.cpp
 *   [1]  0x0212a724  _ZN6RabbitD0Ev.cpp
 *   [2]  0x0212a788  func_ov085_0212a788.c
 *   [3]  0x0212a828  func_ov085_0212a828.cpp
 *   [4]  0x0212a904  func_ov085_0212a904.c
 *   [5]  0x0212aaa4  func_ov085_0212aaa4.c
 *   [6]  0x0212aaec  func_ov085_0212aaec.c
 *   [7]  0x0212ac3c  func_ov085_0212ac3c.c
 *   [8]  0x0212ac4c  func_ov085_0212ac4c.c
 *   [9]  0x0212ad8c  func_ov085_0212ad8c.c
 *   [10] 0x0212ae08  func_ov085_0212ae08.c
 *   [11] 0x0212b3fc  func_ov085_0212b3fc.c
 *   [12] 0x0212b444  func_ov085_0212b444.c
 *   [13] 0x0212b478  func_ov085_0212b478.c
 *   [14] 0x0212b4b4  func_ov085_0212b4b4.c
 *   [15] 0x0212b75c  func_ov085_0212b75c.cpp
 *   [16] 0x0212b86c  func_ov085_0212b86c.c
 *   [17] 0x0212b8a0  func_ov085_0212b8a0.c
 *   [18] 0x0212b8dc  func_ov085_0212b8dc.cpp
 *   [19] 0x0212bc14  func_ov085_0212bc14.c
 *   [20] 0x0212bc78  func_ov085_0212bc78.cpp
 *   [21] 0x0212bcc8  func_ov085_0212bcc8.c
 *   [22] 0x0212bdbc  func_ov085_0212bdbc.cpp
 *   [23] 0x0212bedc  func_ov085_0212bedc.cpp
 *   [24] 0x0212c004  _ZN6Rabbit16CleanupResourcesEv.cpp
 *   [25] 0x0212c070  _ZN6Rabbit16OnPendingDestroyEv.cpp
 *   [26] 0x0212c074  _ZN6Rabbit6RenderEv.cpp
 *   [27] 0x0212c150  func_ov085_0212c150.cpp
 *   [28] 0x0212c230  _ZN6Rabbit8BehaviorEv.cpp
 *   [29] 0x0212c7fc  _ZN6Rabbit13InitResourcesEv.cpp
 *   [30] 0x0212cc18  _ZN6Rabbit13OnYoshiTryEatEv.cpp
 *   [31] 0x0212cc2c  d_a_mip.c
 *
 * The last of those, d_a_mip.c, is absent from build/tu_map.json's span:
 * tu_map segments on symbol NAME, and `daMip_c_classInit` is neither
 * `func_ov085_*` nor `_ZN7daMip_c*`, so nothing labels it. It is contiguous --
 * 0x0212cc2c + 0x5c = 0x0212cc88, the next class's destructor -- and it is this
 * class's own factory, so it belongs here.
 *
 * SHADOW TYPES AND EXTERNAL DATA STAY INSIDE THE MEMBER THAT RECOVERED THEM.
 * Each legacy file recovered its own view of the ov085 statics: 0x021305c0
 * alone was spelled `int[]`, `struct G { int w[2]; }`, an incomplete `struct S`,
 * `void *[]` and plain `char`, and the four matrix-touching members were built
 * against common.h's FLAT `{ s32 m[12] }` Matrix4x3 rather than math/Matrix.h's
 * `{ Matrix3x3 r; Vector3 t; }` one, which this TU now reaches through
 * ModelAnim.h -- and that spelling is non-POD, because Vector3 declares a
 * destructor. Canonicalising either is a measured codegen hazard, not a
 * tidy-up. mwccarm leaves a file-scope variable's name unmangled in C++, so a
 * block-scope `extern` inside a member still names the ROM symbol.
 *
 * FUNCTIONS cannot be handled that way -- a class member function may not sit
 * inside a linkage-specification region -- so every external call this TU makes
 * is declared once, below, with C linkage, on one reconciled signature.
 *
 * deslop
 * Leftover: SetAnim / dCcAc_c::Init / dBgCh_Actr::Init / DropShadowRadHeight
 *   stay mangled (Fix12-by-value, 6az; dBgCh Init header Fix12i mangles as
 *   int). Player+8 param1 / +0x6d9 / +0x6ce belong on Player. data_ov085_*
 *   handles. S14 no g_profile_MIP. common.h first (shadow matrix copy).
 */
#include "common.h"
#include "daMip_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "PathPtr.h"
#include "decl_Message.h"
#include "decl_PathPtr.h"

extern "C" {

/* ground probe / collision */
void  _ZN9dBgCh_GndC1Ev(void *);
void  _ZN9dBgCh_GndD1Ev(void *);
void  _ZN5dBgCh19StartDetectingWaterEv(void *);
void  _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *, const void *, void *);
int   _ZN9dBgCh_Gnd10DetectClsnEv(void *);
int   SurfaceInfo_TestFlag0x20(void *);
int   _ZNK10dBgCh_Actr8IsOnWallEv(void *);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *, void *, s32, s32, void *, void *);
void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, void *, s32, s32, u32, u32);
void  _ZN5dCc_c5ClearEv(void *);
void  _ZN5dCc_c6UpdateEv(void *);

/* dActor_c / dEnemyBase_c, reached by their ROM names.
   dActor_c::ClosestPlayer and dActor_c::FindWithID are deliberately NOT here:
   both are declared members whose recovered signatures carry no by-value class
   parameter, so this TU calls them as members and lets the compiler produce the
   symbol. Everything else keeps the mangled spelling its legacy file matched
   under, because DropShadowRadHeight and Spawn take Fix12<int> and s8/s16 BY
   VALUE in their real declarations and mwccarm passes those differently at the
   call site. */
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32, u32, const void *, const void *, int, int);
void  _ZN8dActor_c9UpdatePosEP5dCc_c(void *, void *);
void *_ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(void *, void *, void *);
void  _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *, void *, void *, int, int, u32);
int   _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void *, void *);
void  _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void *, void *, u32);

/* Player / Message / Sound / SaveData */
int   _ZN6Player7TryGrabER8dActor_c(void *, void *);
int   _ZN6Player9StartTalkER7fBase_cb(void *, void *, int);
int   _ZN6Player12GetTalkStateEv(void *);
void  _ZN6Player9DropActorEv(void *);
int   _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *, void *, u32, const void *, u32, u32);
int   _ZN6Player14IsFrontSlidingEv(void *);
int   _ZN6Player17LostGrabbedObjectEv(void *);
void  _ZN7Message7EndTalkEv(void);
/* The return type is load-bearing: StateTalkMain tests it in an `if`, so the
   `void` spelling three of the four legacy files used cannot be the one. */
int   _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32, u32, u32, s32, int);
int   _ZN8SaveData22NumGlowingRabbitsFoundEv(void);

/* model / animation / shadow */
void  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, s32, u32);
void  _ZN9ModelAnim6RenderEPK7Vector3(void *, void *);
void  _ZN9ModelAnimC1Ev(void *);
int   _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
void  _ZN9ModelBase12ApplyOpacityEj(void *, u32, u32);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
void  _ZN9Animation8LoadFileER13SharedFilePtr(void *);
int   _ZN9Animation8FinishedEv(void *);
void  _ZN9Animation7AdvanceEv(void *);
int   _ZN11ShadowModel12InitCylinderEv(void *);
void  _ZN11ShadowModelC1Ev(void *);

/* path, particle, construction */
void  _ZN7PathPtrC1Ev(void *);
void  _ZN7PathPtr6FromIDEj(void *, u32);
void  _ZNK7PathPtr7GetNodeER7Vector3j(void *, void *, u32);
void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32, u32, s32, s32, s32, void *, void *);
void *_ZN7fBase_cnwEj(u32);
void  _ZN12dEnemyBase_cC2Ev(void *);
void  _ZN7dCcAc_cC1Ev(void *);
void  _ZN10dBgCh_ActrC1Ev(void *);

/* maths and the still-unnamed helpers */
s16   Vec3_HorzAngle(const void *, const void *);
s32   Vec3_HorzDist(const void *, const void *);
s32   Vec3_Dist(const void *, const void *);
void  Vec3_Sub(void *, const void *, const void *);
void  Vec3_MulScalar(void *, const void *, int);
void  Vec3_Asr(void *, void *, int);
void  SubVec3(void *, void *, void *);
s32   LenVec3(const void *);
int   AngleDiff(int, int);
void  ApproachAngle(void *, s16, int, int, int);
/* Same as Sound::PlaySub: StateTalkMain guards on the result. */
int   _Z14ApproachLinearRsss(s16 *, s16, s16);
int   _ZN4cstd4fdivEii(int, int);
void  Matrix4x3_FromTranslation(void *, int, int, int);
void  Matrix4x3_FromRotationY(void *, int);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void *, int, int, int);
void  MulMat4x3Mat4x3(void *, void *, void *);
void  MulMat3x3Mat3x3(void *, void *, void *);
void  MulVec3Mat4x3(const void *, const void *, void *);
/* Unsigned, and that is measured: InitResources shifts the result right by 8
   and the ROM uses LSR. Declared `int`, the same expression comes out ASR. */
u32   RandomIntInternal(int *);
u8    NumStars(void);
u16   DecIfAbove0_Short(void *);
int   func_02013890(int, int);
int   func_02013944(void);
void  func_02012694(int, void *);
void  func_02012790(int);
void  func_02016acc(void *, int);
void  func_02016b24(void *, int);
void  func_02022a4c(s32, s32, s32);
u32   func_02022cbc(int, int, s32, s32, s32, const void *);
void  func_0203c178(void *, int, int, int);

}

/* The state records at 0x0213003c..0x021300bc are 8-byte Itanium
   pointer-to-member-function constants, so SetState reaches one through a real
   PMF. The self type is a shadow rather than daMip_c because the record's
   pointer sits at 0x364 -- mState, which the header keeps as an opaque s32
   precisely because Behavior compares it by ADDRESS against four ov085 objects
   rather than dereferencing it as a type. */
struct daMip_cSelf;
typedef int (daMip_cSelf::*daMip_cStateFn)();
struct daMip_cSelf { char pad[0x364]; daMip_cStateFn *pp; };

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN7daMip_cD1Ev 0x0212a6d4 (0x50)                   */
/*                         _ZN7daMip_cD0Ev 0x0212a724 (0x64)                   */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_cD1Ev
// @symbol _ZN7daMip_cD0Ev
/* recovered: real C++ destructor -- the compiler emits both whole bodies.
 *
 * One vptr store and five member destructor calls, every one a consequence of
 * `struct daMip_c : dEnemyBase_c` and the members that declaration types:
 * ShadowModel (0x3c0), ShadowModel (0x368), ModelAnim (0x300), dBgCh_Actr
 * (0x144), dCcAc_c (0x110), in reverse declaration order, then
 * dEnemyBase_c's own D2 -- which is what a real derived destructor calls.
 *
 * D0, the deleting variant, adds an operator delete that nothing here spells:
 * it is dEnemyBase_c's, inlined because dEnemyBase_c is the immediate base.
 *
 * The cartridge has no D2 for this class. The one this TU emits under
 * `defer_codegen off` is compiler-only output with no home, and is licensed as
 * a plain deadstrip in the manifest. */
daMip_c::~daMip_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- daMip_c::TestWaterBelow, 0x0212a788, size 0xa0             */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c14TestWaterBelowEv
/* Probes the ground 0xc8000 above the rabbit with a water-detecting dBgCh_Gnd,
   caches the surface height at +0x464 and answers whether the surface carries
   flag 0x20. Both callers -- StateFleeMain and StateIdleMain -- use the answer
   to pick the splash effect over the dust one, so this is "am I over water".

   The name is coined; member-ness is NOT proven. The ROM takes the object in r0
   and addresses it this-relatively throughout, which a file-local function
   taking a daMip_c * would do identically. */
int daMip_c::TestWaterBelow()
{
    typedef struct Vector3 { int x, y, z; } Vector3;
    struct RG { char a[0x14]; int detect[16]; };
    char *c = (char *)this;

    struct RG rg;
    Vector3 v;
    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN5dBgCh19StartDetectingWaterEv(&rg);
    int x = *(int *)(c + 0x5c);
    int y = *(int *)(c + 0x60);
    int z = *(int *)(c + 0x64);
    int yk = y + 0xc8000;
    v.x = x;
    v.y = yk;
    v.z = z;
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, c);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg)) {
        *(int *)(c + 0x464) = rg.detect[12];
        if (SurfaceInfo_TestFlag0x20(rg.detect)) {
            _ZN9dBgCh_GndD1Ev(&rg);
            return 1;
        }
    }
    _ZN9dBgCh_GndD1Ev(&rg);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- daMip_c::UpdateGrab, 0x0212a828, size 0xdc                 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c10UpdateGrabEv
/* The pick-up handshake. +0x134 is the id of the actor currently touching this
   one; if it resolves to a Player (actor type 0xbf), the touch carries the grab
   flag 0x1000 and Player::TryGrab agrees, the rabbit records its carrier at
   +0x45c and enters Caught -- or Released, if it has already been caught once
   (+0x426). Name coined; member-ness not proven. */
void daMip_c::UpdateGrab()
{
    extern int data_ov085_021306ac[];
    extern int data_ov085_021306bc[];
    char *c = (char *)this;

    unsigned int id = *(unsigned int *)(c + 0x134);
    if (id == 0) return;
    char *o = (char *)dActor_c::FindWithID(id);
    if (o == 0) return;
    int b = (*(unsigned short *)(o + 0xc) == 0xbf);
    if (b == 0) return;
    if ((*(int *)(c + 0x130) & 0x1000) == 0) return;
    if (_ZN6Player7TryGrabER8dActor_c(o, c) == 0) return;
    *(void **)(c + 0x45c) = o;
    *(int *)(((int)c + 0x128)) |= 2;
    if (*(unsigned char *)(c + 0x426) == 0) {
        SetState(data_ov085_021306ac);
    } else {
        SetState(data_ov085_021306bc);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daMip_c::StateSaveTalkMain, 0x0212a904, size 0x1a0         */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c17StateSaveTalkMainEv
/* The eighth-rabbit epilogue, reached only from StateCaughtMain's flag path
   once SaveData says all eight glowing rabbits are found. The rabbit turns to
   the player it stored at +0x460, offers message 0x148, then drives
   Message::DisplaySaving before handing back to Released.

   Member-ness IS proven here and for every other State* below: each is the
   target of an 8-byte {fnptr, 0} pointer-to-member constant in ov085 .data, and
   such a record can point at nothing but a member of this class. Only the NAME
   is coined. */
int daMip_c::StateSaveTalkMain()
{
    /* A local POD triple, not types.h's Vector3: this member was recovered as C
       and never carried Vector3's destructor. */
    struct V3 { int x, y, z; };
    extern unsigned char data_0209d684;
    extern unsigned char data_0209d660;
    extern char data_ov085_021306bc[];
    char *self = (char *)this;

    char *r4 = *(char **)(self + 0x460);
    struct V3 vec;
    unsigned char gb;
    int state;

    *(short *)(self + 0x424) = Vec3_HorzAngle((struct V3 *)(self + 0x5c), (struct V3 *)(r4 + 0x5c));
    ApproachAngle(self + 0x94, *(short *)(self + 0x424), 1, 0x500, 0x500);

    gb = data_0209d684;
    vec.x = *(int *)(self + 0x5c);
    vec.y = *(int *)(self + 0x60);
    vec.z = *(int *)(self + 0x64);
    vec.y += 0x3c000;

    state = *(int *)(self + 0x41c);
    switch (state) {
    case 0:
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(r4, self, 0x148, &vec, 0, 0)) {
            func_02012790(0xa);
            {
                int *p = (int *)(((int)self + 0x41c));
                (*p)++;
            }
        }
        break;
    case 1:
        if (data_0209d660 == 0) {
            if (gb == 1) {
                func_02012790(0x5e);
                _ZN7Message13DisplaySavingEt(0x295);
                {
                    int *p = (int *)(((int)self + 0x41c));
                    (*p)++;
                }
            } else if (gb == 2) {
                func_02012790(0x98);
                {
                    unsigned short *hp = (unsigned short *)(((int)r4 + 0x6ce));
                    *hp &= ~0x800;
                }
                _ZN7Message7EndTalkEv();
                SetState(data_ov085_021306bc);
            }
        }
        break;
    case 2:
        if (data_0209d660 == 0) {
            unsigned short *hp = (unsigned short *)(((int)r4 + 0x6ce));
            *hp &= ~0x800;
            _ZN7Message7EndTalkEv();
            SetState(data_ov085_021306bc);
        }
        break;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- daMip_c::StateSaveTalkInit, 0x0212aaa4, size 0x48          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c17StateSaveTalkInitEv
int daMip_c::StateSaveTalkInit()
{
    extern int data_ov085_021305c0[];
    void *c = (void *)this;

    *(int *)((char *)c + 0x41c) = 0;
    func_02013944();
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0x300, (void *)data_ov085_021305c0[1], 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daMip_c::StateTalkMain, 0x0212aaec, size 0x150             */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13StateTalkMainEv
/* The ordinary conversation, entered from StateReleasedMain once
   Player::StartTalk agrees. Faces the player, picks the line from mRabbitId,
   and hands back to Released when the talk ends. */
int daMip_c::StateTalkMain()
{
    typedef short s16;
    struct V3 { int x, y, z; };
    extern int data_ov085_021306bc[];
    char *self = (char *)this;

    void *player;
    int *pq;
    struct V3 pos;
    struct V3 pp;
    s16 angle;
    unsigned int id;

    player = *(void **)(self + 0x460);
    pq = (int *)(((int)player + 0x5c));
    pos.x = *(int *)(self + 0x5c);
    pos.y = *(int *)(self + 0x60);
    pos.z = *(int *)(self + 0x64);
    pp.x = pq[0];
    pp.y = pq[1];
    pp.z = pq[2];
    angle = Vec3_HorzAngle((struct V3 *)(self + 0x5c), &pp);

    id = 0x139;
    switch (_ZN6Player12GetTalkStateEv(player)) {
    case 0:
        pos.y = pos.y + 0x46000;
        if (*(int *)(self + 0x43c) == 7) {
            id = 0x13d;
        }
        if (*(int *)(self + 0x43c) == 6) {
            id = 0x13a;
        }
        if (_Z14ApproachLinearRsss((s16 *)(self + 0x94), angle, 0x800)) {
            if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, 0)) {
                _ZN7Message11PrepareTalkEv();
                _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(player, self, id, &pos, 0, 0);
            }
        }
        break;
    case 1:
        break;
    default:
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x7f, 0, 0x7444, 0)) {
            _ZN7Message7EndTalkEv();
            SetState(data_ov085_021306bc);
        }
        break;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daMip_c::StateTalkInit, 0x0212ac3c, size 0x10              */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13StateTalkInitEv
int daMip_c::StateTalkInit()
{
    mActionStep = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daMip_c::StateReleasedMain, 0x0212ac4c, size 0x140         */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c17StateReleasedMainEv
/* Standing free again. Lets go of the carrier at +0x45c once the carry flags
   clear, refuses to act while the closest player is mid-message, and re-opens
   the conversation through Player::StartTalk when the toucher is a Player. */
int daMip_c::StateReleasedMain()
{
    extern int data_ov085_021306dc;
    char *c = (char *)this;

    unsigned short h;
    int ok;
    char *o;
    char *cp;
    int *p128;
    int b;
    int a;
    int flags;
    char *obj;

    obj = *(char **)(c + 0x45c);
    if (obj != 0) {
        flags = *(int *)(c + 0xb0);
        a = (flags & 0x400) ? 1 : 0;
        if (a == 0) {
            b = (flags & 0x2000) ? 1 : 0;
            if (b == 0)
                goto after_clear;
        }
        if (a != 0) {
            if (obj != 0) {
                *(short *)(c + 0x94) = *(short *)(obj + 0x8e);
            }
        }
        p128 = (int *)(c + 0x128);
        b = 0;
        *p128 = (*p128) & ~2;
        *(int *)(c + 0x45c) = b;
    after_clear:
        a = (*(int *)(c + 0xb0) & 0x100) ? 1 : 0;
        if (a == 0) {
            *(int *)(c + 0x45c) = 0;
        }
        obj = *(char **)(c + 0x45c);
        if (obj != 0) {
            if (*(unsigned char *)(obj + 0x706) != 0) {
                *(int *)(c + 0x45c) = 0;
            }
        }
    }

    cp = (char *)ClosestPlayer();
    if (cp != 0) {
        h = *(unsigned short *)(cp + 0x600 + 0xce);
        h = (unsigned short)(h & 0x800);
        if (h != 0)
            return 1;
    }

    if ((*(int *)(c + 0x130) & 0x8000000) != 0) {
        o = (char *)dActor_c::FindWithID(*(unsigned int *)(c + 0x134));
        if (o != 0) {
            ok = (int)(*(unsigned short *)(o + 0xc) == (unsigned short)0xbf);
            if (ok != 0) {
                *(char **)(c + 0x460) = o;
                o = *(char **)(c + 0x460);
                if (_ZN6Player9StartTalkER7fBase_cb(o, c, 0) != 0) {
                    SetState(&data_ov085_021306dc);
                }
            }
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- daMip_c::StateReleasedInit, 0x0212ad8c, size 0x7c          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c17StateReleasedInitEv
int daMip_c::StateReleasedInit()
{
    struct G { int w[2]; };
    extern struct G data_ov085_021305c0;
    char *c = (char *)this;

    int *a = (int *)(((int)c + 0x12c));
    int *b = (int *)(((int)c + 0x128));
    *(int *)(c + 0x9c) = -0x1000;
    *(unsigned char *)(c + 0x426) = 1;
    *a &= ~0x1000;
    *b |= 0x4000000;
    *(int *)(c + 0x114) = 0x78000;
    *a &= ~0x8000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void *)data_ov085_021305c0.w[1], 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- daMip_c::StateCaughtMain, 0x0212ae08, size 0x5f4          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c15StateCaughtMainEv
/* Held in the player's hands: the whole caught conversation, the star spawn
   (actor 0xe5) and the branch that ends with all eight glowing rabbits found. */
int daMip_c::StateCaughtMain()
{
    typedef int s32;
    typedef short s16;
    typedef unsigned int u32;
    typedef unsigned short u16;
    typedef unsigned char u8;
    typedef signed char s8;
    typedef struct { s32 x, y, z; } Vector3;

    extern char data_ov085_021306cc[];
    extern char data_ov085_021306bc[];
    extern char data_ov085_0213068c[];
    extern u8 data_0209d660;
    extern u8 data_0209d6bc;
    char *c = (char *)this;

    char *pl;
    Vector3 pv;
    Vector3 pos;   /* ShowMessage */
    Vector3 pos7;  /* 43c==7 star spawn */
    Vector3 posR;  /* rabbit star spawn */
    int r4;
    int msg;

    pl = *(char **)(c + 0x45c);
    if (pl == 0) {
        SetState(data_ov085_021306cc);
        return 1;
    }

    if (*(s32 *)(c + 0x41c) == 0) {
        {
            int *ps = (int *)(pl + 0x5c);
            pv.x = ps[0];
            pv.y = ps[1];
            pv.z = ps[2];
        }

        if (*(u8 *)(c + 0x426) != 0) {
            s16 ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &pv);
            _Z14ApproachLinearRsss((s16 *)(c + 0x94), ang, 0x800);
            if (AngleDiff(*(s16 *)(c + 0x94), ang) > 0x200)
                return 1;
        }

        {
            int guard = (*(s32 *)(c + 0xb0) & 0x4000) ? 1 : 0;
            if (guard == 0) {
                if (*(u8 *)(c + 0x426) != 2)
                    goto after_first_section;
            }
            {
                if (_ZN6Player9StartTalkER7fBase_cb(pl, c, 1) != 0) {
                    pos.x = *(s32 *)(c + 0x5c);
                    r4 = 0;
                    pos.y = *(s32 *)(c + 0x60);
                    pos.z = *(s32 *)(c + 0x64);

                    if (*(s32 *)(c + 0x43c) == 7)
                        goto msg_13c;
                    if (func_02013890(*(s32 *)(c + 0x43c), *(s32 *)(pl + 8)) == 0) {
                        if (*(s32 *)(pl + 8) != 3) {
                            _ZN7Message11PrepareTalkEv();
                            {
                                int z = r4;
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, z);
                            }
                            if (*(s32 *)(c + 0x43c) == 6)
                                goto msg_123a;
                            msg = (s16)(*(s32 *)(pl + 8) + 0x11b);
                            r4 = 0x163;
                            goto have_msg;
                        msg_123a:
                            msg = (s16)(*(s32 *)(pl + 8) + 0x123);
                            r4 = 0x161;
                            goto have_msg;
                        }
                        /* simple msgs for character id 3 */
                        if (*(s32 *)(c + 0x43c) != 6)
                            msg = 0x12b;
                        else
                            msg = 0x12c;
                        goto have_msg;
                    } else {
                        if (*(s32 *)(pl + 8) != 3) {
                            _ZN7Message11PrepareTalkEv();
                            if (*(u8 *)(c + 0x429) == 0) {
                                {
                                    int z = r4;
                                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, z);
                                }
                                if (*(s32 *)(c + 0x43c) == 6)
                                    goto msg_127a;
                                msg = (s16)(*(s32 *)(pl + 8) + 0x11f);
                                r4 = 0x163;
                                goto have_msg;
                            msg_127a:
                                msg = (s16)(*(s32 *)(pl + 8) + 0x127);
                                r4 = 0x161;
                            } else {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, r4);
                                if (_ZN8SaveData22NumGlowingRabbitsFoundEv() == 7) {
                                    msg = 0x143;
                                    r4 = 0x160;
                                } else {
                                    r4 = 0x162;
                                    msg = 0x142;
                                }
                            }
                            goto have_msg;
                        }
                        /* character id 3 simple path */
                        if (*(u8 *)(c + 0x429) == 0) {
                            if (*(s32 *)(c + 0x43c) != 6)
                                msg = 0x12d;
                            else
                                msg = 0x12e;
                        } else {
                            if (_ZN8SaveData22NumGlowingRabbitsFoundEv() == 7)
                                msg = 0x147;
                            else
                                msg = 0x146;
                        }
                    }
                    goto have_msg;
                    msg_13c:
                        msg = 0x13c;
                    have_msg:
                    {
                        int y = pos.y;
                        int zero = 0;
                        y = y + 0x64000;
                        pos.y = y;
                        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(pl, c, (u32)msg, &pos, zero, zero) == 1) {
                            *(s32 *)(c + 0x41c) = 1;
                            if (r4 != 0)
                                func_02012694(r4, c + 0x74);
                        }
                    }
                    return 1;
                }
            }
        }
    }
    after_first_section:
    if (*(volatile s32 *)(c + 0x41c) != 1)
        return 1;

    if (_ZN6Player12GetTalkStateEv(pl) != -1) {
        if (data_0209d660 != 0) {
            if (data_0209d6bc == 9) {
                if (*(s32 *)(c + 0x43c) != 7) {
                    if (func_02013890(*(s32 *)(c + 0x43c), *(s32 *)(pl + 8)) != 0) {
                        if (*(u8 *)(c + 0x429) == 0)
                            goto talk_active_done;
                    }
                }
                {
                    int z = 0;
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, z, 0x8777, z);
                }
            }
        }
    talk_active_done:
        return 1;
    }

    /* talk ended */
    _ZN6Player9DropActorEv(pl);
    {
        s32 *p128 = (s32 *)(c + 0x128);
        *p128 = *p128 & ~2;
    }
    *(s32 *)(c + 0x98) = 0;

    if (*(s32 *)(c + 0x43c) == 7) {
        pos7.x = *(s32 *)(c + 0x5c);
        pos7.y = *(s32 *)(c + 0x60);
        pos7.z = *(s32 *)(c + 0x64);
        pos7.y = pos7.y + 0x32000;
        {
            s8 cc = *(s8 *)(c + 0xcc);
            int m1 = -1;
            void *spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xe5, *(s32 *)(c + 0x43c), &pos7, (void *)(c + 0x8c), cc, m1);
            if (spawned != 0)
                *(s32 *)((char *)spawned + 0x190) = *(s32 *)(c + 4);
        }
        func_02012790(0xa);
        *(u8 *)(c + 0x427) = 0;
        *(s32 *)(c + 0x45c) = 0;
        SetState(data_ov085_021306bc);
        return 1;
    }

    if (func_02013890(*(s32 *)(c + 0x43c), *(s32 *)(pl + 8)) != 0) {
        if (*(u8 *)(c + 0x429) == 0)
            goto no_spawn;
        if (_ZN8SaveData22NumGlowingRabbitsFoundEv() != 7)
            goto no_spawn;
    }
    {
        posR.x = *(s32 *)(c + 0x5c);
        posR.y = *(s32 *)(c + 0x60);
        posR.z = *(s32 *)(c + 0x64);
        {
            u32 param = *(s32 *)(c + 0x43c);
            if (*(u8 *)(c + 0x429) != 0) {
                param = 0x4d;
                func_02013944();
            }
            posR.y = posR.y + 0x32000;
            {
                s8 cc = *(s8 *)(c + 0xcc);
                int m1 = -1;
                void *spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0xe5, param, &posR, 0, cc, m1);
                if (spawned != 0)
                    *(s32 *)((char *)spawned + 0x190) = *(s32 *)(c + 4);
            }
            func_02012790(0xa);
            *(u8 *)(c + 0x427) = 0;
            {
                u16 *pf = (u16 *)(pl + 0x6ce);
                *pf = (u16)(*pf | 0x800);
            }
        }
        goto after_spawn;
    }
no_spawn:
    if (*(u8 *)(c + 0x427) == 2) {
        {
            u16 *pf = (u16 *)(pl + 0x6ce);
            *pf = (u16)(*pf & ~0x800);
        }
        *(u8 *)(c + 0x427) = 0;
    }
    if (*(u8 *)(c + 0x429) == 0) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x7f, 0, 0x7444, 0);
        _ZN7Message7EndTalkEv();
    }
after_spawn:
    ;

    *(s32 *)(c + 0x45c) = 0;

    if (*(u8 *)(c + 0x429) == 0)
        goto do_306bc;
    if (_ZN8SaveData22NumGlowingRabbitsFoundEv() != 8)
        goto flag_path;
do_306bc:
    SetState(data_ov085_021306bc);
    goto final_return;
flag_path:
    {
        u16 *pf = (u16 *)(pl + 0x6ce);
        *pf = (u16)(*pf | 0x800);
    }
    *(char **)(c + 0x460) = pl;
    SetState(data_ov085_0213068c);
final_return:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- daMip_c::StateCaughtInit, 0x0212b3fc, size 0x48           */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c15StateCaughtInitEv
int daMip_c::StateCaughtInit()
{
    extern int data_ov085_021305b8[];
    char *c = (char *)this;

    *(int*)(c + 0x41c) = 0;
    *(int*)(c + 0x98) = 0;
    *(int*)(c + 0x114) = 0x28000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)data_ov085_021305b8[1], 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- daMip_c::StateRestMain, 0x0212b444, size 0x34             */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13StateRestMainEv
int daMip_c::StateRestMain()
{
    extern int data_ov085_021306cc[];
    char *c = (char *)this;

    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        SetState(data_ov085_021306cc);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- daMip_c::StateRestInit, 0x0212b478, size 0x3c             */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13StateRestInitEv
int daMip_c::StateRestInit()
{
    extern int *data_ov085_021305b0[];
    char *c = (char *)this;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0x300, data_ov085_021305b0[1], 0x40000000, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- daMip_c::StateFleeMain, 0x0212b4b4, size 0x2a8            */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13StateFleeMainEv
/* Running the level's path away from the player: dust or splash at every node,
   PathPtr node stepping in the direction StateFleeInit chose, and a fall back to
   Rest once the player is more than 0x4b0000 away. */
int daMip_c::StateFleeMain()
{
    /* Local POD triples, not types.h's Vector3: this member was recovered as C
       and the six locals never carried Vector3's destructor.
       V3Blk IS THE SAME TWELVE BYTES SPELLED AS AN ARRAY, and the spelling is
       load-bearing for exactly one statement -- the copy of the player's
       position below. mwccarm scalarises a copy of a three-scalar-member struct
       into three ldr/str pairs; the ROM does `ldm r0,{r0,r1,r2}` / `stm`, and
       an array member is what makes the compiler emit the block move. Eight
       bytes, and the only difference in this member after the fold. */
    struct Vector3 { s32 x, y, z; };
    struct V3Blk   { s32 w[3]; };
    extern char data_ov085_0213069c[];
    char *c = (char *)this;

    void *pl;
    char pathptr[8];
    struct V3Blk v;
    struct Vector3 pos;
    struct Vector3 node;
    struct Vector3 node2;
    struct Vector3 delta;
    struct Vector3 scaled;
    s32 len;
    s32 lim;
    s16 ang;
    int idx;
    int ysave;
    pl = ClosestPlayer();
    if (pl != 0)
    {
      v = *((struct V3Blk *) (((char *) pl) + 0x5c));
      if (Vec3_Dist((struct Vector3 *) (c + 0x5c), &v) > 0x4b0000)
      {
        SetState(data_ov085_0213069c);
        return 1;
      }
    }
    {
      s32 t = (*((s32 *) (c + 0x358))) >> 12;
      if (((u16) t) == 0)
      {
        if (TestWaterBelow() == 1)
        {
          func_02012694(0x124, c + 0x74);
          func_02022a4c(*((s32 *) (c + 0x5c)), (*((s32 *) (c + 0x464))) + 0x3000, *((s32 *) (c + 0x64)));
        }
        else
        {
          func_02012694(0x123, c + 0x74);
        }
      }
    }

    if (TestWaterBelow() == 1)
    {
      s32 pair[2];
      pair[0] = *((s32 *) (c + 0x5c));
      pair[1] = *((s32 *) (c + 0x464));
      {
        s32 z = *((s32 *) (c + 0x64));
        s32 y = pair[1] + 0x3000;
        s32 x = pair[0];
        *((volatile s32 *) (&pos.x)) = x;
        *((volatile s32 *) (&pos.y)) = y;
        *((volatile s32 *) (&pos.z)) = z;
        *((s32 *) (c + 0x46c)) = func_02022cbc(*((s32 *) (c + 0x46c)), 0xe8, *((volatile s32 *) (&pos.x)), *(&pos.y), z, 0);
      }
    }

    _ZN7PathPtrC1Ev(pathptr);
    _ZN7PathPtr6FromIDEj(pathptr, *((u32 *) (c + 0x438)));
    _ZNK7PathPtr7GetNodeER7Vector3j(pathptr, &node, *((u32 *) (c + 0x448)));
    ang = Vec3_HorzAngle((struct Vector3 *) (c + 0x5c), &node);
    ApproachAngle((s16 *) (c + 0x94), ang, 1, 0x1000, 0x1000);
    idx = (*((s32 *) (c + 0x448))) + (*((s32 *) (c + 0x44c)));
    if (idx < 0)
    {
      idx = (*((s32 *) (c + 0x444))) - 1;
    }
    if (idx >= (*((s32 *) (c + 0x444))))
    {
      idx = 0;
    }
    _ZNK7PathPtr7GetNodeER7Vector3j(pathptr, &node2, (u32) idx);
    lim = 0x26000;
    if ((*((s32 *) (c + 0x43c))) == 7)
    {
      lim = lim >> 1;
    }
    {
      s32 y = *((s32 *) (c + 0x60));
      node.y = y;
      Vec3_Sub(&delta, (struct Vector3 *) (c + 0x5c), &node);
      len = LenVec3(&delta);
    }
    if ((len == 0) || (len <= lim))
    {
      *((s32 *) (c + 0x5c)) = node.x;
      *((s32 *) (c + 0x60)) = node.y;
      *((s32 *) (c + 0x64)) = node.z;
      {
        s32 *p = (s32 *)(c + 0x448);
        *p = (*p) + (*((s32 *) (c + 0x44c)));
      }
      if ((*((s32 *) (c + 0x448))) >= (*((s32 *) (c + 0x444))))
      {
        *((s32 *) (c + 0x448)) = 0;
      }
      if ((*((s32 *) (c + 0x448))) < 0)
      {
        *((s32 *) (c + 0x448)) = (*((s32 *) (c + 0x444))) - 1;
      }
    }
    else
    {
      ang = Vec3_HorzAngle((struct Vector3 *) (c + 0x5c), &node);
      if (AngleDiff(ang, *((s16 *) (c + 0x94))) < 0x2000)
      {
        ysave = *((s32 *) (c + 0x60));
        {
          int s = _ZN4cstd4fdivEii(lim, len);
          Vec3_MulScalar(&scaled, &delta, s);
          SubVec3((struct Vector3 *) (c + 0x5c), &scaled, (struct Vector3 *) (c + 0x5c));
        }
        if ((*((s32 *) (c + 0xa8))) > 0)
        {
          *((s32 *) (c + 0x60)) = ysave;
        }
        if ((*((s32 *) (c + 0x41c))) == 0)
        {
          if (_ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144) != 0)
          {
            *((s32 *) (c + 0xa8)) = 0xa000;
            *((s32 *) (c + 0x41c)) = 1;
          }
        }
      }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- daMip_c::StateFleeInit, 0x0212b75c, size 0x110            */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13StateFleeInitEv
/* Picks which way round the path to run: whichever of the two neighbouring
   nodes is further from the closest player becomes the step direction at
   +0x44c. This member was recovered as C++ against types.h's Vector3, so it
   keeps it -- the destructor that type declares is free here and was measured
   so before the fold. */
int daMip_c::StateFleeInit()
{
  extern void *data_ov085_021305d0[];
  char *c = (char *)this;

  char pathptr[8];
  int indices[2];
  struct Vector3 v;
  struct Vector3 nodes[2];
  int i;
  int d0;
  struct Vector3 *src;

  char *p = (char *)ClosestPlayer();
  *(int *)(c + 0x41c) = 0;
  if (p)
  {
    src = (struct Vector3 *)(p + 0x5c);
    v = *src;
    _ZN7PathPtrC1Ev(pathptr);
    _ZN7PathPtr6FromIDEj(pathptr, *(int *)(c + 0x438));

    indices[0] = *(int *)(c + 0x448) - 1;
    if (*(int *)(c + 0x448) - 1 < 0)
      indices[0] = *(int *)(c + 0x444) - 1;
    indices[1] = *(int *)(c + 0x448) + 1;
    if (*(int *)(c + 0x448) + 1 >= *(int *)(c + 0x444))
      indices[1] = 0;

    for (i = 0; i < 2; i++)
      _ZNK7PathPtr7GetNodeER7Vector3j(pathptr, &nodes[i], indices[i]);

    *(int *)(c + 0x44c) = 1;
    d0 = Vec3_Dist(&v, &nodes[0]);
    if (d0 > Vec3_Dist(&v, &nodes[1]))
      *(int *)(c + 0x44c) = -1;
  }

  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x300), data_ov085_021305d0[1], 0, 0x1000, 0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- daMip_c::StateStartleMain, 0x0212b86c, size 0x34          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c16StateStartleMainEv
int daMip_c::StateStartleMain()
{
    extern int data_ov085_0213067c[];
    void *c = (void *)this;

    if (_ZN9Animation8FinishedEv((char *)c + 0x350) != 0)
        SetState(data_ov085_0213067c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- daMip_c::StateStartleInit, 0x0212b8a0, size 0x3c          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c16StateStartleInitEv
int daMip_c::StateStartleInit()
{
    extern int *data_ov085_021305c8[];
    char *c = (char *)this;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c+0x300, data_ov085_021305c8[1], 0x40000000, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- daMip_c::StateIdleMain, 0x0212b8dc, size 0x338            */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13StateIdleMainEv
/* The resting rabbit's own machine: startle and flee when the closest player
   comes inside 0x3e8000 from below, otherwise cycle the three-step idle
   animation and drift its facing angle around the spot it was standing on
   (snapshotted at +0x42c by StateIdleInit).
 *
 * The scalar typedefs and Vector3 come from types.h rather than being respelled
 * here: the local copies were a shadow declaration the langmode ratchet counts,
 * and the member was built both ways -- all 0x338 bytes are identical. */
int daMip_c::StateIdleMain()
{
    extern int data_0209e650[];
    extern void *data_ov085_0213066c;
    extern void *data_ov085_021305d0[];
    extern void *data_ov085_021305b0[];
    extern void *data_ov085_021305c0[];
    extern void *data_ov085_021305c8[];
    char *c = (char *)this;

    char *player = (char *)ClosestPlayer();
    if (player == 0) return 1;

    if (TestWaterBelow() == 1) {
        Vector3 sp;
        s32 pair[2];
        pair[0] = *((s32*)(c + 0x5c));
        pair[1] = *((s32*)(c + 0x464));
        s32 z = *((s32*)(c + 0x64));
        s32 y = pair[1] + 0x3000;
        s32 x = pair[0];
        *((volatile s32*)(&sp.x)) = x;
        *((volatile s32*)(&sp.y)) = y;
        *((volatile s32*)(&sp.z)) = z;
        *((s32*)(c + 0x46c)) = func_02022cbc(*((s32*)(c + 0x46c)), 0xe8,
            *((volatile s32*)(&sp.x)), *(&sp.y), z, 0);
    }

    Vector3 pp;
    Vector3* ppp = (Vector3*)(player + 0x5c);
    pp.x = ppp->x;
    pp.y = ppp->y;
    pp.z = ppp->z;
    int lim = 0x3e8000;
    if (*(int*)(c + 0x43c) == 7) lim = 0x2ee000;

    if (Vec3_HorzDist((Vector3*)(c + 0x5c), &pp) < lim) {
        int t = *(int*)(c + 0x43c);
        int cond = 0;
        if (t == 7 || t == 1 ||
            (t == 4 && *(int*)(c + 0x440) == 1) ||
            (t == 4 && *(int*)(c + 0x440) == 3)) {
            if (*(int*)(c + 0x60) + 0x64000 <= pp.y) cond = 1;
        }
        if (!cond) {
            *(int*)(c + 0x98) = 0;
            SetState(&data_ov085_0213066c);
            return 1;
        }
    }

    u32 r = (u32)RandomIntInternal(data_0209e650) >> 8;
    if (*(int*)(c + 0x43c) == 1) {
        if (Vec3_HorzDist((Vector3*)(c + 0x5c), &pp) < 0x4b0000) {
            *(s16*)(c + 0x424) = Vec3_HorzAngle((Vector3*)(c + 0x5c), &pp) + 0x8000;
            *(s16*)(c + 0x100) = 0x1e;
        }
    }

    if (_ZN9Animation8FinishedEv(c + 0x350) != 0) {
        switch (*(int*)(c + 0x41c)) {
        case 1:
            *(int*)(c + 0x98) = 0x4000;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305d0[1], 0x40000000, 0x1000, 0);
            (*(int*)(c + 0x41c))++;
            break;
        case 2:
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305b0[1], 0x40000000, 0x1000, 0);
            *(int*)(c + 0x98) = 0;
            (*(int*)(c + 0x41c))++;
            break;
        case 3:
            *(s16*)(c + 0x100) = (r & 0x1f) + 0x1e;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305c0[1], 0, 0x1000, 0);
            *(int*)(c + 0x41c) = 0;
            break;
        }
    }

    if (*(int*)(c + 0x41c) == 0 && *(u16*)(c + 0x100) == 0) {
        *(s16*)(c + 0x424) = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(c + 0x42c));
        s16* ang = (s16*)(c + 0x424);
        *ang = *ang + (0x1800 - ((r & 3) << 12));
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov085_021305c8[1], 0x40000000, 0x1000, 0);
        *(int*)(c + 0x41c) = 1;
    }

    ApproachAngle((short*)(c + 0x94), *(s16*)(c + 0x424), 1, 0x500, 0x500);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- daMip_c::StateIdleInit, 0x0212bc14, size 0x64             */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13StateIdleInitEv
int daMip_c::StateIdleInit()
{
  extern char data_ov085_021305c0;
  char *p = (char *)this;

  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x300, *(void**)((char*)&data_ov085_021305c0+4), 0, 0x1000, 0);
  *(int*)(p+0x42c)=*(int*)(p+0x5c);
  *(int*)(p+0x430)=*(int*)(p+0x60);
  *(int*)(p+0x434)=*(int*)(p+0x64);
  *(int*)(p+0x41c)=0;
  *(short*)(p+0x100)=0;
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- daMip_c::SetState, 0x0212bc78, size 0x50                  */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c8SetStateEPv
/* Stores the 16-byte state record at mState and immediately runs its first
   pointer-to-member -- the state's Init. The record's second is what Behavior
   calls every frame. Member-ness is proven by the records themselves. */
int daMip_c::SetState(void *record)
{
    daMip_cSelf *c = (daMip_cSelf *)this;
    daMip_cStateFn *p = (daMip_cStateFn *)record;
    c->pp = p; daMip_cStateFn *q = c->pp; if (*q == 0) return 1; return (c->**q)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- daMip_c::UpdateMatrixAndShadow, 0x0212bcc8, size 0xf4     */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c21UpdateMatrixAndShadowEv
/* Name coined; member-ness not proven. The flat Matrix4x3 shadow is
   deliberate -- see the file header. */
void daMip_c::UpdateMatrixAndShadow()
{
    struct Vector3 { s32 x, y, z; };
    struct Mtx43 { s32 m[12]; };
    extern struct Mtx43 data_020a0e68;
    char *c = (char *)this;

    char tmp[0x30];
    struct Vector3 t;
    Vec3_Asr(&t, (struct Vector3*)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(struct Mtx43*)(c + 0x31c) = data_020a0e68;
    MulMat4x3Mat4x3(*(void**)(c + 0x314), (void*)(c + 0x31c), tmp);
    Matrix4x3_FromTranslation(&data_020a0e68,
        *(int*)(c + 0x5c) >> 3,
        (*(int*)(c + 0x60) - 0xe000) >> 3,
        *(int*)(c + 0x64) >> 3);
    *(struct Mtx43*)(c + 0x390) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, (void*)(c + 0x368), (void*)(c + 0x390), 0x46000, 0x258000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- daMip_c::UpdateCarriedMatrix, 0x0212bdbc, size 0x120      */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c19UpdateCarriedMatrixEv
/* While a player is carrying the rabbit its matrix comes from
   dActor_c::UpdateCarry, offset by one of four hold positions in ov085 .bss at
   0x021306ec depending on whether the carrier is sliding and which character it
   is. Name coined; member-ness not proven. */
void daMip_c::UpdateCarriedMatrix()
{
    struct Mtx43 { s32 m[12]; };
    extern char data_ov085_021306ec[];
    extern int data_020a0e68[];
    char *c = (char *)this;

    int idx;
    void* res;
    if (!*(void**)(c + 0x45c)) return;
    if (!*(int*)(*(char**)(c + 0x45c) + 0xc8)) return;
    idx = 0;
    if (_ZN6Player14IsFrontSlidingEv(*(void**)(c + 0x45c)) || _ZN6Player17LostGrabbedObjectEv(*(void**)(c + 0x45c))) {
        idx = 1;
    }
    if (*(int*)(*(char**)(c + 0x45c) + 8) == 2) {
        idx = (idx + 2) & 0xff;
    }
    res = _ZN8dActor_c11UpdateCarryER6PlayerRK7Vector3(c, *(void**)(c + 0x45c), data_ov085_021306ec + idx * 0xc);
    *(struct Mtx43*)(c + 0x31c) = *(struct Mtx43*)res;
    Matrix4x3_FromTranslation(data_020a0e68, *(int*)(c + 0x5c) >> 3, (*(int*)(c + 0x60) - 0xc000) >> 3, *(int*)(c + 0x64) >> 3);
    *(struct Mtx43*)(c + 0x390) = *(struct Mtx43*)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x368, c + 0x390, 0x46000, 0x258000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- daMip_c::UpdateMirrorShadow, 0x0212bedc, size 0x128       */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c18UpdateMirrorShadowEv
/* The second shadow, for the mirrored copy RenderMirrorImage draws. Both are
   gated on the same pair of globals, and this one places the shadow on the far
   side of the mirror plane at x = 0x1086000. Name coined; member-ness not
   proven. */
void daMip_c::UpdateMirrorShadow()
{
    struct Mtx43 { s32 m[12]; };
    struct Vector3_local { int x; int y; int z; };
    struct Obj {
        char pad5c[0x5c];
        int v5c;
        int v60;
        int v64;
        char pad3c0[0x3c0 - 0x68];
        char shadowmodel[0x28];
        struct Mtx43 mtx;
    };
    extern struct Mtx43 data_020a0e68;

    struct Obj *c = (struct Obj *)this;

    struct Vector3_local v;
    struct Vector3_local res;
    struct Vector3_local vd;

    v.x = 0; v.y = 0; v.z = 0;
    res.x = 0; res.y = 0; res.z = 0;
    vd.x = 0; vd.y = 0; vd.z = 0;

    vd.x = c->v5c;
    vd.y = c->v60;
    vd.z = c->v64;
    vd.x = 0x1086000;
    v.z = Vec3_HorzDist((struct Vector3_local *)&c->v5c, &vd);
    Matrix4x3_FromRotationY(&data_020a0e68, Vec3_HorzAngle((struct Vector3_local *)&c->v5c, &vd));
    MulVec3Mat4x3(&v, &data_020a0e68, &res);
    {
        int t;
        vd.x = vd.x + res.x;
        t = c->v60;
        vd.y = t;
        vd.z = vd.z + res.z;
        Matrix4x3_FromTranslation(&data_020a0e68, vd.x >> 3, (t - 0xc000) >> 3, vd.z >> 3);
    }
    c->mtx = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, &c->shadowmodel, &c->mtx, 0x46000, 0x258000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- daMip_c::CleanupResources, 0x0212c004, size 0x6c          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c16CleanupResourcesEv
/* Seven releases, straight-line, no loop -- the ROM writes them out one after
 * another and so does this. They are not in address order, which is why the
 * sequence is reproduced literally rather than tidied into a table walk.
 *
 * The first, data_ov085_021305d8, is the handle daObj_Mip_Key_c releases too. */
int daMip_c::CleanupResources()
{
    extern char data_ov085_021305d8;
    extern char data_ov085_021305b8;
    extern char data_ov085_021305d0;
    extern char data_ov085_021305b0;
    extern char data_ov085_021305c8;
    extern char data_ov085_021305c0;
    extern char data_ov085_021305e0;

    ((SharedFilePtr *)(&data_ov085_021305d8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305b8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305d0))->Release();
    ((SharedFilePtr *)(&data_ov085_021305b0))->Release();
    ((SharedFilePtr *)(&data_ov085_021305c8))->Release();
    ((SharedFilePtr *)(&data_ov085_021305c0))->Release();
    ((SharedFilePtr *)(&data_ov085_021305e0))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- daMip_c::OnPendingDestroy, 0x0212c070, size 0x4           */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c16OnPendingDestroyEv
/* Empty -- the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy, not to do anything itself. */
void daMip_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- daMip_c::Render, 0x0212c074, size 0xdc                    */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c6RenderEv
int daMip_c::Render()
{
    extern signed char data_0209f2f8;
    extern signed char data_02092120;

    if (mIsDisabled == 1) return 1;

    {
        int b = (mFlags & 0x40000) != 0;
        if (b) return 1;
    }

    mScaleX = 0x1500;
    mScaleZ = mScaleX;
    mScaleY = mScaleZ;

    {
        int** base = (int**)&mModelAnim.data;
        int* r3 = base[0];
        char* r1 = (char*)base[1];
        for (unsigned int i = 0; i < *(unsigned int*)((char*)r3 + 0x24); i++) {
            *(int*)(r1 + 0x20) = mMaterialColor;
            r1 += 0x30;
        }
    }

    if (data_0209f2f8 == 5 && data_02092120 == 3) {
        RenderMirrorImage();
    }

    mModelAnim.Render((Vector3 *)&mScaleX);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- daMip_c::RenderMirrorImage, 0x0212c150, size 0xe0         */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c17RenderMirrorImageEv
/* The mirrored second copy: negate the model matrix's X scale, render the model
   again at half opacity, then put the matrix back. Only ever reached with the
   same two globals UpdateMirrorShadow tests. Name coined; member-ness not
   proven. */
void daMip_c::RenderMirrorImage()
{
    struct Mtx43 { s32 m[12]; };
    extern int data_020a0e68;
    char *c = (char *)this;

    struct Mtx43 tmp;
    tmp = *(struct Mtx43 *)(c + 0x31c);
    *(int *)(c + 0x340) = 0x421800 - *(int *)(c + 0x340);
    func_0203c178(&data_020a0e68, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3((void *)(c + 0x31c), &data_020a0e68, (void *)(c + 0x31c));
    func_02016acc((void *)(c + 0x300), 0x80);
    func_02016b24((void *)(c + 0x300), 0x40);
    _ZN9ModelAnim6RenderEPK7Vector3((void *)(c + 0x300), (void *)(c + 0x80));
    _ZN9ModelBase12ApplyOpacityEj((void *)(c + 0x300), 0xff, 0);
    func_02016b24((void *)(c + 0x300), 0x80);
    func_02016acc((void *)(c + 0x300), 0x40);
    *(struct Mtx43 *)(c + 0x31c) = tmp;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- daMip_c::Behavior, 0x0212c230, size 0x5cc                 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c8BehaviorEv
/* The glowing-rabbit chase, and it is mostly a conversation.
 *
 * The opening gate depends on the mode. Normally mIsDisabled latches the rabbit
 * out of the level until data_0209caa0[2] clears bit 0x20000; in mode 0x32 the
 * test inverts -- the rabbit runs only for the player whose character id matches
 * mCharacterId and latches itself off for everyone else, which is what makes one
 * actor behave differently per player.
 *
 * The talk sequence is mTalkState: 0 offers the message, 1 waits for the
 * player's talk state to end, 2 is done. Which message depends on mRabbitId, on
 * whether this rabbit has already been caught, and -- for the last one -- on
 * whether SaveData says all seven are found.
 *
 * mState is the current animation descriptor. It is compared against four ov085
 * objects rather than dereferenced as a type, so it stays an s32 and those four
 * comparisons are what identify the states.
 *
 * The block before Animation::Advance is the ARM/Itanium pointer-to-member
 * sequence written out -- adjustment word, virtual bit, vtable index or direct
 * address -- run on whatever mState points at. It is kept verbatim because there
 * is no recovered type for the descriptor to call a member through.
 *
 * `_ZN9Animation7AdvanceEv((char *)this + 0x350)` must offset from THIS.
 * 0x350 is mModelAnim's Animation base at +0x50; `(char *)&mModelAnim + 0x50`
 * costs a word.
 *
 * The `(long long)(int)` round-trip on c+0x42a (mEatenTimer) is measured:
 * replacing it with `c + 0x42a` or `mEatenTimer = mEatenTimer + 1` size-DIFFs
 * Behavior (0x5cc). The same round-trip on c+0x448 is a no-op and was dropped. */
int daMip_c::Behavior()
{
    /* A LOCAL COORDINATE TRIPLE, NOT A Vector3 OBJECT. Vector3 declares a
       destructor (see include/types.h -- the ROM's __cxa_vec_cleanup calls prove the
       type has one), so a Vector3 local would be destroyed at scope exit and this
       function would come out 8 bytes long. The ROM emits no cleanup for either of
       these, which is itself the evidence that they were never Vector3s: they are
       scratch x/y/z the code fills and reads back. */
    typedef volatile struct { Fix12i x, y, z; } Vec3Scratch;

    extern s8 data_0209f2f8;
    extern s8 data_02092120;
    extern int data_0209caa0[];
    extern void* data_0209f33c;
    extern char data_ov085_021305c0;
    extern char data_ov085_0213068c;
    extern char data_ov085_021306ac;
    extern char data_ov085_021306bc;
    extern char data_ov085_021306dc;

    char* c = (char*)this;
    void* r0p;

    if (data_0209f2f8 != 0x32) {
        if (mIsDisabled == 1) {
            if (data_0209caa0[2] & 0x20000)
                mIsDisabled = 0;
            return 1;
        }
    } else {
        r0p = ClosestPlayer();
        if (r0p == 0)
            return 1;
        if (!(data_0209caa0[2] & 0x20000) || mCharacterId != *(u8*)((char*)r0p + 0x6d9)) {
            mIsDisabled = 1;
            return 1;
        }
        mIsDisabled = 0;
    }

    if (mRabbitId == 7 && !(data_0209caa0[1] & 0x40))
        data_0209f33c = c;

    if (mIsGlowing != 0) {
        Vec3Scratch pv;
        pv.x = mPosX;
        pv.y = mPosY;
        pv.z = mPosZ;
        pv.y = mPosY + 0x3c000;
        *(void**)&mGlowParticle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(volatile u32*)&mGlowParticle, 0x10d, pv.x, pv.y, mPosZ, 0, 0);
    }

    {
        u32 temp_r1 = mTalkState;
        if (temp_r1 < 2) {
            int v = (mFlags & 0x40000) ? 1 : 0;
            if (v == 1) {
                void* temp_r4 = *(void**)&mTalkingPlayer;
                if (temp_r4 != 0) {
                    if (temp_r1 == 0) {
                        Vec3Scratch pos;
                        int var_r6;
                        int var_r2;
                        int t;
                        pos.x = mPosX;
                        pos.y = mPosY;
                        pos.z = mPosZ;
                        _ZN7Message11PrepareTalkEv();
                        t = mRabbitId;
                        if (t != 7) {
                            if (func_02013890(t, *(s32*)((char*)temp_r4 + 8)) == 0) {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (mRabbitId != 6) { var_r6 = 0x162; var_r2 = 0x11e; }
                                else { var_r6 = 0x160; var_r2 = 0x126; }
                            } else if (mIsGlowing == 0) {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x26, 0x12, 0x7f, 0x15ccc, 0);
                                if (mRabbitId != 6) { var_r6 = 0x162; var_r2 = 0x122; }
                                else { var_r6 = 0x160; var_r2 = 0x12a; }
                            } else {
                                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                                if (_ZN8SaveData22NumGlowingRabbitsFoundEv() == 7) { var_r2 = 0x145; var_r6 = 0x160; }
                                else { var_r6 = 0x162; var_r2 = 0x144; }
                            }
                        } else {
                            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x27, 0x12, 0x7f, 0x15ccc, 0);
                            var_r2 = 0x13b; var_r6 = 0x160;
                        }
                        pos.y += 0x64000;
                        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(temp_r4, c, var_r2, 0, 0, 0) == 1) {
                            func_02012694(var_r6, &mCamSpacePosX);
                            mTalkState = 1;
                        }
                    } else if (temp_r1 == 1 && _ZN6Player12GetTalkStateEv(temp_r4) == -1) {
                        _ZN6Player9DropActorEv(temp_r4);
                        mTalkState = 2;
                        *(u16*)((char*)temp_r4 + 0x6ce) |= 0x800;
                    }
                }
            }
        }
    }

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (unk_104 == 5)
                mHorzSpeed = 0;
            if (unk_104 == 0)
                mdCcAc_c.Update();
        }
        if (mEatenByYoshi == 1) {
            *(u8*)((long long)(int)(c + 0x42a)) = *(u8*)((long long)(int)(c + 0x42a)) + 1;
            if (mEatenTimer > 0x96) {
                mEatenByYoshi = 0;
                mEatenTimer = 0;
            }
        }
        UpdateMatrixAndShadow();
        unk_426 = 2;
        SetState(&data_ov085_021306ac);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void**)((char*)&data_ov085_021305c0 + 4), 0, 0x1000, 0);
        if (*(void**)&mTalkingPlayer == 0)
            *(void**)&mTalkingPlayer = ClosestPlayer();
        return 1;
    }

    if (unk_426 != 0) {
        void* p = *(void**)&mState;
        if (p != (void*)&data_ov085_021306ac && p != (void*)&data_ov085_0213068c &&
            p != (void*)&data_ov085_021306bc && p != (void*)&data_ov085_021306dc) {
            SetState(&data_ov085_021306ac);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, *(void**)((char*)&data_ov085_021305c0 + 4), 0, 0x1000, 0);
        }
        mHorzSpeed = 0;
    }

    DecIfAbove0_Short(&mStateTimer);

    {
        int* p = *(int**)&mState;
        if (p[2] != 0) {
            int* q = p + 2;
            int adj = q[1];
            void* thiz = c + (adj >> 1);
            void (*fn)(void*);
            if (adj & 1)
                fn = *(void(**)(void*))(*(char**)thiz + q[0]);
            else
                fn = (void(*)(void*))q[0];
            fn(thiz);
        }
    }

    _ZN9Animation7AdvanceEv(c + 0x350);

    {
        int v = (mFlags & 0x4000) ? 1 : 0;
        if (v != 0)
            UpdateCarriedMatrix();
        else
            UpdateMatrixAndShadow();
    }

    if (data_0209f2f8 == 5 && data_02092120 == 3)
        UpdateMirrorShadow();

    {
        int v = (mFlags & 0x4000) ? 1 : 0;
        if (v == 0) {
            if (*(void**)&mState != (void*)&data_ov085_021306ac || unk_426 != 0)
                UpdatePos(&mdCcAc_c);
            mAngleX = mPrevAngleX;
            mAngleY = mPrevAngleY;
            mAngleZ = mPrevAngleZ;
            UpdateWMClsn(mWithMeshClsn, 0);
            if (*(void**)&mState != (void*)&data_ov085_021306ac)
                UpdateGrab();
        }
    }

    if (*(void**)&mState != (void*)&data_ov085_021306ac) {
        mdCcAc_c.Clear();
        mdCcAc_c.Update();
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- daMip_c::InitResources, 0x0212c7fc, size 0x41c            */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13InitResourcesEv
int daMip_c::InitResources()
{
    extern char data_ov085_021305b8;
    extern char data_ov085_021305d0;
    extern char data_ov085_021305b0;
    extern char data_ov085_021305c8;
    extern char data_ov085_021305c0;
    extern char data_ov085_021305d8;
    extern char data_ov085_021305e0;
    extern char data_ov085_021306cc;
    extern s32 data_ov085_021305ac;
    extern int data_0209caa0[];
    extern s8 data_0209f2f8;
    extern int data_0209e650;

    void* r0;
    void* r6;
    int r1;

    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305b8);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305d0);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305b0);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305c8);
    Animation::LoadFile(*(SharedFilePtr *)&data_ov085_021305c0);
    Model::LoadFile(*(SharedFilePtr *)&data_ov085_021305d8);
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)&data_ov085_021305e0), 1, -1);
    mShadowModel1.InitCylinder();
    mShadowModel2.InitCylinder();

    mPathId = param1 & 0xff;
    if (mPathId == 0xff)
        mPathId = 0;

    /* dActor_c declares param1 u32, but the ROM shifts these two with ASR, not
       LSR -- so this call site reads it signed. Without the casts the function
       comes out two words different; the flat header called 0x008 an s32, which
       is why this was invisible before the rebase. */
    mRabbitId = ((s32)param1 & 0xf00) >> 8;
    if (mRabbitId == 0xff)
        mRabbitId = 0;

    mCharacterId = ((s32)param1 & 0xf000) >> 0xc;
    if (mCharacterId == 0xf)
        mCharacterId = 0;

    r1 = mRabbitId;
    if (r1 != 7) {
        if (!(data_0209caa0[1] & 0x40000000))
            return 0;
    }

    if (r1 == 5 && mCharacterId == 0)
        goto check18;
    if (r1 == 1 && mCharacterId == 1)
        goto check18;
    if (r1 != 6)
        goto skip17;
    if (mCharacterId != 3)
        goto skip17;

check18:
    if (!(data_0209caa0[2] & 0x80000))
        return 0;

skip17:
    mVertAccel = -0x1000;
    mTerminalVelocity = -0x1e000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, ((char*)this), 0x50000, 0x64000, 0xb00004, 0x9000);
    mTalkingPlayer = 0;
    mModelAnim.speed = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, ((char*)this), 0x28000, 0x28000, 0, 0);
    {
        PathPtr path;
        path.FromID(mPathId);
        mPathNodeIndex = 1;
        path.GetNode(*(Vector3 *)&mPosX, mPathNodeIndex);
        mNumPathNodes = path.NumNodes();
    }
    UpdateMatrixAndShadow();
    mEatingPlayer = 0;
    mScaleX = 0x1000;
    mScaleZ = mScaleX;
    mScaleY = mScaleZ;

    if (mRabbitId == 7) {
        if (data_0209caa0[1] & 0x40)
            return 0;
        mIsDisabled = 1;
        goto block_26;
    }

    r0 = ClosestPlayer();
    if (r0 == 0)
        return 0;
    if (data_0209f2f8 != 0x32) {
        if (mCharacterId != *(u8*)((char*)r0 + 0x6d9))
            return 0;
    }

block_26:
    mColorVariant = mCharacterId + 1;
    if (mCharacterId == 3)
        mColorVariant = 0;

    if (mRabbitId == 7) {
        mColorVariant = 0;
        goto block_out;
    }

    r6 = ClosestPlayer();
    if (r6 == 0)
        goto block_out;

    {
        int v;
        v = *(s32*)((char*)r6 + 8);
        if (data_0209f2f8 == 0x32)
            v = 1;
        if (func_02013890(mRabbitId, v) != 0 && data_ov085_021305ac < 8) {
            u32 rnd = RandomIntInternal(&data_0209e650) >> 8;
            if (NumStars() >= 0x51) {
                if (*(s32*)((char*)r6 + 8) == 3) {
                    if ((rnd & 0xf) == 0)
                        mColorVariant = 5;
                } else {
                    if ((rnd & 7) == 0)
                        mColorVariant = 5;
                }
            } else if (NumStars() >= 0x28) {
                if (*(s32*)((char*)r6 + 8) == 3) {
                    if ((rnd & 0x1f) == 0)
                        mColorVariant = 5;
                } else {
                    if ((rnd & 0xf) == 0)
                        mColorVariant = 5;
                }
            }
            if (mColorVariant == 5) {
                data_ov085_021305ac += 1;
                mIsGlowing = 1;
            }
        }
    }

block_out:
    if (data_0209f2f8 == 5) {
        if (mAreaId == 3)
            mFlags = 0x8280;
    }
    mMaterialColor = (mColorVariant << 1) + *(s32*)((char*)mModelAnim.data.materials + 0x20);
    SetState(&data_ov085_021306cc);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- daMip_c::OnYoshiTryEat, 0x0212cc18, size 0x14             */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daMip_c13OnYoshiTryEatEv
s32 daMip_c::OnYoshiTryEat() {
  unsigned char v = mEatenByYoshi;
  if (v != 0) return 0;
  return 7;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- daMip_c_classInit, 0x0212cc2c, size 0x5c                  */
/* -------------------------------------------------------------------------- */
// @symbol daMip_c_classInit
/* The registry factory behind the MIP profile. `return new daMip_c()` MATCHES
 * (size 0x5c); the synthesized ctor stores `_ZTV7daMip_c + 2`.
 *
 * Reconstructed source-style name: SM64DS proves daMip_c through RTTI,
 * allocation size, vtable identity and the MIP registry profile; later EAD
 * lineage supplies classInit. The exact original spelling is not preserved. */
extern "C" daMip_c *daMip_c_classInit(void)
{
    return new daMip_c();
}
