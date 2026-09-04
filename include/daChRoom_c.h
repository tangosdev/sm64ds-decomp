#ifndef DACHROOM_C_H
#define DACHROOM_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daChRoom_c -- daChRoom_c_classInit (was daChRoom_c_Spawn),
 *       g_profile_CH_ROOM (was daChRoom_c_SpawnInfo)
 */

#include "dActor_c.h"

/* An area-transition trigger box -- ov002/daChRoom_c, and the class the ROM
 * spells daChRoom_c rather than the coined VirtualDoor it carried until this
 * commit.
 *
 * THE SIZE IS THE FACTORY'S OWN LITERAL: daChRoom_c_classInit at 0x020b0980 is
 * `mov r0, #0xd4` into fBase_c::operator new, then the dActor_c base
 * constructor, the vptr store and the return. dActor_c ends at 0xd0, so the
 * class's own storage is the final four bytes and nothing in this TU reads
 * them -- pad_0d0[0x4] is padding to a size witness, not a field.
 *
 * EVERY VALUE THE CLASS USES IS INHERITED ACTOR STATE, which is why there are
 * no members here to name: InitResources writes mScaleX/mScaleY/mAngleY and
 * Behavior reads mPosX/mAngleY/mScaleX/mScaleY/mAngleX/mAngleZ, all of them
 * dActor_c's own fields at their own offsets. mScaleX and mScaleY are read as
 * the trigger box's half-width and height, mAngleX/mAngleZ as the two area ids
 * the box switches between depending on which side of it the player is.
 *
 * THE VTABLE WAS DIFFED SLOT BY SLOT against _ZTV8dActor_c (arm9 0x0208e3a4).
 * Both tables are 31 words and they differ in exactly slots 0, 3, 6, 9, 12, 16
 * and 17 -- InitResources, CleanupResources, Behavior, Render,
 * OnPendingDestroy, D1 and D0, precisely the five members declared below plus
 * the destructor pair. Every other slot holds the base's own word and is
 * inherited, so it is deliberately not redeclared here.
 */
struct daChRoom_c : dActor_c {
    u8 pad_0d0[0x4];

    /* INLINE, AND THAT IS WHAT LETS THIS CLASS OWN A TRANSLATION UNIT.
       Declared out of line, mwccarm 2004/b56 emits D0 before D1 -- the reverse
       of the cartridge, which has D1 at 0x020b07f8 and D0 at 0x020b081c -- and
       adds a third, homeless D2 that no ROM address claims; tools/objisolate.py
       then refuses the whole TU rather than one function. Defined here, the
       pair comes out in ROM order and there is no D2 at all.

       SAFE ONLY BECAUSE THIS CLASS IS A LEAF, measured rather than assumed:
       _ZTI10daChRoom_c, ov002 0x021085b8, occurs at exactly one logical place
       anywhere under extracted/ -- file offset 0x5af94 of ov002, which is
       0x021085f4, the typeinfo slot of its own vtable header at _ZTV-4 (the
       symbols.txt address 0x021085f8 is the address point). No other class's
       __si_class_type_info points at it, so no descendant exists to inline this
       body where the ROM has a `bl`.

       THE BODY IS EMPTY, and the ROM agrees. D1 at 0x020b07f8 is 0x24 bytes and
       disassembles to exactly `push {r4,lr} / ldr r1,[pc,#0x14] / mov r4,r0 /
       str r1,[r4] / bl 0x020112c8 / mov r0,r4 / pop {r4,lr} / bx lr`: ONE vptr
       store of its own, then the call into ~dActor_c, which does the rest of
       the chain itself. There is no member destructor call to account for,
       because this class declares no member at all -- contrast daObjWakame_c,
       whose equally empty inline body still compiles to 0x30 bytes because it
       has a ModelAnim to destroy. */
    virtual ~daChRoom_c() {}            /* slots 16 (D1), 17 (D0) */

    /* All five are overrides of fBase_c, not new members -- see
       include/fBase_c.h, which declares InitResources at slot 0,
       CleanupResources at 3, Behavior at 6, Render at 9 and OnPendingDestroy
       at 12. `virtual` is redundant to the compiler and costs no codegen; it is
       written so a reader can tell these from a plain member. */
    virtual s32 InitResources();        /* slot 0  -- 0x020b0938 */
    virtual s32 CleanupResources();     /* slot 3  -- 0x020b0854 */
    virtual s32 Behavior();             /* slot 6  -- 0x020b0868 */
    virtual s32 Render();               /* slot 9  -- 0x020b0860 */
    virtual void OnPendingDestroy();    /* slot 12 -- 0x020b085c */
};

typedef char daChRoom_c_size_must_be_0xd4[
    sizeof(daChRoom_c) == 0xd4 ? 1 : -1];

#endif /* DACHROOM_C_H */
