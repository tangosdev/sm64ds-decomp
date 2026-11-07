//cpp
/* dM3dGLin, a line of two endpoints. Set copies both. No vtable.
 * Only D2 has a ROM home; the compiler's D1 is deadstripped.
 * GetStart/GetEnd return Vector3 through the hidden result pointer in r0,
 * with this in r1, as dBgCh_Lin::ResetResult's calls require.
 */

#include "dM3dGLin.h"

dM3dGLin::~dM3dGLin() {}

/* A line is exactly its two endpoints. The explicit component copies
 * preserve the ROM's interleaved load/store sequence. */
void dM3dGLin::Set(const Vector3 &start_, const Vector3 &end_) {
    start.x = start_.x;
    start.y = start_.y;
    start.z = start_.z;
    end.x = end_.x;
    end.y = end_.y;
    end.z = end_.z;
}

/* Defined last so reverse emission places the getters first in the run. */
Vector3 dM3dGLin::GetStart() const {
    return start;
}

Vector3 dM3dGLin::GetEnd() const {
    return end;
}
