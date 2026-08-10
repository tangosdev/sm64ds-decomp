//cpp
// @symbol _ZN13BigBrickBlockD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct BigBrickBlock : Platform`: its own vptr, then Platform's -- inlined,
 * because Platform's destructor is defined in its class body -- then
 * Platform's Model and MovingMeshCollider, then Actor. This class adds no
 * member with a destructor.
 */
#include "BigBrickBlock.h"

BigBrickBlock::~BigBrickBlock()
{
}
