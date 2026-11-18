//cpp
/**
 * Shared base for enemies and other actors that use the enemy collision,
 * death and Yoshi-eat handling.
 *
 * IsGoingOffCliff probes straight down from the actor and stores the
 * result in mCliffState. AngleAwayFromWallOrCliff turns the actor when
 * that probe says the floor is gone or too steep.
 *
 * The constructor and destructor bodies are empty. The compiler supplies
 * the base call and the vptr store (see the constructor note). Source
 * order is the reverse of the ROM because codegen is deferred.
 * decl_common.h is left out: its helper spellings disagree with this file.
 *
 * The out-of-line destructor is the class's key function. It emits D2, D0
 * and D1 in cartridge order (0x020aed18, 0x020aed3c, 0x020aed74), so this
 * object carries _ZTV/_ZTI/_ZTS12dEnemyBase_c and the inherited RTTI. The
 * manifest's compiler_only_output licenses them.
 *
 * This TU also owns the module's second static initializer: the two
 * non-const PMF tables at the end of the file make mwcc emit
 * __sinit_d_enemy_base.cpp, byte-identical to retail __sinit_ov002_02100938
 * (0x1a4, eighteen relocations), plus the .ctor word and the sixteen
 * pointer-to-member descriptors. See notes/sinit-ownership-pilot-ov002.md.
 *
 * The name is the cartridge's: ov002 holds _ZTI12dEnemyBase_c/
 * _ZTS12dEnemyBase_c at 0x021081c0/0x021081cc, and da1up_c's
 * __si_class_type_info reaches them through its +8 word.
 *
 * leftovers:
 *   - Each named local draws one @NNN number, so dropping one renumbers this
 *     TU's pointer-to-member records (measured). The manifest's
 *     defined_symbol_renames rows pair the numbers mwcc emits now with the
 *     pinned names; a change that adds or drops a named local has to shift
 *     their `from` side to match.
 *   - func_ov002_020ae5c8 keeps an int parameter it never reads: its caller
 *     passes one; the form without it comes out 0x50 against the ROM's
 *     0x5c (measured note in place).
 *   - func_ov002_020ae4cc passes its particle position as
 *     `*(int *)((char *)v + N)`; indexing v[] directly comes out 0xd4 against
 *     the ROM's 0xfc (measured). SpawnParticlesIfHitOtherObj keeps its
 *     coin tests as nested int compares: neither bool nor a single `&&`
 *     matches (the `&&` form is 0xa0 against 0xb8).
 *   - Address-derived member names stand until the handlers' semantics are
 *     recovered.
 */
#include "dEnemyBase_c.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "decl_dBgPi.h"
#include "dBgCh_Lin.h"
#include "daPly_c.h"
#include "SurfaceInfo.h"
#include "dBgCh_Actr.h"
#include "Sound.h"
#include "cstd.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* Written as the real constructor, which is the form the legacy shard recovered
   and byte-matched. Two steps, and neither is spelled below: the dActor_c
   base-subobject call, then this class's own vptr store. `dEnemyBase_c : dActor_c`
   supplies the first and `virtual ~dEnemyBase_c()` the second, in the order mwcc
   always uses (notes/ctor-migration.md sec 6: base calls -> derived vptr store ->
   members -> body).

   The base call is a real `bl` only because include/dActor_c.h DECLARES dActor_c()
   and never defines it; left undeclared the compiler would synthesise and inline
   the base construction here and the candidate would land short. That is
   notes/ctor-migration.md sec 2, and it is why dEnemyBase_c.h declares
   dEnemyBase_c() the same way for the classes below it.

   One definition emits C1 AND C2 (sec 1). There is no C1 file and no C1 in the ROM
   -- nothing constructs a standalone dEnemyBase_c, so the linker never kept one;
   the manifest licenses the C1 this TU emits as compiler-only output. */
dEnemyBase_c::dEnemyBase_c()
{
}

/* One definition, three emitted sections: D2 0x020aed18, D0 0x020aed3c, D1 0x020aed74. */
/* Store this class's vtable over the one dActor_c's constructor left, then run the
   dActor_c subobject destructor. D0 additionally returns the object to the actor
   heap through dEnemyBase_c's own inline operator delete (see include/dEnemyBase_c.h). The
   three legacy files each spelled one variant; the compiler emits all three from
   this. */
dEnemyBase_c::~dEnemyBase_c()
{
}

/* Runs the mesh collision (sel picks which of four update flavours), then caches
   whichever surface normals came back: the floor result's into mFloorNormal,
   the wall result's into mWallNormal. Those two CopyNormalTo calls are what
   evidence both as Vector3 members.

   On a floor that does not limit movement, the vertical velocity is re-derived
   so the motion stays in the floor plane. */
/* local extern: func_02038414, the tail-call veneer to
   dBgCh_Actr::UpdateDiscreteNoLava_2, is the one veneer dBgCh_Actr.h does
   not declare -- decl_common.h already spells it with a void *. */
extern "C" {
extern void func_02038414(dBgCh_Actr *);
}

void dEnemyBase_c::UpdateWMClsn(dBgCh_Actr &clsn, unsigned int sel) {
    switch (sel) {
    case 1:
        func_020383f0(&clsn);
        break;
    case 2:
        dBgCh_Actr_UpdateDiscreteNoLava_veneer(&clsn);
        break;
    case 3:
        func_02038414(&clsn);
        break;
    default:
        dBgCh_Actr_UpdateContinuous_Veneer(&clsn);
        break;
    }
    if (clsn.IsOnGround()) {
        clsn.GetFloorResult()->surface.CopyNormalTo(mFloorNormal);
        if (clsn.GetLimMovFlag() == 0) {
            int dz = mFloorNormal.y;
            if (dz != 0) {
                int nx = mFloorNormal.x;
                int vx = mSpeed.x;
                int nz = mFloorNormal.z;
                int vz = mSpeed.z;
                int a = FX_MUL(nx, vx);
                int b = FX_MUL(nz, vz);
                int num = a + b;
                int q = FX_Div(num, dz);
                mSpeed.y = -(q + 0x8000);
            }
        }
    }
    if (clsn.IsOnWall()) {
        clsn.GetWallResult()->surface.CopyNormalTo(mWallNormal);
    }
}

/* Its legacy file built this from a local `struct dEnemyBase_c : dActor_c` shadow with a
   local POD `struct Vector3`; both are the real ones now. Measured: using
   types.h's Vector3 -- which declares ~Vector3(){} and is therefore NOT a POD --
   for the local `v` costs nothing here, all 0x13c bytes reproduce, which is what
   lets dActor_c::Spawn take it by const reference as its real signature says. */
extern const u16 data_ov002_020ff014[]; /* coin actor IDs: 0x120, 0x121, 0x122 */

void dEnemyBase_c::SpawnCoin()
{
    int i;
    Vector3 v;
    if (mCoinType != 0) {
        int tz = mPos.z;
        int ty = mPos.y + 0x78000;
        int tx = mPos.x;
        v.x = tx;
        v.y = ty;
        v.z = tz;
        if (mCoinType >= 4)
            mCoinType = 1;
        for (i = 0; i < mExtraCoinCount + 1; i++) {
            dActor_c *coin =
                dActor_c::Spawn(data_ov002_020ff014[mCoinType - 1], 0xf2, v, 0, mAreaId, -1);
            if (coin != 0) {
                int rnd1 = RandomIntInternal(&data_0209e650.value);
                int rnd2 = RandomIntInternal(&data_0209e650.value);
                int a = (int)((u32)rnd1 >> 16 << 27) >> 16;
                u32 b = (u32)rnd2 >> 16;
                coin->mPrevAngle.x = 0;
                coin->mPrevAngle.y = (s16)(a * i);
                coin->mPrevAngle.z = 0;
                coin->mHorzSpeed = (((b % 50) + 100) << 15) / 100;
            }
        }
    }
    PoofDust();
}

/* Start the death the caller stored in mDeathState: clear mFlags bit
   0x10000000, zero mDeathTimer, run that state's start handler from
   data_ov002_0210db80, then set mVertAccel to -0x2000 and clear the bit again.
   The attacker and the (nullable) collision are passed through to the
   handler; the handlers read the attacker and ignore the collision. The body
   overwrites r3 before reading it: what callers leave there comes from their
   mDeathState store, not a fourth argument. ov004 has a different function at
   this address. */
typedef void (dEnemyBase_c::*DeathStartPMF)(dActor_c *, dBgCh_Actr *);
extern DeathStartPMF data_ov002_0210db80[];

void dEnemyBase_c::func_ov002_020aea30(dActor_c *a, dBgCh_Actr *b) {
    if (mDeathState == 0)
        return;
    mFlags &= ~0x10000000;
    mDeathTimer = 0;
    (this->*data_ov002_0210db80[mDeathState - 1])(a, b);
    mVertAccel = -0x2000;
    mFlags &= ~0x10000000;
}

void dEnemyBase_c::func_ov002_020aea2c(dActor_c *, dBgCh_Actr *) {}

int dEnemyBase_c::func_ov002_020aea24(dBgCh_Actr &) {
    return 0;
}

/* Stop the enemy (zero mHorzSpeed and mSpeed.y), start a 0xf-frame
   mDeathTimer, then clear mFlags bit 0x1. */
void dEnemyBase_c::func_ov002_020ae9f8(dActor_c *, dBgCh_Actr *) {
    mHorzSpeed = 0;
    mSpeed.y = 0;
    mDeathTimer = 15;
    mFlags &= ~0x1;
}

/* Knock-back strengths, indexed by the attacking player's param1. */
extern "C" {
extern const u16 data_ov002_020ff01c[];
}

/* Actor ID 0xbf is the player. Int-returning, as the 0/1 the ROM
   materialises before testing it requires. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

void dEnemyBase_c::func_ov002_020ae968(dActor_c *arg) {
    mSpeed.y = 0x14000;
    if (IsPlayerActor(arg)) {
        u16 t = data_ov002_020ff01c[(u16)arg->param1];
        mHorzSpeed = ((int)t << 0xf) / 50;
    } else {
        mHorzSpeed = 0x14000;
    }
    mFlags &= ~1;
    Sound::PlayBank0(9, mCamSpacePos);
}

void dEnemyBase_c::func_ov002_020ae954(dActor_c *a, dBgCh_Actr *) {
    mPrevAngle.y = a->mAngle.y;
    func_ov002_020ae968(a);
}

void dEnemyBase_c::func_ov002_020ae8b8(dActor_c *arg) {
    if (IsPlayerActor(arg)) {
        u16 t = data_ov002_020ff01c[(u16)arg->param1];
        int scaled = t << 3;
        int ip = scaled << 12;
        int d1 = ip / 100;
        int d2 = scaled / 100;
        mHorzSpeed = d1;
        mSpeed.y = (d2 + 0x20) << 12;
    } else {
        mHorzSpeed = 0xa000;
        mSpeed.y = 0x28000;
    }
    mFlags &= ~1;
}

void dEnemyBase_c::func_ov002_020ae890(dActor_c *a, dBgCh_Actr *) {
    mPrevAngle.y = a->mAngle.y;
    func_ov002_020ae8b8(a);
    Sound::PlayBank0(0xa, mCamSpacePos);
}

void dEnemyBase_c::func_ov002_020ae87c(dActor_c *a, dBgCh_Actr *) {
    mPrevAngle.y = a->mAngle.y;
    func_ov002_020ae8b8(a);
}

void dEnemyBase_c::func_ov002_020ae844(dActor_c *a, dBgCh_Actr *) {
    s16 angle = Vec3_HorzAngle(&a->mPos, &mPos);
    mPrevAngle.y = angle;
    func_ov002_020ae968(a);
}

void dEnemyBase_c::func_ov002_020ae80c(dActor_c *a, dBgCh_Actr *) {
    mPrevAngle.y = Vec3_HorzAngle(&a->mPos, &mPos);
    func_ov002_020ae8b8(a);
}

void dEnemyBase_c::func_ov002_020ae73c(dActor_c *a, dBgCh_Actr *) {
    mPrevAngle.y = Vec3_HorzAngle(&a->mPos, &mPos);
    mSpeed.y = 0xa000;

    if (IsPlayerActor(a))
        mHorzSpeed = (data_ov002_020ff01c[(unsigned short)a->param1] << 15) / 50;
    else
        mHorzSpeed = 0x14000;

    mHorzSpeed /= 6;
    mDeathTimer = 8;
    mFlags &= ~1;
    Sound::PlayBank0(0xa, mCamSpacePos);
}

/* mDeathState selects which death handler from a table of POINTERS TO MEMBER
   FUNCTION, then the position and mesh collision are updated regardless. */
typedef int (dEnemyBase_c::*DeathPMF)(dBgCh_Actr &);
extern DeathPMF data_ov002_0210dbc0[];

int dEnemyBase_c::UpdateDeath(dBgCh_Actr &clsn) {
    int ret;
    if (mDeathState == 0)
        return 0;
    DecIfAbove0_Short(&mDeathTimer);
    ret = (this->*data_ov002_0210dbc0[mDeathState - 1])(clsn);
    UpdatePos(0);
    UpdateWMClsn(clsn, 0);
    return ret;
}

/* func_ov002_020ae5c8 is written LOWER in this file (the whole TU runs highest
   ROM address first).

   ITS int PARAMETER IS REAL AND IS LOAD-BEARING, and merging is what proved
   it. func_ov002_020ae5c8's own file defined it as `(void *c)` and never read a
   second argument; this file declared it `(char *, int)` and passed one. Built
   the one-argument way, this function comes out 0x50 against the ROM's 0x5c --
   twelve bytes and three instructions short, because `x` no longer has to
   survive the body in a callee-saved register. So both observations are true at once: the
   caller passes it, the callee ignores it, and the definition below carries
   the parameter unused. */
int dEnemyBase_c::func_ov002_020ae64c(dBgCh_Actr &clsn) {
    int x = (int)&clsn;
    if (mDeathTimer >= 5) {
        int v = mScale.y - 0x23d;
        if (v < 0x4cc)
            v = 0x4cc;
        mScale.y = v;
        mScale.z = 0x2000 - mScale.y;
        mScale.x = mScale.z;
    }
    return func_ov002_020ae5c8(x);
}

int dEnemyBase_c::func_ov002_020ae608(dBgCh_Actr &clsn) {
    if (clsn.IsOnGround() == 0)
        return 0;
    SpawnCoin();
    KillAndTrackInDeathTable();
    mDeathState = 0;
    return 1;
}

/* `x` is unused on purpose -- see the note on func_ov002_020ae64c, which passes it. */
int dEnemyBase_c::func_ov002_020ae5c8(int x) {
    if (mDeathTimer != 0)
        return 0;
    SpawnCoin();
    KillAndTrackInDeathTable();
    mDeathState = 0;
    return 1;
}

int dEnemyBase_c::func_ov002_020ae4cc(dBgCh_Actr &clsn) {
    Vector3 v;
    int n = mScale.y;
    if (n == 0) {
        int z = mPos.z;
        int y = mPos.y + 0x50000;
        int x = mPos.x;
        v.x = x;
        v.y = y;
        v.z = z;
    } else {
        int z = mPos.z;
        int y = mPos.y + n * 0x50;
        int x = mPos.x;
        v.x = x;
        v.y = y;
        v.z = z;
    }
    if (clsn.IsOnGround()) {
        SpawnCoin();
        KillAndTrackInDeathTable();
        mDeathState = 0;
        return 1;
    }
    mDeathParticleID0 = Particle::System::New(mDeathParticleID0, kPtclDeathA, v.x, v.y, v.z, 0, 0);
    mDeathParticleID1 =
        Particle::System::NewCleanParticle(mDeathParticleID1, kPtclDeathB, v.x, v.y, v.z, 0);
    return 0;
}

int dEnemyBase_c::func_ov002_020ae454(dBgCh_Actr &clsn) {
    if (mDeathTimer == 0 || clsn.IsOnGround() != 0 || clsn.IsOnWall() != 0) {
        SpawnCoin();
        KillAndTrackInDeathTable();
        mDeathState = 0;
        return 1;
    }
    return 0;
}

/* Written as the real method over the real dBgCh_Lin/dBgPi classes, which is the
   form the legacy shard recovered and byte-matched. The two by-value fx32
   parameters the mangled name claims are spelled as themselves. */
int dEnemyBase_c::IsGoingOffCliff(dBgCh_Actr &clsn, fx32 down, s16 maxSlope, bool detectWater,
                                  bool allowObjFloor, fx32 up) {
    Vector3 start;
    Vector3 end;
    mCliffState = 0;
    if (clsn.IsOnGround() != 0) {
        dBgCh_Lin line;
        start.x = mPos.x;
        start.y = mPos.y;
        start.z = mPos.z;
        start.y += up;
        end.x = mPos.x;
        end.y = mPos.y;
        end.z = mPos.z;
        end.y -= down;
        line.SetObjAndLine(start, end, this);
        if (detectWater)
            line.StartDetectingWater();
        if (line.DetectClsn()) {
            if (line.clsnDist - up >= down)
                mCliffState = 1;
            if (!allowObjFloor) {
                dBgPi result;
                line.CopyTo(result);
                if (result.GetClsnID() != -1) {
                    mCliffState = 1;
                    return 1;
                }
            }
            Vector3 normal;
            line.surface.CopyNormalTo(normal);
            if (normal.y < FX_CosIdx((u16)maxSlope >> 4))
                mCliffState = 2;
        } else {
            mCliffState = 1;
        }
    }
    return mCliffState != 0;
}

/* On a wall, reflect the heading off it; on a cliff edge (mCliffState), turn
   around; otherwise report that nothing was done. */
int dEnemyBase_c::AngleAwayFromWallOrCliff(dBgCh_Actr &clsn, short &outAngle) {
    if (clsn.IsOnWall()) {
        outAngle = ReflectAngle(mWallNormal.x, mWallNormal.z, outAngle);
    } else if (mCliffState) {
        outAngle = (short)(mPrevAngle.y + 0x8000);
    } else {
        return 0;
    }
    return 1;
}

/* One frame of "this enemy is in Yoshi's mouth / has just been spat out". The
   three mFlags bits pick the phase: 0x40000 = held in the mouth (track the
   eater's position and return 2), 0x20000 = swallowed (return 1), 0x80000 = just
   spat out, in which case the launch velocity is built from the eater's facing
   angle and then the body flies under mesh collision until it lands.

   The launch block was the last residue: the cartridge fills the load-use slot
   after `ldr r0,[r6,#0xd0]` with the rounding constant (mov r3,#0x800) while every
   spelling tried here filled it with the position base (add r1,r6,#0x5c). Writing
   the eater test in its POSITIVE sense -- `if (eater != 0) { launch } else v =
   0x14000;` rather than `if (eater == 0) v = 0x14000; else { launch }` -- puts the
   launch block first in the linearisation and the slot falls the cartridge's way.
   Same branch, same semantics; only the order the two arms are visited in.

   The eater is mEatingPlayer, read as a dActor_c for its position, facing and
   speed. On landing, the floor normal's X/Z (scaled by 8) is added into
   mSpeed.x/mSpeed.z and mHorzSpeed becomes the horizontal length of the
   mSpeed.x/mSpeed.y/mSpeed.z triple. mSpitOutTimer is set to 5 at the spit
   and counted down here; the subclasses read 5 and 0 off it. */

/* The three dActor_c::mFlags bits Yoshi's mouth drives. */
enum { YOSHI_SWALLOWED = 0x20000, YOSHI_HOLDING = 0x40000, YOSHI_SPAT_OUT = 0x80000 };

/* Int-returning, as the 0/1 the ROM materialises before each test requires. */
static inline int HasFlag(int flags, int bit) {
    return (flags & bit) != 0;
}

int dEnemyBase_c::UpdateYoshiEat(dBgCh_Actr &clsn) {
    int flags;
    int v;
    int lim;

    flags = mFlags;
    if (HasFlag(flags, YOSHI_HOLDING)) {
        if (mEatingPlayer == 0) {
            mFlags &= ~YOSHI_HOLDING;
        } else {
            Vector3 *o = &mEatingPlayer->mPos;
            mPos.x = o->x;
            mPos.y = o->y;
            mPos.z = o->z;
            mPos.y += 0x30000;
        }
        mEatenByYoshi = 0;
        return 2;
    }
    if (HasFlag(flags, YOSHI_SWALLOWED)) {
        mEatenByYoshi = 0;
        return 1;
    }
    if (HasFlag(flags, YOSHI_SPAT_OUT) || mEatenByYoshi != 0) {
        if (mSpitOutTimer != 0)
            mSpitOutTimer -= 1;
        if (HasFlag(mFlags, YOSHI_SPAT_OUT)) {
            mEatenByYoshi = 1;
            mSpitOutTimer = 5;
            mFlags &= ~YOSHI_SPAT_OUT;
            if (mEatingPlayer != 0) {
                Vector3 *o2 = &mEatingPlayer->mPos;
                mPos.x = o2->x;
                mPos.y = o2->y;
                mPos.z = o2->z;
                mPos.y += 0x32000;
                {
                    u16 ang = mEatingPlayer->mAngle.y;
                    mPos.x += FX_MUL(FX_SinIdx(ang >> 4), 0x32000);
                    mPos.z += FX_MUL(FX_CosIdx(ang >> 4), 0x32000);
                }
                {
                    Vector3_16 *p = &mEatingPlayer->mAngle;
                    mPrevAngle.x = p->x;
                    mPrevAngle.y = p->y;
                    mPrevAngle.z = p->z;
                }
                v = mEatingPlayer->mHorzSpeed;
            } else {
                v = 0x14000;
            }
            if (v >= 0x14000)
                mHorzSpeed = v + 0xa000;
            else
                mHorzSpeed = FX_MUL(v, 0x800) + 0x14000;
            mHorzSpeed = FX_MUL(mHorzSpeed, 0x14cc);
            mSpeed.y = 0xc000;
            mEatingPlayer = 0;
        }
        lim = clsn.GetLimMovFlag();
        clsn.SetLimMovFlag();
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(&clsn);
        if (clsn.IsOnGround() != 0) {
            dBgPi *fr = clsn.GetFloorResult();
            fr->surface.CopyNormalTo(mFloorNormal);
            if (clsn.JustHitGround() == 0) {
                mSpeed.y = 0;
                mEatenByYoshi = 0;
            } else {
                fr->surface.CopyNormalTo(mFloorNormal);
                mSpeed.y = FX_Div((mSpeed.y * -50) / 100, mFloorNormal.y);
            }
            mSpeed.x += mFloorNormal.x << 3;
            mSpeed.z += mFloorNormal.z << 3;
            mHorzSpeed = Vec3_HorzLen(&mSpeed);
        }
        mAngle.y = mPrevAngle.y;
        if (lim == 0)
            clsn.ClearLimMovFlag();
        return 3;
    }
    mEatingPlayer = 0;
    mFlags &= ~(YOSHI_SWALLOWED | YOSHI_HOLDING | YOSHI_SPAT_OUT);
    mEatenByYoshi = 0;
    return 0;
}

/* While mEatenByYoshi is set, a cylinder collision against anything other than actor
   IDs 0x120/0x121 spawns the mega-character particles; otherwise bit 0x20000 on
   the collision is raised. Clearing mEatenByYoshi clears that bit instead. */

int dEnemyBase_c::SpawnParticlesIfHitOtherObj(dCc_c &clsn) {
    if (mEatenByYoshi != 0) {
        unsigned int id = clsn.otherOwner;
        if (id != 0) {
            dActor_c *other = dActor_c::FindWithID(id);
            if (other != 0) {
                unsigned short otherID = other->actorID;
                int isCoin = otherID == 0x120;
                if (!isCoin) {
                    int isRedCoin = otherID == 0x121;
                    if (!isRedCoin) {
                        SpawnMegaCharParticles(*other, (char *)&clsn);
                        return 1;
                    }
                }
            }
        }
        clsn.flags |= 0x20000;
    } else {
        clsn.flags &= ~0x20000;
    }
    return 0;
}

extern "C" {
extern void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern s32 *Vec3_AsrInPlace(s32 *v, s32 sh);
}

void dEnemyBase_c::SpawnMegaCharParticles(dActor_c &other, char *p) {
    s16 dirvec[3];
    Vector3 v;
    Vector3 dst, src;

    dst.x = mPos.x;
    dst.y = mPos.y;
    {
        Vector3 *pv = &other.mPos;
        dst.z = mPos.z;
        src.x = pv->x;
        src.y = pv->y;
        src.z = pv->z;
    }

    dirvec[0] = FX_SinIdx((u16)mAngle.y >> 4);
    dirvec[1] = 0;
    dirvec[2] = FX_CosIdx((u16)mAngle.y >> 4);

    if (p != 0) {
        Vector3 delta;
        s32 w = ((dCc_c *)p)->radius;
        s32 aX, aY;
        s32 iX, iY;
        s32 s;

        Vec3_Sub(&delta, &src, &dst);

        v.x = delta.x;
        v.y = delta.y;
        v.z = delta.z;

        aX = cstd::atan2(v.x, v.z);
        aY = cstd::atan2(v.y, Vec3_HorzLen(&v));

        iX = (u16)aX >> 4;
        iY = (u16)aY >> 4;

        s = FX_MUL(w, FX_CosIdx(iY));
        dst.x += FX_MUL(s, FX_SinIdx(iX));
        dst.y += FX_MUL(w, FX_SinIdx(iY));
        dst.z += FX_MUL(s, FX_CosIdx(iX));

        dst.y += ((dCc_c *)p)->height >> 1;
    } else {
        Vector3 delta;
        Vec3_Sub(&delta, &src, &dst);
        dst.x = delta.x;
        dst.y = delta.y;
        dst.z = delta.z;
        Vec3_AsrInPlace(&dst.x, 1);
        dst.y += 0x46000;
    }

    Particle::System::New(0, kPtclKnockAwayA, dst.x, dst.y, dst.z, 0, 0);
    Particle::System::New(0, kPtclKnockAwayB, dst.x, dst.y, dst.z, 0, 0);
    Particle::System::New(0, kPtclAttackHitA, dst.x, dst.y, dst.z, (const Vector3_16f *)(dirvec),
                          0);
    Particle::System::New(0, kPtclAttackHitB, dst.x, dst.y, dst.z, 0, 0);
}

/* What happens when a mega/invincible Mario runs an enemy over. The enemy is
   turned to face the player, given the launch velocity the caller passed, put into
   death state 8 with a 30-frame timer, and two particle systems are spawned at its
   position raised by whatever OnAimedAtWithEgg (vtable slot 29) reports.

   The third parameter is genuinely unused -- every caller materialises it and no
   body reads it. include/dEnemyBase_c.h carries the census that proves both the
   arity and this address's overlay; this member spent a long time spelled
   func_ov002_020ada40 because the mangled name was sitting on ov004's unrelated
   function at the same address. */
void dEnemyBase_c::KillByInvincibleChar(const Vector3_16 &spin, daPly_c &player, fx32) {
    mFlags &= ~0x10000001;
    mPrevAngle.y = Vec3_HorzAngle(&player.mPos, &mPos);
    mHorzSpeed = 0xa000;
    mSpeed.y = 0x28000;
    mDeathTimer = 30;
    mSpinRateX = spin.x;
    mSpinRateY = spin.y;
    mSpinRateZ = spin.z;
    mDeathState = 8;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x32000;
    Sound::PlayBank0(9, mCamSpacePos);
    Vector3 pos = mPos;
    pos.y += OnAimedAtWithEgg();
    Particle::System::NewSimple(kPtclKnockAwayA, pos.x, pos.y, pos.z);
    Particle::System::NewSimple(kPtclKnockAwayB, pos.x, pos.y, pos.z);
    player.IncMegaKillCount();
}

/* One frame of the death an invincible (mega) character inflicts. `flags` is a
   two-bit selector, not a boolean: bit 0 drops a coin, bit 1 books the kill in
   the death table. Both reference parameters are null-checked -- the compiler
   emits the test rather than folding it away, so `&ref == 0` is how the ROM's
   check is spelled. The model spins about the height OnAimedAtWithEgg (vtable
   slot 29) reports, the point Yoshi's egg aims at: it is lifted to that height,
   rotated, and dropped back. */
int dEnemyBase_c::UpdateKillByInvincibleChar(dBgCh_Actr &clsn, dExtAnmModel_c &anim,
                                             unsigned int flags) {
    Vector3 v;

    if (mDeathState != 8)
        return 0;

    if (mDeathTimer != 0)
        mDeathTimer -= 1;

    if (mDeathTimer == 0 || (&clsn != 0 && clsn.IsOnGround() != 0 && mSpeed.y < 0)) {
        if (flags & 1)
            SpawnCoin();
        if (flags & 2)
            KillAndTrackInDeathTable();
        mDeathState = 0;
        return 2;
    }

    UpdatePos(0);
    if (&clsn != 0) {
        UpdateWMClsn(clsn, 0);
        if (clsn.IsOnWall() != 0)
            mPrevAngle.y = ReflectAngle(mWallNormal.x, mWallNormal.z, mPrevAngle.y);
    }

    if (&anim != 0) {
        mAngle.x += mSpinRateX;
        mAngle.y += mSpinRateY;
        mAngle.z += mSpinRateZ;
        Vec3_Asr(&v, &mPos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, OnAimedAtWithEgg() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, mAngle.x, mAngle.y, mAngle.z);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, (-OnAimedAtWithEgg()) >> 3, 0);
        anim.mat4x3 = data_020a0e68;
    }
    return 1;
}

/* The two state-handler tables. func_ov002_020aea30 dispatches the first on
   mDeathState; UpdateDeath dispatches the second on mDeathState. Non-const
   pointer-to-member arrays live in .bss, so mwcc generates this TU's static
   initializer to copy the descriptor records in at overlay load. */
DeathStartPMF data_ov002_0210db80[8] = {
    &dEnemyBase_c::func_ov002_020ae9f8, &dEnemyBase_c::func_ov002_020ae954,
    &dEnemyBase_c::func_ov002_020ae890, &dEnemyBase_c::func_ov002_020ae87c,
    &dEnemyBase_c::func_ov002_020ae844, &dEnemyBase_c::func_ov002_020ae80c,
    &dEnemyBase_c::func_ov002_020ae73c, &dEnemyBase_c::func_ov002_020aea2c,
};

DeathPMF data_ov002_0210dbc0[8] = {
    &dEnemyBase_c::func_ov002_020ae64c, &dEnemyBase_c::func_ov002_020ae608,
    &dEnemyBase_c::func_ov002_020ae608, &dEnemyBase_c::func_ov002_020ae4cc,
    &dEnemyBase_c::func_ov002_020ae608, &dEnemyBase_c::func_ov002_020ae608,
    &dEnemyBase_c::func_ov002_020ae454, &dEnemyBase_c::func_ov002_020aea24,
};

/* Initialized data this file is the only complete referrer for. */
const unsigned short data_ov002_020ff014[4] = {288, 289, 290, 0};
const unsigned short data_ov002_020ff01c[6] = {150, 150, 400, 80, 400, 0};
