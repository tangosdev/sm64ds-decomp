//cpp
/**
 * Base adapter of dMgJump3DMario_c, used by the Bounce and Pounce scenes.
 * dMg3DHeyhoObjAdapter_c is the cartridge's RTTI spelling; its name alone
 * does not establish a separate Shy Guy object role.
 *
 * This production source contains the constructor at ov006 0x020c4048..0x020c4060.
 * It starts the adapter with no pending command.
 *
 * The cartridge configures only the base-object constructor (C2). The extra
 * complete-object C1 emitted by mwccarm has no configured ROM home and is
 * explicitly discarded. The emitted vtable, type-name string and RTTI are
 * checked against their ov006 homes at 0x0213afd8, 0x0213afe4 and 0x0213b000
 * before text isolation discards their duplicate storage.
 */

#include "dMgJump3DMario_c.h"

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN22dMg3DHeyhoObjAdapter_cC2Ev, 0x020c4048, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN22dMg3DHeyhoObjAdapter_cC2Ev
dMg3DHeyhoObjAdapter_c::dMg3DHeyhoObjAdapter_c() : mCommand(0)
{
}
