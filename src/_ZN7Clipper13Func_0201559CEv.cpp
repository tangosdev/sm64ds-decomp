//cpp
// @symbol _ZN7Clipper13Func_0201559CEv
/* Clipper::Func_0201559C -- rebuild the four view-frustum side planes from the
 * current field of view and near-plane distance.
 *
 * unk_058 is an angle; its top 12 bits index a sine/cosine pair table at
 * data_02082214, and fdiv of that pair gives the tangent. Scaling by unk_050
 * (the plane distance) gives the half-height b, and by unk_04c (the aspect
 * ratio) the half-width c. The four corner vectors at z = -unk_050 follow, and
 * each adjacent pair crossed and normalised is one side plane's normal.
 *
 * This body is what proves unk_04c is SIGNED: it sign-extends the field into a
 * 64-bit multiply, which a u32 cannot do. The header used to type it u32 on the
 * weaker evidence of Func_020156DC, which only stores to it; that is now
 * corrected there rather than cast away here.
 *
 * The name is still the placeholder; renaming it is a symbols.txt change.
 */
#include "Clipper.h"

extern "C" int _ZN4cstd4fdivEii(int a, int b);
extern "C" void CrossVec3(Vector3 *a, Vector3 *b, Vector3 *result);
extern "C" void NormalizeVec3(Vector3 *src, Vector3 *dst);
extern "C" short data_02082214[];

void Clipper::Func_0201559C()
{
    int idx = (int)unk_058 >> 4;
    int q = _ZN4cstd4fdivEii(data_02082214[2 * idx], data_02082214[2 * idx + 1]);
    Fix12i b = (Fix12i)(((long long)unk_050 * q + 0x800) >> 12);
    Fix12i c = (Fix12i)(((long long)unk_04c * b + 0x800) >> 12);
    Vector3 v0, v1, v2, v3;
    v0.x = -c; v0.y = -b; v0.z = -unk_050;
    v1.x = -c; v1.y = b;  v1.z = -unk_050;
    v2.x = c;  v2.y = b;  v2.z = -unk_050;
    v3.x = c;  v3.y = -b; v3.z = -unk_050;
    CrossVec3(&v1, &v0, &mPlaneNormals[0]);
    CrossVec3(&v2, &v1, &mPlaneNormals[1]);
    CrossVec3(&v3, &v2, &mPlaneNormals[2]);
    CrossVec3(&v0, &v3, &mPlaneNormals[3]);
    NormalizeVec3(&mPlaneNormals[0], &mPlaneNormals[0]);
    NormalizeVec3(&mPlaneNormals[1], &mPlaneNormals[1]);
    NormalizeVec3(&mPlaneNormals[2], &mPlaneNormals[2]);
    NormalizeVec3(&mPlaneNormals[3], &mPlaneNormals[3]);
}
