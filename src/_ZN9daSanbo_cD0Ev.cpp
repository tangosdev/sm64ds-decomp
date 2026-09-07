//cpp
// @symbol _ZN9daSanbo_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body.
 * D0 is the deleting destructor (vtable slot); mwcc emits D0/D1/D2 from one
 * ~daSanbo_c() and objisolate keeps the variant this TU is bound to. Body is
 * identical to the D1 file; delinks.txt binds each file to one symbol. */
#include "daSanbo_c.h"

daSanbo_c::~daSanbo_c()
{
}
