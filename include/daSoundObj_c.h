#ifndef DASOUNDOBJ_C_H
#define DASOUNDOBJ_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daSoundObj_c -- daSoundObj_c_classInit (was daSoundObj_c_Spawn),
 *       g_profile_SOUND_OBJ (was daSoundObj_c_SpawnInfo)
 */

#include "types.h"
#include "dActor_c.h"

/* THE SIZE HAS ONE WITNESS AND THE OFFSETS HAVE ANOTHER, and they are different
 * witnesses -- worth stating plainly, because this class initializes nothing in
 * its factory and destroys nothing in its destructor.
 *
 *   SIZE 0xe4 is daSoundObj_c_classInit's own literal: `mov r0, #0xe4` into
 *   fBase_c::operator new. All twelve words of that 0x30-byte function are new,
 *   the dActor_c base constructor, the vptr store and the return -- it touches
 *   no member of its own. `~daSoundObj_c` is 0x24 bytes, one vptr store and the
 *   tail into ~dActor_c, and destroys nothing. Neither one witnesses a field.
 *
 *   THE OFFSETS come from the three functions that actually read them:
 *   InitResources writes 0xd4/0xd8/0xde/0xe0 from the 0xc-stride table at
 *   data_ov002_0210c080 and clears 0xdc, and the two callbacks
 *   func_ov002_020f93a8 / _020f9468 read 0xd4/0xd8/0xde/0xe0 off `this` and take
 *   0xdc by address. pad_0e1[0x3] is chosen to reach the factory's 0xe4; it is
 *   padding to a known total, not itself evidence of a field boundary.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c: the two tables are
 * 31 words each and differ in exactly slots 0, 6, 16 and 17 -- InitResources,
 * Behavior, D1 and D0, precisely the four members declared below. Every other
 * slot holds the base's own word and is inherited, so it is deliberately not
 * redeclared here.
 *
 * FIELD NAMES ARE A READING, NOT PINNED BY THE ROM the way the offsets are.
 * 0xd4 and 0xd8 are named from their use: both are handed straight to
 * Sound::PlaySub as its soundID and vol arguments, and 0xd8's complement drives
 * the distance argument as (0x7f - vol) << 12, 0x7f being the max-volume
 * constant used throughout. 0xdc/0xde are a counter and the limit it is
 * compared against. Read the 0xc-stride table as {soundID s32, volume s32,
 * count u16, flag u8} per row.
 */
struct daSoundObj_c : dActor_c {
    u8  pad_0d0[0x4];
    s32 mSoundID;                /* 0x0d4 -- PlaySub arg 1 */
    s32 mVolume;                 /* 0x0d8 -- PlaySub arg 2; (0x7f - it) << 12 is arg 4 */
    u16 mCounter;                /* 0x0dc */
    u16 mCounterLimit;           /* 0x0de */
    u8  unk_0e0;                 /* 0x0e0 -- PlaySub's last arg */
    u8  pad_0e1[0x3];

    /* INLINE, AND THAT IS WHAT LETS THIS CLASS OWN A TRANSLATION UNIT. Declared
       out of line, mwccarm 2004/b56 emits D0 before D1 -- the reverse of the
       cartridge, which has D1 at 0x020f934c and D0 at 0x020f9370 -- and adds a
       third, homeless D2 that no ROM address claims; tools/objisolate.py then
       refuses the whole TU rather than one function. Defined here, the pair
       comes out in ROM order and there is no D2 at all.

       SAFE ONLY BECAUSE THIS CLASS IS A LEAF, and that is measured rather than
       assumed: _ZTI12daSoundObj_c, ov002 0x0210c048, occurs exactly once as a
       word anywhere under extracted/ -- at ov002 file offset 0x5ea78, which is
       0x0210c0d8, the typeinfo slot of its own vtable header at _ZTV-4. No other
       class's __si_class_type_info points at it, so no descendant exists to
       inline this body where the ROM has a `bl`.

       THE BODY IS EMPTY, and the ROM agrees. D1 at 0x020f934c is 0x24 bytes
       and disassembles to exactly `push {r4,lr} / ldr r1,[pc] / mov r4,r0 /
       str r1,[r4] / bl _ZN8dActor_cD2Ev / mov r0,r4 / pop / bx lr`: ONE vptr
       store of its own, then the tail into the base destructor, which does the
       rest of the chain itself. There is no member destructor call to account
       for, because every member of this class is a scalar. (~dActor_c is not
       empty -- it stores two more vptrs and destroys its own fLiNdBa_c member
       at 0x50 -- but none of that belongs to this class or to this TU.) */
    virtual ~daSoundObj_c() {}          /* slots 16 (D1), 17 (D0) */

    /* Both are overrides of fBase_c, not new members -- see include/fBase_c.h,
       which declares InitResources at slot 0 and Behavior at slot 6. `virtual`
       is redundant to the compiler and costs no codegen; it is written so a
       reader can tell these two from a plain member. */
    virtual int InitResources();        /* slot 0 */
    virtual int Behavior();             /* slot 6 */
};

typedef char daSoundObj_c_size_must_be_0xe4[sizeof(daSoundObj_c) == 0xe4 ? 1 : -1];

#endif /* DASOUNDOBJ_C_H */
