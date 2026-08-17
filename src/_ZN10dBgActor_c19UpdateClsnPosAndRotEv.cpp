//cpp
// @symbol _ZN8Platform19UpdateClsnPosAndRotEv
/* recovered: named members + shared header, real C++ method
 *
 * Was written through three stand-in structs -- an `Obj` whose only job was to
 * reach 0x2ec by counting bytes, an `MMC` that reached 0x124 the same way, and an
 * `M4` standing in for Matrix4x3. All three named offsets the header now names:
 * 0x2ec is mClsnMat, 0x124 is mMeshCollider, and 0xf0 -- reached as `&unk_0f0` --
 * is mModel.mat4x3.
 *
 * With those named, what the function does is legible: take the model's transform,
 * replace its translation row with the actor's position, and hand it to the
 * collider. m[9], m[10] and m[11] are that row, which is why the generated header's
 * unk_310/unk_314/unk_318 sat exactly 0x24/0x28/0x2c into the matrix.
 */
#include "Platform.h"

extern "C" int _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(
    MovingMeshCollider *self, const Matrix4x3 &mat, short angleY);

void Platform::UpdateClsnPosAndRot()
{
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9]  = mPosX;
    mClsnMat.m[10] = mPosY;
    mClsnMat.m[11] = mPosZ;
    _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(&mMeshCollider, mClsnMat, mAngleY);
}
