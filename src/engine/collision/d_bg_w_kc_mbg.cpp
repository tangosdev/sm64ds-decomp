//cpp
/* The constructor/destructor order retains all five lifecycle variants used
 * by dBgW_KcMbgSclY. The func_02039* transform workers retain C linkage.
 * SetFile and SetObjAndSphere retain scalar ABI boundaries for the shared
 * Fix12<int>-by-value signatures. Flat matrix copies use RawMatrix4x3;
 * the shared Matrix4x3 copy changes the retail instruction sequence.
 */
/* dBgW_KcMbg: KCL mesh under a Matrix4x3, with a uniform scale.
 * Type name 0x0209941c, typeinfo 0x02099410, vtable 0x02099434.
 */
#include "types.h"
#include "dBgW_KcMbg.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_SphCrr.h"
#include "dBgPi.h"
#include "nitro/fx/fx.h"

extern "C" {
/* This TU's own free helpers, called from the members below. */
void func_020398fc(dBgW_KcMbg *self);
void func_02039db8(dBgW_KcMbg *thiz, Vector3 *v, Vector3 *res);
void func_02039e18(dBgW_KcMbg *self, Vector3 *v, Vector3 *res);
void func_02039e30(dBgW_KcMbg *self, Vector3 *v, Vector3 *res);
void func_02039e48(dBgW_KcMbg *self, Vector3 *v, Vector3 *res);

/* Shared math workers. */
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *res);
int InvMat4x3(void *dst, void *src);
void Matrix4x3_ApplyInPlaceToScale(Matrix4x3 *m, s32 x, s32 y, s32 z);
int LenVec3(void *v);
void NormalizeVec3(void *src, void *dst);
void func_02039624(char *p);
int func_02053200(int v);

/* dBgW_Kc's ITCM workers, run on `this` through base inheritance. */
void func_01ffb0a4(dBgW_Kc *self);
void func_01ffb07c(dBgW_Kc *self, const Vector3 *v);
void func_01ffb0b0(dBgW_Kc *self);
void func_01ffb0bc(dBgW_Kc *self);
void func_020396d0(int *p, int v);

/* Collision-query plumbing shared with the SphCrr/Gnd/Lin checkers. */
void func_02035394(void *dst, void *src);
/* local extern: the radius is Fix12 by value, so the call spells the symbol. */
void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
    dBgCh_SphCrr *sphere, const Vector3 *pos, Fix12i radius, dActor_c *actor);

extern dBgCh_Lin data_020a0d0c;
extern Vector3 data_020a0d60;
extern dBgPi data_020a0d1c;
extern Matrix4x3 data_020a0e68;
}

/* Flat storage for matrix/vector copies in SetFile and Transform. The shared
 * Matrix4x3 contains a Vector3 with a destructor; copying its words through
 * this local view preserves the retail block copies without changing it.
 */
struct RawMatrix4x3 {
    s32 m[12];
};

struct RawVector3 {
    s32 x, y, z;
};

// @symbol _ZN10dBgW_KcMbgC1Ev
// @symbol _ZN10dBgW_KcMbgC2Ev
dBgW_KcMbg::dBgW_KcMbg()
{
}

// @symbol _ZN10dBgW_KcMbgD2Ev
// @symbol _ZN10dBgW_KcMbgD0Ev
// @symbol _ZN10dBgW_KcMbgD1Ev
dBgW_KcMbg::~dBgW_KcMbg()
{
}

// @symbol _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block
/* Scalar ABI boundary: the shared SetFile declaration takes Fix12<int> by
 * value. This C++ body retains its existing scalar entry and C symbol.
 * The scratch aggregate holds two complete matrices and the local up axis.
 */
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thiz, void *file, const Matrix4x3 *mtx, int scale, short angY, void *clps) {
    dBgW_KcMbg *self = (dBgW_KcMbg *)thiz;
    struct Scratch {
        RawMatrix4x3 rotation;
        RawMatrix4x3 scaled;
        RawVector3 up;
    } scratch;
    self->dBgW_Kc::SetFile((KCL_File *)file, *(CLPS_Block *)clps);
    func_02039624((char *)thiz);
    self->scale = scale;
    *(RawMatrix4x3 *)&self->mat = *(RawMatrix4x3 *)mtx;
    *(RawMatrix4x3 *)&self->invMat = *(RawMatrix4x3 *)&self->mat;
    InvMat4x3(&self->invMat, &self->invMat);
    self->angY = angY;
    self->angVelY = 0;
    self->pos.x = mtx->t.x;
    self->pos.y = mtx->t.y;
    self->pos.z = mtx->t.z;
    self->velocity.x = self->velocity.y = self->velocity.z = 0;
    scratch.rotation = *(RawMatrix4x3 *)&self->mat;
    scratch.rotation.m[9] = 0;
    scratch.rotation.m[10] = 0;
    scratch.rotation.m[11] = 0;
    *(RawMatrix4x3 *)&self->invRotMat = scratch.rotation;
    InvMat4x3(&self->invRotMat, &self->invRotMat);
    self->invScale = func_02053200(scale);
    *(RawMatrix4x3 *)&data_020a0e68 = *(RawMatrix4x3 *)mtx;
    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, self->scale, self->scale, self->scale);
    scratch.scaled = *(RawMatrix4x3 *)&data_020a0e68;
    *(RawMatrix4x3 *)&self->newScaledMat = *(RawMatrix4x3 *)&data_020a0e68;
    *(RawMatrix4x3 *)&self->invScaledMat = scratch.scaled;
    InvMat4x3(&self->invScaledMat, &self->invScaledMat);
    *(RawMatrix4x3 *)&self->prevInvScaledMat = *(RawMatrix4x3 *)&self->invScaledMat;
    *(RawMatrix4x3 *)&self->scaledMat = scratch.scaled;
    scratch.up.x = 0;
    scratch.up.y = 0x1000;
    scratch.up.z = 0;
    func_02039e18(self, (Vector3 *)&scratch.up, (Vector3 *)&self->unk_28);
    self->filterByMoveDir = false;
}

// @symbol _ZN10dBgW_KcMbg9Virtual08Ev
void dBgW_KcMbg::Virtual08()
{
}

// @symbol _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s
void dBgW_KcMbg::Transform(const Matrix4x3 &mtx, s16 angle)
{
    *(RawMatrix4x3 *)&invMat = *(RawMatrix4x3 *)&mat;
    InvMat4x3(&invMat, &invMat);

    int zero = 0;
    *(RawMatrix4x3 *)&mat = *(const RawMatrix4x3 *)&mtx;

    s16 oldAngle = angY;
    angVelY = angle - oldAngle;
    angY = angle;

    velocity.x = mtx.t.x - pos.x;
    velocity.y = mtx.t.y - pos.y;
    velocity.z = mtx.t.z - pos.z;
    pos.x = mtx.t.x;
    pos.y = mtx.t.y;
    pos.z = mtx.t.z;

    RawMatrix4x3 rotation = *(RawMatrix4x3 *)&mat;
    rotation.m[9] = zero;
    rotation.m[10] = zero;
    rotation.m[11] = zero;
    *(RawMatrix4x3 *)&invRotMat = rotation;
    InvMat4x3(&invRotMat, &invRotMat);

    *(RawMatrix4x3 *)&scaledMat = *(RawMatrix4x3 *)&newScaledMat;
    *(RawMatrix4x3 *)&data_020a0e68 = *(const RawMatrix4x3 *)&mtx;
    s32 currentScale = scale;
    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, currentScale, currentScale, currentScale);
    *(RawMatrix4x3 *)&newScaledMat = *(RawMatrix4x3 *)&data_020a0e68;
    *(RawMatrix4x3 *)&prevInvScaledMat = *(RawMatrix4x3 *)&invScaledMat;
    *(RawMatrix4x3 *)&invScaledMat = *(RawMatrix4x3 *)&newScaledMat;
    InvMat4x3(&invScaledMat, &invScaledMat);

    RawVector3 up;
    up.x = 0; up.y = 0x1000; up.z = 0;
    func_02039e18(this, (Vector3 *)&up, (Vector3 *)&unk_28);

    if (!filterByMoveDir)
        return;

    /* Aim dBgW_Kc's face filter along the horizontal motion: unk_35 on,
       unk_38 the direction in collider space, unk_44 its threshold. A
       collider that is not moving sideways sets unk_34 instead. */
    RawVector3 moveDir;
    GetVelocity(*(Vector3 *)&moveDir);
    moveDir.y = 0;
    if (LenVec3((Vector3 *)&moveDir) != 0) {
        func_01ffb0a4(this);
        NormalizeVec3((Vector3 *)&moveDir, (Vector3 *)&moveDir);
        RawVector3 localDir;
        func_02039e18(this, (Vector3 *)&moveDir, (Vector3 *)&localDir);
        func_01ffb07c(this, (Vector3 *)&localDir);
        func_020396d0((int *)this, 0xe66);
        func_01ffb0b0(this);
    } else {
        func_01ffb0bc(this);
    }
}

// @symbol _ZN10dBgW_KcMbg9GetNormalEsR7Vector3
void dBgW_KcMbg::GetNormal(s16 triID, Vector3 &res)
{
    KCL_File *p = kclFile;
    u16 n = p->tris[triID].normalIdx;
    s16 *v = p->normals[n];
    Vector3 tmp;
    tmp.x = v[0] << 2;
    tmp.y = v[1] << 2;
    tmp.z = v[2] << 2;
    func_02039db8(this, &tmp, &res);
}

// @symbol _ZN10dBgW_KcMbg17GetTriangleOriginEsR7Vector3
void dBgW_KcMbg::GetTriangleOrigin(s16 triID, Vector3 &res)
{
    KCL_File *p = kclFile;
    u16 n = p->tris[triID].posIdx;
    s32 *v = p->positions[n];
    Vector3 tmp;
    tmp.x = v[0] << 6;
    tmp.y = v[1] << 6;
    tmp.z = v[2] << 6;
    func_02039e30(this, &tmp, &res);
}

// @symbol func_02039e48
extern "C" void func_02039e48(dBgW_KcMbg *self, Vector3 *v, Vector3 *res)
{
    MulVec3Mat4x3(v, &self->invScaledMat, res);
}

// @symbol func_02039e30
extern "C" void func_02039e30(dBgW_KcMbg *self, Vector3 *v, Vector3 *res)
{
    MulVec3Mat4x3(v, &self->newScaledMat, res);
}

// @symbol func_02039e18
extern "C" void func_02039e18(dBgW_KcMbg *self, Vector3 *v, Vector3 *res)
{
    MulVec3Mat4x3(v, &self->invRotMat, res);
}

// @symbol func_02039db8
/* Transform v by `mat` with the translation dropped, into res. */
extern "C" void func_02039db8(dBgW_KcMbg *thiz, Vector3 *v, Vector3 *res)
{
    Matrix4x3 m;
    *(RawMatrix4x3 *)&m = *(RawMatrix4x3 *)&thiz->mat;
    m.t.x = 0;
    m.t.y = 0;
    m.t.z = 0;
    MulVec3Mat4x3(v, &m, res);
}

// @symbol _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Gnd
int dBgW_KcMbg::DetectClsn(dBgCh_Gnd &ray)
{
    Vector3 localStart;
    Vector3 probePos;
    Vector3 localEnd;
    Vector3 lineEnd;
    Vector3 worldPos;

    ray.GetClsnPos(probePos);
    lineEnd = probePos;

    int probeHeight = ray.mProbeHeight;
    if (ray.hasClsn != 0) {
        int distanceToHit = probePos.y - ray.clsnY;
        if (distanceToHit < probeHeight)
            probeHeight = distanceToHit;
    }
    lineEnd.y -= probeHeight;

    func_02039e48(this, &probePos, &localStart);
    func_02039e48(this, &lineEnd, &localEnd);
    data_020a0d0c.SetObjAndLine(localStart, localEnd, 0);
    func_02035394(&data_020a0d0c, &ray);

    int hit = dBgW_Kc::DetectClsn(data_020a0d0c);
    if (hit != 0) {
        func_02039e30(this, &data_020a0d0c.lineEnd, &worldPos);
        (dBgPi &)ray = static_cast<dBgPi &>(data_020a0d0c);
        ray.clsnY = worldPos.y;
        ray.hasClsn = 1;
    }
    return hit;
}

// @symbol _ZN10dBgW_KcMbg10DetectClsnER12dBgCh_SphCrr
#pragma opt_common_subs off


int dBgW_KcMbg::DetectClsn(dBgCh_SphCrr &sphere)
{
    Vector3 pos;
    /* The local query's broad-phase box, and the same box scaled back out. */
    struct {
        RawVector3 localMin, localMax;
        RawVector3 worldMin, worldMax;
    } box;
    dBgCh_SphCrr loc;
    int r;

    func_02039e48(this, &sphere.centre, &pos);
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
        &loc, &pos, FX_MUL(sphere.radius, invScale), 0);
    loc.unk_0ec = FX_MUL(sphere.unk_0ec, invScale);
    loc.func_02037940(sphere.flags);
    func_02035394(&loc, &sphere);
    r = dBgW_Kc::DetectClsn(loc);
    if (r) {
        loc.func_02037a04((Vector3 *)&box.localMin, (Vector3 *)&box.localMax);
        box.worldMin.x = FX_MUL(box.localMin.x, scale);
        box.worldMin.y = FX_MUL(box.localMin.y, scale);
        box.worldMin.z = FX_MUL(box.localMin.z, scale);
        box.worldMax.x = FX_MUL(box.localMax.x, scale);
        box.worldMax.y = FX_MUL(box.localMax.y, scale);
        box.worldMax.z = FX_MUL(box.localMax.z, scale);
        sphere.func_02037a6c(box.worldMin.x, box.worldMin.y, box.worldMin.z, box.worldMax.x,
                             box.worldMax.y, box.worldMax.z);
        (dBgPi &)sphere = (dBgPi &)loc;
        sphere.flags |= 1;
        if (loc.flags & 4) {
            if (sphere.flags & 4) {
                r &= ~1;
            } else {
                sphere.SetFloorResult(loc.mClsnResult1);
            }
            sphere.flags |= 4;
            if (sphere.unk_100 < loc.unk_100) {
                sphere.func_0203794c(&loc.unk_0fc);
            }
        }
        if (loc.flags & 8) {
            sphere.SetWallResult(loc.mClsnResult2);
            sphere.flags |= 8;
        }
        if (loc.flags & 0x10) {
            sphere.SetUnderResult(loc.mClsnResult3);
            sphere.flags |= 0x10;
        }
    }
    return r;
}

#pragma opt_common_subs on

// @symbol _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Lin
int dBgW_KcMbg::DetectClsn(dBgCh_Lin &ray)
{
    Vector3 start;
    Vector3 end;
    Vector3 worldPos;

    func_02039e48(this, &ray.start, &start);
    func_02039e48(this, &ray.lineEnd, &end);

    u8 hadClsn = ray.hasClsn;
    data_020a0d0c.SetObjAndLine(start, end, 0);
    if (hadClsn != 0)
        data_020a0d0c.hasClsn = 1;
    func_02035394(&data_020a0d0c, &ray);

    int hit = dBgW_Kc::DetectClsn(data_020a0d0c);
    if (hit != 0) {
        Fix12i distance = data_020a0d0c.clsnDist;
        func_02039e30(this, &data_020a0d60, &worldPos);
        ray.SetClsnPos(worldPos);
        ray.clsnDist = distance;
        (dBgPi &)ray = data_020a0d1c;
        ray.hasClsn = 1;
    }
    return hit;
}

// @symbol _ZN10dBgW_KcMbg12TransformPosERK7Vector3RS0_
/* Out of the previous frame's space, into the current one. */
int dBgW_KcMbg::TransformPos(const Vector3 &pos, Vector3 &res)
{
    Vector3 tmp;
    MulVec3Mat4x3((Vector3 *)&pos, &invMat, &tmp);
    MulVec3Mat4x3(&tmp, &mat, &res);
    return 1;
}

// @symbol _ZN10dBgW_KcMbg14GetAngularVelYEv
s16 dBgW_KcMbg::GetAngularVelY()
{
    return angVelY;
}

// @symbol _ZN10dBgW_KcMbg11GetVelocityER7Vector3
void dBgW_KcMbg::GetVelocity(Vector3 &res)
{
    res.x = velocity.x;
    res.y = velocity.y;
    res.z = velocity.z;
}

// @symbol func_020398fc
/* Turns on filterByMoveDir. Called by actor code (Eyerok) once the collider
 * starts moving; it is a free worker, not a method, hence the func_ name. */
extern "C" void func_020398fc(dBgW_KcMbg *self)
{
    self->filterByMoveDir = true;
}
