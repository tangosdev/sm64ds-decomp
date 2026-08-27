//cpp
// @symbol _ZN8dM3dGLin3SetERK7Vector3S2_
/* recovered: real C++ method
 *
 * A line is exactly its two endpoints. The explicit component copies preserve
 * the ROM's interleaved load/store sequence while stating the geometry through
 * the class's proven Vector3 members.
 */
#include "dM3dGLin.h"

void dM3dGLin::Set(const Vector3 &start_, const Vector3 &end_)
{
    start.x = start_.x;
    start.y = start_.y;
    start.z = start_.z;
    end.x = end_.x;
    end.y = end_.y;
    end.z = end_.z;
}
