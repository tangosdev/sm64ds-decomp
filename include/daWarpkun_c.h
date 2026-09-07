#ifndef DAWARPKUN_C_H
#define DAWARPKUN_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daWarpkun_c -- daWarpkun_c_classInit (was daWarpkun_c_Spawn),
 *       g_profile_WARPKUN (was daWarpkun_c_SpawnInfo)
 */

#include "types.h"
#include "dActor_c.h"
#include "dCcAc_c.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   daWarpkun_c_classInit  fBase_c::operator new(264 = 0x108), dActor_c::dActor_c(), stores _ZTV11daWarpkun_c,
 *                 then the member below in this order.
 *   ~daWarpkun_c   the same member destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x108 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daWarpkun_c : dActor_c {
    u8  pad_0d0[0x4];
    dCcAc_c     mdCcAc_c; /* 0x0d4 */

    /* INLINE, AND THAT IS WHAT LETS THIS CLASS OWN A TRANSLATION UNIT. Declared
       out of line, mwccarm 2004/b56 emits D0 before D1 -- the reverse of the
       cartridge, which has D1 at 0x020ec388 and D0 at 0x020ec3b8 -- and adds a
       third, homeless D2 that no ROM address claims; tools/objisolate.py then
       refuses the whole TU rather than one function. (The manifest's old
       compiler_only_output licensed exactly that D2 as `deadstrip`; defined
       here, there is no D2 to license.)

       SAFE ONLY BECAUSE THIS CLASS IS A LEAF, and that is measured rather than
       assumed: _ZTI11daWarpkun_c, ov002 0x0210ac7c, occurs at exactly one
       logical place anywhere under extracted/ -- ov002 file offset 0x5d658,
       which is 0x0210acb8, the typeinfo slot of its own vtable header at
       _ZTV-4. No other class's __si_class_type_info points at it, so no
       descendant exists to inline this body where the ROM has a `bl`.

       THE BODY IS EMPTY BUT THE DESTRUCTOR IS NOT. D1 at 0x020ec388 is 0x30
       bytes -- 0xc more than a memberless class's 0x24 -- and disassembles to
       `push {r4,lr} / mov r4,r0 / ldr r1,[pc,#0x1c] / add r0,r4,#0xd4 /
       str r1,[r4] / bl _ZN7dCcAc_cD1Ev / mov r0,r4 / bl _ZN8dActor_cD2Ev /
       mov r0,r4 / pop {r4,lr} / bx lr`: the vptr store, then the member
       subobject at 0xd4 destroyed, then the tail into the base. Those extra
       three instructions are the whole reason mdCcAc_c must be declared as a
       real dCcAc_c below and not as padding -- write it as padding and the
       compiler emits the 0x24-byte shape instead, which is a different
       function from the cartridge's. The empty braces are correct: the
       compiler generates the member call itself, and there is nothing else to
       do. */
    virtual ~daWarpkun_c() {}          /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
    virtual void  OnPendingDestroy();      /* slot 12 */
};

typedef char daWarpkun_c_size_must_be_0x108[sizeof(daWarpkun_c) == 0x108 ? 1 : -1];

#endif /* DAWARPKUN_C_H */
