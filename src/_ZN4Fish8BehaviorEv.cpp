//cpp
// @symbol _ZN4Fish8BehaviorEv
/* recovered: real C++ method over Fish and inherited actor state */
#include "decl_common.h"
#include "Fish.h"

struct FishState { void (Fish::*method)(); };
extern "C" FishState data_ov100_02148a1c[];
extern "C" Matrix4x3 data_020a0e68;
extern "C" {
void Vec3_Asr(Vector3 *dst, Vector3 *src, int shift);
void Matrix4x3_FromTranslation(Matrix4x3 *matrix, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *matrix, short angle);
}
/* Matrix4x3 contains a non-trivial Vector3 tail, whose generated assignment
 * schedules differently. This POD view preserves the ROM's three ldm/stm copies.
 */
struct RawMatrix4x3 { int words[12]; };

int Fish::Behavior()
{
    Vector3 shiftedPos;
    dActor_c *owner;
    if (mHidden != 0) {
        (this->*data_ov100_02148a1c[mState].method)();
    } else {
        owner = dActor_c::FindWithID(mUniqueID_13c);
        if (owner == 0 || func_ov100_0214639c((int *)owner) != 0) {
            MarkForDestruction();
        } else {
            (this->*data_ov100_02148a1c[mState].method)();
            UpdatePos(0);
            unk_150++;
        }
        Vec3_Asr(&shiftedPos, (Vector3 *)&mPosX, 3);
        Matrix4x3_FromTranslation(
            &data_020a0e68, shiftedPos.x, shiftedPos.y, shiftedPos.z);
        mAngleY = mPrevAngleY;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
        *(RawMatrix4x3 *)&mModelAnim.mat4x3 =
            *(RawMatrix4x3 *)&data_020a0e68;
        mModelAnim.Advance();
    }
    return 1;
}
