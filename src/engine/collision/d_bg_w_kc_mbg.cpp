//cpp
/* The constructor/destructor order retains all five lifecycle variants used
 * by dBgW_KcMbgSclY. The direction-to-world and movement-filter helpers retain C linkage.
 * SetFile and dBgCh_SphCrr::SetObjAndSphere are scalar Fix12i methods.
 * Flat matrix copies use Mtx43;
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
#include "private/mtx43.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

/* Shared math workers.
   local extern: no header in include/ declares them. */
void Matrix4x3_ApplyInPlaceToScale(Matrix4x3 *m, s32 x, s32 y, s32 z);
void VEC_Normalize(const Vector3 *src, Vector3 *dst);
}

/* The scratch line's lineEnd and dBgPi base (both inside data_020a0d0c,
   below), and the scratch scaled matrix.
   local extern: no header in include/ declares them. */
extern Vector3 data_020a0d60;
extern dBgPi data_020a0d1c;

/* The shared scratch line, in collider space. A namespace-scope dBgCh_Lin,
   so mwcc emits __sinit_d_bg_w_kc_mbg.cpp to construct it and register
   ~dBgCh_Lin() (was the handwritten __sinit_0207501c). dBgW_KcMbgSclY
   borrows it by this address name. */
dBgCh_Lin data_020a0d0c;

/* Flat storage for matrix/vector copies in SetFile and Transform. The shared
 * Matrix4x3 contains a Vector3 with a destructor; copying its words through
 * Mtx43 and this local vector view preserves the retail block copies without
 * changing it.
 */

dBgW_KcMbg::dBgW_KcMbg() {}

dBgW_KcMbg::~dBgW_KcMbg() {}

/* The scratch aggregate holds two complete matrices and the local up axis.
 * Matrix words are copied through Mtx43; a Matrix4x3 assignment
 * scalarizes.
 */
void dBgW_KcMbg::SetFile(KCL_File *file, const Matrix4x3 &mtx, Fix12i scale, s16 angY,
                         CLPS_Block &clps) {
    dBgW_KcMbg *self = this;
    struct Scratch {
        Mtx43 rotation;
        Mtx43 scaled;
        VecFx32 up;
    } scratch;
    self->dBgW_Kc::SetFile(file, clps);
    self->Reset();
    self->scale = scale;
    *(Mtx43 *)&self->mat = *(Mtx43 *)&mtx;
    *(Mtx43 *)&self->invMat = *(Mtx43 *)&self->mat;
    MTX_Inverse43(&self->invMat, &self->invMat);
    self->angY = angY;
    self->angVelY = 0;
    self->pos.x = mtx.t.x;
    self->pos.y = mtx.t.y;
    self->pos.z = mtx.t.z;
    self->velocity.x = self->velocity.y = self->velocity.z = 0;
    scratch.rotation = *(Mtx43 *)&self->mat;
    scratch.rotation.r[3].x = 0;
    scratch.rotation.r[3].y = 0;
    scratch.rotation.r[3].z = 0;
    *(Mtx43 *)&self->invRotMat = scratch.rotation;
    MTX_Inverse43(&self->invRotMat, &self->invRotMat);
    self->invScale = FX_Inv(scale);
    *(Mtx43 *)&data_020a0e68 = *(Mtx43 *)&mtx;
    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, self->scale, self->scale, self->scale);
    scratch.scaled = *(Mtx43 *)&data_020a0e68;
    *(Mtx43 *)&self->newScaledMat = *(Mtx43 *)&data_020a0e68;
    *(Mtx43 *)&self->invScaledMat = scratch.scaled;
    MTX_Inverse43(&self->invScaledMat, &self->invScaledMat);
    *(Mtx43 *)&self->prevInvScaledMat = *(Mtx43 *)&self->invScaledMat;
    *(Mtx43 *)&self->scaledMat = scratch.scaled;
    scratch.up.x = 0;
    scratch.up.y = 0x1000;
    scratch.up.z = 0;
    self->WorldToLocalDirection(*(Vector3 *)&scratch.up, *(Vector3 *)&self->upAxis);
    self->filterByMoveDir = false;
}

void dBgW_KcMbg::Virtual08() {}

void dBgW_KcMbg::Transform(const Matrix4x3 &mtx, s16 angle) {
    *(Mtx43 *)&invMat = *(Mtx43 *)&mat;
    MTX_Inverse43(&invMat, &invMat);

    int zero = 0;
    *(Mtx43 *)&mat = *(const Mtx43 *)&mtx;

    s16 oldAngle = angY;
    angVelY = angle - oldAngle;
    angY = angle;

    velocity.x = mtx.t.x - pos.x;
    velocity.y = mtx.t.y - pos.y;
    velocity.z = mtx.t.z - pos.z;
    pos.x = mtx.t.x;
    pos.y = mtx.t.y;
    pos.z = mtx.t.z;

    Mtx43 rotation = *(Mtx43 *)&mat;
    rotation.r[3].x = zero;
    rotation.r[3].y = zero;
    rotation.r[3].z = zero;
    *(Mtx43 *)&invRotMat = rotation;
    MTX_Inverse43(&invRotMat, &invRotMat);

    *(Mtx43 *)&scaledMat = *(Mtx43 *)&newScaledMat;
    *(Mtx43 *)&data_020a0e68 = *(const Mtx43 *)&mtx;
    s32 currentScale = scale;
    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, currentScale, currentScale, currentScale);
    *(Mtx43 *)&newScaledMat = *(Mtx43 *)&data_020a0e68;
    *(Mtx43 *)&prevInvScaledMat = *(Mtx43 *)&invScaledMat;
    *(Mtx43 *)&invScaledMat = *(Mtx43 *)&newScaledMat;
    MTX_Inverse43(&invScaledMat, &invScaledMat);

    Vector3 up;
    up.x = 0;
    up.y = 0x1000;
    up.z = 0;
    WorldToLocalDirection(up, *(Vector3 *)&upAxis);

    if (!filterByMoveDir)
        return;

    /* Aim dBgW_Kc's depth policy along the horizontal motion: depthByDir on,
       depthDir the direction in collider space, depthDirMinDot its threshold.
       A collider that is not moving sideways sets shallowOnly instead. */
    Vector3 moveDir;
    GetVelocity(moveDir);
    moveDir.y = 0;
    if (VEC_Mag(&moveDir) != 0) {
        SetDepthByDir();
        VEC_Normalize(&moveDir, &moveDir);
        Vector3 localDir;
        WorldToLocalDirection(moveDir, localDir);
        SetDepthDir(&localDir);
        SetDepthDirMinDot(0xe66);
        ClearShallowOnly();
    } else {
        SetShallowOnly();
    }
}

void dBgW_KcMbg::GetNormal(s16 triID, Vector3 &res) {
    KCL_File *mesh = kclFile;
    u16 normalIndex = mesh->tris[triID].normalIdx;
    s16 *packedNormal = mesh->normals[normalIndex];
    Vector3 normal;
    normal.x = packedNormal[0] << 2;
    normal.y = packedNormal[1] << 2;
    normal.z = packedNormal[2] << 2;
    LocalToWorldDirection(normal, res);
}

void dBgW_KcMbg::GetTriangleOrigin(s16 triID, Vector3 &res) {
    KCL_File *mesh = kclFile;
    u16 positionIndex = mesh->tris[triID].posIdx;
    s32 *packedPosition = mesh->positions[positionIndex];
    Vector3 localOrigin;
    localOrigin.x = packedPosition[0] << 6;
    localOrigin.y = packedPosition[1] << 6;
    localOrigin.z = packedPosition[2] << 6;
    LocalToWorldPosition(localOrigin, res);
}

void dBgW_KcMbg::WorldToLocalPosition(const Vector3 &position, Vector3 &result) {
    MTX_MultVec43(const_cast<Vector3 *>(&position), &invScaledMat, &result);
}

void dBgW_KcMbg::LocalToWorldPosition(const Vector3 &position, Vector3 &result) {
    MTX_MultVec43(const_cast<Vector3 *>(&position), &newScaledMat, &result);
}

void dBgW_KcMbg::WorldToLocalDirection(const Vector3 &direction, Vector3 &result) {
    MTX_MultVec43(const_cast<Vector3 *>(&direction), &invRotMat, &result);
}

/* Transform a direction by mat, leaving out the translation. */
void dBgW_KcMbg::LocalToWorldDirection(const Vector3 &direction, Vector3 &result) {
    Matrix4x3 rotation;
    *(Mtx43 *)&rotation = *(Mtx43 *)&mat;
    rotation.t.x = 0;
    rotation.t.y = 0;
    rotation.t.z = 0;
    MTX_MultVec43(const_cast<Vector3 *>(&direction), &rotation, &result);
}

int dBgW_KcMbg::DetectClsn(dBgCh_Gnd &ray) {
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

    WorldToLocalPosition(probePos, localStart);
    WorldToLocalPosition(lineEnd, localEnd);
    data_020a0d0c.SetObjAndLine(localStart, localEnd, 0);
    (dBgCh &)data_020a0d0c = ray;

    int hit = dBgW_Kc::DetectClsn(data_020a0d0c);
    if (hit != 0) {
        LocalToWorldPosition(data_020a0d0c.lineEnd, worldPos);
        (dBgPi &)ray = (data_020a0d0c);
        ray.clsnY = worldPos.y;
        ray.hasClsn = 1;
    }
    return hit;
}


int dBgW_KcMbg::DetectClsn(dBgCh_SphCrr &sphere) {
    Vector3 localCentre;
    /* The local query's push-out bounds, and the same bounds scaled back out. */
    struct {
        VecFx32 localMin, localMax;
        VecFx32 scaledMin, scaledMax;
    } bounds;
    dBgCh_SphCrr localSphere;
    int hit;

    WorldToLocalPosition(sphere.centre, localCentre);
    localSphere.SetObjAndSphere(localCentre, FX_MUL(sphere.radius, invScale), 0);
    localSphere.mWallRejectHalfExtent = FX_MUL(sphere.mWallRejectHalfExtent, invScale);
    localSphere.SetQueryFlags(sphere.flags);
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
        (dBgPi &)sphere = localSphere;
        sphere.flags |= 1;
        if (localSphere.flags & 4) {
            if (sphere.flags & 4) {
                hit &= ~1;
            } else {
                sphere.SetFloorResult(localSphere.mClsnResult1);
            }
            sphere.flags |= 4;
            if (sphere.mFloorNormal.y < localSphere.mFloorNormal.y) {
                sphere.SetFloorNormal(&localSphere.mFloorNormal.x);
            }
        }
        if (localSphere.flags & 8) {
            sphere.SetWallResult(localSphere.mClsnResult2);
            sphere.flags |= 8;
        }
        if (localSphere.flags & 0x10) {
            sphere.SetUnderResult(localSphere.mClsnResult3);
            sphere.flags |= 0x10;
        }
    }
    return hit;
}


int dBgW_KcMbg::DetectClsn(dBgCh_Lin &ray) {
    Vector3 start;
    Vector3 end;
    Vector3 worldPos;

    WorldToLocalPosition(ray.start, start);
    WorldToLocalPosition(ray.lineEnd, end);

    u8 hadClsn = ray.hasClsn;
    data_020a0d0c.SetObjAndLine(start, end, 0);
    if (hadClsn != 0)
        data_020a0d0c.hasClsn = 1;
    (dBgCh &)data_020a0d0c = ray;

    int hit = dBgW_Kc::DetectClsn(data_020a0d0c);
    if (hit != 0) {
        Fix12i distance = data_020a0d0c.clsnDist;
        LocalToWorldPosition(data_020a0d60, worldPos);
        ray.SetClsnPos(worldPos);
        ray.clsnDist = distance;
        (dBgPi &)ray = data_020a0d1c;
        ray.hasClsn = 1;
    }
    return hit;
}

/* Out of the previous frame's space, into the current one. */
int dBgW_KcMbg::TransformPos(const Vector3 &localCentre, Vector3 &res) {
    Vector3 localPos;
    MTX_MultVec43(const_cast<Vector3 *>(&localCentre), &invMat, &localPos);
    MTX_MultVec43(&localPos, &mat, &res);
    return 1;
}

s16 dBgW_KcMbg::GetAngularVelY() {
    return angVelY;
}

void dBgW_KcMbg::GetVelocity(Vector3 &res) {
    res.x = velocity.x;
    res.y = velocity.y;
    res.z = velocity.z;
}

/* Sets filterByMoveDir, which makes Transform aim dBgW_Kc's depth policy
 * along the collider's horizontal motion. Called by actor code (daIwante_c) once
 * the collider starts moving. */
void dBgW_KcMbg::SetFilterByMoveDir() {
    filterByMoveDir = true;
}
