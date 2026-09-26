//cpp
// @symbol _ZN16daObjFallBlock_cD0Ev
/* recovered: real C++ deleting destructor -- forced out of line via the key function
 *
 * daObjFallBlock_c.h defines `~daObjFallBlock_c() {}` in the class body on
 * purpose, so no TU that merely includes the header emits
 * _ZN16daObjFallBlock_cD0Ev -- but the ROM carries one at ov098 0x02139f70.
 * The vtable is what needs the deleting destructor as a symbol, and the
 * vtable is emitted by the TU that defines the key function (Kill, the first
 * non-inline virtual). Defining it here makes mwcc emit
 * _ZTV16daObjFallBlock_c and the destructor variants alongside it, and
 * objisolate keeps only the D0 .text this file is bound to, discarding the
 * duplicate Kill body and the .data (notes/dtor-migration.md section 3).
 */
#include "daObjFallBlock_c.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D0 NAME, AND MSVC NEVER EMITS IT. The key
 * function below is only a device to make mwcc emit the vtable and the
 * destructor pair; the real Kill is in the promoted TU
 * (src/actors/daObjFallBlock_c.cpp) and is already in the port's link, so
 * this file cannot define it again (LNK2005). The name the host needs is what
 * slot 17 of _ZTV16daObjFallBlock_c holds (ov098 0x0213c600), and the port
 * hosts that table as an array (hal/actor_classes_ov098.cpp). This arm spells
 * the ROM's own D0 (ov098 0x02139f70, 0x58 bytes) step for step: the same
 * stores and member destructors as the D1 at 0x02139fc8 -- this class's own
 * table, dBgActor_c's, the MovingMeshCollider at +0x124, the Model at +0xd4,
 * the dActor_c base D2 -- then the class's inline operator delete,
 * Memory::Deallocate on the game heap word data_020a0eac. Nothing here
 * reaches mwccarm: it builds the `#else` arm and emits the ROM bytes it always
 * emitted, and the object is byte-identical either way. */
extern "C" {
void _ZN10dBgW_KcMbgD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void *_ZN8dActor_cD2Ev(void *self);
extern void *_ZTV16daObjFallBlock_c[];
extern void *_ZTV10dBgActor_c[];
}
extern "C" daObjFallBlock_c *_ZN16daObjFallBlock_cD0Ev(daObjFallBlock_c *thiz)
{
    char *t = (char *)thiz;
    /* THE WORD IS VOLATILE ON PURPOSE. The two table stores are back to
       back, and MSVC deletes the first as a dead store (then folds this
       body with the other class's identical one); the ROM makes both, so
       both are made here. */
    void *volatile *vptr = (void *volatile *)t;
    *vptr = (void *)_ZTV16daObjFallBlock_c;        /* this class's own table */
    *vptr = (void *)_ZTV10dBgActor_c;              /* the inlined ~dBgActor_c's */
    _ZN10dBgW_KcMbgD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    daObjFallBlock_c::operator delete(thiz);       /* the D0's own deallocation */
    return thiz;
}
#else

void daObjFallBlock_c::Kill()
{
}
#endif
