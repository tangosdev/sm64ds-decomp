//cpp
// @symbol _ZN11daTrsTrap_cD1Ev
#include "daTrsTrap_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D1 NAME, AND MSVC ONLY MAKES ONE DESTRUCTOR.
 * It folds the Itanium D1/D0 pair into the single ??1daTrsTrap_c that
 * src/game/actors/daTrsTrap_c/_ZN11daTrsTrap_cD0Ev.cpp compiles for the port,
 * so this file cannot carry the same definition (LNK2005). This arm spells the
 * ROM's own D1 instead, step for step: the store of this class's own table
 * (_ZTV11daTrsTrap_c), the MovingMeshCollider at +0x15c and the Model at +0xd4
 * by their flat ROM names, then the dActor_c base D2, and no deallocation
 * (slot 16's caller frees). Those are the calls the slot-16 thunk ms_d1 in
 * hal/actor_classes_ov063.cpp made by hand, which forwards here now. Nothing
 * here reaches mwccarm: it builds the `#else` arm and emits the ROM bytes it
 * always emitted, and the object is byte-identical either way. */
extern "C" {
void _ZN10dBgW_KcMbgD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void *_ZN8dActor_cD2Ev(void *self);
extern void *_ZTV11daTrsTrap_c[];
}
extern "C" daTrsTrap_c *_ZN11daTrsTrap_cD1Ev(daTrsTrap_c *thiz)
{
    char *t = (char *)thiz;
    *(void **)t = (void *)_ZTV11daTrsTrap_c;  /* this class's own table */
    _ZN10dBgW_KcMbgD1Ev(t + 0x15c);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN8dActor_cD2Ev(t);                      /* the base, no deallocation */
    return thiz;
}
#else

daTrsTrap_c::~daTrsTrap_c()
{
}
#endif
