//cpp
// @symbol _ZN5BullyD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Two vtable stores and five destructor calls, none of them written here: its own
 * vptr, then daOts_c's -- inlined, because that class's destructor is defined in its
 * class body -- then the base's four members in reverse declaration order, then
 * Enemy. Bully adds no member with a destructor of its own.
 *
 * The two stores in a row are the tell that a base destructor was inlined rather
 * than called, and they are what said daOts_c owns all four members.
 */
#include "Bully.h"

Bully::~Bully()
{
}
