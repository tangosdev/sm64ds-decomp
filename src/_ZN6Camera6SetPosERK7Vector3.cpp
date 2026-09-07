//cpp
// @symbol _ZN6Camera6SetPosERK7Vector3
/* recovered: named members + shared header, real C++ method
 *
 * Stores the world position the camera sits at. Field-by-field rather than a
 * whole-Vector3 assignment, which is what the ROM does.
 *
 * The pos triple is what fixed a header defect: this writes THREE words at
 * 0x8c, so 0x94 -- declared u8 -- is really the z. Camera.h now says so, with
 * SaveCameraStateBeforeTalk's 4-byte copy of the same slot as corroboration.
 */
#include "Camera.h"

void Camera::SetPos(const Vector3 & pos_)
{
    pos.x = pos_.x;
    pos.y = pos_.y;
    pos.z = pos_.z;
}
