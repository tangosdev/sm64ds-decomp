//cpp
// @symbol _ZN21ExtendingMeshColliderD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the MovingMeshCollider subobject destructor. Both
 * come out of `struct ExtendingMeshCollider : MovingMeshCollider` and `virtual ~ExtendingMeshCollider()` in the
 * header, so there is nothing left to write.
 *
 * Its two members are Fix12i, which have no destructors, so the ROM's 0x24
 * bytes are the vtable store and the base call.
 */
#include "ExtendingMeshCollider.h"

ExtendingMeshCollider::~ExtendingMeshCollider()
{
}
