//cpp
/* Which Wiggler (MG_HANACHAN): the scene factory and the constructor for
 * the five 8-byte entries inside each of its fifteen 0x98-byte elements.
 * The scene's own methods are in src/actors/dScMgHanachan_c.cpp.
 *
 * The factory builds the object by hand (operator new, base constructor,
 * vtable word, array construction) because dScMgHanachan_c has no
 * constructor declared yet. data_ov006_0213cab8 is the vtable
 * (_ZTV15dScMgHanachan_c at the same address). The element and entry types
 * are not recovered, so their constructors keep linker names and the
 * entries stay at raw offset 0x18.
 */

#include "dScMgHanachan_c.h"

extern "C" {
extern int _ZN11dScMgBase_cC2Ev(void *p);
extern void __cxa_vec_ctor(void *obj, int a, int b, void *cb1, void *cb2);
extern int data_ov006_0213cab8[];
void *dScMgHanachan_c_classInit(void);
/* local extern: only sibling minigame headers (dScMgAmida_c.h and four
   others) declare it; this TU includes none of them */
extern void NullDestructor_0203d47c(void);
extern void func_0203d738(void);
}

// @symbol func_ov006_020ede80
extern "C" void* func_ov006_020ede80(char* elem)
{
    __cxa_vec_ctor(elem + 0x18, 5, 8, (void*)func_0203d738, (void*)NullDestructor_0203d47c);
    return elem;
}

// @symbol dScMgHanachan_c_classInit
extern "C" void *dScMgHanachan_c_classInit(void) {
    char *scene = (char *)fBase_c::operator new(0x4f68);
    if (scene != 0) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(int *)scene = (int)data_ov006_0213cab8;
        __cxa_vec_ctor(scene + 0x4678, 0xf, 0x98, (void *)func_ov006_020ede80, (void *)func_ov006_020ea324);
    }
    return scene;
}
