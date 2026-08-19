#ifndef DOOR_H
#define DOOR_H

#include "types.h"

#ifdef __cplusplus
#include "dActor_c.h"
#endif

/* The plain warp door -- the leaf dActor_c child at ov100 0x021443f4..0x02145948,
 * distinct from StarDoor and VirtualDoor which are their own classes/headers.
 *
 * DERIVATION. tools/rtti_extract.py has the RTTI record at ov100 0x02148158,
 * mangled "8daDoor_c" -- the ROM struct name the tree's generated header
 * (include/daDoor_c.h) still carries -- with ONE base, dActor_c, at
 * subobject offset 0. It is a leaf: nothing in the image derives from it.
 *
 * VTABLE. data_02148188 (_ZTV4Door) is 31 slots, the same count as dActor_c's
 * own table -- confirmed with tools/rtti_vtables.py --own daDoor_c, which also
 * shows the destructor pair already migrated under the coined name "Door"
 * (_ZN4DoorD1Ev / _ZN4DoorD0Ev, ov100 0x021443f4 / 0x02144424) by earlier
 * work, before this class had its own header. This class overrides five
 * slots beyond the destructor:
 *
 *   0   InitResources      ov100 0x021455a0  (src/_ZN4Door13InitResourcesEv.c)
 *   3   CleanupResources   ov100 0x0214542c  (src/_ZN4Door16CleanupResourcesEv.cpp)
 *   6   Behavior           ov100 0x02145550  (src/_ZN4Door8BehaviorEv.cpp)
 *   9   Render             ov100 0x021454c8  (src/_ZN4Door6RenderEv.cpp)
 *   12  OnPendingDestroy   ov100 0x021454c4  (src/_ZN4Door16OnPendingDestroyEv.c)
 *
 * (config/arm9/overlays/ov100/relocs.txt: 0x02148188/0x02148194/0x021481a0/
 * 0x021481ac/0x021481b8 -- the vtable words at slots 0/3/6/9/12 -- each load
 * exactly the addresses above.) All five bytes still match; only the symbol
 * NAMES were placeholders (func_ov100_0214xxxx), now renamed.
 *
 * NOT CONVERTED TO REAL METHODS BY THIS PASS. Each of the five sources above
 * is still defined as a free function taking the object pointer explicitly,
 * the same idiom src/_ZN7fBase_c13InitResourcesEv.cpp uses for fBase_c's own
 * slot 0: declared here as a virtual override (so the header documents the
 * vtable completely) but never given a `Door::` definition anywhere, so
 * nothing about their bodies or the surrounding field-offset arithmetic
 * (still through include/daDoor_c.h, the flat placeholder, which stays in
 * place and unrenamed) had to change to land the correct mangled symbol.
 * Only src/_ZN4Door13InitResourcesEv.c was ALREADY NONMATCHING before this
 * rename -- config/arm9/overlays/ov100/delinks.txt never carried a `complete`
 * marker for it (999-word diff under the pinned 2004/b56, reproduced on the
 * untouched tree before editing) -- and stays that way; the other four were
 * already byte-verified and are re-verified unchanged under their new names.
 *
 * SIZE. Door_Spawn.c calls `_ZN7fBase_cnwEj(328)` -- 0x148 -- for a fresh Door,
 * then _ZN8dActor_cC2Ev and _ZN9ModelAnimC1Ev at +0xd4. dActor_c is 0xd0
 * (include/dActor_c.h) and ModelAnim is 0x64 (include/ModelAnim.h), so the
 * embedded ModelAnim runs 0xd4..0x138 (the same 4-byte alignment pad
 * include/dBgActor_c.h takes before its own Model member). That leaves
 * 0x138..0x147 (0x10 = 16 bytes) as this class's own storage, all of it
 * touched by the five sources above: two heap-owned pointers at 0x138/0x13c
 * (a second Model*, and something released through SharedFilePtr), a
 * callback-node pointer at 0x140 (read in Behavior as a pointer-to-member
 * dispatch, written in src/func_ov100_021453d8.cpp -- out of this slice), and
 * a key-model index byte at 0x144 -- exactly matching include/daDoor_c.h's
 * existing `unk_144`. None of the four pointer/byte fields were typed more
 * precisely than the existing daDoor_c.h placeholder already has them,
 * because the two Model-shaped virtual calls through 0x138 (CleanupResources'
 * `obj->v1()`, Render's `o->n(...)`/`o2->m(...)`) resolve to unidentified
 * Model vtable slots -- typing that struct is future work, not this pass.
 *
 * Field NAMES elsewhere are placeholders and cannot change codegen. Offsets
 * and widths are observed.
 */

#ifdef __cplusplus
#include "ModelAnim.h"

struct Door : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by Door_Spawn.c's own _ZN9ModelAnimC1Ev call at +0xd4 -- a
       relocation the ROM build checks, same idiom as include/dBgActor_c.h's
       mModel. */
    ModelAnim mModel;        /* 0x0d4 */

    /* This class's own storage, 0x138..0x147 -- see SIZE above. Left as
       opaque pointers/bytes, same as include/daDoor_c.h's existing
       unk_144, because the virtual calls through 0x138 resolve to
       unidentified Model vtable slots (future work, not this pass). */
    void *unk_138;           /* 0x138 -- owned Model*, see SIZE above */
    void *unk_13c;           /* 0x13c -- released through SharedFilePtr */
    void *unk_140;           /* 0x140 -- callback-node pointer, see SIZE above */
    s8   unk_144;            /* 0x144 -- key-model index, matches daDoor_c.h */
    u8   pad_145[0x3];

    /* --- vtable. Declared first, deliberately -- it is already the key
       function (see DERIVATION above): _ZN4DoorD1Ev.c / _ZN4DoorD0Ev.c define
       it as extern "C" free functions, never as a real `Door::~Door()`, so
       nothing here changes which TU the vtable is emitted from. --- */
    virtual ~Door();

    /* --- overrides of inherited fBase_c slots dActor_c left untouched (see
       include/dActor_c.h: "Slots 0, 3, 6, 9, 12 ... still point at the
       fBase_c implementations"). Declared here purely so this header
       documents the vtable completely; each is DEFINED as a free function
       under its mangled symbol, not as a real Door:: method -- see NOT
       CONVERTED above. --- */
    virtual s32 InitResources();          /* slot 0 */
    virtual s32 CleanupResources();       /* slot 3 */
    virtual s32 Behavior();               /* slot 6 */
    virtual s32 Render();                 /* slot 9 */
    virtual void OnPendingDestroy();      /* slot 12 */
};

/* Holds the chain to the size Door_Spawn.c's operator new(0x148) call
   evidences. A silently-added member anywhere fails this. */
typedef char Door_size_must_be_0x148[sizeof(Door) == 0x148 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DOOR_H */
