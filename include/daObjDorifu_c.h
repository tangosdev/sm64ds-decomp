#ifndef DAOBJDORIFU_C_H
#define DAOBJDORIFU_C_H

#include "types.h"

/* Five planks, one of them solid at a time: the abstract base of the rickshaw
 * drifting-platform family. `dorifu` is the katakana clipping of "drift".
 *
 * A LAYER THE TREE DID NOT HAVE. This header used to be a flat struct with
 * `u8 pad_000[0xdc8]` and four bytes on the end, emitted by
 * `tools/rtti_vtables.py --emit-headers` because that pass knew offsets and not
 * sizeof(base). include/Platform.h has since settled sizeof(Platform) = 0x320,
 * so the class can be spelled as what it is. The regenerate line is gone with
 * the generated body: that tool deletes only files that still carry it, and this
 * one is hand-written now.
 *
 *   _ZTI13daObjDorifu_c  ov002 0x02108d70
 *   _ZTS13daObjDorifu_c  ov002 0x02108d7c   "13daObjDorifu_c"
 *   vtable               ov002 0x02108d94, 32 slots, same count as the base
 *   kind                 __si_class_type_info, ONE base, subobject offset 0
 *   base                 dBgActor_c, ov002 0x021089ec -- the tree's Platform
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null.
 * Its own overrides are slots 6 (Behavior), 9 (Render), 16 (D1) and 17 (D0);
 * every other slot holds the base's word and is inherited.
 *
 * THREE DESCENDANTS: daObjKm1_Dorifu_c, daObjRc_Dorifu_c and daObjKm3_Dorifu_c
 * (the tree calls the last one RickshawPlatformBs). Each one's destructor stores
 * this class's vtable between its own and _ZTV8Platform, which is the same fact
 * the RTTI records, arrived at from the other direction.
 *
 * THE TWO ARRAYS ARE THIS CLASS'S. Its own destructor -- ov002 0x020b4af8, whose
 * body is still under a func_ov002_ name -- destroys a MovingMeshCollider[5] at
 * 0x4b0 and a Model[5] at 0x320 while the vptr is this class's, before storing
 * Platform's and destroying Platform's own two. Both arrays close exactly on the
 * next thing: 0x320 + 5 * sizeof(Model) = 0x320 + 5 * 0x50 = 0x4b0, and
 * 0x4b0 + 5 * sizeof(MovingMeshCollider) = 0x4b0 + 5 * 0x1c8 = 0xd98. The strides
 * come from the ROM's __destroy_arr arguments and the sizes from Model.h and
 * MovingMeshCollider.h; they agree without being told to.
 *
 * THE FOUR BYTES AT 0xdc8 ARE THIS CLASS'S TOO, for the same reason:
 * `func_ov002_020b4bfc` is this class's own Behavior, in this class's own
 * overlay, and it reads all four. It is a small state machine -- 0xdca selects
 * the state, 0xdcb indexes an Event bit, 0xdc9 counts down between steps, and
 * 0xdc8 walks 0..4 to pick which of the five colliders is enabled. Render
 * (`func_ov002_020b4bc4`) indexes the Model array at 0x320 with the same 0xdc8.
 * The names below are coined from that; nothing in the ROM names them.
 *
 * SIZE 0xdcc closes the class: all three factories that build a descendant of it
 * -- StairsBdw_Spawn, StairsBs_Spawn and TrickyTriangles_Spawn, one per
 * descendant -- pass 0xdcc to ActorBase::operator new, and each of them then
 * constructs the Model[5] and MovingMeshCollider[5] in place at 0x320 and 0x4b0
 * with the same strides the destructor uses.
 *
 * 0xd98..0xdc8 IS UNOBSERVED, not inherited. Nothing this class's own methods
 * touch lands in there and no descendant declares anything there, so it is left
 * as padding rather than described.
 */

#ifdef __cplusplus

#include "Platform.h"
#include "Model.h"
#include "MovingMeshCollider.h"

struct daObjDorifu_c : Platform {
    /* Field NAMES are placeholders. Offsets, widths and types are observed. */
    Model mPlankModels[5];                  /* 0x320 */
    MovingMeshCollider mPlankClsn[5];       /* 0x4b0 */
    u8  pad_d98[0x30];
    u8  mActivePlank;       /* 0xdc8 - 0..4, which collider is enabled */
    u8  mStepTimer;         /* 0xdc9 - frames until the next step */
    u8  mState;             /* 0xdca - 0..3, see Behavior */
    u8  mEventBit;          /* 0xdcb - index passed to Event::GetBit */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/Platform.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN13daObjDorifu_cD1Ev (which does exist out of line, at ov002 0x020b4af8,
       still under its func_ov002_ name). An out-of-line declaration here would
       make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjDorifu_c() {}
};

typedef char daObjDorifu_c_size_must_be_0xdcc[sizeof(daObjDorifu_c) == 0xdcc ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base sub-object to
   inherit Platform's fields from and so spells the layout flat. Same
   arrangement as include/Platform.h. */
struct daObjDorifu_c {
    u8  pad_000[0xdc8];
    u8  mActivePlank;       /* 0xdc8 */
    u8  mStepTimer;         /* 0xdc9 */
    u8  mState;             /* 0xdca */
    u8  mEventBit;          /* 0xdcb */
};

#endif /* __cplusplus */

#endif /* DAOBJDORIFU_C_H */
