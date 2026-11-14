//cpp
/* The constructor precedes the destructor definition. C2 and D2 have no
 * cartridge home. ToWorldSpace, ToColliderSpace and GetScaleY are the
 * non-virtual members DetectClsn and GetNormal call by address, and
 * daObjKm2_Nobiru_c reads scaleY through GetScaleY.
 * SetFile and SetScaleY are real methods taking the scale as a scalar
 * Fix12i, which is what the bytes pass. ToColliderSpace multiplies y by
 * invScaleY after the parent's inverse matrix; ToWorldSpace applies scaleY
 * before the parent's scaled matrix.
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
#include "nitro/fx/fx_cp.h"
#include "math/MtxStack.h"

extern "C" {
/* Shared math worker.
   local extern: no header in include/ declares it. */
void VEC_Normalize(const Vector3 *src, Vector3 *dst);

/* local extern: no header in include/ declares these three. */
extern dBgCh_Lin data_020a0d0c; /* the scratch line, in collider space */
extern Vector3 data_020a0d60;   /* == data_020a0d0c.lineEnd  (+0x54) */
extern dBgPi data_020a0d1c;     /* == data_020a0d0c's dBgPi   (+0x10) */
}

dBgW_KcMbgSclY::dBgW_KcMbgSclY() {}

dBgW_KcMbgSclY::~dBgW_KcMbgSclY() {}

void dBgW_KcMbgSclY::SetFile(KCL_File *file, const Matrix4x3 &mat, Fix12i scale, s16 angY,
                             CLPS_Block &clps) {
    dBgW_KcMbg::SetFile(file, mat, scale, angY, clps);
    scaleY = 0x1000;
    invScaleY = 0x1000;
}

void dBgW_KcMbgSclY::SetScaleY(Fix12i newScaleY) {
    scaleY = newScaleY;
    if (scaleY == 0)
        return;
    invScaleY = FX_Inv(scaleY);
}

s32 dBgW_KcMbgSclY::GetScaleY() {
    return scaleY;
}

/* World -> collider space: the parent's inverse-scale matrix, then a divide
 * out of the y stretch. */
void dBgW_KcMbgSclY::ToColliderSpace(Vector3 *worldPos, Vector3 *localPos) {
    MTX_MultVec43(worldPos, &invScaledMat, localPos);
    Fix12i inverseVerticalScale = invScaleY;
    if (inverseVerticalScale == 0x1000)
        return;
    localPos->y = FX_MUL(localPos->y, inverseVerticalScale);
}

/* Collider -> world space: fold the y stretch in first, then the parent's
 * scaled matrix. */
void dBgW_KcMbgSclY::ToWorldSpace(Vector3 *localPos, Vector3 *worldPos) {
    Vector3 scaledPos;
    scaledPos.x = localPos->x;
    scaledPos.y = localPos->y;
    scaledPos.z = localPos->z;
    if (scaleY != 0x1000) {
        Fix12i verticalScale = scaleY;
        scaledPos.y = FX_MUL(scaledPos.y, verticalScale);
    }
    MTX_MultVec43(&scaledPos, &newScaledMat, worldPos);
}

void dBgW_KcMbgSclY::Virtual08() {}

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

    ToColliderSpace(&probePos, &localStart);
    ToColliderSpace(&lineEnd, &localEnd);

    dBgCh_Lin ray;
    ray.SetObjAndLine(localStart, localEnd, 0);
    (dBgCh &)ray = ground;

    int hit = dBgW_Kc::DetectClsn(ray);
    if (hit != 0) {
        Vector3 clsnPos = ray.GetClsnPos();
        Vector3 worldPos;
        ToWorldSpace(&clsnPos, &worldPos);
        (dBgPi &)ground = (dBgPi &)ray;
        ground.clsnY = worldPos.y;
        ground.hasClsn = 1;
    }
    return hit;
}


int dBgW_KcMbgSclY::DetectClsn(dBgCh_SphCrr &sphere) {
    Vector3 centre;
    Vector3 localCentre;
    /* The local query's push-out bounds, and the same bounds scaled back out.
       Plain words: Vector3 declares a destructor. */
    struct {
        int localMin[3], localMax[3];
        VecFx32 scaledMin, scaledMax;
    } bounds;
    int inverseScale;
    int localRadius;
    int localExtraExtent;
    int hit;

    sphere.GetCentre(centre);
    ToColliderSpace(&centre, &localCentre);

    inverseScale = invScale;
    localRadius = FX_MUL(sphere.radius, inverseScale);
    localExtraExtent = FX_MUL(sphere.mWallRejectHalfExtent, inverseScale);

    dBgCh_SphCrr localSphere;
    localSphere.SetObjAndSphere(localCentre, localRadius, 0);
    localSphere.mWallRejectHalfExtent = localExtraExtent;
    localSphere.SetQueryFlags(sphere.flags);
    (dBgCh &)localSphere = sphere;
    hit = dBgW_Kc::DetectClsn(localSphere);
    if (hit) {
        localSphere.GetPushOutBounds((Vector3 *)bounds.localMin, (Vector3 *)bounds.localMax);
        bounds.scaledMin.x = FX_MUL(bounds.localMin[0], scale);
        bounds.scaledMin.y = FX_MUL(bounds.localMin[1], scale);
        bounds.scaledMin.z = FX_MUL(bounds.localMin[2], scale);
        bounds.scaledMax.x = FX_MUL(bounds.localMax[0], scale);
        bounds.scaledMax.y = FX_MUL(bounds.localMax[1], scale);
        bounds.scaledMax.z = FX_MUL(bounds.localMax[2], scale);
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


/* A vertically-scaled mesh collider cannot be tested against a world-space
 * segment directly, so both endpoints are transformed into collider space,
 * the unscaled dBgW_Kc test runs on the scratch line there, and the winning
 * point transforms back out. The qualified dBgW_Kc::DetectClsn call keeps
 * this override from dispatching back into itself. */
int dBgW_KcMbgSclY::DetectClsn(dBgCh_Lin &ray) {
    Vector3 start, end, worldPos;

    ToColliderSpace(&ray.start, &start);
    ToColliderSpace(&ray.lineEnd, &end);

    u8 hadClsn = ray.hasClsn;
    data_020a0d0c.SetObjAndLine(start, end, 0);
    if (hadClsn != 0)
        data_020a0d0c.hasClsn = 1;
    (dBgCh &)data_020a0d0c = ray;

    int hit = dBgW_Kc::DetectClsn(data_020a0d0c);
    if (hit != 0) {
        Fix12i dist = data_020a0d0c.clsnDist;
        ToWorldSpace(&data_020a0d60, &worldPos);
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
    VEC_Normalize(&normal, &normal);
    LocalToWorldDirection(normal, res);
}
