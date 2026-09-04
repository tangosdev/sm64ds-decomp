//cpp
// @symbol dMap_c_classInit
/* Allocation factory formerly labelled _ZN7MinimapC1Ev. It ignores an
 * incoming `this`, allocates sizeof(Minimap), and is held in a factory table;
 * no constructor ABI variant has that shape. See dMeter_c_classInit for the measured
 * CodeWarrior placement-new limitation behind the explicit lifecycle steps. */
#include "Minimap.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN7fBase_cC2Ev(void *self);
extern void *_ZTV7dBase_c[];
extern void *_ZTV7Minimap[];
}

/* Reconstructed source-style name: SM64DS proves dMap_c through RTTI,
 * allocation size, vtable identity, and the MAP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Minimap_Spawn. */
extern "C" Minimap *dMap_c_classInit()
{
    Minimap *minimap = (Minimap *)_ZN7fBase_cnwEj(sizeof(Minimap));
    if (minimap) {
        _ZN7fBase_cC2Ev(minimap);
        *(void ***)minimap = _ZTV7dBase_c;
        *(void ***)minimap = _ZTV7Minimap;
    }
    return minimap;
}
