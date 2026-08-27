// @symbol IceSheet_Spawn
/* Measured C++ factory wall. `return new IceSheet` reproduces the construction
 * shape but emits the global `_Znwm` allocator, while the cartridge calls
 * fBase_c::operator new(unsigned int). Keep the verified C ABI construction
 * form until that allocator relationship is representable without a wrong
 * relocation. */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"

int *IceSheet_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x320);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV8IceSheet; }
    return p;
}
