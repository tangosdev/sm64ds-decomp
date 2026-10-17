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
// leftovers: Init stays a free definition spelling its own mangled name (the
// Fix12i params travel by value in the mangling, wall 6az). dBgPiLoc is a
// manually laid-out dBgCh image (UpdateExtraContinous copies a wall result
// field by field, so it needs names without the constructor). GetClsnPos
// keeps the free (res, self) spelling (the member returns Vector3 by value,
// wall 6az). The local externs spell the ROM's direct-call sites. The
// func_0203* workers keep address names (no member evidence names them).

#include "types.h"
#include "common.h"
#include "dBgCh_Actr.h"
#include "dBgCh_Gnd.h"
#include "dActor_c.h"
#include "Player.h"

extern "C" {

/* dBgCh's unnamed flag helpers, from the run just below this one. Each of
   the four fan-out workers below calls one of them on both sub-objects. */
void func_020353e0(char *self);
void func_020353f4(char *self);
void func_02035414(char *self);
void func_02035428(char *self);

/* Checker-family helpers shared across the collision TUs, spelled as
   defined. */
int  func_02037dc4(int p);   /* SurfaceInfo * -> Vector3 * (the hit normal) */
void func_02038234(int a, int b);
void func_02038324(void *arg, int b, int c, int d);
int  func_0203842c(char *self);
int  func_0203859c(void *obj);
int  func_02039794(int x);
int  SurfaceInfo_TestFlag0x20(int *p);
/* local extern: the radius travels by value inside the mangled name
   (wall 6az), so the call spells the symbol with Fix12i rather than going
   through the member. */
void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
    dBgCh_SphCrr *self, const Vector3 *pos, Fix12i radius, dActor_c *actor);
/* local extern: ctor/dtor calls the ROM makes on the dBgPiLoc images in
   the opt_common_subs bodies -- a POD layout has no member spelling. */
dBgPi *_ZN5dBgPiD1Ev(dBgPi *self);
dBgPi *_ZN5dBgPiC1Ev(dBgPi *self);
extern int data_02099368[];  /* _ZTV5dBgPi -- same address, see symbols.txt */

/* This TU's own workers, defined below. */
int  func_0203553c(int *p);       /* (mFlags & 0x4000) == 0 -- water-tracking bit */
void func_02035550(char *self);   /* mFlags |= 0x4000 */
int  func_020355a0(int *p);       /* (mFlags & 0x800) == 0 */
int  func_020355dc(int *p);       /* mFlags & 0x400 */
void func_020356d4(char *self);   /* mFlags |= 0x40 -- left ground */
int  func_02035764(int *p);       /* mFlags & 0x200 */
void func_0203573c(char *self);   /* mFlags |= 0x20 -- just hit ground */
void func_020371b0(dBgCh_Actr *clsn, s32 justHit);  /* floor-result commit */

}

/* Manually laid-out dBgPi image used by UpdateExtraContinous's `tmp`: the
   body copies a wall result's fields one at a time, so it needs the field
   names without the constructor. Layout is dBgPi's own. */
struct dBgPiLoc
{
    u32 *vt;
    SurfaceInfo si;
    unsigned short tri;
    unsigned short clsn;
    u32 objID;
    void *obj;
    void *mesh;
};

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

/* Init stays a free definition spelling its own mangled name: the Fix12i
   parameters travel by value inside the mangling (wall 6az), so no member
   spelling is available in this TU. */

// @symbol _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *self, int actor, int radius, int height, int a, int b)
{
    self->mActor = (dActor_c *)actor;
    self->mRadius = radius;
    self->mHeight = height;
    self->mFlags = 0;
    self->mSphereClsn.mMotionAngPtr = a;
    self->mAngPtr = b;
    self->mScale = 0x1000;
}

// @symbol func_02037318
extern "C" void func_02037318(char *self)
{
    dBgCh_Actr *clsn = (dBgCh_Actr *)self;
    const int *src;

    clsn->mFlags &= ~0x100;
    if (clsn->IsOnGround() == 0)
        return;
    if (((Player *)clsn->mActor)->IsInAir())
        return;
    clsn->mFlags |= 0x100;
    src = &clsn->mSphereClsn.unk_0fc;
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
        Vector3 pos;
        Vector3 *objpos = &clsn->mActor->mPos;
        dBgCh_Gnd rg;
        pos.x = objpos->x;
        pos.y = objpos->y + clsn->mRadius;
        pos.z = objpos->z;
        rg.SetObjAndPos(pos, clsn->mActor);
        if (rg.DetectClsn() != 0) {
            int cy = rg.clsnY;
            int diff = objpos->y - cy;
            if (diff > 0 && diff < (clsn->mRadius << 1)) {
                objpos->y = cy;
                clsn->mSphereClsn.flags |= 4;
                clsn->mSphereClsn.mClsnResult1 = rg;
                clsn->SetGroundFlag();
            }
        }
    }
}

// @symbol func_020371b0
/* Floor-result commit: hand the hit record to the actor, raise on-ground,
   and kill vertical speed unless limited movement is set. `justHit` is the
   frame's ground state on entry, so the just-hit flag only fires on the
   transition. */
extern "C" void func_020371b0(dBgCh_Actr *clsn, s32 justHit)
{
    if (justHit == 0)
        func_0203573c((char *)clsn);
    func_02038234((int)clsn->GetFloorResult(), (int)clsn->mActor);
    clsn->SetGroundFlag();
    if (clsn->GetLimMovFlag() == 0)
        clsn->mActor->mVertSpeed = 0;
}

// @symbol _ZN10dBgCh_Actr20UpdateDiscreteNoLavaEv
void dBgCh_Actr::UpdateDiscreteNoLava()
{
    int onGround;
    int sy;
    struct Vector3 v;
    struct Vector3 *p6c;
    struct Vector3 *src = &mActor->mPos;
    struct Vector3 *p68 = &mActor->mPrevPos;

    if (IsOnGround() && func_020355a0((int *)this)
        && ShouldUpdatePos()) {
        func_02038324((void *)mSphereClsn.GetFloorResult(), (int)src, mSphereClsn.mMotionAngPtr,
                      mAngPtr);
    }
    onGround = IsOnGround();
    ClearAllGroundFlags();
    v.x = src->x;
    sy = src->y;
    v.y = sy;
    v.z = src->z;
    v.y = sy + mHeight;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
        &mSphereClsn, &v, mRadius, mActor);
    mSphereClsn.mScale = mScale;
    if (src->y - p68->y > 0) {
        mSphereClsn.flags |= 0x20;
    }
    if (mSphereClsn.DetectClsn()) {
        p6c = &mSphereClsn.disp;
        if (mSphereClsn.flags & 4) {
            func_020371b0(this, onGround);
        }
        if (ShouldUpdatePos()) {
            src->x += p6c->x;
            if (ShouldUpdatePosY()) {
                src->y += p6c->y;
            }
            src->z += p6c->z;
        }
    }
    if (onGround == 0)
        return;
    if (IsOnGround())
        return;
    func_020356d4(((char *)this));
}

// @symbol _ZN10dBgCh_Actr22UpdateDiscreteNoLava_2Ev
void dBgCh_Actr::UpdateDiscreteNoLava_2()
{
    int onGround;
    int sy;
    struct Vector3 v;
    struct Vector3 *p6c;
    struct Vector3 *src = &mActor->mPos;
    dActor_c *obj = mActor;

    onGround = IsOnGround();
    ClearAllGroundFlags();
    v.x = src->x;
    sy = src->y;
    v.y = sy;
    v.z = src->z;
    v.y = sy + mHeight;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
        &mSphereClsn, &v, mRadius, mActor);
    mSphereClsn.mScale = mScale;
    if (src->y - obj->mPrevPosY > 0) {
        mSphereClsn.flags |= 0x20;
    }
    if (mSphereClsn.func_02038a38()) {
        p6c = &mSphereClsn.disp;
        if (mSphereClsn.flags & 4) {
            func_020371b0(this, onGround);
        }
        if (ShouldUpdatePos()) {
            src->x += p6c->x;
            if (ShouldUpdatePosY()) {
                src->y += p6c->y;
            }
            src->z += p6c->z;
        }
    }
    if (onGround == 0)
        return;
    if (IsOnGround())
        return;
    func_020356d4(((char *)this));
}

/* func_02036acc is UpdateContinuous's free twin: the same wall/floor probing
   sequence but taking the query by raw pointer and running the func_0203842c/
   func_02038824 detection variants. The ROM keeps its unmangled name, so it
   stays extern "C". */

#pragma opt_common_subs off

// @symbol func_02036acc
extern "C" void func_02036acc(char *c)
{
    int floorFlag;
    int wallFlag;
    int height;
    int onGround;
    int* pos;
    int* prev;
    int handled;
    dBgPiLoc res0;
    dBgPiLoc res1;
    Vector3 lineStart, lineEnd;
    dActor_c *a;
    dBgCh_Actr *self = (dBgCh_Actr *)c;

    a = self->mActor;
    pos = &a->mPosX;
    prev = &a->mPrevPosX;

    if (self->IsOnGround() && func_020355a0((int*)c) && self->ShouldUpdatePos())
        func_02038324((void *)self->mSphereClsn.GetFloorResult(), (int)pos,
                      self->mSphereClsn.mMotionAngPtr, self->mAngPtr);

    floorFlag = 0;
    _ZN5dBgPiC1Ev((dBgPi *)&res0);
    wallFlag = 0;
    _ZN5dBgPiC1Ev((dBgPi *)&res1);

    height = self->mHeight;
    {
        int tx = prev[0];
        int tz = prev[2];
        int ty = prev[1] + height;
        lineStart.x = tx;
        lineStart.y = ty;
        lineStart.z = tz;
    }
    {
        int tx = pos[0];
        int tz = pos[2];
        int ty = pos[1] + height;
        lineEnd.x = tx;
        lineEnd.y = ty;
        lineEnd.z = tz;
    }
    self->mRaycastLine.SetObjAndLine(*&lineStart, *&lineEnd, self->mActor);
    if (func_0203842c((char *)&self->mRaycastLine)) {
        int r;
        Vector3 clsnPos = self->mRaycastLine.GetClsnPos();
        Vector3 normal, newStart, newEnd;
        self->mRaycastLine.surface.CopyNormalTo(normal);
        newStart.x = clsnPos.x + (normal.x >> 2);
        newStart.y = (normal.y >> 2) + clsnPos.y;
        newStart.z = clsnPos.z + (normal.z >> 2);
        newEnd.x = newStart.x;
        newEnd.y = newStart.y - height;
        newEnd.z = newStart.z;
        r = func_02039794(normal.y);
        if (r == 1) {
            wallFlag = 1;
            ((const dBgPi &)self->mRaycastLine).CopyTo(*(dBgPi *)&res1);
        } else if (r == 0) {
            floorFlag = 1;
            ((const dBgPi &)self->mRaycastLine).CopyTo(*(dBgPi *)&res0);
        }
        self->mRaycastLine.SetObjAndLine(*&newStart, *&newEnd, self->mActor);
        if (func_0203842c((char *)&self->mRaycastLine)) {
            Vector3 clsnPos2 = self->mRaycastLine.GetClsnPos();
            Vector3 normal2;
            self->mRaycastLine.surface.CopyNormalTo(normal2);
            if (func_02039794(normal2.y) == 0) {
                floorFlag = 1;
                ((const dBgPi &)self->mRaycastLine).CopyTo(*(dBgPi *)&res0);
            }
            if (self->ShouldUpdatePos()) {
                pos[0] = clsnPos2.x - (normal2.x >> 2);
                pos[1] = clsnPos2.y - (normal2.y >> 2) - (height >> 1);
                pos[2] = clsnPos2.z - (normal2.z >> 2);
            }
        } else if (self->ShouldUpdatePos()) {
            pos[0] = newStart.x;
            pos[1] = newStart.y - height;
            pos[2] = newStart.z;
        }
    }

    onGround = self->IsOnGround();
    handled = 0;
    self->ClearAllGroundFlags();
    Vector3 sphere;
    sphere.x = pos[0];
    sphere.y = pos[1];
    sphere.z = pos[2];
    sphere.y += height;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(&self->mSphereClsn, &sphere,
                                                                     self->mRadius, self->mActor);
    if (func_0203553c((int*)c) == 0)
        self->mSphereClsn.flags |= 0x40;
    self->mSphereClsn.mScale = self->mScale;
    if (pos[1] - prev[1] > 0)
        self->mSphereClsn.flags |= 0x20;
    if (floorFlag != 0) {
        self->mSphereClsn.flags |= 4;
        self->mSphereClsn.SetFloorResult(*(const dBgPi *)&res0);
        self->mSphereClsn.flags |= 1;
        (dBgPi &)self->mSphereClsn = *(const dBgPi *)&res0;
        func_020371b0(self, onGround);
        handled = 1;
    }
    if (wallFlag != 0) {
        self->mSphereClsn.flags |= 8;
        self->mSphereClsn.SetWallResult(*(const dBgPi *)&res1);
        self->mSphereClsn.flags |= 1;
        (dBgPi &)self->mSphereClsn = *(const dBgPi *)&res1;
    }
    if (self->mSphereClsn.func_02038824()) {
        prev = (int *)&self->mSphereClsn.disp;
        if ((self->mSphereClsn.flags & 4) && handled == 0)
            func_020371b0(self, onGround);
        if (self->ShouldUpdatePos()) {
            pos[0] += prev[0];
            if (self->ShouldUpdatePosY())
                pos[1] += prev[1];
            pos[2] += prev[2];
        }
    }
    if (onGround && self->IsOnGround() == 0)
        func_020356d4(c);
    _ZN5dBgPiD1Ev((dBgPi *)&res1);
    _ZN5dBgPiD1Ev((dBgPi *)&res0);
}

#pragma opt_common_subs on

#pragma opt_common_subs off

// @symbol _ZN10dBgCh_Actr16UpdateContinuousEv
void dBgCh_Actr::UpdateContinuous()
{
    int floorFlag;
    int wallFlag;
    int height;
    int onGround;
    int* pos;
    int* prev;
    int handled;
    dBgPiLoc res0;
    dBgPiLoc res1;
    Vector3 lineStart, lineEnd;
    dActor_c *a;

    a = mActor;
    pos = &a->mPosX;
    prev = &a->mPrevPosX;

    if (IsOnGround() && func_020355a0((int*)this) && ShouldUpdatePos())
        func_02038324((void *)mSphereClsn.GetFloorResult(), (int)pos, mSphereClsn.mMotionAngPtr,
                      mAngPtr);

    floorFlag = 0;
    _ZN5dBgPiC1Ev((dBgPi *)&res0);
    wallFlag = 0;
    _ZN5dBgPiC1Ev((dBgPi *)&res1);

    height = mHeight;
    {
        int tx = prev[0];
        int tz = prev[2];
        int ty = prev[1] + height;
        lineStart.x = tx;
        lineStart.y = ty;
        lineStart.z = tz;
    }
    {
        int tx = pos[0];
        int tz = pos[2];
        int ty = pos[1] + height;
        lineEnd.x = tx;
        lineEnd.y = ty;
        lineEnd.z = tz;
    }
    mRaycastLine.SetObjAndLine(*&lineStart, *&lineEnd, mActor);
    if (((dBgCh_Lin *)((char*)&mRaycastLine))->DetectClsn())
    {
        int r;
        Vector3 clsnPos = mRaycastLine.GetClsnPos();
        Vector3 normal, newStart, newEnd;
        mRaycastLine.surface.CopyNormalTo(normal);
        newStart.x = clsnPos.x + (normal.x >> 2);
        newStart.y = (normal.y >> 2) + clsnPos.y;
        newStart.z = clsnPos.z + (normal.z >> 2);
        newEnd.x = newStart.x;
        newEnd.y = newStart.y - height;
        newEnd.z = newStart.z;
        r = func_02039794(normal.y);
        if (r == 1) {
            wallFlag = 1;
            ((const dBgPi &)mRaycastLine).CopyTo(*(dBgPi *)&res1);
        } else if (r == 0) {
            floorFlag = 1;
            ((const dBgPi &)mRaycastLine).CopyTo(*(dBgPi *)&res0);
        }
        mRaycastLine.SetObjAndLine(*&newStart, *&newEnd, mActor);
        if (((dBgCh_Lin *)((char*)&mRaycastLine))->DetectClsn()) {
            Vector3 clsnPos2 = mRaycastLine.GetClsnPos();
            Vector3 normal2;
            mRaycastLine.surface.CopyNormalTo(normal2);
            if (func_02039794(normal2.y) == 0) {
                floorFlag = 1;
                ((const dBgPi &)mRaycastLine).CopyTo(*(dBgPi *)&res0);
            }
            if (ShouldUpdatePos()) {
                pos[0] = clsnPos2.x - (normal2.x >> 2);
                pos[1] = clsnPos2.y - (normal2.y >> 2) - (height >> 1);
                pos[2] = clsnPos2.z - (normal2.z >> 2);
            }
        } else if (ShouldUpdatePos()) {
            pos[0] = newStart.x;
            pos[1] = newStart.y - height;
            pos[2] = newStart.z;
        }
    }

    onGround = IsOnGround();
    handled = 0;
    ClearAllGroundFlags();
    Vector3 sphere;
    sphere.x = pos[0];
    sphere.y = pos[1];
    sphere.z = pos[2];
    sphere.y += height;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(&mSphereClsn, &sphere, mRadius,
                                                                     mActor);
    if (func_0203553c((int*)this) == 0)
        mSphereClsn.flags |= 0x40;
    mSphereClsn.mScale = mScale;
    if (pos[1] - prev[1] > 0)
        mSphereClsn.flags |= 0x20;
    if (floorFlag != 0) {
        mSphereClsn.flags |= 4;
        mSphereClsn.SetFloorResult(*(const dBgPi *)&res0);
        mSphereClsn.flags |= 1;
        (dBgPi &)mSphereClsn = *(const dBgPi *)&res0;
        func_020371b0(this, onGround);
        handled = 1;
    }
    if (wallFlag != 0) {
        mSphereClsn.flags |= 8;
        mSphereClsn.SetWallResult(*(const dBgPi *)&res1);
        mSphereClsn.flags |= 1;
        (dBgPi &)mSphereClsn = *(const dBgPi *)&res1;
    }
    if (mSphereClsn.DetectClsn()) {
        prev = (int*)((char*)&mSphereClsn.disp);
        if ((mSphereClsn.flags & 4) && handled == 0)
            func_020371b0(this, onGround);
        if (ShouldUpdatePos()) {
            pos[0] += prev[0];
            if (ShouldUpdatePosY())
                pos[1] += prev[1];
            pos[2] += prev[2];
        }
    }
    if (onGround && IsOnGround() == 0)
        func_020356d4(((char*)this));
    _ZN5dBgPiD1Ev((dBgPi *)&res1);
    _ZN5dBgPiD1Ev((dBgPi *)&res0);
}

#pragma opt_common_subs on

#pragma opt_common_subs off

// @symbol _ZN10dBgCh_Actr22UpdateContinuousNoLavaEv
void dBgCh_Actr::UpdateContinuousNoLava()
{
    int floorFlag;
    int wallFlag;
    int height;
    int onGround;
    int* pos;
    int* prev;
    int handled;
    dBgPiLoc res0;
    dBgPiLoc res1;
    Vector3 lineStart, lineEnd;

    {
        dActor_c *a = mActor;
        pos = &a->mPosX;
        prev = &a->mPrevPosX;
    }

    floorFlag = 0;
    _ZN5dBgPiC1Ev((dBgPi *)&res0);
    wallFlag = 0;
    _ZN5dBgPiC1Ev((dBgPi *)&res1);

    height = mHeight;
    {
        int tx = prev[0];
        int tz = prev[2];
        int ty = prev[1] + height;
        lineStart.x = tx;
        lineStart.y = ty;
        lineStart.z = tz;
    }
    {
        int tx = pos[0];
        int tz = pos[2];
        int ty = pos[1] + height;
        lineEnd.x = tx;
        lineEnd.y = ty;
        lineEnd.z = tz;
    }
    mRaycastLine.SetObjAndLine(*&lineStart, *&lineEnd, mActor);
    if (func_0203859c((char*)&mRaycastLine))
    {
        int r;
        Vector3 clsnPos = mRaycastLine.GetClsnPos();
        Vector3 normal, newStart, newEnd;
        mRaycastLine.surface.CopyNormalTo(normal);
        newStart.x = clsnPos.x + (normal.x >> 2);
        newStart.y = (normal.y >> 2) + clsnPos.y;
        newStart.z = clsnPos.z + (normal.z >> 2);
        newEnd.x = newStart.x;
        newEnd.y = newStart.y - height;
        newEnd.z = newStart.z;
        r = func_02039794(normal.y);
        if (r == 1) {
            wallFlag = 1;
            ((const dBgPi &)mRaycastLine).CopyTo(*(dBgPi *)&res1);
        }
        mRaycastLine.SetObjAndLine(*&newStart, *&newEnd, mActor);
        if (func_0203859c((char*)&mRaycastLine)) {
            Vector3 clsnPos2 = mRaycastLine.GetClsnPos();
            Vector3 normal2;
            mRaycastLine.surface.CopyNormalTo(normal2);
            if (func_02039794(normal2.y) == 0) {
                floorFlag = 1;
                ((const dBgPi &)mRaycastLine).CopyTo(*(dBgPi *)&res0);
            }
            if (ShouldUpdatePos()) {
                pos[0] = clsnPos2.x - (normal2.x >> 2);
                pos[1] = clsnPos2.y - (normal2.y >> 2) - (height >> 1);
                pos[2] = clsnPos2.z - (normal2.z >> 2);
            }
        } else if (ShouldUpdatePos()) {
            pos[0] = newStart.x;
            pos[1] = newStart.y - height;
            pos[2] = newStart.z;
        }
    }

    onGround = IsOnGround();
    ClearAllGroundFlags();
    handled = 0;
    Vector3 sphere;
    sphere.x = pos[0];
    sphere.y = pos[1];
    sphere.z = pos[2];
    sphere.y += height;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(&mSphereClsn, &sphere, mRadius,
                                                                     mActor);
    mSphereClsn.mScale = mScale;
    if (pos[1] - prev[1] > 0)
        mSphereClsn.flags |= 0x20;
    if (floorFlag != 0) {
        mSphereClsn.flags |= 4;
        mSphereClsn.SetFloorResult(*(const dBgPi *)&res0);
        mSphereClsn.flags |= 1;
        (dBgPi &)mSphereClsn = *(const dBgPi *)&res0;
        func_020371b0(this, onGround);
        handled = 1;
    }
    if (wallFlag != 0) {
        mSphereClsn.flags |= 8;
        mSphereClsn.SetWallResult(*(const dBgPi *)&res1);
        mSphereClsn.flags |= 1;
        (dBgPi &)mSphereClsn = *(const dBgPi *)&res1;
    }
    if (mSphereClsn.func_02038a38()) {
        prev = (int*)((char*)&mSphereClsn.disp);
        if ((mSphereClsn.flags & 4) && handled == 0)
            func_020371b0(this, onGround);
        if (ShouldUpdatePos()) {
            pos[0] += prev[0];
            if (ShouldUpdatePosY())
                pos[1] += prev[1];
            pos[2] += prev[2];
        }
    }
    if (onGround && IsOnGround() == 0)
        func_020356d4(((char*)this));
    _ZN5dBgPiD1Ev((dBgPi *)&res1);
    _ZN5dBgPiD1Ev((dBgPi *)&res0);
}

#pragma opt_common_subs on

#define V3D(v, p, dy) { s32 _x, _y, _z; _z = (p)->z; _y = (p)->y; _x = (p)->x; _y = _y + (dy); (v).x = _x; (v).y = _y; (v).z = _z; }
#define V3C(v, p) { s32 _x, _y, _z; _z = (p)->z; _y = (p)->y; _x = (p)->x; (v).x = _x; (v).y = _y; (v).z = _z; }

#pragma opt_common_subs off

// @symbol _ZN10dBgCh_Actr20UpdateExtraContinousEv
/* The full three-result variant: raycast first for wall/floor/ceiling, then
   the sphere, then a follow-up raycast past the displacement. The dBgPiLoc
   `tmp` copies a wall result's fields by hand so the compiler's own dBgPi
   copy is not used there. */
void dBgCh_Actr::UpdateExtraContinous()
{
    s32 f0, f1, f2, vo, wasOnGround, didHit;
    Vector3 *pos, *prev;
    dActor_c *ac;

    ac = mActor;
    pos = &ac->mPos;
    prev = &ac->mPrevPos;

    if (IsOnGround() && func_020355a0((int *)this))
        func_02038324((void *)mSphereClsn.GetFloorResult(), (int)pos, mSphereClsn.mMotionAngPtr,
                      mAngPtr);

    {
        dBgPiLoc res0;
        f0 = 0;
        _ZN5dBgPiC1Ev((dBgPi *)&res0);
        dBgPiLoc res1;
        f1 = 0;
        _ZN5dBgPiC1Ev((dBgPi *)&res1);
        dBgPiLoc res2;
        f2 = 0;
        _ZN5dBgPiC1Ev((dBgPi *)&res2);

        vo = mHeight;

        Vector3 v7c;


        V3D(v7c, prev, vo)
        Vector3 v88;

        V3C(v88, prev)
        mRaycastLine.SetObjAndLine(v88, v7c, mActor);

        if (func_0203859c(&mRaycastLine)) {
            v7c = mRaycastLine.GetClsnPos();
            Vector3 v94;
            mRaycastLine.surface.CopyNormalTo(v94);
            v7c.x = v7c.x + (v94.x >> 2);
            v7c.y = v7c.y + (v94.y >> 2);
            v7c.z = v7c.z + (v94.z >> 2);
        }

        Vector3 va0;


        V3D(va0, pos, vo)
        mRaycastLine.SetObjAndLine(v7c, va0, mActor);

        if (mRaycastLine.DetectClsn()) {
            Vector3 vac;
            mRaycastLine.surface.CopyNormalTo(vac);
            s32 kind; kind = func_02039794(vac.y);
            if (kind == 1)
            {
                f1 = 1;
                ((const dBgPi &)mRaycastLine).CopyTo(*(dBgPi *)&res1);
            }
            else if (kind == 2)
            {
                f2 = 1;
                ((const dBgPi &)mRaycastLine).CopyTo(*(dBgPi *)&res2);
            }
            else if (kind == 0)
            {
                f0 = 1;
                ((const dBgPi &)mRaycastLine).CopyTo(*(dBgPi *)&res0);
            }

            Vector3 vb8 = mRaycastLine.GetClsnPos();
            Vector3 vc4;
            vc4.x = vb8.x + (vac.x >> 2);
            vc4.y = vb8.y + (vac.y >> 2);
            vc4.z = vb8.z + (vac.z >> 2);
            Vector3 vd0;
            vd0.x = vc4.x;
            vd0.y = vc4.y - vo;
            vd0.z = vc4.z;
            mRaycastLine.SetObjAndLine(vc4, vd0, mActor);

            if (mRaycastLine.DetectClsn()) {
                Vector3 vdc = mRaycastLine.GetClsnPos();
                Vector3 ve8;
                mRaycastLine.surface.CopyNormalTo(ve8);
                if (func_02039794(ve8.y) == 0)
                {
                    f0 = 1;
                    ((const dBgPi &)mRaycastLine).CopyTo(*(dBgPi *)&res0);
                }
                vdc.x = vdc.x + (ve8.x << 2);
                vdc.y = vdc.y + (ve8.y << 2);
                vdc.z = vdc.z + (ve8.z << 2);
                Vector3 vf4;
                vf4.x = vdc.x;
                vf4.y = vdc.y + (vo << 1);
                vf4.z = vdc.z;
                mRaycastLine.SetObjAndLine(vdc, vf4, mActor);

                if (mRaycastLine.DetectClsn()) {
                    Vector3 v100 = mRaycastLine.GetClsnPos();
                    pos->x = v100.x;
                    pos->y = ((v100.y + vdc.y) >> 1) - vo;
                    pos->z = v100.z;
                } else {
                    pos->x = vdc.x - (ve8.x >> 2);
                    pos->y = vdc.y - (ve8.y >> 2) - (vo >> 1);
                    pos->z = vdc.z - (ve8.z >> 2);
                }
            } else {
                pos->x = vb8.x + (vac.x >> 2);
                pos->y = vb8.y + (vac.y >> 2) - vo;
                pos->z = vb8.z + (vac.z >> 2);
            }

            Vector3 v10c;


            V3D(v10c, prev, vo)
            Vector3 v118;

            V3D(v118, pos, vo)
            mRaycastLine.SetObjAndLine(v10c, v118, mActor);

            if (mRaycastLine.DetectClsn() &&
                ((Vector3 *)func_02037dc4((int)&mRaycastLine.surface))->y >= 0) {
                Vector3 *cp = &mRaycastLine.lineEnd;
                pos->x = cp->x;
                pos->y = cp->y;
                pos->z = cp->z;
            }
        }

        wasOnGround = IsOnGround();
        ClearAllGroundFlags();
        didHit = 0;

        Vector3 v124;
        v124.x = pos->x;
        v124.y = pos->y;
        v124.z = pos->z;
        v124.y = v124.y + vo;
        _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(&mSphereClsn, &v124,
                                                                         mRadius, mActor);

        mSphereClsn.mScale = mScale;
        if (func_02035764((int *)this))
            mSphereClsn.flags |= 2;
        if (pos->y - prev->y > 0)
            mSphereClsn.flags |= 0x20;

        if (f0)
        {
            mSphereClsn.flags |= 4;
            mSphereClsn.SetFloorResult(*(const dBgPi *)&res0);
            mSphereClsn.flags |= 1;
            (dBgPi &)mSphereClsn = *(const dBgPi *)&res0;
            func_020371b0(this, wasOnGround);
            didHit = 1;
        }
        if (f1)
        {
            mSphereClsn.flags |= 8;
            mSphereClsn.SetWallResult(*(const dBgPi *)&res1);
            mSphereClsn.flags |= 1;
            (dBgPi &)mSphereClsn = *(const dBgPi *)&res1;
        }
        if (f2)
        {
            mSphereClsn.flags |= 0x10;
            mSphereClsn.SetUnderResult(*(const dBgPi *)&res2);
            mSphereClsn.flags |= 1;
            (dBgPi &)mSphereClsn = *(const dBgPi *)&res2;
        }

        if (mSphereClsn.DetectClsn()) {
            Vector3 *pb = &mSphereClsn.disp;
            if ((mSphereClsn.flags & 4) && didHit == 0) {
                func_020371b0(this, wasOnGround);
                didHit = 1;
            }
            pos->x = pos->x + pb->x;
            if (ShouldUpdatePosY())
                pos->y = pos->y + pb->y;
            pos->z = pos->z + pb->z;

            if (func_020355dc((int *)this)) {
                Vector3 v130;

                V3D(v130, pos, (vo << 1))
                Vector3 v13c;

                V3C(v13c, pos)
                mRaycastLine.SetObjAndLine(v130, v13c, mActor);
                if (mRaycastLine.DetectClsn()) {
                    Vector3 *cp = &mRaycastLine.lineEnd;
                    Vector3 *n = (Vector3 *)func_02037dc4((int)&mRaycastLine.surface);
                    pos->x = cp->x + (n->x >> 2);
                    pos->y = cp->y + (n->y >> 2);
                    pos->z = cp->z + (n->z >> 2);
                }
            }

            s32 fl;
            fl = mSphereClsn.flags;
            if (!(fl & 4) && (fl & 8))
            {
                dBgPiLoc tmp;
                dBgPiLoc *src = (dBgPiLoc *)(int)mSphereClsn.GetWallResult();
                SurfaceInfo *dsi = &tmp.si;
                {
                    s32 c0, c1;
                    c0 = *(volatile s32 *)&src->si.clps.w0;
                    c1 = *(volatile s32 *)&src->si.clps.w1;
                    dsi->clps.w0 = c0;
                    dsi->clps.w1 = c1;
                    dsi->normal.x = src->si.normal.x;
                    dsi->normal.y = src->si.normal.y;
                    dsi->normal.z = src->si.normal.z;
                }
                tmp.vt = (u32 *)data_02099368;
                tmp.tri = src->tri;
                tmp.clsn = src->clsn;
                tmp.objID = src->objID;
                tmp.obj = src->obj;
                tmp.mesh = src->mesh;

                if (((Vector3 *)func_02037dc4((int)dsi))->y >= 0)
                {
                    Vector3 v170;

                    V3D(v170, pos, (vo << 1))
                    Vector3 v17c;

                    V3C(v17c, pos)
                    mRaycastLine.SetObjAndLine(v170, v17c, mActor);
                    if (mRaycastLine.DetectClsn()) {
                        Vector3 *cp = &mRaycastLine.lineEnd;
                        Vector3 *n = (Vector3 *)func_02037dc4((int)&mRaycastLine.surface);
                        pos->x = cp->x + (n->x >> 2);
                        pos->y = cp->y + (n->y >> 2);
                        pos->z = cp->z + (n->z >> 2);
                        s32 k3; k3 = func_02039794(n->y);
                        if (k3 == 1 && !(mSphereClsn.flags & 8)) {
                            dBgPi *lr = (dBgPi *)&mRaycastLine;
                            mSphereClsn.flags |= 8;
                            mSphereClsn.SetWallResult(*(const dBgPi *)lr);
                            mSphereClsn.flags |= 1;
                            (dBgPi &)mSphereClsn = *lr;
                        } else if (k3 == 0 && !(mSphereClsn.flags & 4)) {
                            dBgPi *lr = (dBgPi *)&mRaycastLine;
                            mSphereClsn.flags |= 4;
                            mSphereClsn.SetFloorResult(*lr);
                            mSphereClsn.flags |= 1;
                            (dBgPi &)mSphereClsn = *lr;
                            if (didHit == 0)
                                func_020371b0(this, wasOnGround);
                        } else if (k3 == 2 && !(mSphereClsn.flags & 0x10)) {
                            dBgPi *lr = (dBgPi *)&mRaycastLine;
                            mSphereClsn.flags |= 0x10;
                            mSphereClsn.SetUnderResult(*(const dBgPi *)lr);
                            mSphereClsn.flags |= 1;
                            (dBgPi &)mSphereClsn = *lr;
                        }
                    }
                }
                else
                {
                    s32 vo2 = vo << 1;
                    Vector3 v188;

                    V3C(v188, pos)
                    Vector3 v194;

                    V3D(v194, pos, (vo << 1))
                    mRaycastLine.SetObjAndLine(v188, v194, mActor);
                    if (mRaycastLine.DetectClsn()) {
                        Vector3 *cp = &mRaycastLine.lineEnd;
                        Vector3 *n = (Vector3 *)func_02037dc4((int)&mRaycastLine.surface);
                        pos->x = cp->x + (n->x >> 2);
                        pos->y = cp->y + (n->y >> 2) - vo2;
                        pos->z = cp->z + (n->z >> 2);
                    }
                }
                _ZN5dBgPiD1Ev((dBgPi *)&tmp);
            } else if (mSphereClsn.aabbMin.y <= -vo) {
                Vector3 v1a0;

                V3D(v1a0, prev, vo)
                Vector3 v1ac;

                V3D(v1ac, pos, vo)
                mRaycastLine.SetObjAndLine(v1a0, v1ac, mActor);
                if (func_0203859c(&mRaycastLine)) {
                    Vector3 *cp = &mRaycastLine.lineEnd;
                    Vector3 *n = (Vector3 *)func_02037dc4((int)&mRaycastLine.surface);
                    pos->x = cp->x + (n->x >> 2);
                    pos->y = cp->y + (n->y >> 2);
                    pos->z = cp->z + (n->z >> 2);
                }
            }
        }

        if (wasOnGround && !IsOnGround())
            func_020356d4((char *)this);

        _ZN5dBgPiD1Ev((dBgPi *)&res2);
        _ZN5dBgPiD1Ev((dBgPi *)&res1);
        _ZN5dBgPiD1Ev((dBgPi *)&res0);
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
extern "C" void func_02035860(char *o, struct Vector3 *src)
{
    dActor_c *base = ((dBgCh_Actr *)o)->mActor;
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
    func_02035428((char *)&self->mRaycastLine);
    func_02035428((char *)&self->mSphereClsn);
}

// @symbol func_020357e0
extern "C" void func_020357e0(dBgCh_Actr *self)
{
    func_02035414((char *)&self->mRaycastLine);
    func_02035414((char *)&self->mSphereClsn);
}

// @symbol func_020357c0
extern "C" void func_020357c0(dBgCh_Actr *self)
{
    func_020353f4((char *)&self->mRaycastLine);
    func_020353f4((char *)&self->mSphereClsn);
}

// @symbol func_020357a0
extern "C" void func_020357a0(dBgCh_Actr *self)
{
    func_020353e0((char *)&self->mRaycastLine);
    func_020353e0((char *)&self->mSphereClsn);
}

// @symbol func_02035798
extern "C" void func_02035798(int *p, int v)
{
    p[110] = v;   /* mScale */
}

// @symbol func_02035784
extern "C" void func_02035784(char *self)
{
    ((dBgCh_Actr *)self)->mFlags |= 0x200;
}

// @symbol func_02035770
extern "C" void func_02035770(char *self)
{
    ((dBgCh_Actr *)self)->mFlags &= ~0x200;
}

// @symbol func_02035764
extern "C" int func_02035764(int *p)
{
    return p[4] & 512;
}

// @symbol _ZN10dBgCh_Actr19ClearAllGroundFlagsEv
void dBgCh_Actr::ClearAllGroundFlags()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x70;
}

// @symbol func_0203573c
extern "C" void func_0203573c(char *self)
{
    ((dBgCh_Actr *)self)->mFlags |= 0x20;
}

// @symbol _ZN10dBgCh_Actr22ClearJustHitGroundFlagEv
void dBgCh_Actr::ClearJustHitGroundFlag()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x20;
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
    *(unsigned int *)((char *)&mFlags) |= 0x10;
}

// @symbol _ZN10dBgCh_Actr15ClearGroundFlagEv
void dBgCh_Actr::ClearGroundFlag()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x10;
}

// @symbol _ZNK10dBgCh_Actr10IsOnGroundEv
/* Returns 0 or 0x10, the mask itself -- no normalisation. */
s32 dBgCh_Actr::IsOnGround() const
{
    return mFlags & 0x10;
}

// @symbol func_020356d4
extern "C" void func_020356d4(char *self)
{
    ((dBgCh_Actr *)self)->mFlags |= 0x40;
}

// @symbol func_020356c8
extern "C" int func_020356c8(int *p)
{
    return p[4] & 64;
}

// @symbol _ZN10dBgCh_Actr13SetLimMovFlagEv
void dBgCh_Actr::SetLimMovFlag()
{
    *(unsigned int *)((char *)&mFlags) |= 0x80;
}

// @symbol _ZN10dBgCh_Actr15ClearLimMovFlagEv
void dBgCh_Actr::ClearLimMovFlag()
{
    *(unsigned int *)((char *)&mFlags) &= ~0x80;
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
extern "C" int func_0203567c(int p)
{
    return p + 48;   /* the query's own dBgPi base, &mSphereClsn + 0x10 */
}

// @symbol _ZNK10dBgCh_Actr14GetFloorResultEv
void *dBgCh_Actr::GetFloorResult() const
{
    return (void *)const_cast<dBgCh_SphCrr &>(mSphereClsn).GetFloorResult();
}

// @symbol _ZNK10dBgCh_Actr13GetWallResultEv
void *dBgCh_Actr::GetWallResult() const
{
    return (void *)(int)const_cast<dBgCh_SphCrr &>(mSphereClsn).GetWallResult();
}

// @symbol func_0203564c
extern "C" int func_0203564c(int p)
{
    return (int)((dBgCh_Actr *)p)->mSphereClsn.GetUnderResult();
}

// @symbol func_02035644
extern "C" void func_02035644(int *p, int v)
{
    p[67] = v;   /* inside mSphereClsn's result block, +0x10c */
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
extern "C" void func_020355fc(char *self)
{
    ((dBgCh_Actr *)self)->mFlags |= 0x400;
}

// @symbol func_020355e8
extern "C" void func_020355e8(char *self)
{
    ((dBgCh_Actr *)self)->mFlags &= ~0x400;
}

// @symbol func_020355dc
extern "C" int func_020355dc(int *p)
{
    return p[4] & 1024;
}

// @symbol func_020355c8
extern "C" void func_020355c8(char *self)
{
    ((dBgCh_Actr *)self)->mFlags &= ~0x800;
}

// @symbol func_020355b4
extern "C" void func_020355b4(char *self)
{
    ((dBgCh_Actr *)self)->mFlags |= 0x800;
}

// @symbol func_020355a0
extern "C" int func_020355a0(int *p)
{
    return (p[4] & 2048) == 0;
}

// @symbol func_0203558c
extern "C" void func_0203558c(char *self)
{
    ((dBgCh_Actr *)self)->mFlags |= 0x1000;
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
extern "C" void func_02035550(char *self)
{
    ((dBgCh_Actr *)self)->mFlags |= 0x4000;
}

// @symbol func_0203553c
extern "C" int func_0203553c(int *p)
{
    return (p[4] & 16384) == 0;
}
