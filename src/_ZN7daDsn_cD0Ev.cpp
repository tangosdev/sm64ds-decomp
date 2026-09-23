//cpp
// @symbol _ZN7daDsn_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body.
 * D0 is the deleting destructor (vtable slot); mwcc emits D0/D1/D2 from one
 * ~daDsn_c() and objisolate keeps the variant this TU is bound to. Body is
 * identical to the D1 file; delinks.txt binds each file to one symbol. */
#include "daDsn_c.h"

daDsn_c::~daDsn_c()
{
}
