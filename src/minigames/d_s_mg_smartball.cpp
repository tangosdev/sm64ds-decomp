//cpp
/* Smartball minigame scene (MG_SMARTBALL): the factory and the empty
 * constructor for its mArray4 elements.
 *
 * Blocked: the ROM has no dScMgSmartball_c constructor, so the factory
 * builds the scene by hand (base constructor, vtable store, four array
 * constructs) instead of `new dScMgSmartball_c`. The element constructor
 * and destructor stubs are unnamed in symbols.txt.
 */

#include "dScMgSmartball_c.h"

extern "C" {
extern int _ZN11dScMgBase_cC2Ev(void *p);
extern void __cxa_vec_ctor(void *obj, int a, int b, void *cb1, void *cb2);
extern int _ZTV16dScMgSmartball_c[];
extern void func_0203d738(void);
extern void func_ov006_02119900(void);
void *dScMgSmartball_c_classInit(void);
}

// @symbol func_ov006_02119900
/* mArray4's element constructor; the elements need no setup. */
extern "C" void func_ov006_02119900(void)
{
}

// @symbol dScMgSmartball_c_classInit
/* Allocates and constructs the scene; returns null if allocation fails.
 * Historical alias: MgBingoBallSlotsShot_Spawn. */
extern "C" void *dScMgSmartball_c_classInit(void) {
    dScMgSmartball_c *scene = (dScMgSmartball_c *)_ZN7fBase_cnwEj(0x629c);
    if (scene != 0) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(int *)scene = (int)_ZTV16dScMgSmartball_c;
        __cxa_vec_ctor(scene->mArray1, 5, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        __cxa_vec_ctor(scene->mArray2, 0x10, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        __cxa_vec_ctor(scene->mArray3, 0x10, 8, (void *)func_0203d738, (void *)NullDestructor_0203d47c);
        __cxa_vec_ctor(scene->mArray4, 0x40, 0x24, (void *)func_ov006_02119900, (void *)func_ov006_0210d894);
    }
    return scene;
}
