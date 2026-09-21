//cpp
// @symbol dMeter_c_classInit
/* This was historically labelled _ZN8dMeter_cC1Ev, but it ignores an incoming
 * `this`, allocates sizeof(dMeter_c), and is referenced through a factory table.
 * It is the out-of-line fold of `new dMeter_c`, not a constructor ABI variant.
 * CodeWarrior 2004/b56 cannot express that fold from source with this class's
 * custom actor-heap operator new, so the allocation/vptr transitions remain
 * explicit while the object and allocation size use the real typed header. */
#include "dMeter_c.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN7fBase_cC2Ev(void *self);
extern void *_ZTV7dBase_c[];
extern void *_ZTV8dMeter_c[];
}

/* Reconstructed source-style name: SM64DS proves dMeter_c through RTTI,
 * allocation size, vtable identity, and the METER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: HUD_Spawn. */
extern "C" dMeter_c *dMeter_c_classInit()
{
    dMeter_c *hud = (dMeter_c *)_ZN7fBase_cnwEj(sizeof(dMeter_c));
    if (hud) {
        _ZN7fBase_cC2Ev(hud);
        *(void ***)hud = _ZTV7dBase_c;
        *(void ***)hud = _ZTV8dMeter_c;
    }
    return hud;
}
