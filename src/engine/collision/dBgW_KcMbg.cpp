//cpp
/* dBgW_KcMbg -- the KCL mesh collider under a Matrix4x3 transform: the moving
 * counterpart of dBgW_Kc's static mesh, carrying a uniform Fix12 scale. ROM's
 * own RTTI names it (_ZTS10dBgW_KcMbg @ 0x0209941c, _ZTI @ 0x02099410, vtable
 * address point 0x02099434).
 *
 * Deferred codegen emits each lifecycle group in its fixed order -- D2,D0,D1
 * for the destructor and C1,C2 for the constructor -- and the plain members
 * land in reverse definition order, so the file defines ctor and dtor first.
 * All five variants are cartridge-retained: derived dBgW_KcMbgSclY constructs
 * through C2/D2, and the vtable/RTTI ride along since the destructor is the
 * key function. The four func_0203* transform helpers and func_020398fc keep
 * their func_ names -- they are free workers the class's members call by raw
 * address (Eyerok reaches func_020398fc from its own TU), not vtable slots.
 * SetFile and func_02039db8 parse under #pragma cplusplus off because only
 * the C front end emits the 0x30-byte struct block move their copies need.
 */
#include "types.h"
#include "dBgW_KcMbg.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_SphCrr.h"
#include "dBgPi.h"

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
/* local extern: the radius travels by value inside the mangled name
   (wall 6az), so the call spells the symbol rather than going through the
   member. */
void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
    dBgCh_SphCrr *sphere, const Vector3 *pos, Fix12i radius, dActor_c *actor);

extern dBgCh_Lin data_020a0d0c;
extern Vector3 data_020a0d60;
extern dBgPi data_020a0d1c;
extern Matrix4x3 data_020a0e68;
}

/* Matrix4x3 and Vector3 carry the shared, ROM-proven Vector3 destructor.
 * Transform predates those cleanup calls and copies both as flat words, so
 * keep that period codegen local without obscuring the collider's real type. */
struct RawMatrix4x3 {
    s32 m[12];
};
typedef struct RawMatrix4x3 RawMatrix4x3;

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
#pragma cplusplus off
/* The Fix12-by-value signature and its block-move copies keep this on the C
 * front end, exactly as the separate .c shard did. `struct dBgW_KcMbg` in a C
 * region is the class's C spelling; members resolve to their real offsets. */
struct KCL_File;
struct CLPS_Block;
extern void _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block(void *thiz, void *f, void *b);

void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *thiz, void *f, const Matrix4x3 *m, int fix, short s, void *b)
{
  char *c = (char *) thiz;
  struct dBgW_KcMbg *self = (struct dBgW_KcMbg *) thiz;
  int new_var;
  char stk[0x6c];
  _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block(thiz, f, b);
  func_02039624(c);
  self->scale = fix;
  *(RawMatrix4x3 *) &self->mat = *(RawMatrix4x3 *) m;
  *(RawMatrix4x3 *) &self->invMat = *(RawMatrix4x3 *) &self->mat;
  InvMat4x3(&self->invMat, &self->invMat);
  self->angY = s;
  self->angVelY = 0;
  self->pos.x = m->t.x;
  self->pos.y = m->t.y;
  self->pos.z = m->t.z;
  self->velocity.x = (self->velocity.y = (self->velocity.z = 0));
  *((RawMatrix4x3 *) (stk + 0)) = *((RawMatrix4x3 *) (c + 0x54));
  ((Matrix4x3 *) (stk + 0))->t.x = 0;
  ((Matrix4x3 *) (stk + 0))->t.y = 0;
  ((Matrix4x3 *) (stk + 0))->t.z = 0;
  *(RawMatrix4x3 *) &self->invRotMat = *((RawMatrix4x3 *) (stk + 0));
  InvMat4x3(&self->invRotMat, &self->invRotMat);
  self->invScale = (new_var = func_02053200(fix));
  *(RawMatrix4x3 *) &data_020a0e68 = *(RawMatrix4x3 *) m;
  Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, self->scale, self->scale, self->scale);
  *((RawMatrix4x3 *) (stk + 0x30)) = *(RawMatrix4x3 *) &data_020a0e68;
  *(RawMatrix4x3 *) &self->newScaledMat = *(RawMatrix4x3 *) &data_020a0e68;
  *(RawMatrix4x3 *) &self->invScaledMat = *((RawMatrix4x3 *) (stk + 0x30));
  InvMat4x3(&self->invScaledMat, &self->invScaledMat);
  *(RawMatrix4x3 *) &self->prevInvScaledMat = *(RawMatrix4x3 *) &self->invScaledMat;
  *(RawMatrix4x3 *) &self->scaledMat = *((RawMatrix4x3 *) (stk + 0x30));
  *((int *) (stk + 0x60)) = 0;
  *((int *) (stk + 0x64)) = 0x1000;
  *((int *) (stk + 0x68)) = 0;
  func_02039e18(thiz, (Vector3 *) (stk + 0x60), (Vector3 *) (c + 0x28));
  *((unsigned char *) (c + 0x130)) = 0;
}
#pragma cplusplus on

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

    /* The ROM treats the first byte at 0x130 as the enable flag. Keep the
     * shared header's layout-neutral u32 spelling until its 800+ consumers can
     * be migrated as a dedicated header change. */
    if (*(u8 *)&unk_130 == 0) return;

    RawVector3 v;
    GetVelocity(*(Vector3 *)&v);
    v.y = 0;
    if (LenVec3((Vector3 *)&v) != 0) {
        func_01ffb0a4(this);
        NormalizeVec3((Vector3 *)&v, (Vector3 *)&v);
        RawVector3 v2;
        func_02039e18(this, (Vector3 *)&v, (Vector3 *)&v2);
        func_01ffb07c(this, (Vector3 *)&v2);
        func_020396d0((int*)this, 0xe66);
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
#pragma cplusplus off
/* World-to-local transform by `mat` with the translation dropped -- the same
 * block-move copy constraint as SetFile keeps it on the C front end. */
void func_02039db8(struct dBgW_KcMbg *thiz, Vector3 *v, Vector3 *res)
{
    Matrix4x3 m;
    *(RawMatrix4x3 *) &m = *(RawMatrix4x3 *) &thiz->mat;
    m.t.x = 0;
    m.t.y = 0;
    m.t.z = 0;
    MulVec3Mat4x3(v, &m, res);
}
#pragma cplusplus on

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

#define FMUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

int dBgW_KcMbg::DetectClsn(dBgCh_SphCrr &sphere)
{
    Vector3 pos;
    int d[12];
    dBgCh_SphCrr loc;
    int r;

    func_02039e48(this, &sphere.centre, &pos);
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(&loc, &pos,
        FMUL(sphere.radius, invScale), 0);
    loc.unk_0ec = FMUL(sphere.unk_0ec, invScale);
    loc.func_02037940(sphere.flags);
    func_02035394(&loc, &sphere);
    r = dBgW_Kc::DetectClsn(loc);
    if (r) {
        loc.func_02037a04((Vector3 *)d, (Vector3 *)(d + 3));
        d[6] = FMUL(d[0], scale);
        d[7] = FMUL(d[1], scale);
        d[8] = FMUL(d[2], scale);
        d[9] = FMUL(d[3], scale);
        d[10] = FMUL(d[4], scale);
        d[11] = FMUL(d[5], scale);
        sphere.func_02037a6c(d[6], d[7], d[8], d[9], d[10], d[11]);
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
/* Sets the byte at 0x130 -- the enable flag Transform checks before applying
 * velocity feedback. Called by actor code (Eyerok) once the collider starts
 * moving; it is a free worker, not a method, hence the func_ name. */
extern "C" void func_020398fc(dBgW_KcMbg *self)
{
    *(u8 *)&self->unk_130 = 1;
}
