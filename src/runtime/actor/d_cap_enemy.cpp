//cpp
/* dCapEnemy_c -- the ov002 half of the class. dCapEnemy_c's members do not
 * all live in one module: the ten ordinary members, the constructor and D1
 * are in arm9 (0x02005d94..0x02006588); only the base-object destructor D2
 * (0x020aedbc) and the deleting destructor D0 (0x020aedf4) are here.
 *
 * The destructor is never defined in this TU: a single out-of-line
 * ~dCapEnemy_c() would emit the whole D1/D2/D0 group, and the D1 copy --
 * homed in arm9 at 0x0200651c -- comes out STB_GLOBAL, which isolation will
 * not deadstrip as a duplicate. D2 and D0 are spelled out below as two
 * extern "C" ABI bodies instead, D0 written first so mwccarm's reverse
 * emission puts D2 below D0 as the cartridge has it. With no key function
 * defined here, no vtable or RTTI is emitted in this TU.
 *
 * Which copy is which is settled by _ZTV11dCapEnemy_c (0x021082c4): slot 16
 * holds 0x0200651c (D1) and slot 17 holds 0x020aedf4 (D0). D2 has no vtable
 * word and is reached only by `bl` from the derived cap enemies'
 * destructors tearing down their base sub-object.
 *
 * leftovers:
 *  - Both destructors keep extern "C" ABI bodies end to end: the vptr
 *    store has no member spelling; member-destructor calls on the members
 *    emit vtable-dispatch relocs instead of the direct D1 branches the
 *    cartridge has; self->dEnemyBase_c::~dEnemyBase_c() emits D1 where the
 *    chain needs D2; and an out-of-line ~dCapEnemy_c() emits the extra
 *    STB_GLOBAL D1 that isolation will not deadstrip.
 */
#include "dCapEnemy_c.h"
#include "decl_common.h"
#include "decl_Model.h"

extern "C" void _ZN10dCapIcon_cD1Ev(void *);
extern "C" void _ZN12dEnemyBase_cD2Ev(void *);

// @symbol _ZN11dCapEnemy_cD0Ev
/* The deleting destructor: store this vtable, destroy the members in reverse
 * declaration order (CapIcon at 0x164, Model at 0x114), chain to
 * dEnemyBase_c's base-object destructor, then hand the object back to the
 * actor heap. */
extern "C" dCapEnemy_c *_ZN11dCapEnemy_cD0Ev(dCapEnemy_c *self)
{
    *(int *)self = (int)_ZTV11dCapEnemy_c;
    _ZN10dCapIcon_cD1Ev(&self->mCapIcon);
    _ZN5ModelD1Ev(&self->mModel);
    _ZN12dEnemyBase_cD2Ev(self);
    _ZN6Memory10DeallocateEPvPN5mHeap6Heap_tE(self, data_020a0eac);
    return self;
}

// @symbol _ZN11dCapEnemy_cD2Ev
/* The base-object variant, the one every derived cap enemy chains to. It is
 * byte-identical to D1 at arm9 0x0200651c modulo the three relocated `bl`
 * words -- dCapEnemy_c has no virtual bases, so D1 and D2 share one body. */
extern "C" dCapEnemy_c *_ZN11dCapEnemy_cD2Ev(dCapEnemy_c *self)
{
    *(int *)self = (int)_ZTV11dCapEnemy_c;
    _ZN10dCapIcon_cD1Ev(&self->mCapIcon);
    _ZN5ModelD1Ev(&self->mModel);
    _ZN12dEnemyBase_cD2Ev(self);
    return self;
}
