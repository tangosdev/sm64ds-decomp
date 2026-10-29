//cpp
/* dBgCh_Actr -- an actor's mesh-collision query: the sphere-plus-raycast pair
 * every actor embeds (vtable address point 0x02099204; _ZTS10dBgCh_Actr at
 * 0x020991ec). Derives from dBgCh; nothing derives from it, so no D2/C2 ever
 * emitted.
 *
 * Owns the contiguous text run 0x0203553c..0x02037464, between dBgCh's own
 * functions below and dBgCh_Gnd's workers above. Deferred codegen emits the
 * lifecycle group at the tail -- D0 0x020373b8, D1 0x020373f8, C1 0x02037430 --
 * so the ctor and dtor are defined first and the plain functions land in
 * reverse definition order. The func_0203* workers inside the run are its
 * unnamed mFlags plumbing, sub-object accessors, and step helpers; the ROM
 * gives them no member name, so they stay extern "C" free functions taking
 * the object by raw pointer. */
// leftovers: the checker-family helpers stay local
// externs (no non-generated header declares them). The func_0203* workers keep
// address names (no member evidence names them); five of them keep raw-pointer
// params because other TUs declare them with int/char pointer types.

#include "types.h"
#include "common.h"
#include "dBgCh_Actr.h"
#include "dBgCh_Gnd.h"
#include "dActor_c.h"
#include "Player.h"

extern "C" {

/* Checker-family helpers shared across the collision TUs, spelled as
   defined. */
int  func_02037dc4(int p);   /* SurfaceInfo * -> Vector3 * (the hit normal) */
void func_02038234(int a, int b);
void func_02038324(void *arg, int b, int c, int d);
int  func_0203842c(char *self);
int  func_0203859c(void *obj);
int  func_02039794(int x);
int  SurfaceInfo_TestFlag0x20(int *p);

/* This TU's own workers, defined below. */
int func_0203553c(dBgCh_Actr *self);  /* (mFlags & 0x4000) == 0 -- water-tracking bit */
void func_02035550(dBgCh_Actr *self); /* mFlags |= 0x4000 */
int func_020355a0(dBgCh_Actr *self);  /* (mFlags & 0x800) == 0 */
int func_020355dc(dBgCh_Actr *self);  /* mFlags & 0x400 */
void func_020356d4(dBgCh_Actr *self); /* mFlags |= 0x40 -- left ground */
int func_02035764(dBgCh_Actr *self);  /* mFlags & 0x200 */
void func_0203573c(dBgCh_Actr *self); /* mFlags |= 0x20 -- just hit ground */
void func_020371b0(dBgCh_Actr *clsn, s32 wasOnGround); /* floor-result commit */
}

/* Lifecycle first; the group emits at the run's tail. Both bodies are empty:
   the class header supplies the base step, the vtable store, the two typed
   member constructions, and the collision-heap delete. */

// @symbol _ZN10dBgCh_ActrC1Ev
dBgCh_Actr::dBgCh_Actr() {}

// @symbol _ZN10dBgCh_ActrD0Ev
// @symbol _ZN10dBgCh_ActrD1Ev
dBgCh_Actr::~dBgCh_Actr()
{
}

// @symbol _ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_
void dBgCh_Actr::Init(dActor_c *actor, Fix12i radius, Fix12i height, Vector3_16 *a, Vector3_16 *b) {
    mActor = actor;
    mRadius = radius;
    mHeight = height;
    mFlags = 0;
    mSphereClsn.mMotionAngPtr = (s32)a;
    mAngPtr = (s32)b;
    mScale = 0x1000;
}

// @symbol func_02037318
/* Latch the sphere query's floor normal (the floor contact with the greatest
   Y, see dBgCh_SphCrr.h) into mGroundNormal on a grounded frame. Flag 0x100
   marks the latched normal valid; func_020371fc reads it. */
extern "C" void func_02037318(dBgCh_Actr *clsn) {
    const int *src;

    clsn->mFlags &= ~0x100;
    if (clsn->IsOnGround() == 0)
        return;
    if (((Player *)clsn->mActor)->IsInAir())
        return;
    clsn->mFlags |= 0x100;
    src = &clsn->mSphereClsn.mFloorNormal.x;
    clsn->mGroundNormal.x = src[0];
    clsn->mGroundNormal.y = src[1];
    clsn->mGroundNormal.z = src[2];
}

// @symbol func_020371fc
/* The ground-snap step run once per landing: when the sphere query flagged a
   floor hit and the actor is close to a dBgCh_Gnd probed surface, snap the
   actor's position onto it and raise the on-ground state. */
extern "C" void func_020371fc(char *self)
{
    dBgCh_Actr *clsn = (dBgCh_Actr *)self;
    if ((clsn->mSphereClsn.flags & 1) == 0) return;
    if ((clsn->mSphereClsn.flags & 8) == 0) return;
    if ((clsn->mFlags & 0x100) == 0) return;
    if (clsn->mGroundNormal.y >= 0xf20)
        return;
    if (((Player *)clsn->mActor)->IsInAir()) return;
    {
        Vector3 probe;
        Vector3 *actorPos = &clsn->mActor->mPos;
        dBgCh_Gnd ground;
        probe.x = actorPos->x;
        probe.y = actorPos->y + clsn->mRadius;
        probe.z = actorPos->z;
        ground.SetObjAndPos(probe, clsn->mActor);
        if (ground.DetectClsn() != 0) {
            int groundY = ground.clsnY;
            int diff = actorPos->y - groundY;
            if (diff > 0 && diff < (clsn->mRadius << 1)) {
                actorPos->y = groundY;
                clsn->mSphereClsn.flags |= 4;
                clsn->mSphereClsn.mClsnResult1 = ground;
                clsn->SetGroundFlag();
            }
        }
    }
}

// @symbol func_020371b0
/* Floor-result commit: hand the hit record to the actor, raise on-ground,
   and kill vertical speed unless limited movement is set. `wasOnGround` is
   the frame's ground state on entry, so the just-hit flag only fires on the
   transition. */
extern "C" void func_020371b0(dBgCh_Actr *clsn, s32 wasOnGround) {
    if (wasOnGround == 0)
        func_0203573c(clsn);
    func_02038234((int)clsn->GetFloorResult(), (int)clsn->mActor);
    clsn->SetGroundFlag();
    if (clsn->GetLimMovFlag() == 0)
        clsn->mActor->mVertSpeed = 0;
}

// @symbol _ZN10dBgCh_Actr20UpdateDiscreteNoLavaEv
void dBgCh_Actr::UpdateDiscreteNoLava()
{
    int onGround;
    int y;
    struct Vector3 sphere;
    struct Vector3 *disp;
    struct Vector3 *pos = &mActor->mPos;
    struct Vector3 *prev = &mActor->mPrevPos;

    if (IsOnGround() && func_020355a0(this) && ShouldUpdatePos()) {
        func_02038324((void *)mSphereClsn.GetFloorResult(), (int)pos, mSphereClsn.mMotionAngPtr,
                      mAngPtr);
    }
    onGround = IsOnGround();
    ClearAllGroundFlags();
    sphere.x = pos->x;
    y = pos->y;
    sphere.y = y;
    sphere.z = pos->z;
    sphere.y = y + mHeight;
    mSphereClsn.SetObjAndSphere(sphere, mRadius, mActor);
    mSphereClsn.mMaxNormalY = mScale;
    if (pos->y - prev->y > 0) {
        mSphereClsn.flags |= 0x20;
    }
    if (mSphereClsn.DetectClsn()) {
        disp = &mSphereClsn.disp;
        if (mSphereClsn.flags & 4) {
            func_020371b0(this, onGround);
        }
        if (ShouldUpdatePos()) {
            pos->x += disp->x;
            if (ShouldUpdatePosY()) {
                pos->y += disp->y;
            }
            pos->z += disp->z;
        }
    }
    if (onGround == 0)
        return;
    if (IsOnGround())
        return;
    func_020356d4(this);
}

// @symbol _ZN10dBgCh_Actr22UpdateDiscreteNoLava_2Ev
void dBgCh_Actr::UpdateDiscreteNoLava_2()
{
    int onGround;
    int y;
    struct Vector3 sphere;
    struct Vector3 *disp;
    struct Vector3 *pos = &mActor->mPos;
    dActor_c *actor = mActor;

    onGround = IsOnGround();
    ClearAllGroundFlags();
    sphere.x = pos->x;
    y = pos->y;
    sphere.y = y;
    sphere.z = pos->z;
    sphere.y = y + mHeight;
    mSphereClsn.SetObjAndSphere(sphere, mRadius, mActor);
    mSphereClsn.mMaxNormalY = mScale;
    if (pos->y - actor->mPrevPosY > 0) {
        mSphereClsn.flags |= 0x20;
    }
    if (mSphereClsn.func_02038a38()) {
        disp = &mSphereClsn.disp;
        if (mSphereClsn.flags & 4) {
            func_020371b0(this, onGround);
        }
        if (ShouldUpdatePos()) {
            pos->x += disp->x;
            if (ShouldUpdatePosY()) {
                pos->y += disp->y;
            }
            pos->z += disp->z;
        }
    }
    if (onGround == 0)
        return;
    if (IsOnGround())
        return;
    func_020356d4(this);
}

/* func_02036acc is UpdateContinuous's free twin: the same wall/floor probing
   sequence but taking the query by raw pointer and running the func_0203842c/
   func_02038824 detection variants. The ROM keeps its unmangled name, so it
   stays extern "C". */

#pragma opt_common_subs off

// @symbol func_02036acc
extern "C" void func_02036acc(dBgCh_Actr *self) {
    bool hitFloor;
    bool hitWall;
    int height;
    int onGround;
    Vector3 *pos;
    Vector3 *prev;
    bool floorCommitted;
    dActor_c *a;

    a = self->mActor;
    pos = &a->mPos;
    prev = &a->mPrevPos;

    if (self->IsOnGround() && func_020355a0(self) && self->ShouldUpdatePos())
        func_02038324((void *)self->mSphereClsn.GetFloorResult(), (int)pos,
                      self->mSphereClsn.mMotionAngPtr, self->mAngPtr);

    hitFloor = false;
    dBgPi floorHit;
    hitWall = false;
    dBgPi wallHit;
    Vector3 lineStart, lineEnd;

    height = self->mHeight;
    {
        int tx = prev->x;
        int tz = prev->z;
        int ty = prev->y + height;
        lineStart.x = tx;
        lineStart.y = ty;
        lineStart.z = tz;
    }
    {
        int tx = pos->x;
        int tz = pos->z;
        int ty = pos->y + height;
        lineEnd.x = tx;
        lineEnd.y = ty;
        lineEnd.z = tz;
    }
    self->mRaycastLine.SetObjAndLine(lineStart, lineEnd, self->mActor);
    if (func_0203842c((char *)&self->mRaycastLine)) {
        int band;
        Vector3 clsnPos = self->mRaycastLine.GetClsnPos();
        Vector3 normal, newStart, newEnd;
        self->mRaycastLine.surface.CopyNormalTo(normal);
        newStart.x = clsnPos.x + (normal.x >> 2);
        newStart.y = (normal.y >> 2) + clsnPos.y;
        newStart.z = clsnPos.z + (normal.z >> 2);
        newEnd.x = newStart.x;
        newEnd.y = newStart.y - height;
        newEnd.z = newStart.z;
        band = func_02039794(normal.y);
        if (band == 1) {
            hitWall = true;
            self->mRaycastLine.CopyTo(wallHit);
        } else if (band == 0) {
            hitFloor = true;
            self->mRaycastLine.CopyTo(floorHit);
        }
        self->mRaycastLine.SetObjAndLine(newStart, newEnd, self->mActor);
        if (func_0203842c((char *)&self->mRaycastLine)) {
            Vector3 clsnPos2 = self->mRaycastLine.GetClsnPos();
            Vector3 normal2;
            self->mRaycastLine.surface.CopyNormalTo(normal2);
            if (func_02039794(normal2.y) == 0) {
                hitFloor = true;
                self->mRaycastLine.CopyTo(floorHit);
            }
            if (self->ShouldUpdatePos()) {
                pos->x = clsnPos2.x - (normal2.x >> 2);
                pos->y = clsnPos2.y - (normal2.y >> 2) - (height >> 1);
                pos->z = clsnPos2.z - (normal2.z >> 2);
            }
        } else if (self->ShouldUpdatePos()) {
            pos->x = newStart.x;
            pos->y = newStart.y - height;
            pos->z = newStart.z;
        }
    }

    onGround = self->IsOnGround();
    floorCommitted = false;
    self->ClearAllGroundFlags();
    Vector3 sphere;
    sphere.x = pos->x;
    sphere.y = pos->y;
    sphere.z = pos->z;
    sphere.y += height;
    self->mSphereClsn.SetObjAndSphere(sphere, self->mRadius, self->mActor);
    if (func_0203553c(self) == 0)
        self->mSphereClsn.flags |= 0x40;
    self->mSphereClsn.mMaxNormalY = self->mScale;
    if (pos->y - prev->y > 0)
        self->mSphereClsn.flags |= 0x20;
    if (hitFloor) {
        self->mSphereClsn.flags |= 4;
        self->mSphereClsn.SetFloorResult(floorHit);
        self->mSphereClsn.flags |= 1;
        (dBgPi &)self->mSphereClsn = floorHit;
        func_020371b0(self, onGround);
        floorCommitted = true;
    }
    if (hitWall) {
        self->mSphereClsn.flags |= 8;
        self->mSphereClsn.SetWallResult(wallHit);
        self->mSphereClsn.flags |= 1;
        (dBgPi &)self->mSphereClsn = wallHit;
    }
    if (self->mSphereClsn.func_02038824()) {
        Vector3 *disp = &self->mSphereClsn.disp;
        if ((self->mSphereClsn.flags & 4) && !floorCommitted)
            func_020371b0(self, onGround);
        if (self->ShouldUpdatePos()) {
            pos->x += disp->x;
            if (self->ShouldUpdatePosY())
                pos->y += disp->y;
            pos->z += disp->z;
        }
    }
    if (onGround && self->IsOnGround() == 0)
        func_020356d4(self);
}

#pragma opt_common_subs on

#pragma opt_common_subs off

// @symbol _ZN10dBgCh_Actr16UpdateContinuousEv
void dBgCh_Actr::UpdateContinuous()
{
    bool hitFloor;
    bool hitWall;
    int height;
    int onGround;
    Vector3 *pos;
    Vector3 *prev;
    bool floorCommitted;
    dActor_c *a;

    a = mActor;
    pos = &a->mPos;
    prev = &a->mPrevPos;

    if (IsOnGround() && func_020355a0(this) && ShouldUpdatePos())
        func_02038324((void *)mSphereClsn.GetFloorResult(), (int)pos, mSphereClsn.mMotionAngPtr,
                      mAngPtr);

    hitFloor = false;
    dBgPi floorHit;
    hitWall = false;
    dBgPi wallHit;
    Vector3 lineStart, lineEnd;

    height = mHeight;
    {
        int tx = prev->x;
        int tz = prev->z;
        int ty = prev->y + height;
        lineStart.x = tx;
        lineStart.y = ty;
        lineStart.z = tz;
    }
    {
        int tx = pos->x;
        int tz = pos->z;
        int ty = pos->y + height;
        lineEnd.x = tx;
        lineEnd.y = ty;
        lineEnd.z = tz;
    }
    mRaycastLine.SetObjAndLine(lineStart, lineEnd, mActor);
    if (mRaycastLine.DetectClsn()) {
        int band;
        Vector3 clsnPos = mRaycastLine.GetClsnPos();
        Vector3 normal, newStart, newEnd;
        mRaycastLine.surface.CopyNormalTo(normal);
        newStart.x = clsnPos.x + (normal.x >> 2);
        newStart.y = (normal.y >> 2) + clsnPos.y;
        newStart.z = clsnPos.z + (normal.z >> 2);
        newEnd.x = newStart.x;
        newEnd.y = newStart.y - height;
        newEnd.z = newStart.z;
        band = func_02039794(normal.y);
        if (band == 1) {
            hitWall = true;
            mRaycastLine.CopyTo(wallHit);
        } else if (band == 0) {
            hitFloor = true;
            mRaycastLine.CopyTo(floorHit);
        }
        mRaycastLine.SetObjAndLine(newStart, newEnd, mActor);
        if (mRaycastLine.DetectClsn()) {
            Vector3 clsnPos2 = mRaycastLine.GetClsnPos();
            Vector3 normal2;
            mRaycastLine.surface.CopyNormalTo(normal2);
            if (func_02039794(normal2.y) == 0) {
                hitFloor = true;
                mRaycastLine.CopyTo(floorHit);
            }
            if (ShouldUpdatePos()) {
                pos->x = clsnPos2.x - (normal2.x >> 2);
                pos->y = clsnPos2.y - (normal2.y >> 2) - (height >> 1);
                pos->z = clsnPos2.z - (normal2.z >> 2);
            }
        } else if (ShouldUpdatePos()) {
            pos->x = newStart.x;
            pos->y = newStart.y - height;
            pos->z = newStart.z;
        }
    }

    onGround = IsOnGround();
    floorCommitted = false;
    ClearAllGroundFlags();
    Vector3 sphere;
    sphere.x = pos->x;
    sphere.y = pos->y;
    sphere.z = pos->z;
    sphere.y += height;
    mSphereClsn.SetObjAndSphere(sphere, mRadius, mActor);
    if (func_0203553c(this) == 0)
        mSphereClsn.flags |= 0x40;
    mSphereClsn.mMaxNormalY = mScale;
    if (pos->y - prev->y > 0)
        mSphereClsn.flags |= 0x20;
    if (hitFloor) {
        mSphereClsn.flags |= 4;
        mSphereClsn.SetFloorResult(floorHit);
        mSphereClsn.flags |= 1;
        (dBgPi &)mSphereClsn = floorHit;
        func_020371b0(this, onGround);
        floorCommitted = true;
    }
    if (hitWall) {
        mSphereClsn.flags |= 8;
        mSphereClsn.SetWallResult(wallHit);
        mSphereClsn.flags |= 1;
        (dBgPi &)mSphereClsn = wallHit;
    }
    if (mSphereClsn.DetectClsn()) {
        Vector3 *disp = &mSphereClsn.disp;
        if ((mSphereClsn.flags & 4) && !floorCommitted)
            func_020371b0(this, onGround);
        if (ShouldUpdatePos()) {
            pos->x += disp->x;
            if (ShouldUpdatePosY())
                pos->y += disp->y;
            pos->z += disp->z;
        }
    }
    if (onGround && IsOnGround() == 0)
        func_020356d4(this);
}

#pragma opt_common_subs on

#pragma opt_common_subs off

// @symbol _ZN10dBgCh_Actr22UpdateContinuousNoLavaEv
void dBgCh_Actr::UpdateContinuousNoLava()
{
    bool hitFloor;
    bool hitWall;
    int height;
    int onGround;
    Vector3 *pos;
    Vector3 *prev;
    bool floorCommitted;

    {
        dActor_c *a = mActor;
        pos = &a->mPos;
        prev = &a->mPrevPos;
    }

    hitFloor = false;
    dBgPi floorHit;
    hitWall = false;
    dBgPi wallHit;
    Vector3 lineStart, lineEnd;

    height = mHeight;
    {
        int tx = prev->x;
        int tz = prev->z;
        int ty = prev->y + height;
        lineStart.x = tx;
        lineStart.y = ty;
        lineStart.z = tz;
    }
    {
        int tx = pos->x;
        int tz = pos->z;
        int ty = pos->y + height;
        lineEnd.x = tx;
        lineEnd.y = ty;
        lineEnd.z = tz;
    }
    mRaycastLine.SetObjAndLine(lineStart, lineEnd, mActor);
    if (func_0203859c(&mRaycastLine)) {
        int band;
        Vector3 clsnPos = mRaycastLine.GetClsnPos();
        Vector3 normal, newStart, newEnd;
        mRaycastLine.surface.CopyNormalTo(normal);
        newStart.x = clsnPos.x + (normal.x >> 2);
        newStart.y = (normal.y >> 2) + clsnPos.y;
        newStart.z = clsnPos.z + (normal.z >> 2);
        newEnd.x = newStart.x;
        newEnd.y = newStart.y - height;
        newEnd.z = newStart.z;
        band = func_02039794(normal.y);
        if (band == 1) {
            hitWall = true;
            mRaycastLine.CopyTo(wallHit);
        }
        mRaycastLine.SetObjAndLine(newStart, newEnd, mActor);
        if (func_0203859c(&mRaycastLine)) {
            Vector3 clsnPos2 = mRaycastLine.GetClsnPos();
            Vector3 normal2;
            mRaycastLine.surface.CopyNormalTo(normal2);
            if (func_02039794(normal2.y) == 0) {
                hitFloor = true;
                mRaycastLine.CopyTo(floorHit);
            }
            if (ShouldUpdatePos()) {
                pos->x = clsnPos2.x - (normal2.x >> 2);
                pos->y = clsnPos2.y - (normal2.y >> 2) - (height >> 1);
                pos->z = clsnPos2.z - (normal2.z >> 2);
            }
        } else if (ShouldUpdatePos()) {
            pos->x = newStart.x;
            pos->y = newStart.y - height;
            pos->z = newStart.z;
        }
    }

    onGround = IsOnGround();
    ClearAllGroundFlags();
    floorCommitted = false;
    Vector3 sphere;
    sphere.x = pos->x;
    sphere.y = pos->y;
    sphere.z = pos->z;
    sphere.y += height;
    mSphereClsn.SetObjAndSphere(sphere, mRadius, mActor);
    mSphereClsn.mMaxNormalY = mScale;
    if (pos->y - prev->y > 0)
        mSphereClsn.flags |= 0x20;
    if (hitFloor) {
        mSphereClsn.flags |= 4;
        mSphereClsn.SetFloorResult(floorHit);
        mSphereClsn.flags |= 1;
        (dBgPi &)mSphereClsn = floorHit;
        func_020371b0(this, onGround);
        floorCommitted = true;
    }
    if (hitWall) {
        mSphereClsn.flags |= 8;
        mSphereClsn.SetWallResult(wallHit);
        mSphereClsn.flags |= 1;
        (dBgPi &)mSphereClsn = wallHit;
    }
    if (mSphereClsn.func_02038a38()) {
        Vector3 *disp = &mSphereClsn.disp;
        if ((mSphereClsn.flags & 4) && !floorCommitted)
            func_020371b0(this, onGround);
        if (ShouldUpdatePos()) {
            pos->x += disp->x;
            if (ShouldUpdatePosY())
                pos->y += disp->y;
            pos->z += disp->z;
        }
    }
    if (onGround && IsOnGround() == 0)
        func_020356d4(this);
}

#pragma opt_common_subs on

/* Vector copy (V3C) and copy-with-Y-offset (V3D) through scalar temporaries:
   the ROM loads z, y, x in that order before storing, which a plain struct
   copy or member-wise assignment does not reproduce. */
#define V3D(v, p, dy) { s32 _x, _y, _z; _z = (p)->z; _y = (p)->y; _x = (p)->x; _y = _y + (dy); (v).x = _x; (v).y = _y; (v).z = _z; }
#define V3C(v, p) { s32 _x, _y, _z; _z = (p)->z; _y = (p)->y; _x = (p)->x; (v).x = _x; (v).y = _y; (v).z = _z; }

#pragma opt_common_subs off

// @symbol _ZN10dBgCh_Actr20UpdateExtraContinousEv
/* The full three-result variant: raycast first for wall/floor/ceiling, then
   the sphere, then a follow-up raycast past the displacement. */
void dBgCh_Actr::UpdateExtraContinous()
{
    bool hitFloor, hitWall, hitUnderside;
    s32 height, wasOnGround;
    bool floorCommitted;
    Vector3 *pos, *prev;
    dActor_c *actor;

    actor = mActor;
    pos = &actor->mPos;
    prev = &actor->mPrevPos;

    if (IsOnGround() && func_020355a0(this))
        func_02038324((void *)mSphereClsn.GetFloorResult(), (int)pos, mSphereClsn.mMotionAngPtr,
                      mAngPtr);

    {
        hitFloor = false;
        dBgPi floorHit;
        hitWall = false;
        dBgPi wallHit;
        hitUnderside = false;
        dBgPi undersideHit;

        height = mHeight;

        Vector3 start;
        V3D(start, prev, height)
        Vector3 prevFoot;
        V3C(prevFoot, prev)
        mRaycastLine.SetObjAndLine(prevFoot, start, mActor);

        if (func_0203859c(&mRaycastLine)) {
            start = mRaycastLine.GetClsnPos();
            Vector3 normal;
            mRaycastLine.surface.CopyNormalTo(normal);
            start.x = start.x + (normal.x >> 2);
            start.y = start.y + (normal.y >> 2);
            start.z = start.z + (normal.z >> 2);
        }

        Vector3 centre;
        V3D(centre, pos, height)
        mRaycastLine.SetObjAndLine(start, centre, mActor);

        if (mRaycastLine.DetectClsn()) {
            Vector3 hitNormal;
            mRaycastLine.surface.CopyNormalTo(hitNormal);
            s32 band = func_02039794(hitNormal.y);
            if (band == 1) {
                hitWall = true;
                mRaycastLine.CopyTo(wallHit);
            } else if (band == 2) {
                hitUnderside = true;
                mRaycastLine.CopyTo(undersideHit);
            } else if (band == 0) {
                hitFloor = true;
                mRaycastLine.CopyTo(floorHit);
            }

            Vector3 hitPos = mRaycastLine.GetClsnPos();
            Vector3 backedOff;
            backedOff.x = hitPos.x + (hitNormal.x >> 2);
            backedOff.y = hitPos.y + (hitNormal.y >> 2);
            backedOff.z = hitPos.z + (hitNormal.z >> 2);
            Vector3 downEnd;
            downEnd.x = backedOff.x;
            downEnd.y = backedOff.y - height;
            downEnd.z = backedOff.z;
            mRaycastLine.SetObjAndLine(backedOff, downEnd, mActor);

            if (mRaycastLine.DetectClsn()) {
                Vector3 floorPos = mRaycastLine.GetClsnPos();
                Vector3 floorNormal;
                mRaycastLine.surface.CopyNormalTo(floorNormal);
                if (func_02039794(floorNormal.y) == 0) {
                    hitFloor = true;
                    mRaycastLine.CopyTo(floorHit);
                }
                floorPos.x = floorPos.x + (floorNormal.x << 2);
                floorPos.y = floorPos.y + (floorNormal.y << 2);
                floorPos.z = floorPos.z + (floorNormal.z << 2);
                Vector3 upEnd;
                upEnd.x = floorPos.x;
                upEnd.y = floorPos.y + (height << 1);
                upEnd.z = floorPos.z;
                mRaycastLine.SetObjAndLine(floorPos, upEnd, mActor);

                if (mRaycastLine.DetectClsn()) {
                    Vector3 ceilingPos = mRaycastLine.GetClsnPos();
                    pos->x = ceilingPos.x;
                    pos->y = ((ceilingPos.y + floorPos.y) >> 1) - height;
                    pos->z = ceilingPos.z;
                } else {
                    pos->x = floorPos.x - (floorNormal.x >> 2);
                    pos->y = floorPos.y - (floorNormal.y >> 2) - (height >> 1);
                    pos->z = floorPos.z - (floorNormal.z >> 2);
                }
            } else {
                pos->x = hitPos.x + (hitNormal.x >> 2);
                pos->y = hitPos.y + (hitNormal.y >> 2) - height;
                pos->z = hitPos.z + (hitNormal.z >> 2);
            }

            Vector3 prevCentre;
            V3D(prevCentre, prev, height)
            Vector3 newCentre;
            V3D(newCentre, pos, height)
            mRaycastLine.SetObjAndLine(prevCentre, newCentre, mActor);

            if (mRaycastLine.DetectClsn() &&
                ((Vector3 *)func_02037dc4((int)&mRaycastLine.surface))->y >= 0) {
                Vector3 *hit = &mRaycastLine.lineEnd;
                pos->x = hit->x;
                pos->y = hit->y;
                pos->z = hit->z;
            }
        }

        wasOnGround = IsOnGround();
        ClearAllGroundFlags();
        floorCommitted = false;

        Vector3 sphere;
        sphere.x = pos->x;
        sphere.y = pos->y;
        sphere.z = pos->z;
        sphere.y = sphere.y + height;
        mSphereClsn.SetObjAndSphere(sphere, mRadius, mActor);

        mSphereClsn.mMaxNormalY = mScale;
        if (func_02035764(this))
            mSphereClsn.flags |= 2;
        if (pos->y - prev->y > 0)
            mSphereClsn.flags |= 0x20;

        if (hitFloor) {
            mSphereClsn.flags |= 4;
            mSphereClsn.SetFloorResult(floorHit);
            mSphereClsn.flags |= 1;
            (dBgPi &)mSphereClsn = floorHit;
            func_020371b0(this, wasOnGround);
            floorCommitted = true;
        }
        if (hitWall) {
            mSphereClsn.flags |= 8;
            mSphereClsn.SetWallResult(wallHit);
            mSphereClsn.flags |= 1;
            (dBgPi &)mSphereClsn = wallHit;
        }
        if (hitUnderside) {
            mSphereClsn.flags |= 0x10;
            mSphereClsn.SetUnderResult(undersideHit);
            mSphereClsn.flags |= 1;
            (dBgPi &)mSphereClsn = undersideHit;
        }

        if (mSphereClsn.DetectClsn()) {
            Vector3 *disp = &mSphereClsn.disp;
            if ((mSphereClsn.flags & 4) && !floorCommitted) {
                func_020371b0(this, wasOnGround);
                floorCommitted = true;
            }
            pos->x = pos->x + disp->x;
            if (ShouldUpdatePosY())
                pos->y = pos->y + disp->y;
            pos->z = pos->z + disp->z;

            if (func_020355dc(this)) {
                Vector3 top;
                V3D(top, pos, (height << 1))
                Vector3 foot;
                V3C(foot, pos)
                mRaycastLine.SetObjAndLine(top, foot, mActor);
                if (mRaycastLine.DetectClsn()) {
                    Vector3 *hit = &mRaycastLine.lineEnd;
                    Vector3 *normal = (Vector3 *)func_02037dc4((int)&mRaycastLine.surface);
                    pos->x = hit->x + (normal->x >> 2);
                    pos->y = hit->y + (normal->y >> 2);
                    pos->z = hit->z + (normal->z >> 2);
                }
            }

            s32 flags = mSphereClsn.flags;
            if (!(flags & 4) && (flags & 8)) {
                dBgPi wallResult(*mSphereClsn.GetWallResult());

                if (((Vector3 *)func_02037dc4((int)&wallResult.surface))->y >= 0) {
                    Vector3 top;
                    V3D(top, pos, (height << 1))
                    Vector3 foot;
                    V3C(foot, pos)
                    mRaycastLine.SetObjAndLine(top, foot, mActor);
                    if (mRaycastLine.DetectClsn()) {
                        Vector3 *hit = &mRaycastLine.lineEnd;
                        Vector3 *normal = (Vector3 *)func_02037dc4((int)&mRaycastLine.surface);
                        pos->x = hit->x + (normal->x >> 2);
                        pos->y = hit->y + (normal->y >> 2);
                        pos->z = hit->z + (normal->z >> 2);
                        s32 band = func_02039794(normal->y);
                        if (band == 1 && !(mSphereClsn.flags & 8)) {
                            const dBgPi &lineHit = mRaycastLine;
                            mSphereClsn.flags |= 8;
                            mSphereClsn.SetWallResult(lineHit);
                            mSphereClsn.flags |= 1;
                            (dBgPi &)mSphereClsn = lineHit;
                        } else if (band == 0 && !(mSphereClsn.flags & 4)) {
                            const dBgPi &lineHit = mRaycastLine;
                            mSphereClsn.flags |= 4;
                            mSphereClsn.SetFloorResult(lineHit);
                            mSphereClsn.flags |= 1;
                            (dBgPi &)mSphereClsn = lineHit;
                            if (!floorCommitted)
                                func_020371b0(this, wasOnGround);
                        } else if (band == 2 && !(mSphereClsn.flags & 0x10)) {
                            const dBgPi &lineHit = mRaycastLine;
                            mSphereClsn.flags |= 0x10;
                            mSphereClsn.SetUnderResult(lineHit);
                            mSphereClsn.flags |= 1;
                            (dBgPi &)mSphereClsn = lineHit;
                        }
                    }
                } else {
                    s32 twiceHeight = height << 1;
                    Vector3 foot;
                    V3C(foot, pos)
                    Vector3 top;
                    V3D(top, pos, (height << 1))
                    mRaycastLine.SetObjAndLine(foot, top, mActor);
                    if (mRaycastLine.DetectClsn()) {
                        Vector3 *hit = &mRaycastLine.lineEnd;
                        Vector3 *normal = (Vector3 *)func_02037dc4((int)&mRaycastLine.surface);
                        pos->x = hit->x + (normal->x >> 2);
                        pos->y = hit->y + (normal->y >> 2) - twiceHeight;
                        pos->z = hit->z + (normal->z >> 2);
                    }
                }
            } else if (mSphereClsn.mPushOutMin.y <= -height) {
                Vector3 prevCentre;
                V3D(prevCentre, prev, height)
                Vector3 newCentre;
                V3D(newCentre, pos, height)
                mRaycastLine.SetObjAndLine(prevCentre, newCentre, mActor);
                if (func_0203859c(&mRaycastLine)) {
                    Vector3 *hit = &mRaycastLine.lineEnd;
                    Vector3 *normal = (Vector3 *)func_02037dc4((int)&mRaycastLine.surface);
                    pos->x = hit->x + (normal->x >> 2);
                    pos->y = hit->y + (normal->y >> 2);
                    pos->z = hit->z + (normal->z >> 2);
                }
            }
        }

        if (wasOnGround && !IsOnGround())
            func_020356d4(this);
    }
}

#pragma opt_common_subs on

// @symbol _ZN10dBgCh_Actr12Unk_0203589cEv
void dBgCh_Actr::Unk_0203589c()
{
    mSphereClsn.func_02037b5c();
}

// @symbol func_02035860
/* Copy a vector into the actor's current and previous position fields. */
extern "C" void func_02035860(dBgCh_Actr *self, const Vector3 *src) {
    dActor_c *base = self->mActor;
    struct Vector3 *d1 = &base->mPos;
    d1->x = src->x;
    d1->y = src->y;
    d1->z = src->z;
    struct Vector3 *d2 = &base->mPrevPos;
    d2->x = src->x;
    d2->y = src->y;
    d2->z = src->z;
}

// @symbol _ZN10dBgCh_Actr19StartDetectingWaterEv
/* Forwards to both sub-objects' dBgCh bases, the dBgCh_Lin at 0x134 first and
   the dBgCh_SphCrr at 0x20 second -- that is the ROM's order. */
void dBgCh_Actr::StartDetectingWater()
{
    ((dBgCh *)&mRaycastLine)->StartDetectingWater();
    ((dBgCh *)&mSphereClsn)->StartDetectingWater();
}

// @symbol _ZN10dBgCh_Actr18StopDetectingWaterEv
/* Mirror of StartDetectingWater: the line sub-object first, then the sphere. */
void dBgCh_Actr::StopDetectingWater()
{
    ((dBgCh *)&mRaycastLine)->StopDetectingWater();
    ((dBgCh *)&mSphereClsn)->StopDetectingWater();
}

// @symbol func_02035800
extern "C" void func_02035800(dBgCh_Actr *self)
{
    self->mRaycastLine.StartIgnoringVanishSurfaces();
    self->mSphereClsn.StartIgnoringVanishSurfaces();
}

// @symbol func_020357e0
extern "C" void func_020357e0(dBgCh_Actr *self)
{
    self->mRaycastLine.StopIgnoringVanishSurfaces();
    self->mSphereClsn.StopIgnoringVanishSurfaces();
}

// @symbol func_020357c0
extern "C" void func_020357c0(dBgCh_Actr *self)
{
    self->mRaycastLine.StartIgnoringType14Walls();
    self->mSphereClsn.StartIgnoringType14Walls();
}

// @symbol func_020357a0
extern "C" void func_020357a0(dBgCh_Actr *self)
{
    self->mRaycastLine.StopIgnoringType14Walls();
    self->mSphereClsn.StopIgnoringType14Walls();
}

// @symbol func_02035798
extern "C" void func_02035798(dBgCh_Actr *self, Fix12i scale) {
    self->mScale = scale;
}

// @symbol func_02035784
extern "C" void func_02035784(dBgCh_Actr *self) {
    self->mFlags |= 0x200;
}

// @symbol func_02035770
extern "C" void func_02035770(dBgCh_Actr *self) {
    self->mFlags &= ~0x200;
}

// @symbol func_02035764
extern "C" int func_02035764(dBgCh_Actr *self) {
    return self->mFlags & 0x200;
}

// @symbol _ZN10dBgCh_Actr19ClearAllGroundFlagsEv
void dBgCh_Actr::ClearAllGroundFlags()
{
    mFlags &= ~0x70;
}

// @symbol func_0203573c
extern "C" void func_0203573c(dBgCh_Actr *self) {
    self->mFlags |= 0x20;
}

// @symbol _ZN10dBgCh_Actr22ClearJustHitGroundFlagEv
void dBgCh_Actr::ClearJustHitGroundFlag()
{
    mFlags &= ~0x20;
}

// @symbol _ZNK10dBgCh_Actr13JustHitGroundEv
/* Returns 0 or 0x20, the mask itself -- no normalisation. */
s32 dBgCh_Actr::JustHitGround() const
{
    return mFlags & 0x20;
}

// @symbol _ZN10dBgCh_Actr13SetGroundFlagEv
void dBgCh_Actr::SetGroundFlag()
{
    mFlags |= 0x10;
}

// @symbol _ZN10dBgCh_Actr15ClearGroundFlagEv
void dBgCh_Actr::ClearGroundFlag()
{
    mFlags &= ~0x10;
}

// @symbol _ZNK10dBgCh_Actr10IsOnGroundEv
/* Returns 0 or 0x10, the mask itself -- no normalisation. */
s32 dBgCh_Actr::IsOnGround() const
{
    return mFlags & 0x10;
}

// @symbol func_020356d4
extern "C" void func_020356d4(dBgCh_Actr *self) {
    self->mFlags |= 0x40;
}

// @symbol func_020356c8
extern "C" int func_020356c8(dBgCh_Actr *self) {
    return self->mFlags & 0x40;
}

// @symbol _ZN10dBgCh_Actr13SetLimMovFlagEv
void dBgCh_Actr::SetLimMovFlag()
{
    mFlags |= 0x80;
}

// @symbol _ZN10dBgCh_Actr15ClearLimMovFlagEv
void dBgCh_Actr::ClearLimMovFlag()
{
    mFlags &= ~0x80;
}

// @symbol _ZNK10dBgCh_Actr13GetLimMovFlagEv
/* The limited-movement flag: 0 or 0x80, the mask itself. */
s32 dBgCh_Actr::GetLimMovFlag() const
{
    return mFlags & 0x80;
}

// @symbol func_0203568c
extern "C" void func_0203568c(int *p, int v)
{
    p[6] = v;   /* mRadius */
}

// @symbol func_02035684
extern "C" void func_02035684(int *p, int v)
{
    p[7] = v;   /* mHeight */
}

// @symbol func_0203567c
extern "C" dBgPi *func_0203567c(dBgCh_Actr *self) {
    return &self->mSphereClsn; /* its dBgPi base, at +0x10 */
}

// @symbol _ZNK10dBgCh_Actr14GetFloorResultEv
dBgPi *dBgCh_Actr::GetFloorResult() const {
    return const_cast<dBgCh_SphCrr &>(mSphereClsn).GetFloorResult();
}

// @symbol _ZNK10dBgCh_Actr13GetWallResultEv
dBgPi *dBgCh_Actr::GetWallResult() const {
    return const_cast<dBgCh_SphCrr &>(mSphereClsn).GetWallResult();
}

// @symbol func_0203564c
extern "C" int func_0203564c(int p)
{
    return (int)((dBgCh_Actr *)p)->mSphereClsn.GetUnderResult();
}

// @symbol func_02035644
extern "C" void func_02035644(int *p, int v)
{
    p[67] = v; /* mSphereClsn's wall-rejection half extent (its +0xec) */
}

// @symbol func_02035638
extern "C" int func_02035638(unsigned char *p)
{
    return p[144] & 16;   /* mSphereClsn.flags & 0x10 -- the ceiling bit */
}

// @symbol _ZNK10dBgCh_Actr8IsOnWallEv
/* Returns 0 or 8, the mask itself -- no normalisation. */
s32 dBgCh_Actr::IsOnWall() const
{
    return mSphereClsn.flags & 0x8;
}

// @symbol _ZNK10dBgCh_Actr14GetResultFlag1Ev
/* True when the sphere query found any collision at all: mClsnFlags & 1. */
s32 dBgCh_Actr::GetResultFlag1() const
{
    return mSphereClsn.flags & 0x1;
}

// @symbol _ZNK10dBgCh_Actr12TouchesWaterEv
s32 dBgCh_Actr::TouchesWater() const
{
    return SurfaceInfo_TestFlag0x20((int *)&mSphereClsn.surface);
}

// @symbol func_020355fc
extern "C" void func_020355fc(dBgCh_Actr *self) {
    self->mFlags |= 0x400;
}

// @symbol func_020355e8
extern "C" void func_020355e8(dBgCh_Actr *self) {
    self->mFlags &= ~0x400;
}

// @symbol func_020355dc
extern "C" int func_020355dc(dBgCh_Actr *self) {
    return self->mFlags & 0x400;
}

// @symbol func_020355c8
extern "C" void func_020355c8(dBgCh_Actr *self) {
    self->mFlags &= ~0x800;
}

// @symbol func_020355b4
extern "C" void func_020355b4(dBgCh_Actr *self) {
    self->mFlags |= 0x800;
}

// @symbol func_020355a0
extern "C" int func_020355a0(dBgCh_Actr *self) {
    return (self->mFlags & 0x800) == 0;
}

// @symbol func_0203558c
extern "C" void func_0203558c(dBgCh_Actr *self) {
    self->mFlags |= 0x1000;
}

// @symbol _ZNK10dBgCh_Actr16ShouldUpdatePosYEv
/* The inverse of the no-update-pos-Y flag, normalised to 0/1. */
s32 dBgCh_Actr::ShouldUpdatePosY() const
{
    return (mFlags & 0x1000) == 0;
}

// @symbol _ZNK10dBgCh_Actr15ShouldUpdatePosEv
/* The inverse of the no-update-pos flag, normalised to 0/1. */
s32 dBgCh_Actr::ShouldUpdatePos() const
{
    return (mFlags & 0x2000) == 0;
}

// @symbol func_02035550
extern "C" void func_02035550(dBgCh_Actr *self) {
    self->mFlags |= 0x4000;
}

// @symbol func_0203553c
extern "C" int func_0203553c(dBgCh_Actr *self) {
    return (self->mFlags & 0x4000) == 0;
}
