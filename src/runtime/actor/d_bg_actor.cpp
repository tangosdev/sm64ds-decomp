//cpp
/* dBgActor_c -- the base of the level-object family (ov002).
 *
 * Platforms, lifts, blocks: a model, a moving mesh collider, and the matrix
 * pinning the collider to the model. Kill (slot 31) is the one virtual this
 * class adds -- and the key function, so this TU owns the vtable with D1/D0
 * dragged in beside it. Mega Mario launches these actors flying via
 * KillByMegaChar/UpdateKillByMegaChar; IsClsnInRange* toggle the collider by
 * camera distance. Size is asserted in include/dBgActor_c.h (0x320).
 * The TU ends with the collision events a mover sends to the object whose
 * collider it hit (0x020eea84..0x020ef320, first in this file).
 *
 * Written last-to-first: mwccarm emits one .text section per function in
 * reverse source order. Do not reorder.
 *
 * deslop leftovers:
 * - IsClsnInRange / IsClsnInRangeOnScreen stay extern "C" free functions with
 *   int params: the ROM names claim Fix12<int> by value and the bytes refuse
 *   it (notes/mwccarm-codegen.md 6az), which is why the header deliberately
 *   does not declare them. Same spelling every leaf call site uses.
 * - func_ov002_020ee5d0 keeps its ROM label: the mega-knockback model-matrix
 *   helper, called only from UpdateKillByMegaChar. C linkage is load-bearing.
 * - dActor_c::Earthquake / Particle::System::NewSimple stay TU-local mangled
 *   calls: Fix12<int> by value (6az); typed shared-header decls of them are
 *   overload poison (S13).
 * - Player is only forward-declared here, so the closest player's position
 *   is read through a dActor_c cast rather than pulling in Player.h.
 * - data_020a0e68 is the shared scratch matrix; no header owns it, TU-local.
 */
/* dBgActor_c.h FIRST: it pulls common.h ahead of dExtSimpleModel_c.h, which is the
 * Matrix4x3-spelling rule its own header comment records. */
#include "dBgActor_c.h"
#include "dBgW.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Actr.h"
#include "Player.h"
#include "Sound.h"

/* ROM symbols with no header of their own. Spelled to agree with their
 * definitions (const Vector3 * Vec3 pairs, void * rotation target); the two
 * mangled names keep TU-local scalar spellings per the leftovers above. */
extern "C" {
/* Forward: UpdateKillByMegaChar calls down to it (reverse layout). */
void func_ov002_020ee5d0(dBgActor_c *self, int pivotY);

s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
namespace Particle {
struct System {
    static void NewSimple(u32 id, Fix12i x, Fix12i y, Fix12i z);
};
}
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_FromRotationY(void *m, int angleY);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
unsigned char DecIfAbove0_Byte(unsigned char *p);
extern Matrix4x3 data_020a0e68;
}

/* --- Collision events, 0x020eea84..0x020ef320. ------------------------------
 *
 * The character side of a hit on a level object: given the mover's dBgCh_Actr
 * (its mesh collider checker) and the mover, find the actor that owns the
 * collider it touched -- a dBgActor_c -- and deliver one dActor_c event to it.
 * One function per event, written from the top slot down:
 *
 *   func_ov002_020ef2a4  floor    -> OnGroundPounded           (slot 21)
 *   func_ov002_020ef228  wall     -> OnAttacked1               (slot 22)
 *   func_ov002_020ef070  ray      -> OnAttacked2               (slot 23)
 *   func_ov002_020eeeb8  ray      -> OnKicked                  (slot 24)
 *   func_ov002_020eee3c  wall     -> OnPushed                  (slot 25)
 *   func_ov002_020eedc0  wall     -> OnHitByCannonBlastedChar  (slot 26)
 *   func_ov002_020eed24  any      -> OnHitByMegaChar           (slot 27)
 *   func_ov002_020eeca8  ceiling  -> OnHitFromUnderneath       (slot 28)
 *   func_ov002_020eea84  ceiling/wall hazards that hurt the player
 *
 * Callers are Player, daBmb_c and daObjBlockS_c. They take the mover as the
 * event's `other'. The two ray functions cast a line from 60 units above the
 * mover along its facing -- 100 units for OnAttacked2, 150 for OnKicked, 130
 * for either when the mover's param1 is 2 -- and copy the hit record out of
 * the line checker before asking it for the collider ID.
 *
 * Leftovers: C linkage and each caller's own scalar spelling of these names
 * are kept (decl_common.h declares three of them and daObjBlockS_c a fourth;
 * their parameter types stay agreeable with those). The ceiling and any-hit
 * queries on dBgCh_Actr have no member declaration yet and are called by ROM
 * label. The hit record copy is the ROM's field-by-field copy into a local
 * dBgPi-shaped struct; the real dBgPi copy constructor is not what the bytes
 * show. */

extern "C" {
/* dBgCh_Actr queries (src/engine/collision/d_bg_ch_actr.cpp), not yet
   declared in dBgCh_Actr.h. */
int func_02035638(void *clsn); /* mSphereClsn.flags & 0x10: the ceiling bit */
int func_0203564c(int clsn);   /* mSphereClsn.GetUnderResult(): the ceiling hit */
/* Player.cpp: sets 0x20 in Player::mStateFlags. */
void func_ov002_020d8838(void *player);
extern int data_02099368[];   /* dBgPi's vtable */
extern short data_02082214[]; /* sine/cosine pairs, indexed by angle >> 4 */
}

/* Is this actor of the given kind? An int-returning inline: the ROM
   materialises the 0/1 result before it branches on it. */
static inline int IsActorID(const dActor_c *actor, int id) {
    return actor->actorID == id;
}

/* A dBgPi, field for field, as the ray and hazard functions copy one onto
   the stack: the vtable word, dBgPc's SurfaceInfo (two CLPS words, then the
   face normal), and the collider identity. */
struct ClsnHitCopy {
    void *vtable;
    int surface[5];
    u16 triangleID;
    u16 colliderIdx;
    s32 clsnID;
    dActor_c *owner;
    dBgW *collider;
};

// @symbol func_ov002_020ef2a4
extern "C" int func_ov002_020ef2a4(dBgCh_Actr *clsn, dActor_c *other) {
    if (clsn->IsOnGround()) {
        dBgPi *hit = clsn->GetFloorResult();
        if (hit->GetClsnID() != -1) {
            dActor_c *owner = dActor_c::FindWithID(hit->GetClsnID());
            if (owner != 0) {
                owner->OnGroundPounded(*other);
                return 1;
            }
        }
    }
    return 0;
}

// @symbol func_ov002_020ef228
extern "C" int func_ov002_020ef228(dBgCh_Actr *clsn, int otherArg) {
    dActor_c *other = (dActor_c *)otherArg;
    if (clsn->IsOnWall()) {
        dBgPi *hit = clsn->GetWallResult();
        if (hit->GetClsnID() != -1) {
            dActor_c *owner = dActor_c::FindWithID(hit->GetClsnID());
            if (owner != 0) {
                owner->OnAttacked1(*other);
                return 1;
            }
        }
    }
    return 0;
}

// @symbol func_ov002_020ef070
extern "C" int func_ov002_020ef070(void *unused, char *moverArg) {
    Vector3 start, end;
    ClsnHitCopy hit;

    dBgCh_Lin line;

    dActor_c *mover = (dActor_c *)moverArg;
    Vector3 *pos = &mover->mPos;
    int x = pos->x;
    start.x = x;
    int y = pos->y;
    start.y = y;
    int z = pos->z;
    end.x = x;
    end.z = z;
    start.z = z;
    start.y = y + 0x3c000;
    end.y = y + 0x3c000;

    int reach = 0x64;
    if (mover->param1 == 2)
        reach = 0x82;
    end.x += reach * data_02082214[((u16)mover->mAngleY >> 4) << 1];
    end.z += reach * data_02082214[(((u16)mover->mAngleY >> 4) << 1) + 1];

    line.SetObjAndLine(start, end, mover);
    if (line.DetectClsn()) {
        if (IsActorID(mover, 0xbf))
            func_ov002_020d8838(mover);
        {
            int *dst = hit.surface;
            int w0 = line.surface.clps.w0;
            int w1 = line.surface.clps.w1;
            dst[0] = w1 ? w0 : w0;
            dst[1] = w1;
            dst[2] = line.surface.normal.x;
            dst[3] = line.surface.normal.y;
            dst[4] = line.surface.normal.z;
            hit.vtable = data_02099368;
            hit.triangleID = line.triangleID;
            hit.colliderIdx = line.colliderIdx;
            hit.clsnID = line.clsnID;
            hit.owner = line.owner;
            hit.collider = line.collider;
            if (((dBgPi *)&hit)->GetClsnID() != -1) {
                dActor_c *owner = dActor_c::FindWithID(((dBgPi *)&hit)->GetClsnID());
                if (owner) {
                    owner->OnAttacked2(*mover);
                    ((dBgPi *)&hit)->dBgPi::~dBgPi();
                    return 1;
                }
            }
            ((dBgPi *)&hit)->dBgPi::~dBgPi();
        }
    }
    return 0;
}

// @symbol func_ov002_020eeeb8
extern "C" int func_ov002_020eeeb8(void *unused, char *moverArg) {
    Vector3 start, end;
    ClsnHitCopy hit;

    dBgCh_Lin line;

    dActor_c *mover = (dActor_c *)moverArg;
    Vector3 *pos = &mover->mPos;
    int x = pos->x;
    start.x = x;
    int y = pos->y;
    start.y = y;
    int z = pos->z;
    end.x = x;
    end.z = z;
    start.z = z;
    start.y = y + 0x3c000;
    end.y = y + 0x3c000;

    int reach = 0x96;
    if (mover->param1 == 2)
        reach = 0x82;

    end.x += reach * data_02082214[((u16)mover->mAngleY >> 4) << 1];
    end.z += reach * data_02082214[(((u16)mover->mAngleY >> 4) << 1) + 1];

    line.SetObjAndLine(start, end, mover);

    if (line.DetectClsn()) {
        if (IsActorID(mover, 0xbf))
            func_ov002_020d8838(mover);

        {
            int *dst = hit.surface;
            int w0 = line.surface.clps.w0;
            int w1 = line.surface.clps.w1;

            dst[0] = w1 ? w0 : w0;
            dst[1] = w1;
            dst[2] = line.surface.normal.x;
            dst[3] = line.surface.normal.y;
            dst[4] = line.surface.normal.z;

            hit.vtable = data_02099368;
            hit.triangleID = line.triangleID;
            hit.colliderIdx = line.colliderIdx;
            hit.clsnID = line.clsnID;
            hit.owner = line.owner;
            hit.collider = line.collider;

            if (((dBgPi *)&hit)->GetClsnID() != -1) {
                dActor_c *owner = dActor_c::FindWithID(((dBgPi *)&hit)->GetClsnID());
                if (owner) {
                    owner->OnKicked(*mover);
                    ((dBgPi *)&hit)->dBgPi::~dBgPi();
                    return 1;
                }
            }
        }
        ((dBgPi *)&hit)->dBgPi::~dBgPi();
    }

    return 0;
}

// @symbol func_ov002_020eee3c
extern "C" int func_ov002_020eee3c(char *clsnArg, char *otherArg) {
    dBgCh_Actr *clsn = (dBgCh_Actr *)clsnArg;
    dActor_c *other = (dActor_c *)otherArg;
    if (clsn->IsOnWall()) {
        dBgPi *hit = clsn->GetWallResult();
        if (hit->GetClsnID() != -1) {
            dActor_c *owner = dActor_c::FindWithID(hit->GetClsnID());
            if (owner != 0) {
                owner->OnPushed(*other);
                return 1;
            }
        }
    }
    return 0;
}

// @symbol func_ov002_020eedc0
extern "C" int func_ov002_020eedc0(dBgCh_Actr *clsn, dActor_c *other) {
    if (clsn->IsOnWall()) {
        dBgPi *hit = clsn->GetWallResult();
        if (hit->GetClsnID() != -1) {
            dActor_c *owner = dActor_c::FindWithID(hit->GetClsnID());
            if (owner != 0) {
                owner->OnHitByCannonBlastedChar(*other);
                return 1;
            }
        }
    }
    return 0;
}

// @symbol func_ov002_020eed24
extern "C" int func_ov002_020eed24(dBgCh_Actr *clsn, Player *player) {
    dBgPi *hit;
    dActor_c *owner;
    if (clsn->IsOnWall() || clsn->IsOnGround() || func_02035638(clsn)) {
        hit = func_0203567c(clsn);
        if (hit->GetClsnID() != -1) {
            owner = dActor_c::FindWithID(hit->GetClsnID());
            if (owner) {
                owner->OnHitByMegaChar(*player);
                return 1;
            }
        }
    }
    return 0;
}

// @symbol func_ov002_020eeca8
extern "C" int func_ov002_020eeca8(dBgCh_Actr *clsn, dActor_c *other) {
    if (func_02035638(clsn)) {
        dBgPi *hit = func_0203567c(clsn);
        if (hit->GetClsnID() != -1) {
            dActor_c *owner = dActor_c::FindWithID(hit->GetClsnID());
            if (owner != 0) {
                owner->OnHitFromUnderneath(*other);
                return 1;
            }
        }
    }
    return 0;
}

/* Only for the player (actor ID 0xbf): a ceiling owned by actor ID 0x3a hurts
 * for 3, a wall owned by actor ID 0x139 hurts for 1 and plays sound 0xb5 at
 * that actor; both with knockback 0xc000. clsn is the mover's dBgCh_Actr
 * (Player::mMeshClsn at +0x380). The two-word struct copy of the CLPS keeps
 * the ceiling-hit snapshot on the stack under 2004/b56 C++; the explicit
 * snapshot and the scalar Hurt ABI remain measured reconstruction
 * boundaries. */
typedef struct { int w[2]; } ClsnHitHead;

// @symbol func_ov002_020eea84
extern "C" int func_ov002_020eea84(dBgCh_Actr *clsn, dActor_c *player) {
    struct ClsnHitCopy hit;
    Vector3 hurtFrom;
    Vector3 hurtFrom2;
    dActor_c *owner;

    if (!IsActorID(player, 0xbf))
        return 0;

    if (func_02035638(clsn)) {
        const dBgPi *src = (const dBgPi *)func_0203564c((int)clsn);
        int *d = hit.surface;
        *(ClsnHitHead *)d = *(const ClsnHitHead *)&src->surface.clps;
        d[2] = src->surface.normal.x;
        d[3] = src->surface.normal.y;
        d[4] = src->surface.normal.z;
        hit.vtable = data_02099368;
        hit.triangleID = src->triangleID;
        hit.colliderIdx = src->colliderIdx;
        hit.clsnID = src->clsnID;
        hit.owner = src->owner;
        hit.collider = src->collider;
        if (((dBgPi *)&hit)->GetClsnID() != -1) {
            owner = dActor_c::FindWithID(((dBgPi *)&hit)->GetClsnID());
            if (owner != 0 && IsActorID(owner, 0x3a)) {
                {
                    Vector3 *from = &player->mPos;
                    hurtFrom.x = from->x;
                    hurtFrom.y = from->y;
                    hurtFrom.z = from->z;
                }
                ((Player *)player)->Hurt(hurtFrom, 3, 0xc000, 1, 0, 1);
                ((dBgPi *)&hit)->dBgPi::~dBgPi();
                return 1;
            }
        }
        ((dBgPi *)&hit)->dBgPi::~dBgPi();
    }

    if (clsn->IsOnWall()) {
        dBgPi *wallHit = clsn->GetWallResult();
        if (wallHit->GetClsnID() != -1) {
            owner = dActor_c::FindWithID(wallHit->GetClsnID());
            if (owner != 0 && IsActorID(owner, 0x139)) {
                {
                    Vector3 *from = &owner->mPos;
                    hurtFrom2.x = from->x;
                    hurtFrom2.y = from->y;
                    hurtFrom2.z = from->z;
                }
                if (((Player *)player)->Hurt(hurtFrom2, 1, 0xc000, 1, 0, 1) != 0)
                    Sound::PlayBank0(0xb5, owner->mCamSpacePos);
                return 1;
            }
        }
    }
    return 0;
}

/* Base step, vptr store, then mModel and mMeshCollider in declaration order:
 * the compiler's own sequence, so the body is empty. */
// @symbol _ZN10dBgActor_cC2Ev
dBgActor_c::dBgActor_c()
{
}

/* Same enable/disable-by-distance as IsClsnInRange, except an off-screen actor
 * (mFlags & 8) always drops its collider, and radius 0 unconditionally enables
 * instead of falling back to the clip volume. */
// @symbol _ZN10dBgActor_c21IsClsnInRangeOnScreenEii
int dBgActor_c::IsClsnInRangeOnScreen(Fix12i radius, Fix12i yOffset) {
    int offScreen = (mFlags & 8) != 0;
    if (offScreen) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        return 0;
    }
    if (radius == 0) {
        if (!mMeshCollider.IsEnabled())
            mMeshCollider.Enable(this);
    } else {
        Vector3 center;
        center.x = mPosX;
        center.y = mPosY;
        center.z = mPosZ;
        if (yOffset == 0)
            center.y += mClipOffsetY;
        else
            center.y += yOffset;
        Player *player = ClosestPlayer();
        int dist = Vec3_Dist(&center, &((dActor_c *)player)->mPos);
        if (dist > radius) {
            if (mMeshCollider.IsEnabled())
                mMeshCollider.Disable();
            return 0;
        }
        if (!mMeshCollider.IsEnabled())
            mMeshCollider.Enable(this);
    }
    return 1;
}

/* Keeps the mesh collider in the world only while the closest player is within
 * radius of (pos + yOffset). Zero radius falls back to the clip volume
 * (mClipRadius, shifted into fix12); zero yOffset to mClipOffsetY. */
// @symbol _ZN10dBgActor_c13IsClsnInRangeEii
int dBgActor_c::IsClsnInRange(Fix12i radius, Fix12i yOffset) {
    Vector3 center;
    center.x = mPosX;
    center.y = mPosY;
    center.z = mPosZ;
    if (radius == 0)
        radius = mClipRadius << 3;
    if (yOffset == 0)
        center.y += mClipOffsetY;
    else
        center.y += yOffset;
    Player *player = ClosestPlayer();
    int dist = Vec3_Dist(&center, &((dActor_c *)player)->mPos);
    if (dist > radius) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        return 0;
    }
    if (!mMeshCollider.IsEnabled())
        mMeshCollider.Enable(this);
    return 1;
}

/* Rebuilds the model matrix from the yaw and drops the position into its
 * translation row at 1/8 scale, the model-space unit. */
// @symbol _ZN10dBgActor_c21UpdateModelPosAndRotYEv
void dBgActor_c::UpdateModelPosAndRotY()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9]  = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* The collider follows the model: copy its matrix, overwrite the translation
 * row (m[9..11]) with the actor position, and hand it to the collider. */
// @symbol _ZN10dBgActor_c19UpdateClsnPosAndRotEv
void dBgActor_c::UpdateClsnPosAndRot()
{
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9]  = mPosX;
    mClsnMat.m[10] = mPosY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}

/* One frame of the mega-knockback flight KillByMegaChar starts: probe 200
 * units ahead along the launch heading and reverse it into a wall, tumble by
 * the per-frame angle steps, integrate, and Kill() when the fuse runs out.
 * Callers (pushblock, signpost, ski lift, Bill Blaster) pass the X step and a
 * model-pivot height; nonzero return means "still flying". */
// @symbol _ZN10dBgActor_c20UpdateKillByMegaCharEsssi
int dBgActor_c::UpdateKillByMegaChar(s16 angStepX, s16 angStepY, s16 angStepZ, Fix12i pivotY) {
    Vector3 probe;
    Vector3 rayEnd;
    Vector3 sum;

    if (mMegaKnockback == 0)
        return 0;

    probe.x = 0;
    probe.y = 0;
    probe.z = 0xc8000;
    rayEnd.x = 0;
    rayEnd.y = 0;
    rayEnd.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
    MulVec3Mat4x3(&probe, &data_020a0e68, &rayEnd);
    Vec3_Add(&sum, &mPos, &rayEnd);
    rayEnd = sum;

    dBgCh_Lin ray;
    ray.SetObjAndLine(mPos, rayEnd, this);
    if (ray.DetectClsn())
        mPrevAngleY += 0x8000;
    mAngleX += angStepX;
    mAngleY += angStepY;
    mAngleZ += angStepZ;
    UpdatePos(0);
    if (DecIfAbove0_Byte(&mMegaKnockbackTimer) == 0)
        Kill();
    func_ov002_020ee5d0(this, pivotY);
    return 1;
}

/* The mega-knockback model matrix: actor position at 1/8 scale, rotated about
 * a pivot pivotY above it (translate up, rotate, translate back). */
// @symbol func_ov002_020ee5d0
extern "C" {
void func_ov002_020ee5d0(dBgActor_c *self, int pivotY)
{
    Vector3 modelPos;
    Vec3_Asr(&modelPos, &self->mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, modelPos.x, modelPos.y, modelPos.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, pivotY >> 3, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -pivotY >> 3, 0);
    self->mModel.mat4x3 = data_020a0e68;
}
}

/* Slot 31, the one virtual this class adds (and the key function). Poof 100
 * units up, dust at the actor, sound, gone.
 *
 * MEMBERWISE, NOT `dustPos = pos`. Vector3 declares a destructor (see
 * types.h), so it is non-POD, and a whole-object assignment compiles to an
 * ldm/stm pair -- four instructions where the ROM has six. Three field
 * stores are what the cartridge does. */
// @symbol _ZN10dBgActor_c4KillEv
void dBgActor_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x64000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    Particle::System::NewSimple(0xa, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, mCamSpacePos);
    MarkForDestruction();
}

/* Mega Mario's shockwave hits: shake the ground, arm the knockback flight
 * (20 u/frame away from the player, 30 up, gravity 2, 30-frame fuse), and
 * take the mesh collider out of the world. */
// @symbol _ZN10dBgActor_c14KillByMegaCharER6Player
void dBgActor_c::KillByMegaChar(Player &player)
{
    Vector3 v;
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    Earthquake(v, 0x5dc000);
    mMegaKnockback = 1;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x14000;
    mVertSpeed = 0x1e000;
    mPrevAngleY = Vec3_HorzAngle(&((dActor_c *)&player)->mPos, &mPos);
    mMegaKnockbackTimer = 0x1e;
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
}

/* Nothing forces D0/D1 here: this TU defines Kill (slot 31), the class's key
 * function, so the vtable is emitted here and drags both destructor variants
 * with it. */
// @symbol _ZN10dBgActor_cD0Ev
// @symbol _ZN10dBgActor_cD1Ev
