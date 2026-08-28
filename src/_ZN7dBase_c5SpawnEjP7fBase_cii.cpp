//cpp
#include "dBase_c.h"

extern "C" fBase_c *func_02042ffc(u32 actorID, fBase_c *parent);

fBase_c *dBase_c::Spawn(u32 actorID, fBase_c *parent, int, int)
{
    return func_02042ffc(actorID, parent);
}
