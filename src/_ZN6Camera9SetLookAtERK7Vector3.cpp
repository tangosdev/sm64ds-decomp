//cpp
// @symbol _ZN6Camera9SetLookAtERK7Vector3
/* recovered: named members + shared header, real C++ method
 *
 * Stores the point the camera aims at. The mirror of Camera::SetPos, twelve
 * bytes lower in the object.
 */
#include "Camera.h"

void Camera::SetLookAt(const Vector3 & lookAt_)
{
    lookAt.x = lookAt_.x;
    lookAt.y = lookAt_.y;
    lookAt.z = lookAt_.z;
}
