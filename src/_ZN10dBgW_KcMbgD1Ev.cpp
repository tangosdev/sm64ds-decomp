//cpp
// @symbol _ZN18MovingMeshColliderD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out the two things a complete-object
 * destructor does anyway: store this class's vtable over the one the base
 * constructor left behind, then run the base subobject destructor. Both come
 * out of `struct MovingMeshCollider : MeshCollider` and `virtual ~...()` in
 * the header, so there is nothing left to write.
 *
 * None of the members has a destructor -- they are Fix12i, Matrix4x3, Vector3
 * and scalars -- which is why the ROM's 0x24 bytes contain no member calls and
 * the empty body reproduces them.
 */
#include "MovingMeshCollider.h"

MovingMeshCollider::~MovingMeshCollider()
{
}
