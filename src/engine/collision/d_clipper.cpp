//cpp
/* The view clipper (arm9 .text 0x020150e8..0x0201577c): dClipper culls actor
 * positions against the camera frustum. The lone instance, data_0209f43c in
 * bss, is constructed by __sinit_02074e84; the camera setup path feeds it the
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
 * - ClipSphere and TransformAndClip are names inferred from their operations.
 * - ClipSphere's opt_propagation off pair and its materialized 0/1
 *   outside/ok locals are the matched form; dClipper_EmitDeletingDestructor/EmitDestructor
 *   are uncalled stubs that force D0 then D1 out of the inline
 *   destructor (ROM order).
 */
#pragma defer_codegen off

#include "types.h"
#include "dClipper.h"
#include "math/Matrix.h"
#include "nitro/fx/fx.h"

extern "C" {
void MulVec3Mat4x3(const Vector3 *v, const Matrix4x3 *m, Vector3 *res);
void CrossVec3(Vector3 *a, Vector3 *b, Vector3 *result);
void NormalizeVec3(Vector3 *src, Vector3 *dst);
int _ZN4cstd4fdivEii(int a, int b);
}
extern short data_02082214[];

/* Signed distance of (x, y, z) from a frustum side plane, each product rounded to Fix12. */
static inline int PlaneDistance(const ClipperPlane &n, long long x, long long y, long long z) {
    return (int)((z * n.z + 0x800) >> 12) +
           ((int)((x * n.x + 0x800) >> 12) + (int)((y * n.y + 0x800) >> 12));
}

#pragma opt_propagation off
// @symbol _ZN8dClipper10ClipSphereER7Vector3iPh
int dClipper::ClipSphere(Vector3 &v, Fix12i radius, u8 *hint) {
    if (hint != 0)
    {
        unsigned int first;
        long long y;
        long long x;
        long long z;
        long long c;
        int negZ;
        int one;
        int zero;
        long long dot;
        int outside;
        unsigned int i;
        int ok;
        long long d;

        negZ = -v.z;
        if (negZ >= this->mNearZ - radius && negZ <= this->mFarZ + radius) {
            first = *hint & 3;
            x = v.x;
            c = radius;
            dot = ((long long)v.x * this->mPlaneNormals[first].x +
                   (long long)v.y * this->mPlaneNormals[first].y +
                   (long long)v.z * this->mPlaneNormals[first].z + 0x800) >>
                  12;
            /* outside and ok stay materialized 0/1 values: testing dot <= c directly
               shrinks it (0x46c), and testing d > c in the loop shrinks it to 0x45c */
            outside = c < dot;
            if (outside == 0) {
                i = (first + 1) & 3;
                y = (int)*(unsigned int *)&v.y;
                z = (int)*(unsigned int *)&v.z;
                zero = 0;
                one = 1;
                for (;;) {
                    d = (z * this->mPlaneNormals[i].z +
                         (x * this->mPlaneNormals[i].x + y * this->mPlaneNormals[i].y) + 0x800) >>
                        12;
                    ok = d <= c ? one : zero;
                    if (ok == 0)
                        break;
                    i = (i + 1) & 3;
                    if (i == first)
                        return negZ;
                }
                *hint = (u8)i;
            }
        }
        return 0x7FFFFFFF;
    }
    else
    {
        int negZ;

        negZ = -v.z;
        if (negZ >= this->mNearZ - radius && negZ <= this->mFarZ + radius &&
            FX_MUL(v.z, this->mPlaneNormals[0].z) + (FX_MUL(v.x, this->mPlaneNormals[0].x) +
                                                     FX_MUL(v.y, this->mPlaneNormals[0].y)) <=
                radius) {
            long long x = v.x, y = v.y, z = v.z;
            if (PlaneDistance(this->mPlaneNormals[1], x, y, z) <= radius &&
                PlaneDistance(this->mPlaneNormals[2], x, y, z) <= radius &&
                PlaneDistance(this->mPlaneNormals[3], x, y, z) <= radius)
                return negZ;
        }
        return 0x7FFFFFFF;
    }
}
#pragma opt_propagation on

// @symbol _ZN8dClipper16TransformAndClipER9Matrix4x3R7Vector3iS3_
int dClipper::TransformAndClip(Matrix4x3 &mat, Vector3 &srcVec, Fix12i radius, Vector3 &dstVec) {
    MulVec3Mat4x3(&srcVec, &mat, &dstVec);
    return ClipSphere(dstVec, radius, (u8 *)0);
}

// @symbol _ZN8dClipper13Func_0201559CEv
/* Rebuild the four view-frustum side planes from the current field of view
 * and near-plane distance.
 *
 * mFovAngle is an angle; its top 12 bits index a sine/cosine pair table at
 * data_02082214, and fdiv of that pair gives the tangent. Scaling by mNearZ
 * (the plane distance) gives the half-height b, and by mAspectRatio (the
 * aspect ratio) the half-width c. The four corner vectors at z = -mNearZ
 * follow, and each adjacent pair crossed and normalised is one side plane's
 * normal.
 */
void dClipper::Func_0201559C()
{
    int idx = (int)mFovAngle >> 4;
    int q = _ZN4cstd4fdivEii(data_02082214[2 * idx], data_02082214[2 * idx + 1]);
    Fix12i b = FX_MUL(mNearZ, q);
    Fix12i c = FX_MUL(mAspectRatio, b);
    Vector3 v0, v1, v2, v3;
    v0.x = -c; v0.y = -b; v0.z = -mNearZ;
    v1.x = -c; v1.y = b;  v1.z = -mNearZ;
    v2.x = c;  v2.y = b;  v2.z = -mNearZ;
    v3.x = c;  v3.y = -b; v3.z = -mNearZ;
    CrossVec3(&v1, &v0, (Vector3 *)&mPlaneNormals[0]);
    CrossVec3(&v2, &v1, (Vector3 *)&mPlaneNormals[1]);
    CrossVec3(&v3, &v2, (Vector3 *)&mPlaneNormals[2]);
    CrossVec3(&v0, &v3, (Vector3 *)&mPlaneNormals[3]);
    NormalizeVec3((Vector3 *)&mPlaneNormals[0], (Vector3 *)&mPlaneNormals[0]);
    NormalizeVec3((Vector3 *)&mPlaneNormals[1], (Vector3 *)&mPlaneNormals[1]);
    NormalizeVec3((Vector3 *)&mPlaneNormals[2], (Vector3 *)&mPlaneNormals[2]);
    NormalizeVec3((Vector3 *)&mPlaneNormals[3], (Vector3 *)&mPlaneNormals[3]);
}

// @symbol _ZN8dClipper13Func_020156DCEitii
/* The former `Ev` spelling claimed this method took no arguments, contradicting
   every call site and the register/stack reads in the ROM. Scalar parameter
   types reproduce both the observed ABI and the complete function bytes. */
void dClipper::Func_020156DC(Fix12i aspectRatio, u16 fovAngle,
                            Fix12i nearZ, Fix12i farZ)
{
    mAspectRatio = aspectRatio;
    mFovAngle = fovAngle;
    mNearZ = nearZ;
    mFarZ = farZ;
    Func_0201559C();
}

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void dClipper_EmitDeletingDestructor(dClipper *p)
{
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void dClipper_EmitDestructor(dClipper *p)
{
    p->~dClipper();
}

// @symbol _ZN8dClipperC1Ev
/* The default projection: aspect 4:3 (0x1555), angle 0xe38 (20 degrees,
 * the half-angle Func_0201559C takes the tangent of), near plane 1.0 and
 * far plane 5000.0. */
dClipper::dClipper()
{
    Func_020156DC(0x1555, 0xe38, 0x1000, 0x1388000);
}
