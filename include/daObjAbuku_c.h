#ifndef DAOBJABUKU_C_H
#define DAOBJABUKU_C_H
#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

/* TWO WITNESSES:
 *
 *   daObjAbuku_c_Spawn  fBase_c::operator new(276 = 0x114),
 *       dActor_c::dActor_c(), stores the class vtable, then the
 *       dCcAc_c member below.
 *   _ZN12daObjAbuku_cD0Ev  the same member destroyed, then ~dActor_c.
 *
 * SIZE 0x114 is the factory's own literal; unk_110 (4 bytes, 0x110) closes
 * exactly on it.
 *
 * RENAMED FROM "Bubble": each overlay's relocs.txt shows this class's own vtable
 * (found via the typeinfo relocation _ZTI12daObjAbuku_c <- vtable+4, the
 * same technique used tree-wide) filling its D1/D0 slots (16/17) with the
 * addresses the tree had previously named _ZN6BubbleD1Ev/_ZN6BubbleD0Ev --
 * an English guess made before the RTTI evidence (_ZTS12daObjAbuku_c, the
 * ROM's own class-name string) existed. Renamed to match the RTTI truth;
 * "Bubble" is also used unrelatedly by the real class LavaBubble.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names in the old generated header (mPosX/Y/Z at 0x5c/0x60/0x64 and
 * mHorzSpeed at 0x098 already share dActor_c's names). dActor_c ends at
 * exactly 0x0d0, so pad_0d0 (unevidenced, 4 bytes) is this class's own first
 * field. The old header also placed a field at 0x0f8, inside
 * dCcAc_c's own 0x0d4..0x108 span -- daObjAbuku_c_Behavior reads
 * that word directly out of the collision sub-object's own bytes, not out of
 * a field of this class, so no field is declared for it here. The old
 * header also padded 0x10c..0x10e as unevidenced, but
 * daObjAbuku_c_Behavior reads and writes it as a live 16-bit counter, so it
 * is a real field (unk_10c) here.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c: only slot 0
 * (InitResources) and slot 6 (Behavior) differ, both still fBase_c's own
 * slots in dActor_c -- CleanupResources, Render, OnPendingDestroy,
 * OnYoshiTryEat and OnTurnIntoEgg are all still the base's own words.
 * The readable src_tu shadow defines InitResources and Behavior as real member
 * functions and defines one ordinary destructor, from which mwccarm emits D1,
 * D0, D2, RTTI, and the vtable. Only the seven licensed text contributions are
 * currently used for verification. Production still keeps the pre-existing
 * one-function sources and ROM-supplied vtable/data until whole-TU ownership is
 * proven safe.
 */
struct daObjAbuku_c : dActor_c {
    u8  pad_0d0[0x4];
    /* dCcAc_c member, named by daObjAbuku_c_Spawn's own C1 call
       and the class's own destructors' D1 call at +0xd4.
       [daObjAbuku_c_Spawn.c, _ZN12daObjAbuku_cD1Ev.c, _ZN12daObjAbuku_cD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    s32 unk_108;            /* 0x108 */
    s16 unk_10c;            /* 0x10c */
    s16 unk_10e;            /* 0x10e */
    s32 unk_110;            /* 0x110 */

    virtual ~daObjAbuku_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  Behavior();         /* slot  6 */
};

typedef char daObjAbuku_c_size_must_be_0x114[sizeof(daObjAbuku_c) == 0x114 ? 1 : -1];

#endif
