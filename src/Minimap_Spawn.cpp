//cpp
// @symbol Minimap_Spawn
/* Allocation factory formerly labelled _ZN7MinimapC1Ev. It ignores an
 * incoming `this`, allocates sizeof(Minimap), and is held in a factory table;
 * no constructor ABI variant has that shape. See HUD_Spawn for the measured
 * CodeWarrior placement-new limitation behind the explicit lifecycle steps. */
#include "Minimap.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN7fBase_cC2Ev(void *self);
extern void *_ZTV7dBase_c[];
extern void *_ZTV7Minimap[];
}

extern "C" Minimap *Minimap_Spawn()
{
    Minimap *minimap = (Minimap *)_ZN7fBase_cnwEj(sizeof(Minimap));
    if (minimap) {
        _ZN7fBase_cC2Ev(minimap);
        *(void ***)minimap = _ZTV7dBase_c;
        *(void ***)minimap = _ZTV7Minimap;
    }
    return minimap;
}
