//cpp
// @symbol _ZN12dScMgAmida_c9Virtual8CEv
/* dScMgAmida_c::Virtual8C - slot 35, the family's only override of it.

   The base asks whether the low byte of fBase_c::param1 is nonzero; this asks
   whether it is exactly 1.  Nothing in this class calls it -- the thirteen
   dispatch sites are all in dScMgCoin_c, dScMgPanel_c, dScMgSound_c and
   dScMgSnowball_c -- which is why it survived this class's own C++ migration
   untouched.  See the slot-35 block in include/dScMgBase_c.h. */
#include "dScMgAmida_c.h"

int dScMgAmida_c::Virtual8C()
{
    return (param1 & 0xff) == 1;
}
