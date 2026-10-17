//cpp
/* Production translation unit for ov098/daObjBlockS_c.
 * 37 functions, .text 0x02138040..0x02139f70. The small breakable crate
 * (registry profile BLOCK_S): it sits where it is placed, can be picked up
 * and thrown by a player, spat out or swallowed, slides and bounces on
 * the floor, sinks into quicksand, and breaks (paying out three coins once)
 * on a kick, a mega character, an explosion, a fire-labelled hit, water,
 * a wall belonging to an actor other than a BLOCK_L, or hitting a wall faster
 * than 20 units per frame (this list is not exhaustive: a ground pound,
 * a fall faster than 11 units per frame, a DOSUN / DONKAKU / DONGURU / BATAN /
 * ONIMASU touching it while idle and the first cylinder's hit bits also
 * kill it; quicksand and the off-screen respawn path send it to STATE_BROKEN
 * directly, without coins).
 *
 * NAME: _ZTS13daObjBlockS_c is "13daObjBlockS_c" at ov098 0x0213c500; _ZTI at
 * 0x0213c4d4 reads [__si_class_type_info, that string, _ZTI10dBgActor_c], and
 * the word before the _ZTV13daObjBlockS_c address point (0x0213c534) points at
 * that _ZTI. The tree previously called the class Crate (coined).
 *
 * The unit runs from the destructor pair (D1 0x02138040, D0 0x021380bc) to
 * daObjBlockS_c_classInit (0x02139f04). The out-of-line destructor is the key
 * function, so this TU emits the vtable and the RTTI chain; D2 has no ROM home.
 *
 * Crate_SetState and func_ov098_02138b70 run the state table at
 * data_ov098_0213c878: one {enter, update} member-pointer pair per state, and
 * mState (0x560) is the index. The state functions (0x0213814c..0x02138b18
 * and the helpers after them) are methods named func_ov098_<addr>. The first
 * argument is this.
 *
 * Under `#pragma defer_codegen off` .text is laid down in source order, so
 * this file is ROM-ascending.
 *
 * STATES. mState (daObjBlockS_c::State) indexes the table at
 * data_ov098_0213c878, built by __sinit_ov098_0213c058 from fourteen
 * member-pointer words in .data (0x0213c458..0x0213c4c0) into seven
 * {enter, update} pairs. Rows read from the ROM:
 *   0 IDLE       enter 02138b18  update 021389f8
 *   1 CARRIED    enter 021389cc  update 021388bc
 *   2 THROWN     enter 02138818  update 02138734
 *   3 SPIT_OUT   enter 021385e0  update 021384fc
 *   4 BOUNCING   enter 02138484  update 02138344
 *   5 IN_MOUTH   enter 02138318  update 02138238
 *   6 BROKEN     enter 021381e8  update 0213814c
 * The names describe what the handlers do; the ROM stores none. The unnamed
 * helpers (reset, hit handling, wall test, slide step, shadow, collider
 * transform, model matrix) are commented where they are defined.
 *
 * Leftover: Crate_SetState at 0x02138b28 (size 0x48) stays a free function.
 *   It takes the crate plus an int, already has a recovered name, and was
 *   not turned into a method. Call sites stay Crate_SetState(this, state).
 * Leftover: no member conversion was reverted. Each of the 25 helpers
 *   matched at its original size.
 * Leftover: func_ov002_020e496c, func_ov002_020ef228, func_ov002_020f02c8,
 *   func_ov002_020f030c and func_ov002_020f035c stay calls into ov002.
 * Leftover: the word at 0x0d0 (mEatingPlayer) lives in dBgActor_c's pad and is
 *   reached by the BLOCKS_EATING_PLAYER macro below.
 * Leftover: mFlags bits 0x100 / 0x400 / 0x2000 / 0x4000 / 0x80000 / 0x4000000 and
 *   dCc_c flags bits 0x2000 / 0x8000 are not named in dActor_c.h / dCc_c.h;
 *   they are written as numbers and described by what reading them does.
 * Leftover: sound banks 0x41 / 0x51, the PlayLong id 0x93 and the particle ids
 *   0xe / 0x13a / 0x13b have no names in the tree.
 * Leftover: what func_02035638 tests beyond being a sibling of IsOnWall, what
 *   func_ov002_020ef228 / func_ov002_020e496c compute, the result of
 *   func_ov002_020f030c (Player.h names it a slide loss factor; the value
 *   is discarded here), the player word at
 *   0xc8, the extra arguments of Math_Function_0203b14c and
 *   IsClsnInRangeOnScreen, the 0x199 passed to dBgW_KcMbg::SetFile, and what
 *   Player::Hurt's trailing arguments select.
 * Leftover: three reads of another actor's id still go through `(char *)a + 0xc`
 *   (dActor_c's actorID field), and the SurfaceInfo inside a dBgCh_Actr
 *   result is reached as `(char *)fr + 4`; both stay as they were.
 * Leftover: the state names STATE_IN_MOUTH and STATE_SPIT_OUT rest on how the
 *   handlers use mFlags bit 0x80000 and the 0x20000 / 0x40000 bits (nothing
 *   in this file sets 0x80000); mSlideSoundHandle rests on PlayLong's id
 *   0x93 being played with the sliding-dust particle, and that id has no name.
 * Leftover: func_ov098_02138818 clamps its table index to 4 although the two copied
 *   tables hold four words.
 * Leftover: the slide step compares mClsnYOffsetTarget with 0xa0, which the values
 *   stored there never equal.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjBlockS_c_AssetId {
    ASSET_BROKEN_BLOCK_S_BMD = 0x0458,
    ASSET_BROKEN_BLOCK_S_KCL = 0x0459
};

#include "decl_Actor.h"
#include "nitro/fx/fx.h"
#include "decl_common.h"
#include "daObjBlockS_c.h"
#include "dActor_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Actr.h"
#include "dBgCh_Gnd.h"
#include "SurfaceInfo.h"
#include "dCc_c.h"

struct Vector3_16f;

/* Actor profile IDs this file compares against (symbols/actor_debug_names.tsv). */
enum {
    ACTOR_BLOCK_L   = 15,
    ACTOR_DOSUN     = 161,
    ACTOR_DONKAKU   = 162,
    ACTOR_DONGURU   = 163,
    ACTOR_BATAN     = 164,
    ACTOR_PLAYER    = 191,
    ACTOR_BLOCK_S   = 194,
    ACTOR_ONIMASU   = 309
};

/* dActor_c::mFlags bits (include/dActor_c.h): 0x8 is the framework's "off
   screen" bit; 0x20000 / 0x40000 are the yoshi-mouth bits that include/
   dActor_c.h says actor code writes. */
enum {
    ACTOR_FLAG_OFF_SCREEN     = 0x8,
    ACTOR_FLAG_YOSHI_MOUTH_A  = 0x20000,
    ACTOR_FLAG_YOSHI_MOUTH_B  = 0x40000
};

/* Bits of the first cylinder's hitFlags (dCc_c::hitFlags), read through the
   best-effort bit table in include/dCc_c.h; only the explosion bit is
   marked proven there. */
enum {
    CC_HIT_MEGA_CHAR  = 0x10,
    CC_HIT_KICKS      = 0x380,      /* kick | breakdance | slide kick */
    CC_HIT_GRAB       = 0x1000,
    CC_HIT_EXPLOSION  = 0x4000,
    CC_HIT_FIRE       = 0x40000
};

/* The word at 0x0d0 is dBgActor_c's pad, not a member of this class; the
   other actor headers name it mEatingPlayer. */
#define BLOCKS_EATING_PLAYER(o) (*(Player **)((char *)(o) + 0xd0))

extern "C" {
extern u32 data_0209b454;
extern int data_ov098_0213c4c8[];
extern s16 data_02082214[];
extern int Vec3_HorzDist(const struct Vector3 *a, const struct Vector3 *b);
extern u8 DecIfAbove0_Byte(u8 *p);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int c, int d, int e, const void *v, void *cb);
extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    u32 a, u32 b, int c, int d, int e, const Vector3_16f *v);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(s32 x, s32 y, s32 z);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    char *thiz, int f, char *m, int fix, short s, int blk);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    char *thiz, char *actor, int b, int d, void *v, int f);

extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, void *pos, unsigned int n, int speed, short ang);
extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *self, int a, int b, short c);
extern void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mtx, int fix, int t1, int t2, unsigned int n);
extern int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *thiz, void *actor, void *vec, int fix, int t, unsigned u1, unsigned u2);

extern void dBgCh_Actr_UpdateContinuous_Veneer(void *p);
extern int SurfaceInfo_TestFlag0x20(int *p);
extern int func_0203567c(int p);
extern int func_02037e38(u32 *p);
extern int func_02037e58(u32 *p);
extern int func_ov002_020ef228(void *c, int arg);
extern int func_ov002_020f030c(int x);
extern int func_ov002_020e496c(char *c);
extern s16 func_02010844(void *unused, Vector3 *v, s16 angle);

extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *p, struct Vector3 *pos, u32 a, int b, u32 d, u32 e, u32 f);

extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, const struct Vector3 *pos);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, const Vector3 *pos, u32 e);

extern int _Z14ApproachLinearRiii(int *a, int b, int c);
extern void _Z11UpdateAngleRssis(s16 *a, s16 b, int c, s16 d);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern int _ZN4cstd4fdivEii(int a, int b);
extern s32 Vec3_HorzLen(const Vector3 *v0);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Math_Function_0203b14c(char *dst, int a, int b, int c, int d);

void Crate_SetState(daObjBlockS_c *c, int i);
}

enum Bool { FALSE, TRUE };

struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(int);
};

/* The state table: data_ov098_0213c878 holds one {enter, update} pair of
   member pointers per state, indexed by mState (the word at 0x560). The host
   struct is an opaque stand-in for daObjBlockS_c so the member-pointer calls
   stay plain `this`-adjusted calls. */
struct BlockSStateHost;
typedef void (BlockSStateHost::*BlockSStateFn)();
struct BlockSStateEntry { BlockSStateFn fn[2]; };
extern BlockSStateEntry data_ov098_0213c878[];
struct BlockSStateHost { char pad[0x560]; int idx; };

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model, func_02017b4c /
 * SharedFilePtr_Destruct_Clsn for the collision), spelled through
 * declared-only subclasses so the static initializer names the real entry
 * points. */
struct BlockSModelFilePtr : SharedFilePtr {
    u32 words[2];
    BlockSModelFilePtr(u32 fileID);
    ~BlockSModelFilePtr();
};
struct BlockSCollisionFilePtr : SharedFilePtr {
    u32 words[2];
    BlockSCollisionFilePtr(u32 fileID);
    ~BlockSCollisionFilePtr();
};
extern BlockSModelFilePtr gBrokenBlockSModelFile;
extern BlockSCollisionFilePtr gBrokenBlockSCollisionFile;

/* Four-word copy source for the thrown state's speed tables. */
struct BlockSW4 { u32 w[4]; };
extern "C" BlockSW4 data_ov098_0213c4e0;
extern "C" BlockSW4 data_ov098_0213c4f0;

#pragma defer_codegen off

// @symbol _ZN13daObjBlockS_cD1Ev
// @symbol _ZN13daObjBlockS_cD0Ev
/* Destroys dCcAcPos_c x2, dExtShadowModel_c and dBgCh_Actr in reverse declaration
 * order, then dBgActor_c's inline destructor stores its own vptr and destroys
 * its dBgW_KcMbg and Model before chaining to dActor_c. D0 then returns the
 * object through the inline operator delete. */
daObjBlockS_c::~daObjBlockS_c()
{
}

// @symbol _ZN13daObjBlockS_c19func_ov098_0213814cEv
/* STATE_BROKEN, update. Every frame it puts the crate back at its home (the
 * reset helper), keeps the collider disabled and refreshes the model matrix
 * and collider transform. Once the crate is off screen (mFlags 0x8) and more
 * than 2000 units (0x7d0000) from the closest player, it respawns as
 * STATE_IDLE. Returns before touching the cylinders in that case. */
void daObjBlockS_c::func_ov098_0213814c()
{
    func_ov098_02138ce0();
    unsigned b = (unsigned)((mFlags & ACTOR_FLAG_OFF_SCREEN) != 0);
    if (b != 0 && DistToCPlayer() > 0x7d0000) {
        Crate_SetState(this, daObjBlockS_c::STATE_IDLE);
        return;
    }
    mdCcAcPos_c1.Clear();
    mdCcAcPos_c2.Clear();
    func_ov098_02139850();
    func_ov098_021397c8();
    if (((dBgW *)&mMeshCollider)->IsEnabled())
        ((dBgW *)&mMeshCollider)->Disable();
}

// @symbol _ZN13daObjBlockS_c19func_ov098_021381e8Ev
/* STATE_BROKEN, enter. Disables the collider, resets the crate to its home
 * and refreshes the model matrix and collider transform. Clears mFlags bits
 * 0x20000 / 0x40000 / 0x80000 (the two yoshi-mouth bits and 0x80000, which
 * STATE_IN_MOUTH treats as "spit it out"). */
void daObjBlockS_c::func_ov098_021381e8()
{
    if (((dBgW *)&mMeshCollider)->IsEnabled())
        ((dBgW *)&mMeshCollider)->Disable();
    func_ov098_02138ce0();
    func_ov098_02139850();
    func_ov098_021397c8();
    mFlags &= ~(ACTOR_FLAG_YOSHI_MOUTH_A | ACTOR_FLAG_YOSHI_MOUTH_B | 0x80000);
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138238Ev
/* STATE_IN_MOUTH, update. mFlags 0x80000 sends the crate to STATE_SPIT_OUT;
 * with both yoshi-mouth bits (0x20000 / 0x40000) clear it goes to
 * STATE_BOUNCING. While 0x20000 is clear and mEatingPlayer is set, the crate
 * copies that player's position. Then clears the first cylinder, refreshes
 * the model matrix and keeps the collider disabled. */
void daObjBlockS_c::func_ov098_02138238()
{
    unsigned int flags = mFlags;
    int t1;
    t1 = flags & 0x80000;
    t1 = t1 != 0;

    if (t1 != 0) {
        Crate_SetState(this, daObjBlockS_c::STATE_SPIT_OUT);
    } else {
        int t2;
        t2 = flags & ACTOR_FLAG_YOSHI_MOUTH_A;
        t2 = t2 != 0;
        if (t2 == 0) {
            int t3;
            t3 = flags & ACTOR_FLAG_YOSHI_MOUTH_B;
            t3 = t3 != 0;
            if (t3 == 0) {
                Crate_SetState(this, daObjBlockS_c::STATE_BOUNCING);
            }
        }
    }

    {
        unsigned int flags2 = mFlags;
        int t4;
        t4 = flags2 & ACTOR_FLAG_YOSHI_MOUTH_A;
        t4 = t4 != 0;
        if (t4 == 0) {
            Player *p = BLOCKS_EATING_PLAYER(this);
            if (p != 0) {
                int *src = (int *)&p->mPosX;
                mPosX = src[0];
                mPosY = src[1];
                mPosZ = src[2];
            }
        }
    }

    mdCcAcPos_c1.Clear();
    func_ov098_02139850();
    if (!((dBgW *)&mMeshCollider)->IsEnabled()) return;
    ((dBgW *)&mMeshCollider)->Disable();
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138318Ev
/* STATE_IN_MOUTH, enter. Drops the sink offset and its target to 0, stops
 * the horizontal motion and clears dCc_c flags bit 0x2000 on the first
 * cylinder (the bit the thrown / spit-out enters set). */
void daObjBlockS_c::func_ov098_02138318()
{
    mClsnYOffsetTarget = 0;
    mClsnYOffset = 0;
    mHorzSpeed = 0;
    mdCcAcPos_c1.flags &= ~0x2000;
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138344Ev
/* STATE_BOUNCING, update. Moves the crate (UpdatePos, then the
 * collision step) and slows its horizontal speed by 1/3 unit per frame
 * (0x555). Each frame on the ground it plays sound bank 0x51, counts
 * mBounceCount down and relaunches the crate: vertical speed 10 units
 * (0xa000) and horizontal speed 5 units (0x5000) per bounce left. Once the
 * horizontal speed and the count are both 0 it returns to STATE_IDLE. The
 * rest of the frame is the shared tail: slide step, hit handling, wall test,
 * water kill, cylinders, model matrix, and the shadow while airborne and
 * the collider transform while the collider is in range on screen. */
void daObjBlockS_c::func_ov098_02138344()
{
    UpdatePos(&mdCcAcPos_c1);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    _Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x555);
    if (mWithMeshClsn.IsOnGround() != 0) {
        _ZN5Sound9PlayBank3EjRK7Vector3(0x51, &mCamSpacePos);
        DecIfAbove0_Byte(&mBounceCount);
        mVertSpeed = mBounceCount * 0xa000;
        mHorzSpeed = mBounceCount * 0x5000;
    }
    if (mHorzSpeed == 0 && mBounceCount == 0) {
        Crate_SetState(this, daObjBlockS_c::STATE_IDLE);
    }
    func_ov098_02139228();
    func_ov098_02138e6c();
    func_ov098_021390ec();
    if (func_ov098_02138bb8() != 0) {
        Kill();
    }
    mdCcAcPos_c1.Clear();
    mdCcAcPos_c1.Update();
    func_ov098_02139850();
    if (mWithMeshClsn.IsOnGround() == 0) {
        func_ov098_021396a4();
    }
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0) != 0) {
        func_ov098_021397c8();
    }
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138484Ev
/* STATE_BOUNCING, enter. Zeroes the sink offset and target, clears mFlags
 * bit 0x80000, starts mBounceCount at 3 (vertical speed 30 units, horizontal
 * speed 15 units to begin with) and forgets mEatingPlayer and
 * mHoldingPlayer. Clears dCc_c flags bit 0x2000 on the first cylinder. */
void daObjBlockS_c::func_ov098_02138484()
{
    mClsnYOffsetTarget = 0;
    mClsnYOffset = 0;
    mFlags &= ~0x80000;
    mBounceCount = 3;
    mVertSpeed = mBounceCount * 0xa000;
    mHorzSpeed = mBounceCount * 0x5000;
    BLOCKS_EATING_PLAYER(this) = 0;
    mHoldingPlayer = 0;
    mdCcAcPos_c1.flags &= ~0x2000;
}

// @symbol _ZN13daObjBlockS_c19func_ov098_021384fcEv
/* STATE_SPIT_OUT, update. The same frame as STATE_THROWN's: move, run the
 * slide step, and go to STATE_IDLE as soon as that reports the crate on the
 * ground; otherwise ease the horizontal speed down by 1/3 unit per frame
 * (0x555) and run the shared tail (hit handling, wall test, water kill,
 * cylinders, model matrix, shadow while airborne). The collider is kept
 * disabled throughout. */
void daObjBlockS_c::func_ov098_021384fc()
{
    UpdatePos(&mdCcAcPos_c1);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (func_ov098_02139228()) {
        Crate_SetState(this, daObjBlockS_c::STATE_IDLE);
        return;
    }
    _Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x555);
    func_ov098_02138e6c();
    func_ov098_021390ec();
    if (func_ov098_02138bb8()) {
        Kill();
    }
    mdCcAcPos_c1.Clear();
    mdCcAcPos_c1.Update();
    func_ov098_02139850();
    if (!mWithMeshClsn.IsOnGround()) {
        func_ov098_021396a4();
    }
    if (((dBgW *)&mMeshCollider)->IsEnabled()) {
        ((dBgW *)&mMeshCollider)->Disable();
    }
}

// @symbol _ZN13daObjBlockS_c19func_ov098_021385e0Ev
/* STATE_SPIT_OUT, enter. Zeroes the sink offset and target, then puts the
 * crate 80 units ahead of mEatingPlayer along that player's Y angle (the x
 * offset is table word 0 and the z offset table word 1 of data_02082214 at
 * that angle, scaled by 0x50000; the table holds {0, 0x1000} for angle 0, so
 * word 0 is the sine and word 1 the cosine) and 80 units
 * above it, facing the same way (mAngleY, mPrevAngleY). Clears mFlags bit
 * 0x80000, launches it with horizontal speed 30 units (0x1e000) and vertical
 * speed 15 units (0xf000), hands mEatingPlayer over to mPrevHolder and sets
 * dCc_c flags bit 0x2000 on the first cylinder. */
void daObjBlockS_c::func_ov098_021385e0()
{
    Player *obj;
    short angle;
    int idx;
    int xw, zw;

    mClsnYOffsetTarget = 0;
    mClsnYOffset = 0;

    obj = BLOCKS_EATING_PLAYER(this);
    angle = obj->mAngleY;
    mAngleY = angle;
    mPrevAngleY = mAngleY;

    obj = BLOCKS_EATING_PLAYER(this);
    {
        int *osrc = (int *)&obj->mPosX;
        mPosX = osrc[0];
        mPosY = osrc[1];
        mPosZ = osrc[2];
    }

    idx = (unsigned short)mAngleY >> 4;
    xw = data_02082214[idx * 2];
    mPosX = mPosX + FX_MUL(xw, 0x50000);

    mPosY = mPosY + 0x50000;

    idx = (unsigned short)mAngleY >> 4;
    zw = data_02082214[idx * 2 + 1];
    mPosZ = mPosZ + FX_MUL(zw, 0x50000);

    mFlags &= ~0x80000;

    mHorzSpeed = 0x1e000;
    mVertSpeed = 0xf000;
    mPrevHolder = BLOCKS_EATING_PLAYER(this);
    BLOCKS_EATING_PLAYER(this) = 0;
    mdCcAcPos_c1.flags |= 0x2000;
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138734Ev
/* STATE_THROWN, update. The same frame as STATE_SPIT_OUT's: move, run the
 * slide step and go to STATE_IDLE as soon as that reports the crate on the
 * ground; otherwise ease the horizontal speed down by 1/3 unit per frame
 * (0x555) and run the shared tail. The collider is kept disabled. */
void daObjBlockS_c::func_ov098_02138734()
{
    UpdatePos(&mdCcAcPos_c1);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (func_ov098_02139228()) {
        Crate_SetState(this, daObjBlockS_c::STATE_IDLE);
        return;
    }
    _Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x555);
    func_ov098_02138e6c();
    func_ov098_021390ec();
    if (func_ov098_02138bb8()) {
        Kill();
    }
    mdCcAcPos_c1.Clear();
    mdCcAcPos_c1.Update();
    func_ov098_02139850();
    if (!mWithMeshClsn.IsOnGround()) {
        func_ov098_021396a4();
    }
    if (((dBgW *)&mMeshCollider)->IsEnabled()) {
        ((dBgW *)&mMeshCollider)->Disable();
    }
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138818Ev
/* STATE_THROWN, enter. Zeroes the sink offset and target and launches the
 * crate: the player in mHoldingPlayer picks an index (its param1, which
 * daMky_c compares with Player::mCharacter, clamped to 4; 0 when nobody
 * holds the crate) into two four-word tables copied from
 * data_ov098_0213c4e0 (horizontal speed: 40, 40, 80, 30 units) and
 * data_ov098_0213c4f0 (vertical speed: 20, 20, 20, 15 units); an index of 4
 * would read one word past each copy. mHoldingPlayer moves to mPrevHolder
 * and is cleared, and dCc_c flags bit 0x2000 is set on the first cylinder. */
void daObjBlockS_c::func_ov098_02138818()
{
    BlockSW4 arr1;
    BlockSW4 arr2;
    Player *p;
    u32 idx;

    mClsnYOffsetTarget = 0;
    mClsnYOffset = 0;

    arr1 = data_ov098_0213c4e0;
    arr2 = data_ov098_0213c4f0;

    p = mHoldingPlayer;
    idx = 0;
    if (p != 0) {
        idx = p->param1;
        if (idx > 4) idx = 4;
    }

    mHorzSpeed = arr1.w[idx];
    mVertSpeed = arr2.w[idx];

    mPrevHolder = mHoldingPlayer;
    mHoldingPlayer = 0;

    mdCcAcPos_c1.flags |= 0x2000;
}

// @symbol _ZN13daObjBlockS_c19func_ov098_021388bcEv
/* STATE_CARRIED, update. Runs the collision step, and kills the crate (after
 * telling the carrying player to drop it) if it is standing on water. Then
 * follows three mFlags bits whose meaning is not recovered: 0x400 set goes
 * to STATE_THROWN, else 0x2000 set goes to STATE_BOUNCING, else 0x100 clear
 * goes back to STATE_IDLE. Clears the
 * first cylinder, refreshes the model matrix and the shadow, and keeps the
 * collider disabled. */
void daObjBlockS_c::func_ov098_021388bc()
{
    int flags;
    bool t;

    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (mWithMeshClsn.IsOnGround() != 0) {
        if (SurfaceInfo_TestFlag0x20((int *)((char *)mWithMeshClsn.GetFloorResult() + 4)) != 0) {
            Player *p = mHoldingPlayer;
            if (p != 0) {
                p->DropActor();
                Kill();
                return;
            }
        }
    }

    flags = mFlags;
    t = flags & 0x400;
    if (t != false) {
        Crate_SetState(this, daObjBlockS_c::STATE_THROWN);
    } else {
        t = flags & 0x2000;
        if (t != false) {
            Crate_SetState(this, daObjBlockS_c::STATE_BOUNCING);
        } else {
            t = flags & 0x100;
            if (t == false) {
                Crate_SetState(this, daObjBlockS_c::STATE_IDLE);
            }
        }
    }

    mdCcAcPos_c1.Clear();
    func_ov098_02139850();
    func_ov098_021396a4();
    if (((dBgW *)&mMeshCollider)->IsEnabled()) {
        ((dBgW *)&mMeshCollider)->Disable();
    }
}

// @symbol _ZN13daObjBlockS_c19func_ov098_021389ccEv
/* STATE_CARRIED, enter. Stops the horizontal motion, clears dCc_c flags bit
 * 0x2000 on the first cylinder and zeroes the sink offset and target. */
void daObjBlockS_c::func_ov098_021389cc()
{
    mHorzSpeed = 0;
    mdCcAcPos_c1.flags &= ~0x2000;
    mClsnYOffsetTarget = 0;
    mClsnYOffset = 0;
}

// @symbol _ZN13daObjBlockS_c19func_ov098_021389f8Ev
/* STATE_IDLE, update. While off screen (mFlags 0x8) with both speeds at 0
 * there is nothing to do and it returns at once. Otherwise: move, run the
 * collision and slide steps, then hit handling and the wall test. It is
 * killed on water or when func_ov098_02138bfc names one of its five
 * actors. Updates both cylinders, the model matrix and
 * the shadow (while airborne), and refreshes the collider transform when
 * IsClsnInRangeOnScreen accepts the arguments (0x600000, 0). */
void daObjBlockS_c::func_ov098_021389f8()
{
    int flag = (mFlags & ACTOR_FLAG_OFF_SCREEN) != 0;
    if (flag) {
        if (mHorzSpeed == 0) {
            if (mVertSpeed == 0) {
                return;
            }
        }
    }
    UpdatePos(&mdCcAcPos_c1);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    func_ov098_02139228();
    func_ov098_02138e6c();
    func_ov098_021390ec();
    if (func_ov098_02138bb8() || func_ov098_02138bfc()) {
        Kill();
    }
    mdCcAcPos_c1.Clear();
    mdCcAcPos_c1.Update();
    mdCcAcPos_c2.Clear();
    mdCcAcPos_c2.Update();
    func_ov098_02139850();
    if (!mWithMeshClsn.IsOnGround()) {
        func_ov098_021396a4();
    }
    if (!_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0x600000, 0)) {
        return;
    }
    func_ov098_021397c8();
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138b18Ev
/* STATE_IDLE, enter. Forgets mHoldingPlayer and mEatingPlayer. */
void daObjBlockS_c::func_ov098_02138b18()
{
    mHoldingPlayer = 0;
    BLOCKS_EATING_PLAYER(this) = 0;
}

// @symbol Crate_SetState
/* Enters state `i`: stores it in mState and calls that row's enter member of
 * the table at data_ov098_0213c878. */
extern "C" void Crate_SetState(daObjBlockS_c *self, int i)
{
    BlockSStateHost *c = (BlockSStateHost *)self;
    c->idx = i;
    int j = c->idx;
    (c->*data_ov098_0213c878[j].fn[0])();
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138b70Ev
/* Runs the update member of the current state's row (called once a frame
 * from the end of Behavior). */
void daObjBlockS_c::func_ov098_02138b70()
{
    BlockSStateHost *c = (BlockSStateHost *)this;
    int j = c->idx;
    (c->*data_ov098_0213c878[j].fn[1])();
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138bb8Ev
/* Returns 1 when the crate is on the ground and the floor's CLPS entry has
 * the water bit set (include/CLPS.h, `w0 & 0x20`); the callers kill the
 * crate on it. */
int daObjBlockS_c::func_ov098_02138bb8()
{
    if (mWithMeshClsn.IsOnGround()) {
        if (SurfaceInfo_TestFlag0x20((int *)((char *)mWithMeshClsn.GetFloorResult() + 4)))
            return 1;
    }
    return 0;
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138bfcEv
/* Returns 1 when func_02035638 (a hit test on the crate's dBgCh_Actr, a
 * sibling of IsOnWall) holds and the actor owning the collision it reports is
 * a DOSUN, DONKAKU, DONGURU, BATAN or ONIMASU; STATE_IDLE kills the crate
 * on it. Which surface func_02035638 tests is not recovered here. */
int daObjBlockS_c::func_ov098_02138bfc()
{
    int r;
    void *a;
    unsigned short type;
    if (!func_02035638((unsigned char *)&mWithMeshClsn)) return 0;
    r = func_0203567c((int)&mWithMeshClsn);
    if (((dBgPi *)r)->GetClsnID() == -1)
        return 0;
    a = dActor_c::FindWithID((unsigned int)((dBgPi *)r)->GetClsnID());
    if (!a) return 0;
    type = *(unsigned short *)((char *)a + 0xc);
    {
        Bool b;
        b = (Bool)(type == ACTOR_ONIMASU); if (b) goto out;
        b = (Bool)(type == ACTOR_DONKAKU); if (b) goto out;
        b = (Bool)(type == ACTOR_DONGURU); if (b) goto out;
        b = (Bool)(type == ACTOR_DOSUN); if (b) goto out;
        b = (Bool)(type == ACTOR_BATAN); if (!b) return 0;
    out:
        return 1;
    }
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138ce0Ev
/* Puts the crate back in its starting condition: zeroes the sink offset,
 * target, particle handles and mBreakTimer, restores the home position and
 * angles, zeroes horizontal / vertical speed and the two velocity words
 * around mVertSpeed, then initialises both cylinders at the crate's
 * own position:
 *   cylinder 1, radius 60 (0x3c000), height 110 (0x6e000), flags 0x200002,
 *     vulnFlags 0x4d390 -- the bits dCc_c.h labels mega / kick / breakdance /
 *     slide kick / grab / explosion / yoshi tongue / fire, which is what
 *     func_ov098_02138e6c reacts to;
 *   cylinder 2, radius 60, height 30 (0x1e000), flags 0x800004, no vulnFlags. */
void daObjBlockS_c::func_ov098_02138ce0()
{
    struct Vector3 zero, a1, a2;

    mClsnYOffsetTarget = 0;
    mClsnYOffset = 0;
    mParticleHandle1 = 0;
    mParticleHandle2 = 0;
    mBreakTimer = 0;
    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;
    mAngleX = mHomeAngleX;
    mAngleY = mHomeAngleY;
    mAngleZ = mHomeAngleZ;
    mHorzSpeed = 0;
    unk_0a4 = 0;
    mVertSpeed = 0;
    unk_0ac = 0;

    ((struct Vector3 *)(((long long)(int)&zero)))->x = 0;
    ((struct Vector3 *)(((long long)(int)&zero)))->y = 0;
    ((struct Vector3 *)(((long long)(int)&zero)))->z = 0;

    a1.x = zero.x;
    a1.y = zero.y;
    a1.z = zero.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c1, this, &a1, 0x3c000, 0x6e000, 0x200002, 0x4d390);

    a2.x = zero.x;
    a2.y = zero.y;
    a2.z = zero.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c2, this, &a2, 0x3c000, 0x1e000, 0x800004, 0);
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138e08Ev
/* Pays out three coins at the crate's position (spread argument 15 units, 0xf000;
 * angle 0), the first time only: mCoinsPaid guards it. */
void daObjBlockS_c::func_ov098_02138e08()
{
    int v[3];
    if (mCoinsPaid == 1) return;
    v[0] = mPosX;
    v[1] = mPosY;
    v[2] = mPosZ;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, v, 3, 0xf000, 0);
    mCoinsPaid = 1;
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02138e6cEv
/* Reacts to what touched the crate, from the first cylinder's hit state.
 * Does nothing while mBreakTimer runs. A set mFlags 0x20000 or 0x40000 (the
 * yoshi-mouth bits) enters STATE_IN_MOUTH. Otherwise, if the cylinder has an
 * otherOwner (the ID of the actor that hit it):
 *   hit bit 0x40000 (labelled fire in dCc_c.h)  starts mBreakTimer at 60 and
 *                                              clears dCc_c flags bit 0x8000;
 *   hit bit 0x4000 (explosion)                  kills the crate;
 * and, if that other actor is the PLAYER profile:
 *   hit bits 0x380 (kick / breakdance / slide kick) or 0x10 (mega
 *                                              character)  kill the crate;
 *   hit bit 0x1000 (grab)                       Player::TryGrab, and on
 *                                              success mHoldingPlayer is set
 *                                              and the crate is CARRIED;
 *   otherwise, in STATE_THROWN, a hit on any player other than mPrevHolder
 *   calls Player::Hurt with the crate's position, 1, 0xc000 (12 units),
 *   1, 0, 1; what those arguments select is not recovered here. */
void daObjBlockS_c::func_ov098_02138e6c()
{
    void *a;
    u32 fl;
    int b;

    if (mBreakTimer != 0) return;

    fl = mFlags;
    {
        int b1 = (int)((fl & ACTOR_FLAG_YOSHI_MOUTH_A) != 0);
        if (b1 != 0) {
            Crate_SetState(this, daObjBlockS_c::STATE_IN_MOUTH);
        } else {
            int b2 = (int)((fl & ACTOR_FLAG_YOSHI_MOUTH_B) != 0);
            if (b2 != 0) {
                Crate_SetState(this, daObjBlockS_c::STATE_IN_MOUTH);
            }
        }
    }

    if (mdCcAcPos_c1.otherOwner == 0) return;

    if ((mdCcAcPos_c1.hitFlags & CC_HIT_FIRE) != 0) {
        u32 *pp = &mdCcAcPos_c1.flags;
        mBreakTimer = 0x3c;
        *pp = *pp & ~0x8000u;
    }
    if ((mdCcAcPos_c1.hitFlags & CC_HIT_EXPLOSION) != 0) {
        Kill();
    }

    a = dActor_c::FindWithID(mdCcAcPos_c1.otherOwner);
    if (a == 0) return;
    {
        int bf = (int)(*(u16 *)((char *)a + 0xc) == ACTOR_PLAYER);
        if (bf == 0) return;
    }

    fl = mdCcAcPos_c1.hitFlags;
    if ((fl & CC_HIT_KICKS) != 0) {
        Kill();
        return;
    }
    if ((fl & CC_HIT_MEGA_CHAR) != 0) {
        Kill();
        return;
    }
    if ((fl & CC_HIT_GRAB) != 0) {
        if (((Player *)a)->TryGrab(*this) == 0)
            return;
        mHoldingPlayer = (Player *)a;
        Crate_SetState(this, daObjBlockS_c::STATE_CARRIED);
        return;
    }

    if (mState != daObjBlockS_c::STATE_THROWN) return;
    b = 1;
    if (mPrevHolder == a) b = 0;
    if (b == 0) return;

    {
        struct Vector3 v;
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, 1, 0xc000, 1, 0, 1);
    }
}

// @symbol _ZN13daObjBlockS_c4KillEv
/* Slot 31. Runs the one-time coin payout, then puffs a simple particle
 * (id 0xe, the same NewSimple id daObjTatefuda_c and daObjYajirusi_c use)
 * and a dust poof at the crate's own position raised 40 units
 * (0x28000), plays sound bank 0x41 at the camera-space position, and parks
 * the crate in STATE_BROKEN. */
void daObjBlockS_c::Kill()
{
    Vector3 vec;
    Vector3 vec2;
    int x, y, z;
    func_ov098_02138e08();
    x = mPosX;
    y = mPosY + 0x28000;
    z = mPosZ;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xe, vec.x, vec.y, vec.z);
    ((int *)&vec2)[0] = ((int *)&vec)[0];
    ((int *)&vec2)[1] = ((int *)&vec)[1];
    ((int *)&vec2)[2] = ((int *)&vec)[2];
    _ZN8dActor_c19DisappearPoofDustAtERK7Vector3((char *)this, &vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, &mCamSpacePos);
    Crate_SetState(this, STATE_BROKEN);
}

// @symbol _ZN13daObjBlockS_c19func_ov098_021390ecEv
/* Wall test. Returns early while mWallCooldown, decremented by one per call,
 * is still nonzero after the decrement (set to 3, the next two calls return
 * early). If the dBgCh_Actr reports a wall hit: a wall owned by
 * any actor other than BLOCK_L kills the crate (when the wall has no owner
 * ID, or no actor is found for it, that check is skipped). Then func_ov002_020ef228 (nonzero) restarts
 * mWallCooldown at 3 and ends the test. Otherwise, with a wall hit, the crate
 * is killed if it is faster than 20 units per frame (0x14000), and
 * if not mPrevAngleY is reflected off the wall's normal (ReflectAngle on the
 * normal's x and z). */
void daObjBlockS_c::func_ov098_021390ec()
{
    if (DecIfAbove0_Byte(&mWallCooldown) != 0)
        return;
    if (mWithMeshClsn.IsOnWall() != 0) {
        dBgPi *wr = (dBgPi *)mWithMeshClsn.GetWallResult();
        if (wr->GetClsnID() != -1) {
            dActor_c *a = dActor_c::FindWithID((u32)wr->GetClsnID());
            if (a != 0) {
                int isF = (*(unsigned short *)((char *)a + 0xc) == ACTOR_BLOCK_L);
                if (isF == 0) {
                    Kill();
                    return;
                }
            }
        }
    }
    if (func_ov002_020ef228(&mWithMeshClsn, (int)this) != 0) {
        mWallCooldown = 3;
        return;
    }
    if (mWithMeshClsn.IsOnWall() == 0)
        return;
    if (mHorzSpeed > 0x14000) {
        Kill();
        return;
    }
    Vector3 v;
    ((SurfaceInfo *)((char *)mWithMeshClsn.GetWallResult() + 4))->CopyNormalTo(v);
    mPrevAngleY =
        _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(this, v.x, v.z, mPrevAngleY);
}

#pragma push
// @symbol _ZN13daObjBlockS_c19func_ov098_02139228Ev
/* Ground contact and slide step; returns 1 when it ran to the end, 0 when the
 * crate is airborne or it left early (see below).
 *
 * Reads the floor under the crate. Surface type 6..9 (the quicksand tiers,
 * include/Player.h) sets mClsnYOffsetTarget to 30 / 45 / 60 / 100 units
 * (0x1e000 / 0x2d000 / 0x3c000 / 0x64000); when the 100 unit tier is reached
 * and mClsnYOffset has caught up with it the crate goes to STATE_BROKEN
 * (returns 0). mClsnYOffset then eases toward the target by 0.5 unit per
 * frame (0x800). It returns 0 early, clearing dCc_c flags bit 0x2000 on the
 * first cylinder, when mFloorNormalY (still the value from the previous
 * call at this point) and mHorzSpeed are both 0. A vertical speed below -11
 * units per frame (-0xb000) calls Kill, and the step carries on.
 *
 * The slide: CopyNormalTo fills mFloorNormalX/Y/Z, n is the floor's CLPS
 * type (func_02037e58; include/Player.h calls it the slipperiness class),
 * hl its slide acceleration (func_ov002_020f02c8). a1 is the current
 * velocity (mHorzSpeed along mPrevAngleY, through the sin / cos table at
 * data_02082214), a2 is hl in the direction atan2 of the normal's x and z
 * gives (through the same table), scaled by Vec3_HorzLen of the normal;
 * their sum becomes the new mPrevAngleY and mHorzSpeed (capped at 100 units,
 * 0x64000). mVertSpeed is set to -((normalX * unk_0a4 + normalZ * unk_0ac)
 * / normalY + 8 units); unk_0a4 / unk_0ac sit either side of mVertSpeed and
 * are treated here as its x and z companions, which this file does not
 * otherwise confirm. When func_ov002_020f035c(n, mFloorNormalY) (the
 * per-class slope test, include/Player.h) is nonzero, the crate is nearly
 * level (|mAngleX| and |mAngleZ| below 0x10) and mHorzSpeed is above 5 units
 * (0x5000), sound bank 0x51 plays if mVertSpeed is above 10 units and
 * mVertSpeed is then clamped to a limit derived from the speed. Friction
 * slows mHorzSpeed by 0.5 unit per frame (0x800); while it has not reached
 * 0 and is above 10 units (0xa000) the sliding-dust particle and the slide
 * sound (mSlideSoundHandle) run (the extra compare of mClsnYOffsetTarget
 * with 0xa0 is true for every value this file stores there). Finally mAngleX / mAngleZ ease (a quarter of the
 * difference, at most 0x1000 per call) toward the angles func_02010844 gives
 * for the floor normal, the crate's yaw and the yaw minus a quarter turn. */
int daObjBlockS_c::func_ov098_02139228()
{

#pragma opt_propagation off
    int base = 2;
    void *fr;
    int n;
    int ang;
    int newAng;
    int hl;
    int i94, iang;
    Vector3 a1;
    Vector3 a2;
    Vector3 sum;

    if (mWithMeshClsn.IsOnGround() == 0)
        return 0;

    fr = mWithMeshClsn.GetFloorResult();
    switch (func_02037e38((u32 *)((char *)fr + 4))) {
    case 6:
        mClsnYOffsetTarget = 0x1e000;
        break;
    case 7:
        mClsnYOffsetTarget = 0x2d000;
        break;
    case 8:
        mClsnYOffsetTarget = 0x3c000;
        break;
    case 9:
        mClsnYOffsetTarget = 0x64000;
        if (mClsnYOffsetTarget == mClsnYOffset) {
            Crate_SetState(this, daObjBlockS_c::STATE_BROKEN);
            return 0;
        }
        break;
    }

    _Z14ApproachLinearRiii(&mClsnYOffset, mClsnYOffsetTarget, 0x800);

    if ((mFloorNormalY | mHorzSpeed) == 0) {
        u32 *p = &mdCcAcPos_c1.flags;
        *p &= ~0x2000;
        return 0;
    }

    if (mVertSpeed < -0xb000) {
        Kill();
    }

    ((SurfaceInfo *)((char *)fr + 4))->CopyNormalTo(*(Vector3 *)&mFloorNormalX);
    n = func_02037e58((u32 *)((char *)fr + 4));
    ang = _ZN4cstd5atan2E5Fix12IiES1_(mFloorNormalX, mFloorNormalZ);
    hl = func_ov002_020f02c8(n);
    func_ov002_020f030c(n);


    {
        u16 r_94 = mPrevAngleY;
        int sa = (u16)ang;
        sa >>= 4;
        int s94 = r_94;
        s94 >>= 4;
        int li94 = s94 * 2;
        s32 v98 = mHorzSpeed;
        s16 x94 = data_02082214[li94];
        s16 z94 = data_02082214[li94 + 1];
        a1.y = 0;
        a2.y = 0;
        s32 a1x = FX_MUL(v98, x94);
        a1.z = FX_MUL(v98, z94);
        s16 xa = data_02082214[sa * 2];
        s16 za = data_02082214[sa * 2 + 1];
        a2.x = FX_MUL(hl, xa);
        a2.z = FX_MUL(hl, za);
        a1.x = a1x;
    }
    Vec3_MulScalarInPlace(&a2.x, Vec3_HorzLen((Vector3 *)&mFloorNormalX));
    Vec3_Add(&sum, &a1, &a2);
    newAng = _ZN4cstd5atan2E5Fix12IiES1_(sum.x, sum.z);
    mHorzSpeed = Vec3_HorzLen(&sum);
    if (mHorzSpeed > 0x64000)
        mHorzSpeed = 0x64000;
    mPrevAngleY = newAng;
    AngleDiff(mPrevAngleY, mAngleY);

    {
        s32 m0 = FX_MUL(mFloorNormalX, unk_0a4);
        s32 m1 = FX_MUL(mFloorNormalZ, unk_0ac);
        mVertSpeed = -(_ZN4cstd4fdivEii(m0 + m1, mFloorNormalY) + 0x8000);
    }

    if (func_ov002_020f035c(n, mFloorNormalY) != 0 && mHorzSpeed > 0x5000) {
        int q0 = mAngleX;
        int q1;
        int sd;
        if (q0 < 0) q0 = ((-q0) << 16) >> 16;
        if (q0 < 0x10) {
            q1 = mAngleZ;
            if (q1 < 0) q1 = ((-q1) << 16) >> 16;
            if (q1 < 0x10) {
                s32 v = mHorzSpeed;
                sd = _ZN4cstd4fdivEii(FX_MUL(v, 8), 0xa);
                if (sd < 0) sd = -sd;
                if (mVertSpeed > 0xa000)
                    _ZN5Sound9PlayBank3EjRK7Vector3(0x51, &mCamSpacePos);
                if (mVertSpeed > sd)
                    mVertSpeed = sd;
            }
        }
    }

    if (_Z14ApproachLinearRiii(&mHorzSpeed, 0, 0x800) == 0
        && mClsnYOffsetTarget != 0xa0
        && mHorzSpeed > 0xa000) {
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(mPosX, mPosY, mPosZ);
        mSlideSoundHandle =
            _ZN5Sound8PlayLongEjjjRK7Vector3s(mSlideSoundHandle, 3, 0x93, &mCamSpacePos, 0);
    }

    {
        s16 r5 = func_02010844(this, (Vector3 *)&mFloorNormalX, mAngleY);
        s16 r4 = func_02010844(this, (Vector3 *)&mFloorNormalX, (s16)(mAngleY - 0x4000));
        _Z11UpdateAngleRssis(&mAngleX, r5, 4, 0x1000);
        _Z11UpdateAngleRssis(&mAngleZ, r4, 4, 0x1000);
    }
    return 1 | (base & 0);
}
#pragma pop

// @symbol _ZN13daObjBlockS_c19func_ov098_021396a4Ev
/* Drop shadow. Probes for ground 10 units below the crate (0xa000) with
 * dBgCh_Gnd and stores the ground height in mGroundY (the probe's own Y if
 * nothing is found). The height above it, h = mPosY - mGroundY, is floored
 * at 1 unit (0x1000); the shadow's scale is 100 units minus h * 0x180 / 0x1000
 * (so 3/32 of h), floored at 10 units (0xa000). Builds mShadowMtx from
 * mAngleY, puts its translation row at the crate's position (y lowered by
 * 20 units, 0x14000; all three >> 3) and calls dActor_c::DropShadowScaleXYZ with it,
 * mShadowModel, that scale twice and h + 40 units (0x28000) in between,
 * and 0xf last (the opacity argument in include/dActor_c.h; the ScaleXYZ in
 * the method name suggests the three before it are per-axis scales). */
void daObjBlockS_c::func_ov098_021396a4()
{
    struct Vector3 v;
    int r5;
    int r4;

    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    v.y -= 0xa000;
    dBgCh_Gnd rg;
    rg.SetObjAndPos(v, 0);
    mGroundY = v.y;
    if (rg.DetectClsn()) {
        mGroundY = rg.clsnY;
    }
    r5 = mPosY - mGroundY;
    if (r5 <= 0x1000) r5 = 0x1000;
    r4 = 0x64000 - FX_MUL(r5, 0x180);
    if (r4 < 0xa000) r4 = 0xa000;
    Matrix4x3_FromRotationY(&mShadowMtx, mAngleY);
    mShadowMtx.m[9] = mPosX >> 3;
    mShadowMtx.m[10] = (mPosY - 0x14000) >> 3;
    mShadowMtx.m[11] = mPosZ >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &mShadowMtx, r4, r5 + 0x28000, r4, 0xf);
}

// @symbol _ZN13daObjBlockS_c19func_ov098_021397c8Ev
/* Collider transform. Copies the model matrix (mModel's mat4x3, at 0xf0)
 * into mClsnMat, overwrites its translation row with the crate's position
 * lowered by mClsnYOffset, and hands it and mAngleY to dBgW_KcMbg::Transform
 * on mMeshCollider. */
void daObjBlockS_c::func_ov098_021397c8()
{
    volatile int tmp[3];
    tmp[0] = mPosX;
    int origY = mPosY;
    tmp[1] = origY;
    tmp[2] = mPosZ;
    tmp[1] = origY - mClsnYOffset;
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9] = tmp[0];
    mClsnMat.m[10] = tmp[1];
    mClsnMat.m[11] = tmp[2];
    mMeshCollider.Transform(mClsnMat, mAngleY);
}

// @symbol _ZN13daObjBlockS_c19func_ov098_02139850Ev
/* Model matrix. When mHoldingPlayer is set, mFlags bit 0x4000 is set and the
 * player's word at 0xc8 (a dActor_c word not yet named) is nonzero, the
 * crate follows the player's hand: it picks a row r5 of the carry-offset
 * table (data_ov098_0213bf60 / 64 / 68 are its x / y / z columns, stride 12
 * bytes) -- 0 normally, 1 if the player is front-sliding, or if it has lost its grabbed
 * object and ((w << 4) >> 16) < 14 for the word w at +0x58 of what
 * func_ov002_020e496c returns (unrecovered), plus 2 if the player's param1 is 2 -- eases
 * mCarryOffset toward that row, takes the matrix UpdateCarry returns as the
 * model matrix, and sets mFlags bit 0x4000000. The rows, in units, are
 * (40, 0, 45), (40, 0, 70), (48, 16, 0) and (48, 0, 60).
 * Otherwise it builds the model matrix from the crate's own angles with the
 * translation row at its position lowered by mClsnYOffset (>> 3), zeroes
 * mCarryOffset, and clears mFlags bit 0x4000000 while the crate is on the
 * ground. */
void daObjBlockS_c::func_ov098_02139850()
{
    Player *obj = mHoldingPlayer;
    int b;

    if (obj == 0) goto other;
    b = (int)((mFlags & 0x4000) != 0);
    if (b == 0) goto other;
    if (*(int *)((char *)obj + 0xc8) == 0) goto other;

    {
        char *r4 = (char *)func_ov002_020e496c((char *)obj);
        int r5 = 0;
        if (mHoldingPlayer->IsFrontSliding() != 0)
            r5 = 1;
        if (mHoldingPlayer->LostGrabbedObject() != 0) {
            if (((u32)*(int *)(r4 + 0x58) << 4) >> 0x10 < 0xe) r5 = 1;
        }
        if (mHoldingPlayer->param1 == 2) {
            r5 = (r5 + 2) & 0xff;
        }
        Math_Function_0203b14c((char *)&mCarryOffsetX, *(int *)((char *)data_ov098_0213bf60 + r5 * 0xc), 0x800, 0x3e8000, 4);
        Math_Function_0203b14c((char *)&mCarryOffsetY, *(int *)((char *)data_ov098_0213bf64 + r5 * 0xc), 0x800, 0x3e8000, 4);
        Math_Function_0203b14c((char *)&mCarryOffsetZ, *(int *)((char *)data_ov098_0213bf68 + r5 * 0xc), 0x800, 0x3e8000, 4);
        {
            char *res = (char *)UpdateCarry(*(Player *)mHoldingPlayer, *(Vector3 *)&mCarryOffsetX);
            mModel.mat4x3 = *(struct Matrix4x3 *)res;
        }
        mFlags |= 0x4000000;
    }
    return;

other:
    {
        volatile int tmp[3];
        tmp[0] = mPosX;
        tmp[1] = mPosY;
        tmp[2] = mPosZ;
        tmp[1] = mPosY - mClsnYOffset;
        Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
        mModel.mat4x3.m[9] = tmp[0] >> 3;
        mModel.mat4x3.m[10] = tmp[1] >> 3;
        mModel.mat4x3.m[11] = tmp[2] >> 3;
        mCarryOffsetX = 0;
        mCarryOffsetY = 0;
        mCarryOffsetZ = 0;
        if (mWithMeshClsn.IsOnGround() != 0) {
            mFlags &= ~0x4000000;
        }
    }
}

// @symbol _ZN13daObjBlockS_c16CleanupResourcesEv
/* Disables the collider, then releases the model and collision files
 * (data_ov098_0213c4c8[0] / [1]) when the actor is BLOCK_S. Always returns 1. */
int daObjBlockS_c::CleanupResources()
{
    int *f;
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled())
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    f = 0;
    if (actorID == ACTOR_BLOCK_S)
        f = data_ov098_0213c4c8;
    if (f) {
        ((SharedFilePtr *)((void *)f[0]))->Release();
        ((SharedFilePtr *)((void *)f[1]))->Release();
    }
    return 1;
}

// @symbol _ZN13daObjBlockS_c6RenderEv
/* Draws the model through Model's vtable slot 5 (Render) unless the crate is
 * in STATE_BROKEN or mFlags 0x40000 (a yoshi-mouth bit) is set. The call goes
 * through the local `Sub` shadow so it stays a virtual dispatch on the member
 * object. Always returns 1. */
int daObjBlockS_c::Render()
{
    if (mState == STATE_BROKEN)
        return 1;
    {
        int b = (int)((mFlags & ACTOR_FLAG_YOSHI_MOUTH_B) != 0);
        if (b)
            return 1;
    }
    Sub *o = (Sub *)&mModel;
    o->m5(0);
    return 1;
}

// @symbol _ZN13daObjBlockS_c8BehaviorEv
/* Per-frame update; always returns 1.
 *  - While mFlags bit 0x4000000 is set, data_0209b454 has bit 0x4000000
 *    (an unrecovered global) and a player is holding the crate, that player
 *    drops it.
 *  - Off screen (mFlags 0x8), not horizontally at home and more than 2000 units
 *    (0x7d0000) from the closest player: STATE_BROKEN, which respawns it at
 *    home, and the frame ends.
 *  - While mBreakTimer is nonzero (a fire-labelled hit sets it to 60), each
 *    frame it spawns the 0x13a / 0x13b particle pair (the same pair
 *    dEnemyBase_c spawns) 80 units above the crate (0x50000) and counts the
 *    timer down; when it reaches 0 it pays out the coins, poofs the crate at
 *    +40 units and enters STATE_BROKEN, like Kill without the particle
 *    and sound, and the frame ends.
 *  - Otherwise both cylinders are moved to the crate's position lowered by
 *    mClsnYOffset and the current state's update runs. */
int daObjBlockS_c::Behavior()
{
    struct Vector3 v;
    struct Vector3 vec;
    struct Vector3 t;
    struct Vector3 vec2;
    enum Bool b1, b2;
    int x, y, z;

    b1 = (enum Bool)((mFlags & 0x4000000) != 0);
    if (b1 != FALSE && (data_0209b454 & 0x4000000) && mHoldingPlayer) {
        mHoldingPlayer->DropActor();
    }

    b2 = (enum Bool)((mFlags & ACTOR_FLAG_OFF_SCREEN) != 0);
    if (b2 != FALSE && Vec3_HorzDist(&mPos, (const struct Vector3 *)&mHomePosX) &&
        DistToCPlayer() > 0x7d0000) {
        Crate_SetState(this, STATE_BROKEN);
        return 1;
    }

    if (mBreakTimer != 0) {
        x = mPosX;
        z = mPosZ;
        y = mPosY + 0x50000;
        ((int *)&v)[0] = x;
        ((int *)&v)[1] = y;
        ((int *)&v)[2] = z;
        if (DecIfAbove0_Byte(&mBreakTimer)) {
            mParticleHandle1 = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticleHandle1, 0x13a, v.x, v.y, v.z, 0, 0);
            mParticleHandle2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                mParticleHandle2, 0x13b, v.x, v.y, v.z, 0);
            goto done;
        }
        func_ov098_02138e08();
        x = mPosX;
        z = mPosZ;
        y = mPosY + 0x28000;
        vec.x = x;
        vec.y = y;
        vec.z = z;
        ((int *)&vec2)[0] = ((int *)&vec)[0];
        ((int *)&vec2)[1] = ((int *)&vec)[1];
        ((int *)&vec2)[2] = ((int *)&vec)[2];
        _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(((char *)this), &vec2);
        Crate_SetState(this, STATE_BROKEN);
        return 1;
    }

done:
    ((int *)&t)[0] = mPosX;
    ((int *)&t)[1] = mPosY;
    ((int *)&t)[2] = mPosZ;
    ((int *)&t)[1] = t.y - mClsnYOffset;
    mdCcAcPos_c1.pos.x = t.x;
    mdCcAcPos_c1.pos.y = t.y;
    mdCcAcPos_c1.pos.z = t.z;
    mdCcAcPos_c2.pos.x = t.x;
    mdCcAcPos_c2.pos.y = t.y;
    mdCcAcPos_c2.pos.z = t.z;
    func_ov098_02138b70();
    return 1;
}

// @symbol _ZN13daObjBlockS_c13InitResourcesEv
/* Fails (returns 0) unless the actor is BLOCK_S. Saves the spawn position and
 * angles as the home, then builds the crate from data_ov098_0213c4c8 (three
 * words: the model file, the collision file and a CLPS block): model,
 * cuboid shadow, KCL mesh collider (SetFile with mClsnMat, a Fix12 0x199
 * (about 0.1), mAngleY and the CLPS block word) and the dBgCh_Actr (Init with radius 40 and height
 * 40 units, 0x28000 each) with water detection on. Gravity is -2 units per
 * frame^2 (-0x2000) with a terminal velocity of -60 units per frame
 * (-0x3c000). Enters STATE_IDLE, clears mEatingPlayer and resets the crate
 * to its home condition. Returns 1. */
int daObjBlockS_c::InitResources()
{
    char *f = 0;
    if (actorID == ACTOR_BLOCK_S)
        f = (char *)data_ov098_0213c4c8;
    if (f == 0)
        return 0;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    mModel.SetFile((BMD_File *)Model::LoadFile(**(SharedFilePtr **)(f)), 1, -1);
    mShadowModel.InitCuboid();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (char *)&mMeshCollider, (int)dBgW_Kc::LoadFile(**(SharedFilePtr **)(f + 4)),
        (char *)&mClsnMat, 0x199, mAngleY, *(int *)(f + 8));
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        (char *)&mWithMeshClsn, ((char *)this), 0x28000, 0x28000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    Crate_SetState(this, STATE_IDLE);
    /* dBgActor_c's own generic 0xd0..0xd4 pad (include/dBgActor_c.h), not a
       daObjBlockS_c field -- reached by raw offset as mEatingPlayer. */
    BLOCKS_EATING_PLAYER(this) = 0;
    func_ov098_02138ce0();
    return 1;
}

// @symbol _ZN13daObjBlockS_c15OnGroundPoundedER8dActor_c
/* Slot 21 (include/dActor_c.h), recovered from vtable slot identity.
 * Ground-pounding the crate just calls its own Kill (slot 31); the
 * `dActor_c &other` parameter is unused, matching the ROM body -- it loads
 * the vtable slot and calls through it without ever touching r1. */
void daObjBlockS_c::OnGroundPounded(dActor_c &other)
{
    Kill();
}

// @symbol _ZN13daObjBlockS_c13OnYoshiTryEatEv
/* Slot 18. Returns 0 while mBreakTimer is running and 6 otherwise.
 * Behavior counts mBreakTimer down once a frame and poofs the crate when it
 * reaches 0, so Yoshi gets 0 while that is under way. What 6 means to the
 * caller is not recovered here. */
int daObjBlockS_c::OnYoshiTryEat()
{
    unsigned char v = mBreakTimer;
    if (v != 0)
        return 0;
    return 6;
}

// @symbol _ZN13daObjBlockS_c13OnTurnIntoEggER6Player
/* Slot 19. Pays out three coins the first time only, tracked by mCoinsPaid:
 * handed over directly if the player is mid cap-collect, otherwise banked on
 * the egg. Either way the crate goes to STATE_BROKEN. The shared actor hook
 * returns void, so the method ends after the state change with no invented
 * result. */
void daObjBlockS_c::OnTurnIntoEgg(Player &player)
{
    char *r4 = (char *)&player;
    if (((Player *)r4)->IsCollectingCap()) {
        if (mCoinsPaid != 1) {
            ((dActor_c *)this)->GivePlayerCoins(*(Player *)r4, 3, 0);
            mCoinsPaid = 1;
        }
    } else {
        unsigned int count = 0;
        if (mCoinsPaid != 1) {
            mCoinsPaid = 1;
            count = 3;
        }
        ((Player *)r4)->RegisterEggCoinCount(count, 0, 0);
    }
    Crate_SetState(this, STATE_BROKEN);
}

// @symbol daObjBlockS_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjBlockS_c through RTTI,
 * allocation size, vtable identity, and the BLOCK_S registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Crate_Spawn. */
extern "C" daObjBlockS_c *daObjBlockS_c_classInit()
{
    return new daObjBlockS_c();
}

// @symbol __sinit_d_a_obj_block_s.cpp
BlockSModelFilePtr gBrokenBlockSModelFile(ASSET_BROKEN_BLOCK_S_BMD);       /* model */
BlockSCollisionFilePtr gBrokenBlockSCollisionFile(ASSET_BROKEN_BLOCK_S_KCL);   /* collision */

/* The fourteen state member-pointer records live in .data
 * (0x0213c458..0x0213c4c8); the static initializer copies them into the
 * .bss table below, an enter/update pair per state. */
extern "C" BlockSStateFn data_ov098_0213c4a8;
extern "C" BlockSStateFn data_ov098_0213c488;
extern "C" BlockSStateFn data_ov098_0213c490;
extern "C" BlockSStateFn data_ov098_0213c468;
extern "C" BlockSStateFn data_ov098_0213c498;
extern "C" BlockSStateFn data_ov098_0213c470;
extern "C" BlockSStateFn data_ov098_0213c4b0;
extern "C" BlockSStateFn data_ov098_0213c480;
extern "C" BlockSStateFn data_ov098_0213c4a0;
extern "C" BlockSStateFn data_ov098_0213c478;
extern "C" BlockSStateFn data_ov098_0213c458;
extern "C" BlockSStateFn data_ov098_0213c460;
extern "C" BlockSStateFn data_ov098_0213c4c0;
extern "C" BlockSStateFn data_ov098_0213c4b8;

BlockSStateEntry data_ov098_0213c878[7] = {
    { data_ov098_0213c4a8, data_ov098_0213c488 },   /* STATE_IDLE */
    { data_ov098_0213c490, data_ov098_0213c468 },   /* STATE_CARRIED */
    { data_ov098_0213c498, data_ov098_0213c470 },   /* STATE_THROWN */
    { data_ov098_0213c4b0, data_ov098_0213c480 },   /* STATE_SPIT_OUT */
    { data_ov098_0213c4a0, data_ov098_0213c478 },   /* STATE_BOUNCING */
    { data_ov098_0213c458, data_ov098_0213c460 },   /* STATE_IN_MOUTH */
    { data_ov098_0213c4c0, data_ov098_0213c4b8 },   /* STATE_BROKEN */
};
