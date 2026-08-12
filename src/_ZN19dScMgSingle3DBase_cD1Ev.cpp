//cpp
// @symbol _ZN19dScMgSingle3DBase_cD1Ev
/* ~dScMgSingle3DBase_c() is declared, NOT defined inline, in
   dScMgSingle3DBase_c.h -- see the header's own note. mwcc doesn't inline a
   body this size into a descendant (same measured rule as dScMgBase_c's
   own D1/D0), so this is a REAL out-of-line definition. The empty body is
   enough: the compiler destroys mSysTracker (a real member with a
   declared, non-inline destructor -- matches the ROM's own
   `_ZN8Particle10SysTrackerD1Ev(c+0x471c)` call) and then implicitly
   chains to ~dScMgBase_c() (matches the ROM's own `_ZN11dScMgBase_cD2Ev(c)`
   call), same as Stage::~Stage()'s own empty body reproduces its three
   members plus Scene's chain. D0Ev.cpp carries an identical copy for its
   own key-function TU. */
#include "dScMgSingle3DBase_c.h"
dScMgSingle3DBase_c::~dScMgSingle3DBase_c()
{
}
