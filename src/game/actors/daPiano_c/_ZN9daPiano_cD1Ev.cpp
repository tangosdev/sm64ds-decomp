//cpp
// @symbol _ZN9daPiano_cD1Ev
/* Member and base ownership make CodeWarrior emit every teardown step in ROM
 * order; the recovered source destructor itself is empty. */
#include "daPiano_c.h"

daPiano_c::~daPiano_c()
{
}
