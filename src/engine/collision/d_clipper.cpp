//cpp
/* The view clipper (arm9 .text 0x020150e8..0x0201577c): dClipper culls actor
 * positions against the camera frustum. The lone instance, data_0209f43c in
 * bss, is a file-scope object of d_s_stage_methods.cpp; the camera setup path feeds it the
 * projection parameters through Func_020156DC, and dActor_c::BeforeBehavior
 * queries it per actor.
 *
 * The file is in ROM order under `#pragma defer_codegen off`, which emits
 * each function as it is parsed. ~dClipper is the only virtual (vtable
 * _ZTV8dClipper at 0x0208e730, slots D1/D0) and is inline in dClipper.h;
 * the cartridge stores D0 (0x020156fc) below D1 (0x02015720) with C1
 * (0x02015730) behind them, so two uncalled functions below ask for the
 * destructor's variants in that order before the constructor is written.
 *
 * leftovers:
 * - ClipSphere, TransformAndClip and UpdateFrustumPlanes are names inferred
 *   from their operations.
 * - ClipSphere's opt_propagation off pair, its materialized 0/1
 *   outside/insidePlane locals and its unsigned re-reads of viewPos.y/z are
 *   the matched form; dClipper_EmitDeletingDestructor/EmitDestructor
 *   are uncalled stubs that force D0 then D1 out of the inline
 *   destructor (ROM order).
 */
#pragma defer_codegen off

#include "types.h"
#include "dClipper.h"
#include "math/Matrix.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"
#include "math/MtxStack.h"

/* local extern: no non-generated header declares these vector helpers. */
extern "C" {

void VEC_CrossProduct(const Vector3 *a, const Vector3 *b, Vector3 *result);
void VEC_Normalize(const Vector3 *src, Vector3 *dst);
}

/* Signed distance of (x, y, z) from a frustum side plane, each product rounded to Fix12. */
static inline int PlaneDistance(const ClipperPlane &n, long long x, long long y, long long z) {
    return FX_MUL(z, n.z) + (FX_MUL(x, n.x) + FX_MUL(y, n.y));
}

int dClipper::ClipSphere(Vector3 &viewPos, Fix12i radius, u8 *cachedPlane) {
    if (cachedPlane != 0) {
        unsigned int firstPlane;
        long long y;
        long long x;
        long long z;
        long long wideMargin;
        int depth;
        int one;
        int zero;
        long long firstPlaneDistance;
        int outside;
        unsigned int planeIndex;
        int insidePlane;
        long long planeDistance;

        depth = -viewPos.z;
        if (depth >= this->mNearZ - radius && depth <= this->mFarZ + radius) {
            firstPlane = *cachedPlane & 3;
            x = viewPos.x;
            wideMargin = radius;
            firstPlaneDistance =
                ((long long)viewPos.x * this->mPlaneNormals[firstPlane].x +
                 (long long)viewPos.y * this->mPlaneNormals[firstPlane].y +
                 (long long)viewPos.z * this->mPlaneNormals[firstPlane].z + 0x800) >>
                12;
            /* outside and insidePlane stay materialized 0/1 values: testing firstPlaneDistance <= wideMargin directly
               shrinks it (0x46c), and testing planeDistance > wideMargin in the loop shrinks it to 0x45c */
            outside = wideMargin < firstPlaneDistance;
            if (outside == 0) {
                planeIndex = (firstPlane + 1) & 3;
                /* Re-read through an unsigned view: a plain `y = viewPos.y`
                   reorders the loop's register web (57 words). */
                y = (int)*(unsigned int *)&viewPos.y;
                z = (int)*(unsigned int *)&viewPos.z;
                zero = 0;
                one = 1;
                for (;;) {
                    planeDistance = (z * this->mPlaneNormals[planeIndex].z +
                                     (x * this->mPlaneNormals[planeIndex].x +
                                      y * this->mPlaneNormals[planeIndex].y) +
                                     0x800) >>
                                    12;
                    insidePlane = planeDistance <= wideMargin ? one : zero;
                    if (insidePlane == 0)
                        break;
                    planeIndex = (planeIndex + 1) & 3;
                    if (planeIndex == firstPlane)
                        return depth;
                }
                *cachedPlane = (u8)planeIndex;
            }
        }
        return 0x7FFFFFFF;
    } else {
        // This path rounds each product separately; the cached path rounds the sum.
        int depth;

        depth = -viewPos.z;
        if (depth >= this->mNearZ - radius && depth <= this->mFarZ + radius &&
            FX_MUL(viewPos.z, this->mPlaneNormals[0].z) +
                    (FX_MUL(viewPos.x, this->mPlaneNormals[0].x) +
                     FX_MUL(viewPos.y, this->mPlaneNormals[0].y)) <=
                radius) {
            long long x = viewPos.x, y = viewPos.y, z = viewPos.z;
            if (PlaneDistance(this->mPlaneNormals[1], x, y, z) <= radius &&
                PlaneDistance(this->mPlaneNormals[2], x, y, z) <= radius &&
                PlaneDistance(this->mPlaneNormals[3], x, y, z) <= radius)
                return depth;
        }
        return 0x7FFFFFFF;
    }
}

int dClipper::TransformAndClip(Matrix4x3 &worldToView, Vector3 &worldPos, Fix12i radius,
                               Vector3 &viewPos) {
    MTX_MultVec43(&worldPos, &worldToView, &viewPos);
    return ClipSphere(viewPos, radius, (u8 *)0);
}

/* Rebuild the four view-frustum side planes from the current field of view
 * and near-plane distance.
 *
 * mFovAngle is an angle; its top 12 bits index a sine/cosine pair table at
 * FX_SinCosTable_, and fdiv of that pair gives the tangent. Scaling by mNearZ
 * (the plane distance) gives the half-height, and by mAspectRatio (the
 * aspect ratio) the half-width. The four corner vectors at z = -mNearZ
 * follow, and each adjacent pair crossed and normalised is one side plane's
 * normal.
 */
void dClipper::UpdateFrustumPlanes() {
    int tanHalfFov = FX_Div(FX_SinIdx(mFovAngle >> 4), FX_CosIdx(mFovAngle >> 4));
    Fix12i halfHeight = FX_MUL(mNearZ, tanHalfFov);
    Fix12i halfWidth = FX_MUL(mAspectRatio, halfHeight);
    Vector3 corner0, corner1, corner2, corner3;
    corner0.x = -halfWidth;
    corner0.y = -halfHeight;
    corner0.z = -mNearZ;
    corner1.x = -halfWidth;
    corner1.y = halfHeight;
    corner1.z = -mNearZ;
    corner2.x = halfWidth;
    corner2.y = halfHeight;
    corner2.z = -mNearZ;
    corner3.x = halfWidth;
    corner3.y = -halfHeight;
    corner3.z = -mNearZ;
    VEC_CrossProduct(&corner1, &corner0, (Vector3 *)&mPlaneNormals[0]);
    VEC_CrossProduct(&corner2, &corner1, (Vector3 *)&mPlaneNormals[1]);
    VEC_CrossProduct(&corner3, &corner2, (Vector3 *)&mPlaneNormals[2]);
    VEC_CrossProduct(&corner0, &corner3, (Vector3 *)&mPlaneNormals[3]);
    VEC_Normalize((Vector3 *)&mPlaneNormals[0], (Vector3 *)&mPlaneNormals[0]);
    VEC_Normalize((Vector3 *)&mPlaneNormals[1], (Vector3 *)&mPlaneNormals[1]);
    VEC_Normalize((Vector3 *)&mPlaneNormals[2], (Vector3 *)&mPlaneNormals[2]);
    VEC_Normalize((Vector3 *)&mPlaneNormals[3], (Vector3 *)&mPlaneNormals[3]);
}

/* The former `Ev` spelling claimed this method took no arguments, contradicting
   every call site and the register/stack reads in the ROM. Scalar parameter
   types reproduce both the observed ABI and the complete function bytes. The
   fov angle is signed: both callers pass a short camera angle unconverted
   (Camera_UpdateMatrices loads it with ldrsh), which a u16 parameter would
   re-narrow; the body only stores it, so s16 and u16 compile alike here. */
void dClipper::Func_020156DC(Fix12i aspectRatio, s16 fovAngle, Fix12i nearZ, Fix12i farZ) {
    mAspectRatio = aspectRatio;
    mFovAngle = fovAngle;
    mNearZ = nearZ;
    mFarZ = farZ;
    UpdateFrustumPlanes();
}

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void dClipper_EmitDeletingDestructor(dClipper *p) {
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void dClipper_EmitDestructor(dClipper *p) {
    p->~dClipper();
}

/* The default projection: aspect 4:3 (0x1555), angle 0xe38 (20 degrees,
 * the half-angle UpdateFrustumPlanes takes the tangent of), near plane 1.0 and
 * far plane 5000.0. */
dClipper::dClipper() {
    Func_020156DC(0x1555, 0xe38, 0x1000, 0x1388000);
}
