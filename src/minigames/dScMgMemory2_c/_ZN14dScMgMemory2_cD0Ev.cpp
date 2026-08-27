//cpp
// @symbol _ZN14dScMgMemory2_cD0Ev
/* recovered: real C++ destructor. The one explicit call reproduces the
   ROM's own recovered body exactly: destroy the shared 0x270-byte table at
   0x4f38, addressed by raw offset from `this` because the header keeps it
   opaque. Everything after -- own vtable store, mSysTracker destruction,
   chain to ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgSingle3DBase_c's now-inline destructor (see
   include/dScMgSingle3DBase_c.h's own note).

   The pre-migration source destroyed mSysTracker through a LOCAL
   `struct SysTracker { ~SysTracker(); char pad[4]; };` shadow. That
   mangles to _ZN10SysTrackerD1Ev, a symbol that exists nowhere in the ROM
   -- the real one is namespaced, _ZN8Particle10SysTrackerD1Ev. The bytes
   matched anyway, because build_pin and fdiff wildcard relocated words, so
   the file byte-matched while eligible.py quietly refused to enroll it and
   dsd served the ROM's own bytes. Inheriting the base's real member is what
   retires that phantom reference. */
#include "dScMgMemory2_c.h"
dScMgMemory2_c::~dScMgMemory2_c()
{
    func_ov006_020c1c64((char *)this + 0x4f38);
}
