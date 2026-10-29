//cpp
/* daIbl_c: the rolling iron ball (IRONBALL), ov100.
 *
 * param1's low nibble is mVariant. Behavior runs that kind's handler:
 * 0 spawner, 1 free roll, 2 path (slope speed), 3 off-path (breaks at
 * mKillY), 4 path (ramps to the cap). Positions and speeds are 20.12;
 * angles are 0x10000 per turn; model and shadow matrices take pos >> 3.
 *
 * .text 0x02141f04..0x021431c4, between daBtfly_c_classInit and
 * daWanwan2_c's D1. Out-of-line destructor is the key function (D1
 * 0x02141f04, D0 0x02141f4c), so this TU emits the vtable and RTTI.
 * ROM-ascending under defer_codegen off. Factory daIbl_c_classInit
 * at 0x0214316c.
 */
/* leftovers:
 * - The eight helpers stay func_ov100_02141fb0 .. 02142b90. Behavior
 *   reads gIblKindHandlers by hand, through two-word records, rather
 *   than calling a pointer-to-member.
 * - Player::Hurt stays mangled: it takes Fix12<int> by value. The local
 *   Hurt declaration agrees with the current scalar definition.
 * - The spawner copies Player position through a byte view of the whole object
 *   into a real Vector3. The inherited position fields remain separate scalars.
 * - Spawn still borrows this actor's flat position and angle fields through
 *   the existing reference interface; those base fields are not vector objects.
 * - data_0209f2f8 is the level id (a signed byte). The tests 6, 0x16,
 *   0x18 and 0x19 have no level names here, and neither do the distances,
 *   speeds and kill heights chosen from them.
 * - func_020383f0 and func_02038414 are the two probes picked by that
 *   level id. func_02012694 plays a bank-3 sound at a position.
 * - Behavior's dispatch and InitResources' param1 shift keep the
 *   spellings the bytes need.
 */

#pragma defer_codegen off

#include "daIbl_c.h"
#include "types.h"
#include "nitro/fx/fx.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "SpawnInfo.h"
#include "Player.h"
#include "dExtSimpleModel_c.h"
#include "dActor_c.h"
#include "dBgCh_Actr.h"
#include "dCc_c.h"
#include "Sound.h"
#include "cstd.h"
#include "math/Approach.h"

/* Actor IDs from symbols/actor_debug_names.tsv. */
enum {
    daIbl_ACTOR_PLAYER   = 191,     /* PLAYER */
    daIbl_ACTOR_IRONBALL = 220      /* IRONBALL, this class: a spawner makes more of itself */
};

/* Bits of the ball's dCc_c hitFlags (mdCcAc_c.hitFlags). dCc_c.h says its
   bit table is a best-effort reading, so these name the table's entries. */
enum {
    daIbl_HIT_MEGA   = 0x10,    /* the table's "mega character" */
    daIbl_HIT_ATTACK = 0x3c0    /* the table's punch | kick | breakdance | slide kick */
};

/* dEnemyBase_c::mDeathState value that KillByInvincibleChar sets: the enemy is
   being knocked away spinning. */
enum { daIbl_DEATH_KNOCKED = 8 };

/* Sound IDs, as the callers pass them; the ID-to-sound mapping itself is not
   recovered here, so the names say when the ball plays them. */
enum {
    daIbl_SND_LAND          = 0x40,     /* the frame the ball touches down */
    daIbl_SND_ROLL          = 0x8a,     /* PlayLong's looping roll sound (with its argument 3) */
    daIbl_SND_BREAK         = 0x173,    /* the ball breaks up */
    daIbl_SND_HIT_NOT_BROKEN = 0xb5     /* attacked, but not by something that breaks it */
};

/* Particle played at the ball's position where it breaks up. */
enum { daIbl_PTCL_BREAK = 0xf };

/* The model and shadow matrices, as twelve words: a copy through the C++
   Matrix4x3 DIFFs. */
struct M48 { int w[12]; };

/* Behavior's handler table, gIblKindHandlers: one pointer-to-member
   per kind, as the two words mwcc lays one out in (function or vtable
   offset, then this-adjustment doubled with the virtual bit in bit 0). */
struct VtEntry {
    int field0;
    int field1;
};
typedef void (*FnPtr)(void *);

namespace Sound {
/* local extern: including Sound.h renumbers @NNN. */
int PlayLong(u32, u32, u32, const Vector3 &, s16);
}

extern "C" {
/* Defined below. */

namespace Particle {
struct System {
    static void NewSimple(u32 id, int x, int y, int z);
};
}
int func_02012694(int a, void *p);
void d_a_iblPlayLongHold(u32 a, u32 b, u32 c, void *v, u32 d);
void func_02038414(void *p);
int atan2_hold_d_a_ibl_1(int y, int x);
int func_ov002_020ad660(void *a, void *b, void *d, int e);
int Vec3_HorzLen(void *v);
int Vec3_HorzDist(const void *a, const void *b);
int Vec3_Equal(void *a, void *b);

extern int IDENTITY_MATRIX4X3[];
}

extern signed char data_0209f2f8;
extern int data_02092138;
extern s16 data_02082214[];

/* The model handle reaches its file through this wrapper's constructor and
 * destructor veneers; the manifest maps each to the retail destination.
 * A descriptive local type, not a recovered spelling. */
struct IblModelFile : SharedFilePtr {
    IblModelFile(u32 fileID);
    ~IblModelFile();
};

typedef void (daIbl_c::*IblKindHandler)();

/* This TU's statics, in the order the retail initializer (0x021474e8)
 * builds them: the ball's model handle (d_a_btfly.cpp also reads it,
 * through an extern), then one handler per kind. */
IblModelFile data_ov100_02148668(0x9c00);

IblKindHandler gIblKindHandlers[5] = {
    &daIbl_c::func_ov100_02142b90, &daIbl_c::func_ov100_02142918, &daIbl_c::func_ov100_021424c0,
    &daIbl_c::func_ov100_0214272c, &daIbl_c::func_ov100_021424c0,
};

// @symbol _ZN7daIbl_cD1Ev
// @symbol _ZN7daIbl_cD0Ev
/* D1 tears the members down in reverse declaration order, then the
 * dEnemyBase_c base. D0 is that teardown plus the base operator delete. */
daIbl_c::~daIbl_c()
{
}

#ifdef _MSC_VER
/* The host needs the ROM's flat D0 name, and MSVC never emits it: it folds
 * the Itanium destructor variants into the one ~daIbl_c() above. This arm
 * spells out what the deleting destructor does -- the D1 body, called
 * qualified so it is a direct call, then the class-specific operator
 * delete. Nothing here reaches mwccarm. */
extern "C" daIbl_c *_ZN7daIbl_cD0Ev(daIbl_c *thiz)
{
    thiz->daIbl_c::~daIbl_c();
    daIbl_c::operator delete(thiz);
    return thiz;
}
#endif

// @symbol _ZN7daIbl_c16OnAimedAtWithEggEv
/* Returns 0x82000 (130 units). func_ov100_02141fb0 adds it to the ball's Y
 * before KillByInvincibleChar. */
s32 daIbl_c::OnAimedAtWithEgg()
{
    return 532480;
}

// @symbol _ZN7daIbl_c19func_ov100_02141fb0Ev
/* Contact check, shared by every handler. mdCcAc_c.otherOwner is the uniqueID
   of the actor that touched the ball; when that is a Player (actor 191) who is
   not vanished (mIsVanish), mdCcAc_c.hitFlags decides what happens:
     - the mega-character bit: the ball is lifted by OnAimedAtWithEgg's answer
       (130 units), handed to KillByInvincibleChar (which starts the
       mDeathState 8 knock-away that func_ov002_020ad660 then steps), and its
       ground flag is cleared;
     - an attack bit (punch, kick, breakdance, slide kick, per dCc_c.h's
       best-effort and unproven bit table): when the Player's
       param1 is 2 (read here as its character index; index 2 is Wario in
       SaveData's unlock-bit order) the ball breaks up (particle, poof, MarkForDestruction, break
       sound); any other character only gets the not-broken sound;
     - any other contact hurts the Player (Player::Hurt, given the ball's
       position). */
void daIbl_c::func_ov100_02141fb0()
{
    Player *a;
    u32 fl;
    u32 id = mdCcAc_c.otherOwner;

    if (id == 0) return;
    dActor_c *actor = dActor_c::FindWithID(id);
    if (actor == 0) return;
    {
        int b = (actor->actorID == daIbl_ACTOR_PLAYER);
        if (b == 0) return;
    }
    a = static_cast<Player *>(actor);
    if (a->mIsVanish != 0) return;
    fl = mdCcAc_c.hitFlags;
    if ((fl & daIbl_HIT_MEGA) != 0) {
        Vector3_16 s;
        u32 r = OnAimedAtWithEgg();
        mPosY += r;
        s.x = 0; s.y = 0; s.z = 0;
        KillByInvincibleChar(s, *a, 0);
        mWithMeshClsn.ClearGroundFlag();
        return;
    }
    if ((fl & daIbl_HIT_ATTACK) != 0) {
        if (a->param1 == 2) {
            Particle::System::NewSimple(daIbl_PTCL_BREAK, mPosX, mPosY, mPosZ);
            PoofDust();
            MarkForDestruction();
            func_02012694(daIbl_SND_BREAK, &mCamSpacePosX);
            return;
        }
        Sound::PlayBank0(daIbl_SND_HIT_NOT_BROKEN, mCamSpacePos);
        return;
    }
    {
        Vector3 v;
        v.x = mPosX;
        v.y = mPosY;
        v.z = mPosZ;
        a->Hurt(v, 2, 0xc000, 1, 0, 1);
    }
}

// @symbol _ZN7daIbl_c19func_ov100_02142130Ev
/* The wall and floor probe. mHitWall is cleared, the collision object is
   updated (sublevel 0x19 runs func_020383f0, every other sublevel
   func_02038414; both unidentified), and mHitWall is set when the ball touches
   a wall and the angle between the ball's heading mPrevAngleY and the wall
   normal's angle exceeds a quarter turn (0x4000), i.e. it is heading into
   the wall. On the ground it copies the floor normal to mFloorNormal and,
   unless it hit a wall, puts X and Z back to last frame's position and, when
   the floor normal's Y is non-zero, sets mVertSpeed to -(slope + 8
   units/frame), where slope = (n.x * vel.x + n.z * vel.z) / n.y, so the ball
   keeps to the slope. */
void daIbl_c::func_ov100_02142130()
{
    mHitWall = 0;
    if (data_0209f2f8 == 0x19) func_020383f0(&mWithMeshClsn);
    else func_02038414(&mWithMeshClsn);
    if (mWithMeshClsn.IsOnWall() != 0) {
        ((SurfaceInfo *)((char *)mWithMeshClsn.GetWallResult() + 4))
            ->CopyNormalTo(*(Vector3 *)&mWallNormalX);
        short a = cstd::atan2(mWallNormalX, mWallNormalZ);
        if (GetSubtraction(mPrevAngleY, a) > 0x4000)
            mHitWall = 1;
    }
    if (mWithMeshClsn.IsOnGround() == 0)
        return;
    ((SurfaceInfo *)((char *)mWithMeshClsn.GetFloorResult() + 4))
        ->CopyNormalTo(*(Vector3 *)&mFloorNormalX);
    if (mHitWall != 0) return;
    mPosX = mPrevPosX;
    mPosZ = mPrevPosZ;
    if (mFloorNormalY == 0) return;
    int s = FX_MUL(mFloorNormalX, mSpeedX) + FX_MUL(mFloorNormalZ, mSpeedZ);
    mVertSpeed = -(cstd::fdiv(s, mFloorNormalY) + 0x8000);
}

// @symbol _ZN7daIbl_c19func_ov100_02142264Ev
/* Per-frame model and shadow matrices. The model matrix is the identity with
   its translation set to mPos >> 3 (the ball's position in model units). It
   is copied to the shadow's matrix, whose Y translation is then replaced with
   (50 units * scale + mPosY) >> 3 -- 50 units times the draw scale above the
   ball's position -- and the drop shadow is cast with radius 200 units * scale,
   depth 90 units * scale on the ground (0x32 + 0x28) or 450 units * scale in
   the air (0x32 + 0x190), and opacity 0xf. "Scale" is mDrawScale.x, 0x1000
   being 1.0. */
void daIbl_c::func_ov100_02142264()
{
    *(M48 *)&mModel.mat4x3 = *(M48 *)IDENTITY_MATRIX4X3;
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
    *(M48 *)mShadowMtx = *(M48 *)&mModel.mat4x3;
    int k = 0x32;
    mShadowMtx[10] = (mDrawScale.x * k + mPosY) >> 3;
    if (mWithMeshClsn.IsOnGround() != 0)
        k += 0x28;
    else
        k += 0x190;
    DropShadowRadHeight(mShadowModel, *(Matrix4x3 *)mShadowMtx, mDrawScale.x * 0xc8,
                        k * mDrawScale.x, 0xf);
}

// @symbol _ZN7daIbl_c19func_ov100_0214233cEv
/* Path-node advance. mNextNodePos is the node the ball is heading for and
 * mPrevNodePos the one it last passed. Stores the atan2 heading to the node
 * in mHeadingToNode, then takes a 3-axis sign-dot of (node-prev) vs
 * (node-pos). If the actor is not still approaching the node (dot <= 0),
 * copy node->prev, advance path index (wrap), fetch next node.
 * Returns -1 when the index wrapped to 0, else 1; 0 if still approaching.
 *
 * Codegen notes (mwccarm 1.2/sp2p3):
 *  - Long-lived values use 6q variable-identity mapping so birth-order colors
 *    match the ROM (A holds C8, B holds C7, C8 holds C6, ...).
 *  - Sign result s8/s6 names are swapped vs the values they hold so free-reg
 *    dest colors land on r1/r3 like the ROM; mul uses those names accordingly.
 *  - u64-launder materializes pidx between the first path-node load and store.
 */
int daIbl_c::func_ov100_0214233c()
{
    int px = mNextNodePosX;
    int ax = mPosX;
    int prevx = mPrevNodePosX;
    /* name A  = node.x - prev.x  (C8)  -- high-priority web -> r8 */
    int A = px - prevx;
    int pz = mNextNodePosZ;
    int az = mPosZ;
    int prevz = mPrevNodePosZ;
    int py = mNextNodePosY;
    int prevy = mPrevNodePosY;
    /* name B  = node.y - prev.y  (C7) */
    int B = py - prevy;
    /* name C8 = node.z - prev.z  (C6) */
    int C8 = pz - prevz;
    int ay = mPosY;
    /* name C7 = node.y - pos.y   (Csl) */
    int C7 = py - ay;
    /* name C6 = node.x - pos.x   (true A) */
    int C6 = px - ax;
    /* name Csl= node.z - pos.z   (true B) */
    int Csl = pz - az;
    int sA, sB, s8, s7, s6, ssl;
    int dot;

    mHeadingToNode = cstd::atan2(C6, Csl);

    /* s6/s8 dest names swapped vs the values they store (free-reg coloring). */
    if (A != 0) {
        if (A < 0)
            s6 = -1;
        else
            s6 = 1;
    } else {
        s6 = 0;
    }
    if (B != 0) {
        if (B < 0)
            s7 = -1;
        else
            s7 = 1;
    } else {
        s7 = 0;
    }
    if (C8 != 0) {
        if (C8 < 0)
            s8 = -1;
        else
            s8 = 1;
    } else {
        s8 = 0;
    }
    if (C6 != 0) {
        if (C6 < 0)
            sA = -1;
        else
            sA = 1;
    } else {
        sA = 0;
    }
    if (C7 != 0) {
        if (C7 < 0)
            ssl = -1;
        else
            ssl = 1;
    } else {
        ssl = 0;
    }
    if (Csl != 0) {
        if (Csl < 0)
            sB = -1;
        else
            sB = 1;
    } else {
        sB = 0;
    }

    /* s6=sign(C8), s8=sign(C6) after the dest-name swap above */
    dot = s6 * sA + s7 * ssl + s8 * sB;
    if (dot <= 0) {
        int v = mNextNodePosX;
        int *pidx = &mPathNodeIndex;
        mPrevNodePosX = v;
        v = mNextNodePosY;
        mPrevNodePosY = v;
        v = mNextNodePosZ;
        mPrevNodePosZ = v;
        *pidx = *pidx + 1;
        if (mPathNodeIndex >= mNumPathNodes) {
            mPathNodeIndex = 0;
        }
        mPathPtr.GetNode(*(Vector3 *)&mNextNodePosX, mPathNodeIndex);
        return (mPathNodeIndex == 0) ? -1 : 1;
    }
    return 0;
}

// @symbol _ZN7daIbl_c19func_ov100_021424c0Ev
/* Kinds 2 and 4, the path followers. First the knock-away death that
 * KillByInvincibleChar starts: func_ov002_020ad660 (a sibling of
 * dEnemyBase_c's UpdateKillByInvincibleChar at 0x020ad838, same mDeathState 8
 * check) steps it (0 = not dying, 1 = still spinning, 2 = it just ended; with
 * the flags 3 passed here it also spawns the coin and calls
 * KillAndTrackInDeathTable itself before returning 2) and, on 2, the ball
 * puffs (a Particle) and plays the break sound. Then the contact check, and with mDeathState 8 the handler
 * is done. Otherwise func_ov100_0214233c advances along the path and the
 * heading mPrevAngleY turns toward mHeadingToNode by at most 0x800 (11.25
 * degrees) a frame. When the path index wraps (-1) the ball becomes kind 3
 * and, if it is on the ground, has its vertical speed zeroed and is lifted 15
 * units (so it falls again); otherwise it runs the probe. If the path index
 * did not wrap it probes, then: a wall newly hit reflects the heading
 * and restarts the roll sound; on the ground a touch-down plays the landing
 * sound and dust and bounces it up at half the impact speed when it fell
 * faster than 20 units/frame, while a roll plays the rolling sound; and
 * horizontal speed is capped at 0x23000 (35 units/frame) -- below the cap, kind
 * 4 ramps toward it by 0x400 (0.25 units/frame) a frame, and kind 2 adds
 * the floor normal's X and Z to its velocity. Last comes the collision
 * update. */
void daIbl_c::func_ov100_021424c0() {
    int r;
    int vy;

    MakeVanishLuigiWork(mdCcAc_c);
    r = func_ov002_020ad660(this, &mWithMeshClsn, &mModel, 3);
    if (r != 0) {
        if (r != 2)
            return;
        Particle::System::NewSimple(daIbl_PTCL_BREAK, mPosX, mPosY, mPosZ);
        func_02012694(daIbl_SND_BREAK, &mCamSpacePosX);
        return;
    }

    func_ov100_02141fb0();
    if (mDeathState == daIbl_DEATH_KNOCKED)
        return;

    r = func_ov100_0214233c();
    ApproachLinear(mPrevAngleY, mHeadingToNode, 0x800);

    if (r == -1) {
        mVariant = daIbl_KIND_ROLL_OFF;
        /* mVariant was just set to 3, so the != 4 test cannot fail here. */
        if (mVariant != daIbl_KIND_PATH_RAMP && mWithMeshClsn.IsOnGround() != 0) {
            mVertSpeed = 0;
            mPosY += 0xf000;
        } else {
            func_ov100_02142130();
        }
    } else {
        int had = (mHitWall != 0);
        vy = mVertSpeed;
        func_ov100_02142130();
        if (mHitWall != 0 && had == 0) {
            mPrevAngleY = ReflectAngle(mWallNormalX, mWallNormalZ, mPrevAngleY);
            mRollSoundHandle = 0;
        } else if (mWithMeshClsn.IsOnGround() != 0) {
            if (mWithMeshClsn.JustHitGround() != 0) {
                func_02012694(daIbl_SND_LAND, &mCamSpacePosX);
                LandingDust(1);
                if (vy < -0x14000)
                    mVertSpeed = (int)(-vy + ((u32)-vy >> 31)) >> 1;
                mRollSoundHandle = 0;
            } else {
                mRollSoundHandle = Sound::PlayLong(mRollSoundHandle, 3, daIbl_SND_ROLL,
                                                   *(Vector3 *)&mCamSpacePosX, 0);
            }
            if (mHorzSpeed >= 0x23000) {
                mHorzSpeed = 0x23000;
            } else if (mVariant == daIbl_KIND_PATH_RAMP) {
                ApproachLinear(mHorzSpeed, 0x23000, 0x400);
            } else {
                mSpeedX += mFloorNormalX;
                mSpeedZ += mFloorNormalZ;
                mHorzSpeed = Vec3_HorzLen(&mSpeedX);
            }
        }
    }

    UpdatePos(0);
    func_ov100_02142264();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
}

// @symbol _ZN7daIbl_c19func_ov100_0214272cEv
/* Kind 3, the ball that has left its path. After the knock-away death step
   (see func_ov100_021424c0) and the contact check, it breaks up in a triple
   poof when it is below the kill height mKillY or the probe says it hit a
   wall. Otherwise, on the ground it adds three times the floor normal's X and Z
   to its velocity (so it rolls downhill), points its heading along that
   velocity, and on touch-down plays the landing sound and dust and, when the
   vertical speed the probe just left (-(slope + 8 units/frame)) is below
   -8 units/frame, replaces it with 1.5 times its magnitude; in a roll it
   plays the rolling sound. Vertical speed falls by gravity down
   to the terminal velocity, then the position updates from speed alone. */
void daIbl_c::func_ov100_0214272c()
{
    int r;

    MakeVanishLuigiWork(mdCcAc_c);
    r = func_ov002_020ad660(this, &mWithMeshClsn, &mModel, 3);
    if (r != 0) {
        if (r != 2)
            return;

        Particle::System::NewSimple(daIbl_PTCL_BREAK, mPosX, mPosY, mPosZ);
        func_02012694(daIbl_SND_BREAK, &mCamSpacePosX);
        return;
    }

    func_ov100_02141fb0();
    func_ov100_02142130();

    if (mPosY >= mKillY) {
        if (mHitWall == 0)
            goto ground;
    }

    Particle::System::NewSimple(daIbl_PTCL_BREAK, mPosX, *(volatile int *)&mPosY, mPosZ);
    func_02012694(daIbl_SND_BREAK, &mCamSpacePosX);
    TriplePoofDust();
    MarkForDestruction();
    return;

ground:
    if (mWithMeshClsn.IsOnGround()) {
        int *pa4;
        int *pac;

        pa4 = &mSpeedX;
        pac = &mSpeedZ;

        *pa4 = *pa4 + mFloorNormalX * 3;
        *pac = *pac + mFloorNormalZ * 3;
        mHorzSpeed = Vec3_HorzLen(pa4);

        mPrevAngleY = cstd::atan2(mSpeedX, mSpeedZ);

        if (mWithMeshClsn.JustHitGround()) {
            func_02012694(daIbl_SND_LAND, &mCamSpacePosX);
            LandingDust(1);

            if (mVertSpeed < -0x8000)
                mVertSpeed = mVertSpeed * -3 / 2;
        } else {
            mRollSoundHandle =
                Sound::PlayLong(mRollSoundHandle, 3, daIbl_SND_ROLL, *(Vector3 *)&mCamSpacePosX, 0);
        }
    }

    {
        int v;
        int lim;

        v = mVertSpeed + mVertAccel;
        lim = mTerminalVelocity;
        if (v >= lim)
            lim = v;
        mVertSpeed = lim;
    }

    UpdatePosWithOnlySpeed(0);
    func_ov100_02142264();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
}

// @symbol _ZN7daIbl_c19func_ov100_02142918Ev
/* Kind 1, the free-rolling ball: after the knock-away death step and the
   contact check (a mDeathState of 8 ends the handler), it bounces off walls
   and rolls down slopes. A wall hit reflects the heading and rebuilds the
   velocity as speed times the sine and cosine table's entries for the new
   heading (X from the even entry, Z from the odd), then three times the
   wall normal is subtracted from the velocity. On the ground it adds three times the floor normal's X and Z
   to the velocity and, on flat ground (normal Y == 0x1000, 1.0), caps its speed at
   0x1c000 (28 units/frame), pointing the heading along the velocity and
   advancing the position along it. A touch-down plays the landing sound and
   dust; otherwise it plays the rolling sound. Its legacy source carried the
   recovered name Butterfly_Kill ("daBtfly_c::Kill, from vtable slot
   identity"); no vtable holds it -- its only reference is the
   pointer-to-member constant at 0x02147f18 that seeds entry 1 of
   Behavior's table. */
void daIbl_c::func_ov100_02142918()
{
    int r;
    MakeVanishLuigiWork(mdCcAc_c);
    r = func_ov002_020ad660(this, &mWithMeshClsn, &mModel, 3);
    if (r != 0) {
        if (r != 2)
            return;
        Particle::System::NewSimple(daIbl_PTCL_BREAK, mPosX, mPosY, mPosZ);
        func_02012694(daIbl_SND_BREAK, &mCamSpacePosX);
        return;
    }

    func_ov100_02141fb0();
    if (mDeathState == daIbl_DEATH_KNOCKED)
        return;
    func_ov100_02142130();

    if (mHitWall != 0) {
        int ang;
        mPrevAngleY = ReflectAngle(mWallNormalX, mWallNormalZ, mPrevAngleY);
        ang = *(u16 *)&mPrevAngleY;
        mSpeedX = FX_MUL(mHorzSpeed, data_02082214[(ang >> 4) << 1]);
        ang = *(u16 *)&mPrevAngleY;
        mSpeedZ = FX_MUL(mHorzSpeed, data_02082214[((ang >> 4) << 1) + 1]);
        {
            int *pa4 = &mSpeedX;
            int *pac = &mSpeedZ;
            *pa4 = *pa4 - mWallNormalX * 3;
            *pac = *pac - mWallNormalZ * 3;
            mHorzSpeed = Vec3_HorzLen(pa4);
        }
        goto Lend;
    }

    if (mWithMeshClsn.IsOnGround() == 0)
        goto Lend;

    {
        int *pa4 = &mSpeedX;
        int *pac = &mSpeedZ;
        *pa4 = *pa4 + mFloorNormalX * 3;
        *pac = *pac + mFloorNormalZ * 3;
        mHorzSpeed = Vec3_HorzLen(pa4);
    }
    if (mFloorNormalY == 0x1000 && mHorzSpeed > 0x1c000) {
        mHorzSpeed = 0x1c000;
        mPrevAngleY = cstd::atan2(mSpeedX, mSpeedZ);
        UpdatePosWithHorzSpeedAndAng();
    }

    if (mWithMeshClsn.JustHitGround() != 0) {
        func_02012694(daIbl_SND_LAND, &mCamSpacePosX);
        LandingDust(1);
        goto Lend;
    }

    mRollSoundHandle =
        Sound::PlayLong(mRollSoundHandle, 3, daIbl_SND_ROLL, *(Vector3 *)&mCamSpacePosX, 0);

Lend: {
    int nv = mVertSpeed + mVertAccel;
    int lim = mTerminalVelocity;
    if (nv >= lim)
        lim = nv;
    mVertSpeed = lim;
}
    UpdatePosWithOnlySpeed(0);
    func_ov100_02142264();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
}

// @symbol _ZN7daIbl_c19func_ov100_02142b90Ev
/* Kind 0, the spawner. mStateTimer is its countdown. When it is zero, and the
   spawner has fewer than 3 balls out in sublevel 6 or 6 elsewhere
   (mLiveBalls), it looks at the closest player: unless the ball it would
   spawn is kind 4 (see below), the player must be more than 40 units below it
   (0x28000), and the horizontal distance to the player must lie within
   mMinSpawnDist..mMaxSpawnDist. In sublevel 0x16 a player lower than -2500
   units stops it. Passing all that reloads mStateTimer (with 0x3f in sublevel
   0x16, 0x7f otherwise) and spawns another IRONBALL (actor 220) at its own
   position with its own param1, area ID and angles. param1 was shifted down by
   four in InitResources, so what the new ball takes as its kind (the low
   nibble) is the spawner's second nibble, and (param1 & 0xf) != 4 is the test
   for "the spawned ball is not kind 4". The new ball stores the spawner in
   mDispenser, and mLiveBalls counts it. */
void daIbl_c::func_ov100_02142b90()
{
    Vector3 pos;
    u16 *timer = (u16 *)&mStateTimer;
    Player *pl;
    int r1;

    if (*timer != 0) {
        *timer = *timer - 1;
        return;
    }

    if (data_0209f2f8 == 6) r1 = 3; else r1 = 6;
    if (mLiveBalls >= (u32)r1) return;

    pl = ClosestPlayer();
    if (pl == 0) return;

    {
        /* dActor_c still declares these as separate s32 subobjects at
           0x5c/0x60/0x64. Read them through the whole Player representation;
           this creates neither a vector view nor an array over those fields.
           Direct named loads lose the shared address under 2004/b56. */
        const unsigned char *coords = reinterpret_cast<const unsigned char *>(pl) + 0x5c;
        pos.x = *reinterpret_cast<const s32 *>(coords);
        pos.y = *reinterpret_cast<const s32 *>(coords + 4);
        pos.z = *reinterpret_cast<const s32 *>(coords + 8);
    }

    if ((param1 & 0xf) != daIbl_KIND_PATH_RAMP) {
        if (pos.y >= mPosY - 0x28000) return;
    }

    {
        int d = Vec3_HorzDist(&mPosX, &pos);
        if (d < mMinSpawnDist) return;
        if (d > mMaxSpawnDist) return;
    }

    if (data_0209f2f8 == 0x16) {
        if (pos.y < (int)0xff63c000) return;
        mStateTimer = 0x3f;
    } else {
        mStateTimer = 0x7f;
    }

    {
        daIbl_c *a;
        int cc = mAreaId;
        a = (daIbl_c *)dActor_c::Spawn(daIbl_ACTOR_IRONBALL, param1, mPos, &mPrevAngle, cc, -1);
        if (a == 0) return;
        {
            u8 *cnt = &mLiveBalls;
            *cnt = *cnt + 1;
        }
        a->mDispenser = this;
    }
}

// @symbol _ZN7daIbl_c16CleanupResourcesEv
/* Gives a spawner's slot back: a ball made by a spawner holds it in
   mDispenser, and its count of balls out drops by one. Then the model file
   reference is released. */
int daIbl_c::CleanupResources()
{
    daIbl_c *dispenser = mDispenser;

    if (dispenser != 0) {
        (dispenser->mLiveBalls)--;
    }

    data_ov100_02148668.Release();
    return 1;
}

// @symbol _ZN7daIbl_c6RenderEv
/* Draws the model with mDrawScale, except kind 0, the spawner, which has
   nothing to draw. */
int daIbl_c::Render()
{
    unsigned char b = mVariant;
    if (b) {
        mModel.Render(&mDrawScale);
    }
    return 1;
}

// @symbol _ZN7daIbl_c8BehaviorEv
/* Calls the pointer-to-member handler for this ball's kind from the table at
   gIblKindHandlers, on this actor. */
int daIbl_c::Behavior()
{
    unsigned char idx = mVariant;
    struct VtEntry *e = (struct VtEntry *)((char *)gIblKindHandlers + ((int)idx << 3));
    int f1 = e->field1;
    void *obj = (void *)((char *)((void *)this) + (f1 >> 1));
    FnPtr fn;
    if (f1 & 1) {
        fn = (FnPtr)*(int *)((char *)*(int **)obj + e->field0);
    } else {
        fn = (FnPtr)e->field0;
    }
    fn(obj);
    return 1;
}

// @symbol _ZN7daIbl_c13InitResourcesEv
/* Loads the model and shadow, sets gravity (-4 units/frame^2, mVertAccel
   -0x4000) and terminal velocity (-70 units/frame, -0x46000), picks the kind
   from param1's low nibble and sets up that kind. Distances and speeds below
   are in units (0x1000 = 1). The sublevel tests read data_0209f2f8 (a signed
   byte): 6, 0x16, 0x18 and 0x19 are the sublevels this class has special
   numbers for. */
int daIbl_c::InitResources()
{
    int kind;
    int d;

    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov100_02148668), 1, -1);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mVertAccel = -0x4000;
    mTerminalVelocity = -0x46000;
    mVariant = param1 & 0xf;
    /* The spawn word is packed: low nibble was the variant (read above), the
     * rest shifts down in place here. The redundant cast is load-bearing:
     * plain `param1 = param1 >> 4` value-numbers both sides together and
     * emits an extra `add r1, r4, #8` at +0x60 (0x390, 186/228 words
     * differ); the cast keeps the ROM's folded `ldr/lsr/str` (0x38c, 0
     * differ). Same lever as daDoor_c::InitResources. */
    param1 = (u32)param1 >> 4;
    kind = mVariant;

    /* Kinds 2 and 4: the path followers. The path ID is the next nibble of
       param1. mPrevNodePos is set to where the ball spawned and mNextNodePos to
       the path's node 0 (node 1 when the ball is already standing on node 0),
       the node it heads for first. */
    if (kind == daIbl_KIND_PATH_SLOPE || kind == daIbl_KIND_PATH_RAMP) {
        mPathPtr.FromID(param1 & 0xf);
        mNumPathNodes = mPathPtr.NumNodes();
        mPathNodeIndex = 0;
        mPrevNodePosX = mPosX;
        mPrevNodePosY = mPosY;
        mPrevNodePosZ = mPosZ;
        mPathPtr.GetNode(*(Vector3 *)&mNextNodePosX, mPathNodeIndex);
        if (Vec3_Equal(&mPosX, &mNextNodePosX)) {
            mPathNodeIndex += 1;
            mPathPtr.GetNode(*(Vector3 *)&mNextNodePosX, mPathNodeIndex);
        }
        func_ov100_0214233c();
        mPrevAngleY = mHeadingToNode;
        mKillY = *(int *)&data_02092138;
        d = *(signed char *)&data_0209f2f8;
        /* Sublevel 0x19 is the half-size ball: speed 10 units/frame, scale
           0.5 (0x800), collider radius and height 30 units. */
        if (d == 0x19) {
            mHorzSpeed = 0xa000;
            mDrawScale.x = 0x800;
            mDrawScale.y = 0x800;
            mDrawScale.z = 0x800;
            mdCcAc_c.Init(this, 0x1e000, 0x1e000, 0x200004, 0x3c0);
            mWithMeshClsn.Init(this, 0x1e000, 0x1e000, 0, 0);
            mKillY = -0x640000;        /* -1600 units */
        } else {
            mDrawScale.x = 0x1000;
            mDrawScale.y = 0x1000;
            mDrawScale.z = 0x1000;
            mdCcAc_c.Init(this, 0x64000, 0x64000, 0x200004, 0x3c0);
            mWithMeshClsn.Init(this, 0x64000, 0x64000, 0, 0);
            /* Full size: scale 1.0, collider radius and height 100 units.
               Speed per frame: 25 units in sublevel 0x18, 20 in 6, else 10. */
            d = *(volatile signed char *)&data_0209f2f8;
            if (d == 0x18) {
                mHorzSpeed = 0x19000;
                mKillY = 0xfec78000;       /* -5000 units */
            } else if (d == 0x16) {
                mHorzSpeed = 0xa000;
                mKillY = 0xff63c000;       /* -2500 units */
            } else {
                if (d == 6)
                    mHorzSpeed = 0x14000;
                else
                    mHorzSpeed = 0xa000;
            }
        }
        UpdatePos(0);
        mWithMeshClsn.SetLimMovFlag();
        goto end;
    }

    /* Kind 1, the free roller: no path. mFlags |= 1 clip-tests the ball, so
       its Behavior is skipped while it is off screen. */
    if (kind == daIbl_KIND_FREE_ROLL) {
        mFlags |= 1;
        mDrawScale.x = 0x1000;
        mDrawScale.y = 0x1000;
        mDrawScale.z = 0x1000;
        mdCcAc_c.Init(this, 0x64000, 0x64000, 0x200004, 0x3c0);
        mWithMeshClsn.Init(this, 0x64000, 0x64000, 0, 0);
        mWithMeshClsn.SetLimMovFlag();
        goto end;
    }

    /* Kind 0, the spawner: only picks the distance band, in units. Sublevel
       0x19: 512 to 6000. Otherwise the minimum is 1024 and the maximum 7000 in
       sublevels 0x18 and 0x16, 4600 in 6, 6500 elsewhere. */
    if (kind == daIbl_KIND_SPAWNER) {
        d = *(signed char *)&data_0209f2f8;
        if (d == 0x19) {
            mMinSpawnDist = 0x200000;
            mMaxSpawnDist = 0x1770000;
        } else {
            mMinSpawnDist = 0x400000;
            if (d == 0x18)
                mMaxSpawnDist = 0x1b58000;
            else if (d == 6)
                mMaxSpawnDist = 0x11f8000;
            else if (d == 0x16)
                mMaxSpawnDist = 0x1b58000;
            else
                mMaxSpawnDist = 0x1964000;
        }
    }

end:
    mStateTimer = 0;
    mCoinType = 0; /* a dEnemyBase_c byte; nothing in this file reads it */
    mRollSoundHandle = 0;
    mDispenser = 0;
    mLiveBalls = 0;
    return 1;
}

// @symbol daIbl_c_classInit
/* return new constructs the ball and its members. */
extern "C" daIbl_c *daIbl_c_classInit(void)
{
    return new daIbl_c;
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h). The
 * retail values are unchanged. */
SpawnInfo<daIbl_c> g_profile_IRONBALL = {
    daIbl_c_classInit, /* classInit */
    0x00dc,            /* behaviorPriority: the IRONBALL registry id */
    0x0045,            /* renderPriority */
    2,                 /* actorFlags */
    0x64000,           /* clipOffsetY */
    0x7d0000,          /* clipRadius */
    0x01800000,        /* clipDistance */
    0x01800000         /* farDistance */
};
