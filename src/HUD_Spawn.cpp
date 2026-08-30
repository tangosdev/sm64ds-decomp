//cpp
// @symbol HUD_Spawn
/* This was historically labelled _ZN3HUDC1Ev, but it ignores an incoming
 * `this`, allocates sizeof(HUD), and is referenced through a factory table.
 * It is the out-of-line fold of `new HUD`, not a constructor ABI variant.
 * CodeWarrior 2004/b56 cannot express that fold from source with this class's
 * custom actor-heap operator new, so the allocation/vptr transitions remain
 * explicit while the object and allocation size use the real typed header. */
#include "HUD.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN7fBase_cC2Ev(void *self);
extern void *_ZTV7dBase_c[];
extern void *_ZTV3HUD[];
}

extern "C" HUD *HUD_Spawn()
{
    HUD *hud = (HUD *)_ZN7fBase_cnwEj(sizeof(HUD));
    if (hud) {
        _ZN7fBase_cC2Ev(hud);
        *(void ***)hud = _ZTV7dBase_c;
        *(void ***)hud = _ZTV3HUD;
    }
    return hud;
}
