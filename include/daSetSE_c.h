#ifndef DASETSE_C_H
#define DASETSE_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daSetSE_c -- daSetSE_c_classInit (was daSetSE_c_Spawn),
 *       g_profile_SET_SE (was daSetSE_c_SpawnInfo)
 */

#include "dActor_c.h"

/* SIZE 0xd8 is daSetSE_c_classInit's own literal -- it allocates 0xd8 and
 * constructs only the dActor_c base, touching no member of its own.
 *
 * THE ONE DERIVED FIELD is witnessed by Behavior, which reads 0xd4, passes it
 * to Sound::PlayLong as the handle argument and stores the result back: a
 * long-sound handle. The effect selector is not a member at all -- it is the
 * inherited param1 at 0x08. Field names are a reading; the offset is pinned.
 *
 * NOTHING INITIALIZES IT, and that is the cartridge's behaviour, not an
 * omission here. daSetSE_c_classInit allocates 0xd8 and constructs only the
 * dActor_c base -- it never touches 0xd4 -- so Behavior's first call reads
 * whatever fBase_c::operator new left there and hands it to PlayLong, which
 * treats a handle it does not recognise as "not playing". Do not add an
 * initializer: this class has no constructor of its own in the ROM, and giving
 * it one would emit code the cartridge does not have.
 */
struct daSetSE_c : dActor_c {
    u8  pad_0d0[0x4];
    u32 mSoundHandle;       /* 0x0d4 */

    /* INLINE, AND THAT IS WHAT LETS THIS CLASS OWN A TRANSLATION UNIT. Declared
       out of line, mwccarm 2004/b56 emits D0 before D1 -- the reverse of the
       cartridge, which has D1 at 0x020f198c and D0 at 0x020f19b0 -- and adds a
       third, homeless D2 that no ROM address claims; tools/objisolate.py then
       refuses the whole TU rather than one function. That is exactly the
       `functions_occur_in_expected_order: PARTIAL -- [(0, 1)]` this entry
       carried. Defined here, the pair comes out in ROM order and there is no
       D2 at all.

       SAFE ONLY BECAUSE THIS CLASS IS A LEAF, and that is measured rather than
       assumed: _ZTI9daSetSE_c, ov002 0x0210b470, occurs exactly once as a word
       anywhere under extracted/ -- at ov002 file offset 0x5de64, which is
       0x0210b4c4, the typeinfo slot of its own vtable header at _ZTV-4 (the
       address point in symbols.txt is 0x0210b4c8). No other class's
       __si_class_type_info points at it, so no descendant exists to inline this
       body where the ROM has a `bl`.

       THE BODY IS EMPTY, and the ROM agrees. D1 at 0x020f198c is 0x24 bytes
       and disassembles to exactly `push {r4,lr} / ldr r1,[pc] / mov r4,r0 /
       str r1,[r4] / bl _ZN8dActor_cD2Ev / mov r0,r4 / pop / bx lr`: ONE vptr
       store of its own, then the tail into the base destructor, which does the
       rest of the chain itself. There is no member destructor call to account
       for, because this class's one member is a u32. (~dActor_c is not empty --
       it stores two more vptrs and destroys its own fLiNdBa_c member at 0x50 --
       but none of that belongs to this class or to this TU.) */
    virtual ~daSetSE_c() {}         /* slots 16 (D1), 17 (D0) */

    /* All five are overrides of fBase_c/dActor_c, not new members. The slot
       numbers are measured: _ZTV9daSetSE_c and _ZTV8dActor_c are 31 words each
       and differ in exactly slots 0, 3, 6, 9, 12, 16 and 17, and each differing
       word is the address of the member named beside it. Every other slot holds
       the base's own word and is inherited, so it is deliberately not
       redeclared here. */
    virtual s32 InitResources();        /* slot 0  -- 0x020f1ac4 */
    virtual s32 CleanupResources();     /* slot 3  -- 0x020f19e8 */
    virtual s32 Behavior();             /* slot 6  -- 0x020f19fc */
    virtual s32 Render();               /* slot 9  -- 0x020f19f4 */
    virtual void OnPendingDestroy();    /* slot 12 -- 0x020f19f0 */
};

typedef char daSetSE_c_size_must_be_0xd8[
    sizeof(daSetSE_c) == 0xd8 ? 1 : -1];

#endif /* DASETSE_C_H */
