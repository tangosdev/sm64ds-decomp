//cpp
/* dView_c -- the shared 0x80-byte view base dCamera_c derives from.
 *
 * ROM evidence: the three enrolled members tile 0x0202fc98..0x0202fd2c and
 * the class vtable at 0x02092720 (_ZTV7dView_c) names [D1 0x0202fc98,
 * D0 0x0202fcc8, Render 0x0202fd0c] under the canonical {0, _ZTI} header --
 * all three inside this run. _ZTS7dView_c at 0x02086e48 reads "7dView_c"
 * and _ZTI7dView_c at 0x02086ecc is an __si_class_type_info on dBase_c.
 * The destructor is declared inline on the header (dCamera_c's ROM dtor
 * inlines dView_c's vptr store), so the two emit-forcers below pin the
 * out-of-line D1/D0 copies the vtable slots point at.
 *
 * func_0202fd2c (arm9 0x0202fd2c..0x0202fd4c) is absorbed below: it only
 * zeroes the two kuppa-script words. Defined first because this file emits
 * .text in reverse source order, so the tail body lands at the run's right
 * edge.
 *
 * leftovers:
 * - data_0209fc48/data_0209fc4c keep the shard's `int' spelling: no header
 *   declares the kuppa-script words and the tree's other spellings disagree.
 * - dView_c_EmitDestructor/dView_c_EmitDeletingDestructor are byte-required
 *   pins for the header-inline destructor; the wrappers have no cartridge
 *   home (manifest-licensed emit-forcers).
 * - CopyToViewMat is declared TU-locally: no header declares the free
 *   _Z13CopyToViewMatPK9Matrix4x3 shard, and the C++ spelling mangles to it.
 */
#include "dView_c.h"

void CopyToViewMat(const Matrix4x3 *mat);

// @symbol func_0202fd2c
extern "C" {
extern int data_0209fc48, data_0209fc4c;
void func_0202fd2c(void)
{
    data_0209fc48 = 0;
    data_0209fc4c = 0;
}
}

// @symbol _ZN7dView_c6RenderEv
s32 dView_c::Render()
{
    CopyToViewMat(&viewMat);
    return 1;
}

// @symbol _ZN7dView_cD0Ev -- pinned by the delete below
void dView_c_EmitDeletingDestructor(dView_c *view)
{
    delete view;
}

// @symbol _ZN7dView_cD1Ev -- pinned by the qualified call below
void dView_c_EmitDestructor(dView_c *view)
{
    view->~dView_c();
}
