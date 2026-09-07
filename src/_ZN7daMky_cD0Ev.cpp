//cpp
// @symbol _ZN7daMky_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body.
 * D0 is the deleting destructor (vtable slot); mwcc emits D0/D1/D2 from one
 * ~daMky_c() and objisolate keeps the variant this TU is bound to. Body is
 * identical to the D1 file; delinks.txt binds each file to one symbol. */
#include "daMky_c.h"

daMky_c::~daMky_c()
{
}
