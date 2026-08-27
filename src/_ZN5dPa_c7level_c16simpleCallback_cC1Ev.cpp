//cpp
// @symbol _ZN5dPa_c7level_c16simpleCallback_cC1Ev
/* Complete-object constructor. The compiler emits the callback_c base vptr,
 * the simpleCallback_c vptr, and the body initializer in the ROM's order. */
#include "dPa_c.h"

dPa_c::level_c::simpleCallback_c::simpleCallback_c()
    : value(0)
{
}
