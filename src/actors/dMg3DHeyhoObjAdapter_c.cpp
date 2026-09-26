//cpp
/**
 * Command adapter for the Shy Guy ("Heyho") objects in the 3D minigame.
 *
 * The whole translation unit is this one constructor: the adapter starts with
 * no pending command. ov006 0x020c4048..0x020c4060.
 *
 * The cartridge configures only the base-object constructor (C2) for this
 * class. The complete-object C1 the compiler emits beside it, and the vtable,
 * type-name string and RTTI record the definition drags in, are declared as
 * compiler-only output in the manifest and compared against their ov006 homes
 * at 0x0213afd8, 0x0213afe4 and 0x0213b000.
 */

#include "dMgJump3DMario_c.h"

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN22dMg3DHeyhoObjAdapter_cC2Ev, 0x020c4048, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN22dMg3DHeyhoObjAdapter_cC2Ev
dMg3DHeyhoObjAdapter_c::dMg3DHeyhoObjAdapter_c() : mCommand(0)
{
}
