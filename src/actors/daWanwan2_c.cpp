//cpp
/* daWanwan2_c -- the unchained Chain Chomp (registry profile WANWAN2).
 *
 * The chomp walks a path (param1's low byte is the path ID, mPathID) and
 * drags a six-link chain behind it: func_ov100_021437d4 steps the links
 * (mLinkPos), func_ov100_02143b68 builds the per-frame model and shadow
 * matrices, func_ov100_0214344c handles contact (some hits start the
 * mHaltTimer and double the chomp's scale, a player it touches gets hurt) and
 * func_ov100_021435e8 keeps the actor it spawned at the chain's last link
 * in step. func_ov100_02143370 is the floor probe. Behavior runs a state
 * through the pointer-to-member pair mStatePair (+0x668): func_ov100_02143b18
 * enters a state and calls its first member, Behavior calls the second each
 * frame.
 *
 * Units, for the numbers below: positions, speeds and scales are 20.12 fixed
 * point (0x1000 = 1 unit, or 1.0 for a scale; a speed of 0x1000 is 1 unit per
 * frame); angles are 16 bit (0x10000 = a full turn, 0x4000 = a quarter turn);
 * model and shadow matrices take a position >> 3.
 * The only state table, data_ov100_021486f4, is seeded by the module's
 * static initializer from the constants at data_ov100_02148000
 * (func_ov100_02143ae0, the enter) and data_ov100_02147ff8
 * (func_ov100_02143aa4, the execute).
 *
 * Those two constants sit straight after daIbl_c's 31-slot vtable
 * (0x02147f7c..0x02147ff8), which is why a vtable scan read three extra
 * slots into daIbl_c and once labelled func_ov100_02143aa4 a daIbl_c
 * "Kill". Neither is a virtual function of either class: both run on this
 * class's fields.
 *
 * This file is the whole linker unit 0x021431c4..0x021443f4, 17 functions:
 * D1 and D0 (daIbl_c_classInit, the last function of src/actors/daIbl_c.cpp,
 * ends exactly at 0x021431c4 below them), the eight helpers
 * func_ov100_02143370 through func_ov100_02143b68, CleanupResources,
 * OnPendingDestroy, Render, Behavior, InitResources, OnAimedAtWithEgg and
 * the registry factory daWanwan2_c_classInit (hand-spelled; see its note at
 * the end of this file). daDoor_c's D1 starts at 0x021443f4 above it. The out-of-line destructor is the key
 * function, so this TU also emits the vtable and the RTTI.
 *
 * It replaces the one-function sources for _ZN11daWanwan2_cD1Ev,
 * _ZN11daWanwan2_cD0Ev, func_ov100_02143370 .. func_ov100_02143b68,
 * _ZN11daWanwan2_c16CleanupResourcesEv, _ZN11daWanwan2_c16OnPendingDestroyEv,
 * _ZN11daWanwan2_c6RenderEv, _ZN11daWanwan2_c8BehaviorEv,
 * _ZN11daWanwan2_c13InitResourcesEv, _ZN11daWanwan2_c16OnAimedAtWithEggEv and
 * daWanwan2_c_classInit.
 * Each member keeps the provenance notes its source carried.
 *
 * `#pragma defer_codegen off` keeps this file in ROM order.
 *
 * Leftover: the eight helpers keep their C-ABI cartridge names (their
 *   original method names are not recovered). Five take the chomp as a
 *   daWanwan2_c * and func_ov100_02143b18 takes the ChompPmfSelf stand-in;
 *   func_ov100_02143370 and func_ov100_02143b68 stay `char *` because
 *   include/decl_common.h declares them that way and check_decl_agreement
 *   compares the two. func_ov100_02143aa4 and func_ov100_02143ae0 are the
 *   state pair's members, called through ChompPmfSelf rather than as methods.
 * Leftover: the callees with Fix12<int> parameters (dActor_c's shadow drop,
 *   Player::Hurt, ModelAnim::SetAnim, dCcAcPos_c::Init, cstd::atan2) stay
 *   spelled as mangled extern-C free functions.
 * Leftover: func_ov100_02143b68 reaches the link positions and the link
 *   models' translations through two raw char pointers that start at the
 *   chomp, and InitResources' link seeding loop does the same. Written as
 *   Vector3 * / Model * walking the member arrays, both forms measure
 *   different from the ROM here, so those reads keep their offsets,
 *   commented.
 * Leftover: data_0209f2d8 is the game-mode byte (CURRENT_GAMEMODE in
 *   symbols/verified.tsv); this class tests it for 1 in two places
 *   (func_ov100_021435e8 and Behavior) and no name for that mode is
 *   recovered here. The sound IDs 0x39 and 0x3a are named by when they play,
 *   not by what they sound like.
 * Leftover: unk_6a8 is counted down and tested by Behavior but nothing in
 *   this class's source sets it; unk_6c9, unk_6cc and unk_6d4 are set by
 *   InitResources and never read here; mUnk_768 is only constructed and
 *   destroyed. The only state in the state table is the walking one, and the
 *   word at +0x440 of the star func_ov100_021435e8 spawns is only known to be
 *   tested for 5.
 */

#pragma defer_codegen off

#include "daWanwan2_c.h"
#include "types.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "dBgCh_Lin.h"
#include "Player.h"
#include "daStar_c.h"

/* Actor IDs from symbols/actor_debug_names.tsv. */
enum {
    daWanwan2_ACTOR_YOSHI_EGG   = 9,
    daWanwan2_ACTOR_STAR        = 0xb2,   /* 178, a daStar_c profile */
    daWanwan2_ACTOR_SILVER_STAR = 0xb3,   /* 179, a daStar_c profile too */
    daWanwan2_ACTOR_STARBASE    = 0xb4,   /* 180 */
    daWanwan2_ACTOR_PLAYER      = 0xbf,   /* 191 */
    daWanwan2_ACTOR_COIN        = 0x120   /* 288 */
};

/* Bits of the chomp's dCc_c hitFlags (mdCcAcPos_c.hitFlags). dCc_c.h says its
   bit table is a best-effort reading, and proves only the explosion and egg
   bits; these name the table's entries. */
enum {
    daWanwan2_HIT_MEGA      = 0x10,     /* the table's "mega character" */
    daWanwan2_HIT_EGG       = 0x2000,   /* proven: dActor_c::FindEgg's mask */
    daWanwan2_HIT_EXPLOSION = 0x4000    /* proven: FindExplosionActor's mask */
};

/* Sound IDs, as the callers pass them to func_02012694 (with the chomp's
   camera-space position); named by when they play, not by what they are. */
enum {
    daWanwan2_SND_FLOOR_HIT   = 0x39,   /* the floor probe hit, in Behavior's walking path */
    daWanwan2_SND_COIN_SPAWN  = 0x3a    /* Behavior's COIN spawn (played whether or not Spawn returned an actor) */
};

/* Three plain words: the stack vectors of the two helpers that were C,
   which carry none of Vector3's empty destructor. */
struct Vec3i { s32 x, y, z; };

/* func_ov100_02143b68's identity matrix, as twelve words: a copy through the
   C++ Matrix4x3 splits into two block moves here. */
struct M48 { int w[12]; };

/* The state machine's pointer-to-member pair. The `self` type is a stand-in,
   NOT the real dActor_c: a pointer-to-member on a non-polymorphic,
   single-base class is laid out differently from one on the real class, so
   the shape here is codegen, not decoration, and binding it to the real
   dActor_c makes mwccarm abort with an internal compiler error. */
struct ChompPmfSelf;
typedef int (ChompPmfSelf::*ChompPmf)();
struct ChompState {
    ChompPmf enter;     /* func_ov100_02143b18 calls it on entry */
    ChompPmf execute;   /* Behavior calls it every frame */
};
struct ChompPmfSelf {
    char pad[0x668];
    ChompState *state;  /* 0x668, daWanwan2_c::mStatePair */
};

struct Vector3_16;
struct dCc_c;

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void _Z14ApproachLinearRiii(int &x, int target, int step);
int __aeabi_idiv(int, int);
void _ZN8dActor_c9UpdatePosEP5dCc_c(dActor_c *thiz, dCc_c *c);
void _ZN8dActor_c15HugeLandingDustEb(dActor_c *thiz, bool b);
dActor_c *_ZN8dActor_c13ClosestPlayerEv(dActor_c *thiz);
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, const Vector3 *pos, const Vector3_16 *rot, int area, int unk);
void *_ZN8dActor_c10FindWithIDEj(u32 id);
void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void *self, void *sm, void *mtx, int a, int b, unsigned int g);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *clsn, const void *pos);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *clsn, dActor_c *actor, const Vector3 &offset,
    s32 radius, s32 height, u32 flags, u32 vulnFlags);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, const void *pos, u32 a, int b, u32 c, u32 d, u32 e);
void _ZN5dCc_c5ClearEv(void *thiz);
int _ZN5dCc_c6UpdateEv(void *thiz);
int func_02012694(int, void *);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
void _ZN15dExtFrameCtrl_c7AdvanceEv(void *anim);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
void *_ZN7PathPtrC1Ev(void *thiz);
void _ZN7PathPtr6FromIDEj(void *thiz, unsigned int id);
void _ZNK7PathPtr7GetNodeER7Vector3j(void *thiz, Vector3 &out, unsigned int idx);
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
void MulVec3Mat4x3(void *in, void *m, void *out);
void Vec3_Add(void *out, void *a, void *b);
void Vec3_Sub(void *out, void *a, void *b);
void Vec3_MulScalar(void *out, const void *in, int scale);
int Vec3_HorzLen(void *v);
int LenVec3(Vector3 *v);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
void ApproachAngle(short *cur, short target, int step, int a, int b);
int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
void LoadSilverStarAndNumber();
void UnloadSilverStarAndNumber();

extern SharedFilePtr data_ov002_0211092c;
extern SharedFilePtr data_ov100_021486bc;
extern SharedFilePtr data_ov100_021486a4;
extern SharedFilePtr data_ov100_021486ac;
extern SharedFilePtr data_ov100_021486b4;
extern s32 data_ov100_02148008[3];
extern ChompState data_ov100_021486f4;
extern unsigned char data_0209f2d8[];
extern short data_02082214[];
extern char data_020a0e68[];
}

extern Matrix4x3 IDENTITY_MATRIX4X3;

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN11daWanwan2_cD1Ev, 0x021431c4, size 0xcc;
 *                         _ZN11daWanwan2_cD0Ev, 0x02143290, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWanwan2_cD1Ev
// @symbol _ZN11daWanwan2_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * One vtable store and ten teardowns, every one a consequence of
 * `struct daWanwan2_c : dEnemyBase_c` and the members that declaration
 * types. Five of them are arrays, and the compiler's own loops reproduce the
 * ROM's __cxa_vec_cleanup calls with the same counts and strides -- which is
 * what makes this body the evidence for the header rather than a
 * transcription of it. D0 is the same teardown followed by dEnemyBase_c's
 * inline operator delete. */
daWanwan2_c::~daWanwan2_c()
{
}

#ifdef _MSC_VER
/* The host needs the ROM's flat D0 name, and MSVC never emits it: it folds
 * the Itanium destructor variants into the one ~daWanwan2_c() above. This
 * arm spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator
 * delete. Nothing here reaches mwccarm. */
extern "C" daWanwan2_c *_ZN11daWanwan2_cD0Ev(daWanwan2_c *thiz)
{
    thiz->daWanwan2_c::~daWanwan2_c();
    daWanwan2_c::operator delete(thiz);
    return thiz;
}
#endif

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov100_02143370, 0x02143370, size 0xdc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02143370
/* The floor probe: a vertical line from 10 units above the chomp (0xa000) to
   184 units below it (0xb8000). Returns 1 on a hit. */
extern "C" int func_ov100_02143370(char *self)
{
    daWanwan2_c *c = (daWanwan2_c *)self;
    Vector3 va;
    Vector3 vb;
    int ya, yb;
    dBgCh_Lin line1;
    dBgCh_Lin line2;
    va.x = 0; va.y = 0; va.z = 0;
    vb.x = 0; vb.y = 0; vb.z = 0;
    va.x = c->mPosX;
    ya = c->mPosY;
    va.y = ya;
    va.z = c->mPosZ;
    vb.x = c->mPosX;
    yb = c->mPosY;
    vb.y = yb;
    vb.z = c->mPosZ;
    va.y = ya + 0xa000;
    vb.y = yb - 0xb8000;
    line1.SetObjAndLine(va, vb, (dActor_c *)c);
    if (line1.DetectClsn()) {
        return 1;
    }
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov100_0214344c, 0x0214344c, size 0x19c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_0214344c
/* Contact. Moves the collision body to the chomp (the offset in
   data_ov100_02148008 is {0, -0xa0000, 0}, i.e. -160 units in Y), then looks
   at the actor whose uniqueID is in mdCcAcPos_c.otherOwner, if it still
   exists. Any of these hits sets mHaltTimer to 90 and the scale to 2.0,
   which Behavior eases back to 1.0: the explosion bit, the mega-character bit,
   or the egg bit when that actor is a YOSHI_EGG. Otherwise, a Player who is
   not vanished (mIsVanish) is hurt from the chomp's position. Was a C
   source; its `enum Bool` comparison temporaries are kept. */
enum Bool { FALSE, TRUE };

extern "C" void func_ov100_0214344c(daWanwan2_c *self)
{
    Vec3i v;
    dActor_c *other;

    int flags;
    u32 id;

    v.x = data_ov100_02148008[0];
    v.y = data_ov100_02148008[1];
    v.z = data_ov100_02148008[2];
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(&self->mdCcAcPos_c, &v);
    id = self->mdCcAcPos_c.otherOwner;
    if (id == 0) return;
    other = (dActor_c *)_ZN8dActor_c10FindWithIDEj(id);
    if (other == 0) return;
    flags = self->mdCcAcPos_c.hitFlags;
    if (flags & daWanwan2_HIT_EXPLOSION) {
        self->mHaltTimer = 90;
        self->mScaleX = 0x2000;     /* 2.0 */
        self->mScaleY = self->mScaleX;
        self->mScaleZ = self->mScaleY;
        return;
    }
    if (flags & daWanwan2_HIT_MEGA) {
        self->mHaltTimer = 90;
        self->mScaleX = 0x2000;     /* 2.0 */
        self->mScaleY = self->mScaleX;
        self->mScaleZ = self->mScaleY;
        return;
    }

    {
        enum Bool b = (enum Bool)(other->actorID == daWanwan2_ACTOR_YOSHI_EGG);
        if (b != FALSE && (flags & daWanwan2_HIT_EGG)) {
            self->mHaltTimer = 90;
            self->mScaleX = 0x2000;     /* 2.0 */
            self->mScaleY = self->mScaleX;
            self->mScaleZ = self->mScaleY;
            return;
        }
    }
    {
        enum Bool b = (enum Bool)(other->actorID == daWanwan2_ACTOR_PLAYER);
        if (b == FALSE) return;
    }
    if (((Player *)other)->mIsVanish != 0) return;
    {
        Vec3i pos;
        pos.x = self->mPosX;
        pos.y = self->mPosY;
        pos.z = self->mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &pos, 2, 0xc000, 1, 0, 1);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov100_021435e8, 0x021435e8, size 0x1ec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_021435e8
/* The actor kept at the chain's last link (mLinkPos[5]). While
   mChainEndActorID is 0 it is spawned (once Spawn succeeds): SILVER_STAR (0xb3) normally, or
   when data_0209f2d8[0] is 1 a STAR (0xb2, param mSpawnParam | 0x30) plus a
   STARBASE (0xb4) whose ID is not kept. Its uniqueID goes into
   mChainEndActorID, and afterwards the actor is pinned to the link every
   frame until it is gone or its +0x440 word (daStar_c's unk_440) reads 5;
   either latches mChainEndDone, clears mChainEndActorID and stops the
   updates. */
extern "C" void func_ov100_021435e8(daWanwan2_c *c)
{
    if (c->mChainEndDone != 0) return;

    int flag = (data_0209f2d8[0] == 1);
    if (!flag) {
        if (c->mChainEndActorID == 0) {
            dActor_c *a = (dActor_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(daWanwan2_ACTOR_SILVER_STAR, 0,
                (Vector3 *)&c->mLinkPos[5], 0, c->mAreaId, -1);
            if (a != 0) c->mChainEndActorID = a->uniqueID;
        } else {
            dActor_c *a = (dActor_c *)_ZN8dActor_c10FindWithIDEj(c->mChainEndActorID);
            if (a != 0) {
                if (((daStar_c *)a)->unk_440 == 5) {
                    c->mChainEndDone = 1;
                    c->mChainEndActorID = 0;
                    return;
                }
                a->mPosX = c->mLinkPos[5].x;
                a->mPosY = c->mLinkPos[5].y;
                a->mPosZ = c->mLinkPos[5].z;
                return;
            }
            c->mChainEndDone = 1;
            c->mChainEndActorID = 0;
        }
    } else {
        if (c->mChainEndActorID == 0) {
            dActor_c *a = (dActor_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(daWanwan2_ACTOR_STAR, c->mSpawnParam | 0x30,
                (Vector3 *)&c->mLinkPos[5], 0, c->mAreaId, -1);
            if (a != 0) c->mChainEndActorID = a->uniqueID;
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(daWanwan2_ACTOR_STARBASE, c->mSpawnParam | 0x30,
                (Vector3 *)&c->mLinkPos[5], 0, c->mAreaId, -1);
        } else {
            dActor_c *a = (dActor_c *)_ZN8dActor_c10FindWithIDEj(c->mChainEndActorID);
            if (a != 0) {
                if (((daStar_c *)a)->unk_440 == 5) {
                    c->mChainEndDone = 1;
                    c->mChainEndActorID = 0;
                    return;
                }
                a->mPosX = c->mLinkPos[5].x;
                a->mPosY = c->mLinkPos[5].y;
                a->mPosZ = c->mLinkPos[5].z;
                return;
            }
            c->mChainEndDone = 1;
            c->mChainEndActorID = 0;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov100_021437d4, 0x021437d4, size 0x2d0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_021437d4
/* The chain. It is anchored 250 units (0xfa000) behind the chomp along its
   rotation (data_020a0e68, MATRIX_SCRATCH_PAPER in symbols/verified.tsv, is
   built from mAngleX/Y/Z). The anchor is kept in a local; it is also written
   into mLinkPos[0], but the loop overwrites that. mChainTimer is then
   stepped, and each of the six links (positions mLinkPos, velocities
   mLinkVel, lower Y bounds mLinkMinY) is placed in turn 50 units (0x32000)
   from the point before it (the anchor for link 0, else the previous link's
   new position), in the direction from that point to where the link would
   drift to: its position plus its velocity, with Y lowered by 2 units
   (0x2000) and then moved by -SINE_TABLE[idx * 2] scaled by 10 units
   (0xa000, rounded, >> 12). idx is angle16 >> 4, with angle16 =
   (mChainTimer << 12) + 0x2000 * link: each step advances the phase 1/16 of a
   turn and each link is offset 1/8 of a turn from the one before. The
   drifted Y is not allowed below mLinkMinY[link]. The link's velocity then
   becomes its movement times 3000/4096 (0xbb8); for link 0 that is the
   movement from the anchor, not from its old position. mLinkMinY[link] becomes
   the chomp's Y minus 200 units (0xc8000), or the link's own new Y if that
   bound ended up more than 200 units above it. Was a C source; the
   materialised mChainTimer address and the `added` copy are the shapes the ROM
   carries. */
extern "C" void func_ov100_021437d4(daWanwan2_c *thisx)
{
    Vec3i in, out, cur, delta, added, v48, v54, v60;
    Vec3i *pos, *vel, *src;
    int *heights;
    int loop;
    int angOff;
    int idx, val, ang, hlen, angz;

    pos = (Vec3i *)thisx->mLinkPos;
    vel = (Vec3i *)thisx->mLinkVel;
    heights = thisx->mLinkMinY;

    in.z = -0xfa000; in.x = 0; in.y = 0;    /* 250 units behind */
    out.x = 0; out.y = 0; out.z = 0;

    Matrix4x3_FromRotationXYZExt(data_020a0e68, thisx->mAngleX, thisx->mAngleY, thisx->mAngleZ);
    MulVec3Mat4x3(&in, data_020a0e68, &out);
    Vec3_Add(&added, (Vec3i *)&thisx->mPosX, &out);

    {
        int ax = added.x, ay = added.y, az = added.z;
        int *p6a0;
        int zin;
        loop = 0;
        cur.x = ax; cur.y = ay; cur.z = az;
        thisx->mLinkPos[0].x = ax;
        ay = cur.y;
        p6a0 = (int *)(int)&thisx->mChainTimer;
        thisx->mLinkPos[0].y = ay;
        {
            int cz = cur.z;
            zin = 0x32000;     /* 50 units between links */
            thisx->mLinkPos[0].z = cz;
            {
                int c = *p6a0;
                *p6a0 = c + 1;
            }
        }
        in.z = zin;
        angOff = loop;
        in.x = 0;
        in.y = 0;
    }

    for (; loop < 6; ) {
        if (loop != 0) src = (Vec3i *)((char *)pos - 0xc);
        else src = &cur;
        delta.x = vel->x + (pos->x - src->x);
        delta.z = vel->z + (pos->z - src->z);
        idx = ((unsigned short)(short)(angOff + (thisx->mChainTimer << 12))) >> 4;
        val = (pos->y + vel->y) - 0x2000     /* 2 units down */
            + (int)(((s64)data_02082214[idx * 2] * (-0xa000) + 0x800) >> 12);
        if (val <= *heights) val = *heights;
        delta.y = val - src->y;
        ang = _ZN4cstd5atan2E5Fix12IiES1_(delta.x, delta.z);
        hlen = Vec3_HorzLen(&delta);
        angz = (s16)(-_ZN4cstd5atan2E5Fix12IiES1_(delta.y, hlen));
        Matrix4x3_FromRotationY(data_020a0e68, ang);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, angz);
        MulVec3Mat4x3(&in, data_020a0e68, &out);
        vel->x = pos->x; vel->y = pos->y; vel->z = pos->z;
        Vec3_Add(&v48, src, &out);
        pos->x = v48.x; pos->y = v48.y; pos->z = v48.z;
        Vec3_Sub(&v54, pos, vel);
        Vec3_MulScalar(&v60, &v54, 0xbb8);
        vel->x = v60.x; vel->y = v60.y; vel->z = v60.z;
        *heights = thisx->mPosY - 0xc8000;     /* 200 units below the chomp */
        if (*heights - pos->y > 0xc8000) *heights = pos->y;
        angOff += 0x2000;
        vel = (Vec3i *)((char *)vel + 0xc);
        pos = (Vec3i *)((char *)pos + 0xc);
        loop++;
        heights = heights + 1;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov100_02143aa4, 0x02143aa4, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02143aa4
/* The walking state's execute member (data_ov100_02147ff8, copied into
   data_ov100_021486f4.execute): advance the walk animation at full speed,
   then step the chain, the contact check and the chain-end actor. Formerly
   recovered as "RollingIronBall_Kill" / "daIbl_c::Kill, from vtable slot
   identity": the pointer-to-member constant that holds it follows daIbl_c's
   vtable directly, and that is the whole of the old claim. Was a C source. */
extern "C" int func_ov100_02143aa4(daWanwan2_c *c)
{
    c->mModelAnim.speed = 4096;     /* 1.0 */
    _ZN15dExtFrameCtrl_c7AdvanceEv((dExtFrameCtrl_c *)&c->mModelAnim);
    func_ov100_021437d4(c);
    func_ov100_0214344c(c);
    func_ov100_021435e8(c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov100_02143ae0, 0x02143ae0, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02143ae0
/* The walking state's enter member (data_ov100_02148000): start the walk
   animation from data_ov100_021486ac. Was a C source. */
extern "C" int func_ov100_02143ae0(daWanwan2_c *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&c->mModelAnim,
        *(void **)((char *)&data_ov100_021486ac + 4), 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov100_02143b18, 0x02143b18, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02143b18
/* Enter a state: store the pair in mStatePair (+0x668) and call its enter
   member, if it has one. */
extern "C" int func_ov100_02143b18(ChompPmfSelf *c, ChompState *p)
{
    c->state = p;
    ChompState *q = c->state;
    if (q->enter == 0) return 1;
    return (c->*(q->enter))();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov100_02143b68, 0x02143b68, size 0x148 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov100_02143b68
/* The per-frame matrices: the body model's rotation (from mAngleX/Y/Z) and
   position (mPos >> 3), its drop shadow (radius 350 units, depth 500 units,
   opacity 0xf), then for each of the six links an identity matrix at the
   link's position (mLinkPos >> 3) and a smaller drop shadow (radius 120
   units, depth 500 units). */
extern "C" void func_ov100_02143b68(char *self)
{
    daWanwan2_c *c = (daWanwan2_c *)self;
    M48 tmp;
    int i;
    Model *mdst;
    char *rd;
    char *st;
    dExtShadowModel_c *sm;
    Matrix4x3_FromRotationXYZExt(&c->mModelAnim.mat4x3, c->mAngleX, c->mAngleY, c->mAngleZ);
    c->mModelAnim.mat4x3.t.x = c->mPosX >> 3;
    c->mModelAnim.mat4x3.t.y = c->mPosY >> 3;
    c->mModelAnim.mat4x3.t.z = c->mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(c, &c->mShadowModel, &c->mModelAnim.mat4x3, 0x15e000, 0x1f4000, 0xf);
    tmp = *(M48 *)&IDENTITY_MATRIX4X3;
    i = 0;
    mdst = c->mModels;
    /* rd and st both start at the chomp itself and are stepped by one link
       (rd: 0xc, st: 0x50, a Model), so the offsets below are from the chomp:
       rd + 0x6d8 is mLinkPos[i] and st + 0x3b0 is mModels[i].mat4x3.t. */
    rd = (char *)c;
    st = (char *)c;
    sm = c->mShadowModels;
    for (; i < daWanwan2_NUM_LINKS; i++) {
        *(M48 *)&mdst->mat4x3 = tmp;
        *(int *)(st + 0x3b0) = *(int *)(rd + 0x6d8) >> 3;
        *(int *)(st + 0x3b4) = *(int *)(rd + 0x6dc) >> 3;
        *(int *)(st + 0x3b8) = *(int *)(rd + 0x6e0) >> 3;
        _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(c, sm, &mdst->mat4x3, 0x78000, 0x1f4000, 0xf);
        mdst++;
        rd += 0xc;
        st += 0x50;
        sm++;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN11daWanwan2_c16CleanupResourcesEv, 0x02143cb0, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWanwan2_c16CleanupResourcesEv
/* Releases the five shared files InitResources loaded and unloads the silver
   star and number resources. */
int daWanwan2_c::CleanupResources()
{
    data_ov002_0211092c.Release();
    data_ov100_021486bc.Release();
    data_ov100_021486a4.Release();
    data_ov100_021486ac.Release();
    data_ov100_021486b4.Release();
    UnloadSilverStarAndNumber();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- _ZN11daWanwan2_c16OnPendingDestroyEv, 0x02143d08, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWanwan2_c16OnPendingDestroyEv
void daWanwan2_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- _ZN11daWanwan2_c6RenderEv, 0x02143d0c, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWanwan2_c6RenderEv
/* recovered: real C++ method over the typed model members. The body is drawn
   with the actor's own scale (mScaleX/Y/Z) and five of the six link models
   are drawn. */
int daWanwan2_c::Render()
{
    mModelAnim.Render((Vector3 *)&mScaleX);
    for (int i = 0; i < 5; i++)
        mModels[i].Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- _ZN11daWanwan2_c8BehaviorEv, 0x02143d64, size 0x324 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWanwan2_c8BehaviorEv
/* While mHaltTimer is nonzero after its count-down (set to 90 by contact, so
   89 frames) the state's execute member is not run, the chomp's scale eases
   back to 1.0 (0x500 a frame), its horizontal speed is 0, and when the floor
   probe hits its fall limit (mTerminalVelocity) is set to 0, and the collision
   body is cleared and updated. Otherwise it sets the fall limit to
   -60 units/frame (-0x3c000), runs the current state's execute member, moves
   at 23 units/frame (0x17000), and when the floor probe hits plays
   SND_FLOOR_HIT (unless unk_6a8 is nonzero), sets its vertical speed to 20
   units/frame (0x14000) and kicks up landing dust. Every 200 frames
   (mCoinTimer) in game mode 1 (data_0209f2d8[0] is 1) it spawns a COIN at its
   position, its mPrevAngleY set to 0x10000 / mPathNodeIndex (truncated to 16
   bits; this divides by zero if the index has wrapped to 0), its mVertSpeed to
   4 units/frame and its unk_0a4 and unk_0ac to 0, playing SND_COIN_SPAWN
   (unless unk_6a8 is nonzero; played even if Spawn returned no actor). It then turns
   toward the path node mPathNodeIndex: when it is closer than 400 units
   (0x190000) the index advances (back to 0 at mNumPathNodes) and the next
   node is fetched; mTargetAngle is the horizontal angle to the node,
   mPrevAngleY eases toward it and mAngleY copies mPrevAngleY. The model
   matrices are rebuilt, and the collision body is updated only when a Player
   exists and is not vanished. */
int daWanwan2_c::Behavior()
{
    DecIfAbove0_Short((unsigned short *)&mCoinTimer);
    DecIfAbove0_Short((unsigned short *)&unk_6a8);
    if (DecIfAbove0_Short((unsigned short *)&mHaltTimer) != 0) {
        _Z14ApproachLinearRiii(mScaleX, 0x1000, 0x500);
        mScaleZ = mScaleX;
        mScaleY = mScaleZ;
        func_ov100_02143b68((char *)this);
        mHorzSpeed = 0;
        _ZN8dActor_c9UpdatePosEP5dCc_c(((dActor_c *)this), (dCc_c *)&mdCcAcPos_c);
        if (func_ov100_02143370((char *)this) != 0) {
            mTerminalVelocity = 0;
        }
        _ZN5dCc_c5ClearEv(&mdCcAcPos_c);
        _ZN5dCc_c6UpdateEv(&mdCcAcPos_c);
        return 1;
    }

    mTerminalVelocity = -0x3c000;

    {
        ChompState *q = (ChompState *)mStatePair;
        if (q->execute != 0) {
            (((ChompPmfSelf *)this)->*(q->execute))();
        }
    }

    mHorzSpeed = 0x17000;
    _ZN8dActor_c9UpdatePosEP5dCc_c(((dActor_c *)this), (dCc_c *)&mdCcAcPos_c);

    if (func_ov100_02143370((char *)this) != 0) {
        if (unk_6a8 == 0) {
            func_02012694(daWanwan2_SND_FLOOR_HIT, &mCamSpacePosX);
        }
        mVertSpeed = 0x14000;
        _ZN8dActor_c15HugeLandingDustEb(((dActor_c *)this), true);
    }

    int flag = (data_0209f2d8[0] == 1);
    if (flag != 0 && mCoinTimer == 0) {
        int q16 = __aeabi_idiv(0x10000, mPathNodeIndex);
        short spd = (short)q16;
        dActor_c *pl = _ZN8dActor_c13ClosestPlayerEv(((dActor_c *)this));
        (void)pl;

        volatile Vector3 v;
        v.x = 0;
        v.y = 4;
        v.z = 0;

        dActor_c *sp = (dActor_c *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            daWanwan2_ACTOR_COIN, 2, (Vector3 *)&mPosX, (const Vector3_16 *)0,
            mAreaId, -1);
        if (unk_6a8 == 0) {
            func_02012694(daWanwan2_SND_COIN_SPAWN, &mCamSpacePosX);
        }
        if (sp != 0) {
            sp->mPrevAngleX = 0;
            sp->mPrevAngleY = spd;
            sp->mPrevAngleZ = 0;
            int vx = v.x;
            int vy = v.y;
            sp->unk_0a4 = vx << 12;
            sp->mVertSpeed = vy << 12;
            sp->unk_0ac = vx << 12;
        }
        mCoinTimer = 0xc8;
    }

    {
        char path[8];
        Vector3 node;
        Vector3 diff;
        _ZN7PathPtrC1Ev(path);
        _ZN7PathPtr6FromIDEj(path, mPathID);
        _ZNK7PathPtr7GetNodeER7Vector3j(path, node, mPathNodeIndex);

        Vec3_Sub(&diff, (Vector3 *)&mPosX, &node);

        if (LenVec3(&diff) < 0x190000) {
            mPathNodeIndex += 1;
            if (mPathNodeIndex >= mNumPathNodes) {
                mPathNodeIndex = 0;
            }
            _ZNK7PathPtr7GetNodeER7Vector3j(path, node, mPathNodeIndex);
        }

        short ang = Vec3_HorzAngle((Vector3 *)&mPosX, &node);
        mTargetAngle = ang;

        ApproachAngle(&mPrevAngleY, mTargetAngle, 0x10, 0x20, 0x500);

        mAngleY = mPrevAngleY;

        func_ov100_02143b68((char *)this);
        _ZN5dCc_c5ClearEv(&mdCcAcPos_c);

        dActor_c *p = _ZN8dActor_c13ClosestPlayerEv(((dActor_c *)this));
        if (p != 0 && ((Player *)p)->mIsVanish == 0) {
            _ZN5dCc_c6UpdateEv(&mdCcAcPos_c);
        }
        return 1;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- _ZN11daWanwan2_c13InitResourcesEv, 0x02144088, size 0x24c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWanwan2_c13InitResourcesEv
/* Loads the files (data_ov100_021486bc is the body model, loaded into
   mModelAnim; data_ov100_021486a4 the link model, set on each mModels[i];
   data_ov100_021486ac the walk animation the enter member plays;
   data_ov100_021486b4 is loaded and released here and used nowhere else in
   this source; data_ov002_0211092c is not identified), sets up the shadows,
   reads the path ID and spawn parameter out of param1, sets gravity and the
   collision cylinder, seeds the links, the coin timer and the path, and
   enters the walking state. */
int daWanwan2_c::InitResources()
{
    Model::LoadFile(data_ov002_0211092c);
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov100_021486bc), 1, -1);
    Model::LoadFile(data_ov100_021486a4);
    dExtFrameCtrl_c::LoadFile(data_ov100_021486ac);
    dExtFrameCtrl_c::LoadFile(data_ov100_021486b4);
    LoadSilverStarAndNumber();

    {
        int i = 0;
        Model *model = mModels;
        do {
            model->SetFile(*(BMD_File **)((char *)&data_ov100_021486a4 + 4), 1, -1);
            i++;
            model++;
        } while (i < daWanwan2_NUM_LINKS);
    }

    /* The body's drop shadow, then one per link. */
    mShadowModel.InitCylinder();
    {
        int i = 0;
        dExtShadowModel_c *shadow = mShadowModels;
        do {
            shadow->InitCylinder();
            i++;
            shadow++;
        } while (i < daWanwan2_NUM_LINKS);
    }

    mPathID = param1 & 0xff;
    mSpawnParam = (param1 >> 8) & 0xf;
    if (mPathID == 0xff)
        mPathID = 0;

    {
        PathPtr path;
        path.FromID(mPathID);
        mNumPathNodes = path.NumNodes();
    }

    mVertAccel = -0x2000;           /* gravity: -2 units/frame per frame */
    mTerminalVelocity = -0x3c000;   /* -60 units/frame */

    {
        Vector3 offset;
        offset.x = data_ov100_02148008[0];
        offset.y = data_ov100_02148008[1];
        offset.z = data_ov100_02148008[2];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            &mdCcAcPos_c, this, offset,
            0xaa000, 0x140000, 0x200004, 0x6010);   /* radius 170, height 320 units */
    }

    unk_6c9 = 0x1f;
    unk_6cc = 3;

    mAngleY = mPrevAngleY;
    mTargetAngle = mAngleY;

    mChainEndActorID = 0;
    unk_6d4 = 0;

    /* Every link starts at the position it was placed at. */
    {
        int i = 0;
        char *position = (char *)this;
        do {
            *(s32 *)(position + 0x6d8) = mPosX;     /* mLinkPos[i] */
            i++;
            *(s32 *)(position + 0x6dc) = mPosY;
            *(s32 *)(position + 0x6e0) = mPosZ;
            position += sizeof(Vector3);
        } while (i < daWanwan2_NUM_LINKS);
    }

    mCoinTimer = 0xc8;

    /* Start at node 1: GetNode writes that node's position over mPosX/Y/Z,
       after the links were seeded above from the placed position. */
    {
        PathPtr path;
        path.FromID(mPathID);
        mPathNodeIndex = 1;
        path.GetNode(*(Vector3 *)&mPosX, mPathNodeIndex);
    }

    /* Raise it 100 units (0x64000). Preserve the ROM's materialized
       read-modify-write address for mPosY. */
    *(s32 *)((int)this + 0x60) += 0x64000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    /* Start in the walking state. */
    func_ov100_02143b18((ChompPmfSelf *)this, &data_ov100_021486f4);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN11daWanwan2_c16OnAimedAtWithEggEv, 0x021442d4, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWanwan2_c16OnAimedAtWithEggEv
/* recovered from vtable slot identity (slot 29); historical alias
   UnchainedChomp_OnAimedAtWithEgg. Returns 0. */
s32 daWanwan2_c::OnAimedAtWithEgg()
{
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- daWanwan2_c_classInit, 0x021442dc, size 0x118 */
/* -------------------------------------------------------------------------- */
// @symbol daWanwan2_c_classInit
/* local extern: the factory below spells the constructor chain by hand (see
 * its comment), so it names each constructor, destructor and array callback
 * by its mangled symbol rather than through the class headers. */
extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
dEnemyBase_c *_ZN12dEnemyBase_cC2Ev(dEnemyBase_c *object);
dCcAcPos_c *_ZN10dCcAcPos_cC1Ev(dCcAcPos_c *object);
dBgCh_Actr *_ZN10dBgCh_ActrC1Ev(dBgCh_Actr *object);
ModelAnim *_ZN9ModelAnimC1Ev(ModelAnim *object);
dExtShadowModel_c *_ZN17dExtShadowModel_cC1Ev(dExtShadowModel_c *object);
void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
    void (*ctor)(void *), void (*dtor)(void *));
extern void *_ZTV11daWanwan2_c[];
Model *_ZN5ModelC1Ev(Model *object);
Model *_ZN5ModelD1Ev(Model *object);
dExtShadowModel_c *_ZN17dExtShadowModel_cD1Ev(dExtShadowModel_c *object);
Vector3 *_ZN7Vector3D1Ev(Vector3 *object);
void func_0203d384(void);
Vector3s *_ZN8Vector3sD1Ev(Vector3s *object);
void func_0203d73c(void);
}

/* Reconstructed source-style name: SM64DS proves daWanwan2_c through RTTI,
 * allocation size, vtable identity, and the WANWAN2 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: UnchainedChomp_Spawn.
 *
 * Spelled by hand rather than `return new daWanwan2_c;`, which comes out
 * 0xa4 bytes for the ROM's 0x118: the ROM constructs mLinkPos and mLinkVel
 * through __cxa_vec_ctor(..., func_0203d384, _ZN7Vector3D1Ev) and mUnk_768
 * through __cxa_vec_ctor(..., func_0203d73c, _ZN8Vector3sD1Ev), with an empty
 * constructor function, and types.h's Vector3 and Vector3s declare no
 * constructor, so the implicit one never emits those three calls. The array
 * callbacks receive the element address and discard lifecycle results; the
 * empty func_0203d384/func_0203d73c callbacks ignore that address. This TU
 * emits the vtable, whose symbol names the vtable object two words ahead of
 * the slot array, so the vptr store reads &_ZTV11daWanwan2_c[2]. */
extern "C" daWanwan2_c *daWanwan2_c_classInit()
{
    daWanwan2_c *actor =
        (daWanwan2_c *)_ZN7fBase_cnwEj(sizeof(daWanwan2_c));
    if (actor) {
        _ZN12dEnemyBase_cC2Ev(actor);
        *(void **)actor = &_ZTV11daWanwan2_c[2];
        _ZN10dCcAcPos_cC1Ev(&actor->mdCcAcPos_c);
        _ZN10dBgCh_ActrC1Ev(&actor->mWithMeshClsn);
        _ZN9ModelAnimC1Ev(&actor->mModelAnim);
        __cxa_vec_ctor(actor->mModels, 6, sizeof(Model),
            (void (*)(void *))_ZN5ModelC1Ev, (void (*)(void *))_ZN5ModelD1Ev);
        __cxa_vec_ctor(actor->mShadowModels, 6, sizeof(dExtShadowModel_c),
            (void (*)(void *))_ZN17dExtShadowModel_cC1Ev, (void (*)(void *))_ZN17dExtShadowModel_cD1Ev);
        _ZN17dExtShadowModel_cC1Ev(&actor->mShadowModel);
        __cxa_vec_ctor(actor->mLinkPos, 6, sizeof(Vector3),
            (void (*)(void *))func_0203d384, (void (*)(void *))_ZN7Vector3D1Ev);
        __cxa_vec_ctor(actor->mLinkVel, 6, sizeof(Vector3),
            (void (*)(void *))func_0203d384, (void (*)(void *))_ZN7Vector3D1Ev);
        __cxa_vec_ctor(actor->mUnk_768, 6, sizeof(Vector3s),
            (void (*)(void *))func_0203d73c, (void (*)(void *))_ZN8Vector3sD1Ev);
    }
    return actor;
}
