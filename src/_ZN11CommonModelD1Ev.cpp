//cpp
// @symbol _ZN11CommonModelD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the ModelBase subobject destructor. Both
 * come out of `struct CommonModel : ModelBase` and `virtual ~CommonModel()` in the
 * header, so there is nothing left to write.
 *
 * Its two members -- a ModelComponents pointer and a Matrix4x3 -- have no
 * destructors, so the ROM's 0x24 bytes are the vtable store and the base call
 * and nothing else.
 */
#include "CommonModel.h"

CommonModel::~CommonModel()
{
}
