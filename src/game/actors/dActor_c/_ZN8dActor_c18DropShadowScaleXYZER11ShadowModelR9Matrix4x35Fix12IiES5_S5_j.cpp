//cpp
// @symbol _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j
/* recovered: named members + shared header, real C++ method */
#include "dActor_c.h"
#include "ShadowModel.h"

/* ShadowModel::InitModel is itself wall 6az (its own Fix12<int> params), so
   its DEFINITION is the plain-scalar mangled name below -- calling that
   directly, rather than through the real method, needs no Fix12<int>
   temporary at this call site. See include/ShadowModel.h. */
extern "C" void _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(
    ShadowModel *self, Matrix4x3 *m, int sx, int sy, int sz, u32 opacity);

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in dActor_c.h is the real one and callers may use it. */
extern "C" void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *self, ShadowModel *shadow, Matrix4x3 *matrix,
    int scaleX, int scaleY, int scaleZ, u8 opacity)
{
    if (self->mFlags & 0x10)
        return;
    _ZN11ShadowModel9InitModelEP9Matrix4x35Fix12IiES3_S3_j(shadow, matrix, scaleX, scaleY, scaleZ, opacity);
}
