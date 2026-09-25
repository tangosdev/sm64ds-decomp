#ifndef DAOBJFL_RING_C_H
#define DAOBJFL_RING_C_H

#include "types.h"
#include "dBgActor_c.h"

/* Lethal Lava Land's rotating fire ring (registry profile FL_RING, actor 0xf3's
 * parent): a dBgActor_c that spins about Y, and on a timer spawns the flame
 * actor that rides it.
 *
 * NAME: daObjFl_Ring_c is the cartridge's own RTTI spelling. _ZTS at ov022
 * 0x02113ce0 is the byte string "14daObjFl_Ring_c", and _ZTI at 0x02113cd4
 * points at that string and at _ZTI10dBgActor_c, so the ROM states the direct
 * base too. The vtable symbol ov022/symbols.txt spells _ZTV14daObjFl_Ring_c
 * (0x02113d18) carries a relocation at its address point minus 4 -- ov022
 * relocs.txt `from:0x02113d14 to:0x02113cd4` -- which is that same _ZTI, so
 * the three records are one triple and this is the class the ROM names.
 *
 * TWO WITNESSES, and they close on each other:
 *
 *   daObjFl_Ring_c_classInit  fBase_c::operator new(808 = 0x328), dBgActor_c::dBgActor_c(), stores _ZTV14daObjFl_Ring_c,
 *                 then the members below in this order.
 *   ~daObjFl_Ring_c   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x328 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjFl_Ring_c : dBgActor_c {
    /* dBgActor_c ends at 0x31e (its own sizeof rounds 0x31e up to 0x320);
       these two bytes are this class's own, in the base's tail padding --
       same shape as daObjRc_Guruguru_c's single s16 there (see
       include/dBgActor_c.h). Field names are placeholders: offsets, widths
       and read/write sites are observed from Behavior (slot 6, ov022
       0x021112ac), not proven semantics. */
    u8  mState;            /* 0x31e -- Behavior's own two-case switch */
    u8  mSpawnGuard;        /* 0x31f -- DecIfAbove0_Byte-gated once per spawn */
    u8  mTriggered;        /* 0x320 -- read once, reset to 0 every Behavior call */
    u8  pad_321[0x1];
    u16 mCooldown;          /* 0x322 -- DecIfAbove0_Short, gates the whole switch */
    u16 mSpawnCount;        /* 0x324 -- incremented once per actorID 0xf3 spawn */
    u8  pad_326[0x2];

    /* MEASURED -- DEFINED INLINE, EMPTY, AND DECLARED FIRST. This is the key
       function, so the TU that defines it emits _ZTV14daObjFl_Ring_c together
       with _ZTI14daObjFl_Ring_c and _ZTS14daObjFl_Ring_c as vague linkage, and
       drags the inherited bases' records along. All of them are the
       cartridge's own spellings and carry configured ROM homes, so they
       license as deadstrip-data and the six-function run isolates as one
       object.

       Inline and empty is what puts D1 ahead of D0, the cartridge's order
       (0x021111a0 then 0x021111e4): written out of line mwccarm emits the
       synthesized D0 first, which isolation refuses, and it also emits a D2
       this class has no home for. The brace stays on the signature line --
       tools/check_header_offsets.py recognises an inline body only when the
       signature line carries it. */
    virtual ~daObjFl_Ring_c() {}     /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();     /* slot  0 */
    virtual s32 CleanupResources();  /* slot  3 */
    virtual s32 Behavior();          /* slot  6 */
    virtual s32 Render();            /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_Ring_c_size_must_be_0x328[sizeof(daObjFl_Ring_c) == 0x328 ? 1 : -1];
#endif

#endif /* DAOBJFL_RING_C_H */
