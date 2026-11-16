//cpp
/* StarCamera_Spawn -- the factory the STAR_CAMERA actor profile registers
 * (ov002 0x020ebe5c..0x020ebe8c). It allocates 0xd4 bytes, exactly
 * sizeof(dActor_c), runs dActor_c's constructor on the allocation and hands
 * it back: the surviving vptr is dActor_c's. No most-derived vptr is
 * installed and the ROM keeps no identifying RTTI record for it, so this is
 * a bare dActor_c. StarCamera is the profile-derived label, not a recovered
 * class name.
 *
 * leftovers:
 *  - This is a production source for the factory, not a recovered
 *    translation-unit boundary.
 */
#include "fBase_c.h"
#include "dActor_c.h"

void *StarCamera_Spawn(void) {
    return new dActor_c;
}
