//cpp
// @symbol _ZN8dActor_c11SpawnNumberERK7Vector3jbtPS_
/* Spawn a floating score in this actor's area, optionally pack the value
 * into one digit, and attach it to its owner after setting the display delay. */
#include "daObjNumber_c.h"

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

    static_cast<daObjNumber_c *>(number)->mDelay = delay;
    return func_ov002_020f0918(number, owner);
}
