//cpp
// @symbol _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_
/* dActor_c::SpawnNumber(const Vector3 &pos, u32 value, bool packLowNibble,
 *                    u16 delay, dActor_c *owner) at 0x02010044.
 *
 * Puts up actor 0x14a -- the floating number popup -- at pos, in this actor's
 * own area, then hands it to the helper that attaches it to `owner`.
 *
 * A member: mAreaId is loaded off `this` (`ldrsb r3, [r0, #0xcc]`) on both
 * arms of the branch, which is also why the load appears twice rather than
 * once above it.
 *
 * packLowNibble arrives in r3 and is only ever `cmp r3, #0`, so the bytes
 * cannot separate bool from int; the mangled name says `b` and nothing
 * contradicts it. What it selects is visible though: `and #0xf` then
 * `orr #0x10`, so the value is squeezed to a nibble and tagged, versus passed
 * through untouched.
 *
 * delay is `ldrh` -- u16 -- and lands in the popup's own 0x14c, reached as
 * `add r2, r0, #0x100` / `strh r3, [r2, #0x4c]` because 0x14c does not fit a
 * single store offset. `owner` is `ldr` from the next stack slot and the
 * mangled name types it PS_, this class again.
 */
#include "dActor_c.h"

extern "C" dActor_c *func_ov002_020f0918(dActor_c *number, dActor_c *owner);

dActor_c *dActor_c::SpawnNumber(const Vector3 &pos, u32 value, bool packLowNibble,
                          u16 delay, dActor_c *owner)
{
    dActor_c *number;

    if (packLowNibble)
        number = Spawn(0x14a, (value & 0xf) | 0x10, pos, 0, mAreaId, -1);
    else
        number = Spawn(0x14a, value, pos, 0, mAreaId, -1);

    if (number == 0)
        return number;

    *(u16 *)((char *)number + 0x14c) = delay;
    return func_ov002_020f0918(number, owner);
}
