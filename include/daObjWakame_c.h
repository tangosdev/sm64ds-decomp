#ifndef DAOBJWAKAME_C_H
#define DAOBJWAKAME_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daObjWakame_c -- daObjWakame_c_classInit (was daObjWakame_c_Spawn),
 *       g_profile_WAKAME (was daObjWakame_c_SpawnInfo)
 */

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"

/* An animated seaweed prop -- ov002/daObjWakame_c, a scenery model and nothing
 * else.
 *
 * THE SIZE HAS TWO WITNESSES AND THEY CLOSE ON EACH OTHER, which is the whole
 * reason this layout can be stated flatly rather than hedged.
 *
 *   SIZE 0x138 is daObjWakame_c_classInit's own literal: `mov r0, #312` into
 *   fBase_c::operator new. The factory then runs dActor_c's constructor, stores
 *   the vptr, and constructs ONE member, at this+0xd4.
 *
 *   THE MEMBER CLOSES ON IT. include/ModelAnim.h asserts sizeof(ModelAnim) ==
 *   0x64, and 0xd4 + 0x64 = 0x138 exactly. Two independent numbers -- a factory
 *   literal in ov002 and a size assert on an arm9 class -- meet with nothing
 *   left over, so there is no room for a trailing field this header has missed.
 *
 * THE MEMBER'S OFFSET HAS FOUR WITNESSES, all of them code that runs:
 *   - the factory calls _ZN9ModelAnimC1Ev on this+0xd4;
 *   - ~daObjWakame_c calls _ZN9ModelAnimD1Ev on this+0xd4 (see below);
 *   - Render loads a vptr from this+0xd4 and dispatches slot 5 through it,
 *     which is Model::Render -- so +0xd4 is a polymorphic object, not bytes;
 *   - Behavior calls Animation::Advance (arm9 0x02015c3c) on this+0x124, and
 *     0x124 - 0xd4 = 0x50, which include/ModelAnim.h pins as exactly where the
 *     Animation base sits inside a ModelAnim. The second base lands where the
 *     class says it should.
 *
 * pad_0d0[0x4] IS AN UNKNOWN FIELD, NOT ALIGNMENT. dActor_c asserts its own
 * size at 0xd0 and ModelAnim needs no more than 4-byte alignment, so those four
 * bytes are a gap the ROM leaves and nothing in this TU reads or writes. It is
 * named pad_ because its purpose is unknown, not because it is known to be
 * padding.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (arm9 0x0208e3a4).
 * Both tables are 31 words, and they differ in exactly slots 0, 3, 6, 9, 16 and
 * 17 -- InitResources, CleanupResources, Behavior, Render, D1 and D0, precisely
 * the six members declared below. Every other slot holds the base's own word and
 * is inherited, so it is deliberately not redeclared here.
 *
 * THE CLASS NAME IS THE CARTRIDGE'S, not coined. ov002 0x02109b78 is a
 * __si_class_type_info whose _ZTS at 0x02109b84 reads exactly `13daObjWakame_c`
 * and whose one base, at subobject offset 0, is arm9 0x0208e390 = _ZTI8dActor_c.
 */
struct daObjWakame_c : dActor_c {
    u8        pad_0d0[0x4];
    ModelAnim mModelAnim;               /* 0x0d4 -- model and its animation */

    /* INLINE, AND THAT IS WHAT LETS THIS CLASS OWN A TRANSLATION UNIT. Declared
       out of line, mwccarm 2004/b56 emits D0 before D1 -- the reverse of the
       cartridge, which has D1 at 0x020bc414 and D0 at 0x020bc444 -- and adds a
       third, homeless D2 that no ROM address claims; tools/objisolate.py then
       refuses the whole TU rather than one function. Defined here, the pair comes
       out in ROM order and there is no D2 at all.

       SAFE ONLY BECAUSE THIS CLASS IS A LEAF, and that is measured rather than
       assumed: _ZTI13daObjWakame_c, ov002 0x02109b78, occurs exactly once as a
       word anywhere under extracted/ -- at ov002 file offset 0x5c554, which is
       0x02109bb4, the typeinfo slot of its own vtable header at _ZTV-4 (the
       address point in symbols.txt is 0x02109bb8). The scan reports two hits
       because extracted/dsd/arm9_overlays/ov002.bin and
       extracted/overlays/overlay_0002.bin are the same image under two paths. No
       other class's __si_class_type_info points at it, so no descendant exists
       to inline this body where the ROM has a `bl`.

       THE BODY IS EMPTY, WHICH IS NOT THE SAME AS THE DESTRUCTOR BEING EMPTY.
       D1 at 0x020bc414 is 0x30 bytes and disassembles to `push {r4,lr} / mov r4,
       r0 / ldr r1,[pc] / add r0,r4,#0xd4 / str r1,[r4] / bl _ZN9ModelAnimD1Ev /
       mov r0,r4 / bl _ZN8dActor_cD2Ev / mov r0,r4 / pop / bx lr`: one vptr store,
       then the member destroyed, then the tail into the base destructor. All
       three are things the compiler emits for an empty body -- the member call
       BECAUSE mModelAnim has a destructor of its own, which is exactly why this
       class's D1 is 0x30 bytes where a scalar-only sibling's is 0x24. Writing
       anything between the braces would add code the cartridge does not have. */
    virtual ~daObjWakame_c() {}         /* slots 16 (D1), 17 (D0) */

    /* All four are overrides of fBase_c/dActor_c, not new members -- see the
       vtable diff above. `virtual` is redundant to the compiler and costs no
       codegen; it is written so a reader can tell these from a plain member. */
    virtual s32 InitResources();        /* slot 0 -- 0x020bc540 */
    virtual s32 CleanupResources();     /* slot 3 -- 0x020bc4c8 */
    virtual s32 Behavior();             /* slot 6 -- 0x020bc520 */
    virtual s32 Render();               /* slot 9 -- 0x020bc4f8 */
};

typedef char daObjWakame_c_size_must_be_0x138[
    sizeof(daObjWakame_c) == 0x138 ? 1 : -1];

#endif /* DAOBJWAKAME_C_H */
