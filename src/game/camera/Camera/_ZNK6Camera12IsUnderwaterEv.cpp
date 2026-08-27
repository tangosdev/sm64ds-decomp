//cpp
// @symbol _ZNK6Camera12IsUnderwaterEv
/* recovered: named members + shared header, real C++ method
 *
 * Returns the under-water bit of mFlags MASKED, not normalised to 0/1 -- the
 * ROM's `and r0,r0,#1` is the whole body, so a `!= 0` would be a different
 * function. Preserved deliberately.
 */
#include "Camera.h"

enum CameraFlags {
    IS_UNDERWATER = 1 << 0,
};

int Camera::IsUnderwater() const
{
    return mFlags & IS_UNDERWATER;
}
