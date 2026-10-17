//cpp
/* StarCamera_Spawn -- the factory the STAR_CAMERA actor profile registers
 * (ov002 0x020ebe5c..0x020ebe8c). It allocates 0xd4 bytes, runs dActor_c's
 * constructor on the allocation and hands it back: the surviving vptr is
 * dActor_c's. The most-derived identity is unresolved -- the allocation is
 * four bytes over sizeof(dActor_c), no most-derived vptr is installed and
 * the ROM keeps no identifying RTTI record for it. StarCamera is the
 * profile-derived label, not a recovered class name.
 *
 * leftovers:
 *  - The constructor is still the mangled extern "C" call: `new dActor_c`
 *    picks a different emitted variant (C2 vs the cartridge's C1) and the
 *    four extra bytes keep their own question; the experiment is deferred
 *    (tangosdev/sm64ds-decomp#3198).
 *  - This is a production source for the factory, not a recovered
 *    translation-unit boundary.
 */
#include "fBase_c.h"

extern "C" void *_ZN8dActor_cC1Ev(void *thiz);

// @symbol StarCamera_Spawn
extern "C" void *StarCamera_Spawn(void)
{
    void *p = fBase_c::operator new(0xd4);
    if (p == 0) return p;
    return _ZN8dActor_cC1Ev(p);
}
