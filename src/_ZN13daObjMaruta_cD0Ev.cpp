//cpp
// @symbol _ZN13daObjMaruta_cD0Ev
/* recovered: real C++ deleting destructor -- forced out of line via the key function
 *
 * daObjMaruta_c.h defines `~daObjMaruta_c() {}` in the class body on purpose,
 * so no TU that merely includes the header emits _ZN13daObjMaruta_cD0Ev --
 * but the ROM carries one at ov080 0x02126fbc. The vtable is what needs the
 * deleting destructor as a symbol, and the vtable is emitted by the TU that
 * defines the key function (OnHitByMegaChar, the first non-inline virtual --
 * include/daObjMaruta_c.h records the rule). Defining it here makes mwcc emit
 * _ZTV13daObjMaruta_c and the destructor variants alongside it, and
 * objisolate keeps only the D0 .text this file is bound to, discarding the
 * duplicate OnHitByMegaChar body and the .data
 * (notes/dtor-migration.md section 3).
 */
#include "daObjMaruta_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. The key
 * function below is only a device to make mwcc emit the vtable and the
 * destructor pair; the real OnHitByMegaChar is its own TU
 * (src/_ZN13daObjMaruta_c15OnHitByMegaCharER6Player.cpp) and is already in the
 * port's link, so this file cannot define it again (LNK2005). The name the
 * host needs is what slot 17 of _ZTV13daObjMaruta_c holds (ov080 0x0212837c),
 * and the port hosts that table as an array (hal/actor_classes_ov080.cpp).
 * This arm spells the ROM's own D0 (ov080 0x02126fbc, 0x58 bytes) step for
 * step: the same stores and member destructors as the D1 at 0x02127014 -- this
 * class's own table, dBgActor_c's, the MovingMeshCollider at +0x124, the Model
 * at +0xd4, the dActor_c base D2 -- then the class's inline operator delete,
 * Memory::Deallocate on the game heap word data_020a0eac. Nothing here reaches
 * mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" {
void _ZN10dBgW_KcMbgD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void *_ZN8dActor_cD2Ev(void *self);
extern void *_ZTV13daObjMaruta_c[];
extern void *_ZTV10dBgActor_c[];
}
extern "C" daObjMaruta_c *_ZN13daObjMaruta_cD0Ev(daObjMaruta_c *thiz)
{
    char *t = (char *)thiz;
    /* THE WORD IS VOLATILE ON PURPOSE. The two table stores are back to
       back, and MSVC deletes the first as a dead store (then folds this
       body with the other class's identical one); the ROM makes both, so
       both are made here. */
    void *volatile *vptr = (void *volatile *)t;
    *vptr = (void *)_ZTV13daObjMaruta_c;        /* this class's own table */
    *vptr = (void *)_ZTV10dBgActor_c;           /* the inlined ~dBgActor_c's */
    _ZN10dBgW_KcMbgD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    daObjMaruta_c::operator delete(thiz);       /* the D0's own deallocation */
    return thiz;
}
#else

void daObjMaruta_c::OnHitByMegaChar(Player &player)
{
}
#endif
