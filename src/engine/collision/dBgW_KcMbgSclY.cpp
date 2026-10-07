//cpp
/* dBgW_KcMbgSclY -- the vertically scaled mesh collider (the Snowman's body,
 * lift chains): a dBgW_KcMbg whose collision space stretches the mesh in y.
 * ROM's own RTTI names it (_ZTS14dBgW_KcMbgSclY @ 0x02099474, _ZTI @
 * 0x02099468, vtable address point 0x02099490).
 *
 * Deferred codegen emits the lifecycle groups in their fixed order -- D0,D1
 * for the destructor, then C1 -- and the plain members land in reverse
 * definition order, so the file defines ctor and dtor first. C2 and D2 have
 * no cartridge home (nothing derives from SclY) and are discarded by the
 * linker. The func_0203aa* trio keep their func_ names: they are the
 * collider-space transform workers the DetectClsn overrides and GetNormal
 * call by raw address, not vtable slots. func_0203aad0 is the scaleY read
 * daObjKm2_Nobiru_c reaches through its own extern.
 */
#include "types.h"
#include "dBgW_KcMbgSclY.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_SphCrr.h"
#include "dBgPi.h"

extern "C" {
/* This TU's own free helpers, called from the members below. */
void func_0203aa10(dBgW_KcMbgSclY *self, Vector3 *v, Vector3 *res);
void func_0203aa74(dBgW_KcMbgSclY *self, Vector3 *v, Vector3 *res);
s32 func_0203aad0(dBgW_KcMbgSclY *self);

/* Shared math workers. */
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *res);
void NormalizeVec3(Vector3 *src, Vector3 *dst);
void func_02039db8(dBgW_KcMbg *self, Vector3 *v, Vector3 *res);
int func_02053200(int v);

/* Collision-query plumbing shared with the KcMbg checkers. */
void func_02035394(void *dst, void *src);

/* local extern: the radius travels by value inside the mangled name
   (wall 6az), so the call spells the symbol rather than going through the
   member. */
void _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(
    dBgCh_SphCrr *sphere, const Vector3 *pos, Fix12i radius, dActor_c *actor);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thiz, void *f, const Matrix4x3 *m, int fix, short s, void *b);

extern dBgCh_Lin data_020a0d0c;   /* the scratch line, in collider space */
extern Vector3   data_020a0d60;   /* == data_020a0d0c.lineEnd  (+0x54) */
extern dBgPi     data_020a0d1c;   /* == data_020a0d0c's dBgPi   (+0x10) */
}

// @symbol _ZN14dBgW_KcMbgSclYC1Ev
dBgW_KcMbgSclY::dBgW_KcMbgSclY()
{
}

// @symbol _ZN14dBgW_KcMbgSclYD0Ev
// @symbol _ZN14dBgW_KcMbgSclYD1Ev
dBgW_KcMbgSclY::~dBgW_KcMbgSclY()
{
}

// @symbol _ZN14dBgW_KcMbgSclY7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block
extern "C" void _ZN14dBgW_KcMbgSclY7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbgSclY *self, KCL_File *f, const Matrix4x3 *m, Fix12i fix, s16 s, CLPS_Block *b)
{
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(self, f, m, fix, s, b);
    self->scaleY = 0x1000;
    self->invScaleY = 0x1000;
}

// @symbol _ZN14dBgW_KcMbgSclY9SetScaleYE5Fix12IiE
extern "C" void _ZN14dBgW_KcMbgSclY9SetScaleYE5Fix12IiE(dBgW_KcMbgSclY *self, Fix12i newScaleY)
{
    self->scaleY = newScaleY;
    if (self->scaleY == 0) return;
    self->invScaleY = func_02053200(self->scaleY);
}

// @symbol func_0203aad0
extern "C" s32 func_0203aad0(dBgW_KcMbgSclY *self)
{
    return self->scaleY;
}

/* World -> collider space: the parent's inverse-scale matrix, then a divide
 * out of the y stretch. */
// @symbol func_0203aa74
extern "C" void func_0203aa74(dBgW_KcMbgSclY *self, Vector3 *v, Vector3 *res)
{
    MulVec3Mat4x3(v, &self->invScaledMat, res);
    Fix12i isy = self->invScaleY;
    if (isy == 0x1000)
        return;
    res->y = (Fix12i)(((s64)res->y * isy + 0x800) >> 12);
}

/* Collider -> world space: fold the y stretch in first, then the parent's
 * scaled matrix. */
// @symbol func_0203aa10
extern "C" void func_0203aa10(dBgW_KcMbgSclY *self, Vector3 *v, Vector3 *res)
{
    Vector3 tmp;
    tmp.x = v->x;
    tmp.y = v->y;
    tmp.z = v->z;
    if (self->scaleY != 0x1000) {
        Fix12i sy = self->scaleY;
        tmp.y = (Fix12i)(((s64)tmp.y * sy + 0x800) >> 12);
    }
    MulVec3Mat4x3(&tmp, &self->newScaledMat, res);
}

// @symbol _ZN14dBgW_KcMbgSclY9Virtual08Ev
void dBgW_KcMbgSclY::Virtual08()
{
}

// @symbol _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Gnd
int dBgW_KcMbgSclY::DetectClsn(dBgCh_Gnd &ground)
{
    Vector3 localStart;
    Vector3 probePos;
    Vector3 localEnd;
    Vector3 lineEnd;

    ground.GetClsnPos(probePos);
    lineEnd = probePos;

    int probeHeight = ground.mProbeHeight;
    if (ground.hasClsn != 0) {
        int distanceToHit = probePos.y - ground.clsnY;
        if (distanceToHit < probeHeight)
            probeHeight = distanceToHit;
    }
    lineEnd.y -= probeHeight;

    func_0203aa74(this, &probePos, &localStart);
    func_0203aa74(this, &lineEnd, &localEnd);

    dBgCh_Lin ray;
    ray.SetObjAndLine(localStart, localEnd, 0);
    func_02035394(&ray, &ground);

    int hit = dBgW_Kc::DetectClsn(ray);
    if (hit != 0) {
        Vector3 clsnPos = ray.GetClsnPos();
        Vector3 worldPos;
        func_0203aa10(this, &clsnPos, &worldPos);
        (dBgPi &)ground = (dBgPi &)ray;
        ground.clsnY = worldPos.y;
        ground.hasClsn = 1;
    }
    return hit;
}

// @symbol _ZN14dBgW_KcMbgSclY10DetectClsnER12dBgCh_SphCrr
#pragma opt_common_subs off

#define FMUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

int dBgW_KcMbgSclY::DetectClsn(dBgCh_SphCrr &sphere)
{
    Vector3 centre;
    Vector3 localCentre;
    int d[12];
    int inverseScale;
    int radius1;
    int radius2;
    int r;

    sphere.GetCentre(centre);
    func_0203aa74(this, &centre, &localCentre);

    inverseScale = invScale;
    radius1 = FMUL(sphere.radius, inverseScale);
    radius2 = FMUL(sphere.unk_0ec, inverseScale);

    dBgCh_SphCrr loc;
    _ZN12dBgCh_SphCrr15SetObjAndSphereERK7Vector35Fix12IiEP8dActor_c(&loc, &localCentre, radius1, 0);
    loc.unk_0ec = radius2;
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
                sphere.SetFloorResult(*loc.GetFloorResult());
            }
            sphere.flags |= 4;
            if (sphere.unk_100 < loc.unk_100) {
                sphere.func_0203794c(&loc.unk_0fc);
            }
        }
        if (loc.flags & 8) {
            sphere.SetWallResult(*loc.GetWallResult());
            sphere.flags |= 8;
        }
        if (loc.flags & 0x10) {
            sphere.SetUnderResult(*loc.GetUnderResult());
            sphere.flags |= 0x10;
        }
    }
    return r;
}

#pragma opt_common_subs on

/* A vertically-scaled mesh collider cannot be tested against a world-space
 * segment directly, so both endpoints are transformed into collider space,
 * the unscaled dBgW_Kc test runs on the scratch line there, and the winning
 * point transforms back out. The qualified dBgW_Kc::DetectClsn call keeps
 * this override from dispatching back into itself. */
// @symbol _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Lin
int dBgW_KcMbgSclY::DetectClsn(dBgCh_Lin &ray)
{
    Vector3 start, end, worldPos;

    func_0203aa74(this, &ray.start, &start);
    func_0203aa74(this, &ray.lineEnd, &end);

    u8 hadClsn = ray.hasClsn;
    data_020a0d0c.SetObjAndLine(start, end, 0);
    if (hadClsn != 0)
        data_020a0d0c.hasClsn = 1;
    func_02035394(&data_020a0d0c, &ray);

    int hit = dBgW_Kc::DetectClsn(data_020a0d0c);
    if (hit != 0) {
        Fix12i dist = data_020a0d0c.clsnDist;
        func_0203aa10(this, &data_020a0d60, &worldPos);
        ray.SetClsnPos(worldPos);
        ray.clsnDist = dist;
        /* the dBgPi base sub-object, at +0x10 */
        (dBgPi &)ray = data_020a0d1c;
        ray.hasClsn = 1;
    }
    return hit;
}

/* The mesh only stretches in y, so the normal's x and z scale by scaleY
 * and the vector is re-normalized. The double scaleY read mirrors the ROM. */
// @symbol _ZN14dBgW_KcMbgSclY9GetNormalEsR7Vector3
void dBgW_KcMbgSclY::GetNormal(s16 triID, Vector3 &res)
{
    KCL_File *p = kclFile;
    u16 n = p->tris[triID].normalIdx;
    s16 *v = p->normals[n];
    Vector3 tmp;
    Fix12i sy;
    tmp.x = v[0] << 2;
    tmp.y = v[1] << 2;
    tmp.z = v[2] << 2;
    sy = scaleY;
    tmp.x = (Fix12i)(((s64)tmp.x * sy + 0x800) >> 12);
    sy = scaleY;
    tmp.z = (Fix12i)(((s64)tmp.z * sy + 0x800) >> 12);
    NormalizeVec3(&tmp, &tmp);
    func_02039db8(this, &tmp, &res);
}
