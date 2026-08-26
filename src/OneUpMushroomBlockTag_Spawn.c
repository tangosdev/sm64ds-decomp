// @symbol OneUpMushroomBlockTag_Spawn
/* Measured C++ factory wall: `new BrickBlock` selects the global allocator,
 * while the cartridge calls fBase_c's actor allocator. Keep the verified C ABI
 * construction sequence with the exact class size, base ctor and vtable. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV10BrickBlock */
int *OneUpMushroomBlockTag_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0xdc);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV10BrickBlock; }
    return p;
}
