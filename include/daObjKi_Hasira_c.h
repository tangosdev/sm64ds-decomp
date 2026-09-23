#ifndef DAOBJKI_HASIRA_C_H
#define DAOBJKI_HASIRA_C_H

#include "types.h"
#include "dBgActor_c.h"

/* NAME: daObjKi_Hasira_c is the cartridge's own spelling (Jolly Roger Bay's
 * falling stone pillar, KI_HASIRA; `hasira` = pillar). Read out of
 * extracted/overlays/overlay_0016.bin (ov016 loads at 0x021111a0):
 *
 *   _ZTV address point 0x02114b00: word at -8 is 0 (offset-to-top), word at
 *       -4 is 0x02114abc -- the _ZTI below (ov016 relocs.txt
 *       from:0x02114afc kind:load to:0x02114abc).
 *   _ZTI 0x02114abc: [__si_class_type_info+8 (0x0209a764), 0x02114ac8,
 *       _ZTI10dBgActor_c (ov002 0x021089ec)] -- one base, dBgActor_c.
 *   _ZTS 0x02114ac8: "16daObjKi_Hasira_c\0".
 *
 * The tree called this class RockPillar until the vtable joined the _ZTI/_ZTS
 * pair under the ROM name. The 32-slot table is dBgActor_c's length, a second
 * witness for the base the _ZTI names.
 *
 * TWO WITNESSES for the layout, and they close on each other:
 *
 *   daObjKi_Hasira_c_classInit  fBase_c::operator new(808 = 0x328), dBgActor_c::dBgActor_c(), stores _ZTV16daObjKi_Hasira_c,
 *                 then the members below in this order.
 *   ~daObjKi_Hasira_c   the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x328 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct Player;

struct daObjKi_Hasira_c : dBgActor_c {
    /* dBgActor_c ends at 0x31e (its own sizeof rounds 0x31e up to 0x320);
       the first two fields live in the base's tail padding. */
    u8  mState;             /* 0x31e -- 0 wait for a player, 1 turn to face, 2 fall */
    u8  mTurnTimer;         /* 0x31f -- frames spent turning before the fall */
    s16 mFallSpeed;         /* 0x320 -- added to mAngleX each falling frame */
    s16 mFallAccel;         /* 0x322 -- grows by 4 each falling frame */
    Player *mTarget;        /* 0x324 -- ClosestPlayer, cached while waiting */

    /* MEASURED -- DEFINED INLINE, EMPTY, AND DECLARED FIRST. The destructor is
       the key function, so the TU that defines it emits _ZTV16daObjKi_Hasira_c,
       _ZTI16daObjKi_Hasira_c and _ZTS16daObjKi_Hasira_c as vague linkage with
       the inherited bases' records; all carry configured ROM homes.

       Inline and empty is what puts D1 ahead of D0, the cartridge's order
       (0x02112a00 then 0x02112a44): written out of line mwccarm emits the
       synthesized D0 first and a D2 this class has no home for. Same lever as
       the ov022 dBgActor_c leaves daObjFl_Ring_c and daObjFl_Block_c. The
       brace stays on the signature line -- tools/check_header_offsets.py
       recognises an inline body only when the signature line carries it. */
    virtual ~daObjKi_Hasira_c() {}   /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();     /* slot  0 */
    virtual s32 CleanupResources();  /* slot  3 */
    virtual s32 Behavior();          /* slot  6 */
    virtual s32 Render();            /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjKi_Hasira_c_size_must_be_0x328[sizeof(daObjKi_Hasira_c) == 0x328 ? 1 : -1];
#endif

#endif /* DAOBJKI_HASIRA_C_H */
