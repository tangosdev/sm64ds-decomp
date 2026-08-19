// @symbol _ZN9daSCoin_cD0Ev
/* recovered: vtable identified, renamed to Class_Method, declarations from
 * a shared header. Was _ZN15InvisibleSecretD0Ev -- renamed to match the
 * RTTI class name (_ZTS9daSCoin_c), see include/daSCoin_c.h.
 * The DELETING DESTRUCTOR, vtable slot 17: stores the class vtable,
 * destroys the members and calls Memory::Deallocate. */
#include "decl_Actor.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
extern int _ZTV9daSCoin_c[];
extern void *data_020a0eac;
int *_ZN9daSCoin_cD0Ev(int *t)
{
    t[0] = (int)_ZTV9daSCoin_c;
    _ZN7dCcAc_cD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
