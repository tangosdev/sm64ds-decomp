//cpp
/* dCapEnemy_c -- the ov002 half of the class. dCapEnemy_c's members do not
 * all live in one module: the ten ordinary members, the constructor and D1
 * are in arm9 (0x02005d94..0x02006588, src/runtime/actor/d_cap_enemy_arm9.cpp);
 * only the base-object destructor D2 (0x020aedbc) and the deleting
 * destructor D0 (0x020aedf4) are here.
 *
 * The destructor is defined out of line, so it is this TU's key function:
 * mwccarm emits D2, D0 and D1, then the vtable and the RTTI chain. D2 and D0
 * are the cartridge's ov002 bodies, in ROM order. D1 duplicates the arm9 copy
 * at 0x0200651c and the vtable and RTTI records have their homes in the
 * cartridge's data, so the manifest discards those copies against their
 * canonical addresses.
 *
 * Which copy is which is settled by _ZTV11dCapEnemy_c (0x02108284): slot 16
 * holds 0x0200651c (D1) and slot 17 holds 0x020aedf4 (D0). D2 has no vtable
 * word and is reached only by `bl` from the derived cap enemies'
 * destructors tearing down their base sub-object.
 */
#include "dCapEnemy_c.h"

/* Destroys the members in reverse declaration order (the CapIcon at 0x164,
 * then the dExtSimpleModel_c at 0x114) and chains to dEnemyBase_c. */
dCapEnemy_c::~dCapEnemy_c() {}
