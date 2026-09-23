//cpp
/**
 * The line segment primitive: two endpoints, Set copies both.
 *
 * Non-polymorphic (no vptr, no vtable); the ROM keeps only the
 * trivial base-subobject D2 (`bx lr`). The D1 the same definition
 * emits is homeless and deadstripped by the manifest.
 */

#include "dM3dGLin.h"

// @symbol _ZN8dM3dGLinD2Ev
dM3dGLin::~dM3dGLin()
{
}

// @symbol _ZN8dM3dGLin3SetERK7Vector3S2_
/* A line is exactly its two endpoints. The explicit component copies
 * preserve the ROM's interleaved load/store sequence. */
void dM3dGLin::Set(const Vector3 &start_, const Vector3 &end_)
{
    start.x = start_.x;
    start.y = start_.y;
    start.z = start_.z;
    end.x = end_.x;
    end.y = end_.y;
    end.z = end_.z;
}
