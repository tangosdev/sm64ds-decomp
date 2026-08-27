// @symbol IceSlideManager_Spawn
/* The natural C++ spelling `return new IceSlideManager` reproduces the text,
 * but relocates the allocation call to unavailable global `_Znwm` rather than
 * the cartridge's fBase_c::operator new. Keep the measured C allocator veneer. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"

int *IceSlideManager_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(216);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV15IceSlideManager; }
    return p;
}
