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
 *  - The allocation is four bytes over sizeof(dActor_c), and this TU does not
 *    include that class. Placement new on the local declaration is the
 *    cartridge's C1; its null test is the old explicit check.
 *  - This is a production source for the factory, not a recovered
 *    translation-unit boundary.
 */
#include "fBase_c.h"

struct dActor_c {
    dActor_c();
};

inline void *operator new(size_t, void *p) {
    return p;
}

// @symbol StarCamera_Spawn
extern "C" void *StarCamera_Spawn(void) {
    void *p = fBase_c::operator new(0xd4);
    return ::new (p) dActor_c();
}
