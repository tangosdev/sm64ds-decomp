//cpp
// @symbol _ZN7dScMB_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out what a complete-object destructor does
 * anyway: store this class's vtable over the one the base constructor left,
 * destroy the members in reverse declaration order, then run the base
 * subobject destructor. All of it follows from `struct dScMB_c : dScene_c` and the member types
 * in the header, so the body is empty and the bytes still reproduce.
 */
#include "dScMB_c.h"

dScMB_c::~dScMB_c()
{
}
