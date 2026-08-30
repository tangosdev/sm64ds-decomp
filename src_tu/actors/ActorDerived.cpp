//cpp
/* Original dBase_c translation unit.
 *
 * The destructor is intentionally defined inline in dBase_c.h. Defining the
 * real key function below makes mwccarm emit the class vtable and, through its
 * destructor slots, the out-of-line D1 and D0 copies in the cartridge's order.
 * There is no hand-spelled destructor and no forcing scaffold in this TU.
 */
#include "dBase_c.h"

extern "C" {
extern void func_02042fe4(int a, int b, int c);
extern fBase_c *func_02042ffc(u32 actorID, fBase_c *parent);
}

void dBase_c::AfterInitResources(u32 vfSuccess)
{
    if (vfSuccess == 1)
        MarkForDestruction();
    fBase_c::AfterInitResources(vfSuccess);
}

fBase_c *dBase_c::Spawn(u32 actorID, fBase_c *parent, int, int)
{
    return func_02042ffc(actorID, parent);
}

extern "C" void func_02013edc(int a, int b, int c)
{
    func_02042fe4(a, b, c);
}
