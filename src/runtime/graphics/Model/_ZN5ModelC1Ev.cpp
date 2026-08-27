//cpp
// @symbol _ZN5ModelC1Ev
/* recovered: real C++ constructor -- the first in the tree
 *
 * Only the member initialization below is written by hand. Everything else in
 * these 0x50 bytes is what `Model : ModelBase` with a declared-not-defined
 * base constructor already means: the compiler emits the out-of-line call to
 * _ZN9ModelBaseC2Ev, then the _ZTV5Model vptr store, in that order, around
 * the body.
 *
 * Two non-obvious choices, both measured:
 *
 * - ModelBase declares its own constructor (include/ModelBase.h). Left
 *   undeclared, the compiler synthesises it inline and the ROM's
 *   `bl _ZN9ModelBaseC2Ev` never appears.
 * - common.h is included FIRST, so this TU sees the flat
 *   `struct Matrix4x3 { s32 m[12]; }` spelling (see include/common.h: the two
 *   spellings share one guard, whichever a TU sees first stands). Under the
 *   structured `Matrix3x3 r; Vector3 t;` spelling from math/Matrix.h the same
 *   assignment compiles 28 bytes longer -- the copy splits into a subobject
 *   copy of r plus field-by-field stores of t, where the ROM does one flat
 *   block move of all twelve words.
 *
 * The compiler emits this TU's object with THREE functions: C1, an identical
 * C2, and a local Matrix4x3 destructor helper. objisolate keeps only the C1,
 * so _ZN5ModelC2Ev.cpp continues to provide the base-object variant, exactly as
 * a real ~Class() coexists with its hand-written D0.
 */
#include "common.h"
#include "Model.h"

extern Matrix4x3 IDENTITY_MATRIX4X3;

Model::Model() : transformsBuf(0)
{
    mat4x3 = IDENTITY_MATRIX4X3;
}
