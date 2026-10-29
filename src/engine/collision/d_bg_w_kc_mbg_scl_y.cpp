//cpp
/* The constructor precedes the destructor definition. C2 and D2 have no
 * cartridge home. func_0203aa10, func_0203aa74 and func_0203aad0 retain
 * their address names and C linkage; DetectClsn and GetNormal call them
 * by address, and daObjKm2_Nobiru_c reads scaleY through func_0203aad0.
 * SetFile and SetScaleY are real methods taking the scale as a scalar
 * Fix12i, which is what the bytes pass. func_0203aa74 multiplies y by invScaleY after the parent's inverse
 * matrix; func_0203aa10 applies scaleY before the parent's scaled matrix.
 */
/* dBgW_KcMbgSclY: mesh collider stretched in y (Snowman's body, lift
 * chains). Type name 0x02099474, typeinfo 0x02099468, vtable 0x02099490.
 */
#include "types.h"
#include "dBgW_KcMbgSclY.h"
#include "dBgCh_Lin.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_SphCrr.h"
#include "dBgPi.h"
#include "nitro/fx/fx.h"

extern "C" {
/* This TU's own free helpers, called from the members below. */
void func_0203aa10(dBgW_KcMbgSclY *self, Vector3 *v, Vector3 *res);
void func_0203aa74(dBgW_KcMbgSclY *self, Vector3 *v, Vector3 *res);
s32 func_0203aad0(dBgW_KcMbgSclY *self);

/* Shared math workers.
   local extern: no header in include/ declares them. */
void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *res);
void NormalizeVec3(Vector3 *src, Vector3 *dst);
int func_02053200(int v);
/* local extern: dBgW_KcMbg's direction rotate, a free function of
   d_bg_w_kc_mbg.cpp that no header declares. */
void func_02039db8(dBgW_KcMbg *self, Vector3 *v, Vector3 *res);

/* local extern: no header in include/ declares these three. */
extern dBgCh_Lin data_020a0d0c; /* the scratch line, in collider space */
extern Vector3 data_020a0d60;   /* == data_020a0d0c.lineEnd  (+0x54) */
extern dBgPi data_020a0d1c;     /* == data_020a0d0c's dBgPi   (+0x10) */
}

// @symbol _ZN14dBgW_KcMbgSclYC1Ev
dBgW_KcMbgSclY::dBgW_KcMbgSclY() {}

// @symbol _ZN14dBgW_KcMbgSclYD0Ev
// @symbol _ZN14dBgW_KcMbgSclYD1Ev
dBgW_KcMbgSclY::~dBgW_KcMbgSclY() {}

// @symbol _ZN14dBgW_KcMbgSclY7SetFileEP8KCL_FileRK9Matrix4x3isR10CLPS_Block
void dBgW_KcMbgSclY::SetFile(KCL_File *file, const Matrix4x3 &mat, Fix12i scale, s16 angY,
                             CLPS_Block &clps) {
    dBgW_KcMbg::SetFile(file, mat, scale, angY, clps);
    scaleY = 0x1000;
    invScaleY = 0x1000;
}

// @symbol _ZN14dBgW_KcMbgSclY9SetScaleYEi
void dBgW_KcMbgSclY::SetScaleY(Fix12i newScaleY) {
    scaleY = newScaleY;
    if (scaleY == 0)
        return;
    invScaleY = func_02053200(scaleY);
}

// @symbol func_0203aad0
extern "C" s32 func_0203aad0(dBgW_KcMbgSclY *self) {
    return self->scaleY;
}

/* World -> collider space: the parent's inverse-scale matrix, then a divide
 * out of the y stretch. */
// @symbol func_0203aa74
extern "C" void func_0203aa74(dBgW_KcMbgSclY *self, Vector3 *worldPos, Vector3 *localPos) {
    MulVec3Mat4x3(worldPos, &self->invScaledMat, localPos);
    Fix12i inverseVerticalScale = self->invScaleY;
    if (inverseVerticalScale == 0x1000)
        return;
    localPos->y = FX_MUL(localPos->y, inverseVerticalScale);
}

/* Collider -> world space: fold the y stretch in first, then the parent's
 * scaled matrix. */
// @symbol func_0203aa10
extern "C" void func_0203aa10(dBgW_KcMbgSclY *self, Vector3 *localPos, Vector3 *worldPos) {
    Vector3 scaledPos;
    scaledPos.x = localPos->x;
    scaledPos.y = localPos->y;
    scaledPos.z = localPos->z;
    if (self->scaleY != 0x1000) {
        Fix12i verticalScale = self->scaleY;
        scaledPos.y = FX_MUL(scaledPos.y, verticalScale);
    }
    MulVec3Mat4x3(&scaledPos, &self->newScaledMat, worldPos);
}

// @symbol _ZN14dBgW_KcMbgSclY9Virtual08Ev
void dBgW_KcMbgSclY::Virtual08() {}

// @symbol _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Gnd
int dBgW_KcMbgSclY::DetectClsn(dBgCh_Gnd &ground) {
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
    (dBgCh &)ray = ground;

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

int dBgW_KcMbgSclY::DetectClsn(dBgCh_SphCrr &sphere) {
    Vector3 centre;
    Vector3 localCentre;
    /* The local query's push-out bounds, and the same bounds scaled back out.
       Plain words: Vector3 declares a destructor. */
    struct BoxCorner {
        s32 x, y, z;
    };
    struct {
        BoxCorner localMin, localMax;
        BoxCorner scaledMin, scaledMax;
    } bounds;
    int inverseScale;
    int localRadius;
    int localExtraExtent;
    int hit;

    sphere.GetCentre(centre);
    func_0203aa74(this, &centre, &localCentre);

    inverseScale = invScale;
    localRadius = FX_MUL(sphere.radius, inverseScale);
    localExtraExtent = FX_MUL(sphere.mWallRejectHalfExtent, inverseScale);

    dBgCh_SphCrr localSphere;
    localSphere.SetObjAndSphere(localCentre, localRadius, 0);
    localSphere.mWallRejectHalfExtent = localExtraExtent;
    localSphere.func_02037940(sphere.flags);
    (dBgCh &)localSphere = sphere;
    hit = dBgW_Kc::DetectClsn(localSphere);
    if (hit) {
        localSphere.GetPushOutBounds((Vector3 *)&bounds.localMin, (Vector3 *)&bounds.localMax);
        bounds.scaledMin.x = FX_MUL(bounds.localMin.x, scale);
        bounds.scaledMin.y = FX_MUL(bounds.localMin.y, scale);
        bounds.scaledMin.z = FX_MUL(bounds.localMin.z, scale);
        bounds.scaledMax.x = FX_MUL(bounds.localMax.x, scale);
        bounds.scaledMax.y = FX_MUL(bounds.localMax.y, scale);
        bounds.scaledMax.z = FX_MUL(bounds.localMax.z, scale);
        sphere.ExpandPushOutBounds(bounds.scaledMin.x, bounds.scaledMin.y, bounds.scaledMin.z,
                                   bounds.scaledMax.x, bounds.scaledMax.y, bounds.scaledMax.z);
        (dBgPi &)sphere = (dBgPi &)localSphere;
        sphere.flags |= 1;
        if (localSphere.flags & 4) {
            if (sphere.flags & 4) {
                hit &= ~1;
            } else {
                sphere.SetFloorResult(*localSphere.GetFloorResult());
            }
            sphere.flags |= 4;
            if (sphere.mFloorNormal.y < localSphere.mFloorNormal.y) {
                sphere.SetFloorNormal(&localSphere.mFloorNormal.x);
            }
        }
        if (localSphere.flags & 8) {
            sphere.SetWallResult(*localSphere.GetWallResult());
            sphere.flags |= 8;
        }
        if (localSphere.flags & 0x10) {
            sphere.SetUnderResult(*localSphere.GetUnderResult());
            sphere.flags |= 0x10;
        }
    }
    return hit;
}

#pragma opt_common_subs on

/* A vertically-scaled mesh collider cannot be tested against a world-space
 * segment directly, so both endpoints are transformed into collider space,
 * the unscaled dBgW_Kc test runs on the scratch line there, and the winning
 * point transforms back out. The qualified dBgW_Kc::DetectClsn call keeps
 * this override from dispatching back into itself. */
// @symbol _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Lin
int dBgW_KcMbgSclY::DetectClsn(dBgCh_Lin &ray) {
    Vector3 start, end, worldPos;

    func_0203aa74(this, &ray.start, &start);
    func_0203aa74(this, &ray.lineEnd, &end);

    u8 hadClsn = ray.hasClsn;
    data_020a0d0c.SetObjAndLine(start, end, 0);
    if (hadClsn != 0)
        data_020a0d0c.hasClsn = 1;
    (dBgCh &)data_020a0d0c = ray;

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
void dBgW_KcMbgSclY::GetNormal(s16 triID, Vector3 &res) {
    KCL_File *mesh = kclFile;
    u16 normalIndex = mesh->tris[triID].normalIdx;
    s16 *packedNormal = mesh->normals[normalIndex];
    Vector3 normal;
    Fix12i verticalScale;
    normal.x = packedNormal[0] << 2;
    normal.y = packedNormal[1] << 2;
    normal.z = packedNormal[2] << 2;
    verticalScale = scaleY;
    normal.x = FX_MUL(normal.x, verticalScale);
    verticalScale = scaleY;
    normal.z = FX_MUL(normal.z, verticalScale);
    NormalizeVec3(&normal, &normal);
    func_02039db8(this, &normal, &res);
}
