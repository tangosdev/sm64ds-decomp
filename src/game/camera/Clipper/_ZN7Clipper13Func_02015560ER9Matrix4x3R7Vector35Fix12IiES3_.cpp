//cpp
// @symbol _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_
/* recovered: named members + shared header, real C++ method */
#include "Clipper.h"
#include "math/Matrix.h"

extern "C" void MulVec3Mat4x3(const Vector3 *v, const Matrix4x3 *m, Vector3 *res);
extern "C" int _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(Clipper *thiz, Vector3 *v, int clip, u8 *hint);

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in Clipper.h is the real one and callers may use it. */
extern "C" void _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
    Clipper *thiz, Matrix4x3 *mat, Vector3 *srcVec, int scale, Vector3 *dstVec)
{
    MulVec3Mat4x3(srcVec, mat, dstVec);
    _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(thiz, dstVec, scale, (u8 *)0);
}
