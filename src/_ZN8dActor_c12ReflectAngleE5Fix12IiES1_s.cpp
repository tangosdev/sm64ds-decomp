//cpp
// @symbol _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s
/* recovered: named members + shared header, real C++ method */
#include "dActor_c.h"

extern "C" short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);

/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in dActor_c.h is the real one and callers may use it. */
extern "C" short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(
    dActor_c *self, int normalX, int normalZ, short angToReflect)
{
    short normalAngle = _ZN4cstd5atan2E5Fix12IiES1_(normalX, normalZ);
    short delta = (short)((angToReflect + 0x8000) - normalAngle);
    return (short)(normalAngle - delta);
}
