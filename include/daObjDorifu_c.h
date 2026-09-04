#ifndef DAOBJDORIFU_C_H
#define DAOBJDORIFU_C_H

#include "types.h"

/* Five planks, one of them solid at a time: the abstract base of the rickshaw
 * drifting-platform family. `dorifu` is the katakana clipping of "drift".
 *
 * A LAYER THE TREE DID NOT HAVE. This header used to be a flat struct with
 * `u8 pad_000[0xdc8]` and four bytes on the end, emitted by
 * `tools/rtti_vtables.py --emit-headers` because that pass knew offsets and not
 * sizeof(base). include/dBgActor_c.h has since settled sizeof(dBgActor_c) = 0x320,
 * so the class can be spelled as what it is. The regenerate line is gone with
 * the generated body: that tool deletes only files that still carry it, and this
 * one is hand-written now.
 *
 *   _ZTI13daObjDorifu_c  ov002 0x02108d70
 *   _ZTS13daObjDorifu_c  ov002 0x02108d7c   "13daObjDorifu_c"
 *   vtable               ov002 0x02108d94, 32 slots, same count as the base
 *   kind                 __si_class_type_info, ONE base, subobject offset 0
 *   base                 dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null.
 * Its own overrides are slots 6 (Behavior), 9 (Render), 16 (D1) and 17 (D0);
 * every other slot holds the base's word and is inherited.
 *
 * THREE DESCENDANTS: daObjKm1_Dorifu_c, daObjRc_Dorifu_c and daObjKm3_Dorifu_c
 * (the tree calls the last one daObjKm3_Dorifu_c). Each one's destructor stores
 * this class's vtable between its own and _ZTV10dBgActor_c, which is the same fact
 * the RTTI records, arrived at from the other direction.
 *
 * THE TWO ARRAYS ARE THIS CLASS'S. Its own destructor --
 * daObjDorifu_c::~daObjDorifu_c at ov002 0x020b4af8 -- destroys a
 * dBgW_KcMbg[5] at
 * 0x4b0 and a Model[5] at 0x320 while the vptr is this class's, before storing
 * dBgActor_c's and destroying dBgActor_c's own two. Both arrays close exactly on the
 * next thing: 0x320 + 5 * sizeof(Model) = 0x320 + 5 * 0x50 = 0x4b0, and
 * 0x4b0 + 5 * sizeof(dBgW_KcMbg) = 0x4b0 + 5 * 0x1c8 = 0xd98. The strides
 * come from the ROM's __destroy_arr arguments and the sizes from Model.h and
 * dBgW_KcMbg.h; they agree without being told to.
 *
 * THE FOUR BYTES AT 0xdc8 ARE THIS CLASS'S TOO, for the same reason:
 * `daObjDorifu_c::Behavior` is this class's own slot-6 override, in its own
 * overlay, and it reads all four. It is a small state machine -- 0xdca selects
 * the state, 0xdcb indexes an Event bit, 0xdc9 counts down between steps, and
 * 0xdc8 walks 0..4 to pick which of the five colliders is enabled. Render
 * (`daObjDorifu_c::Render`) indexes the Model array at 0x320 with the same 0xdc8.
 * The names below are coined from that; nothing in the ROM names them.
 *
 * SIZE 0xdcc closes the class: all three factories that build a descendant of it
 * -- daObjKm1_Dorifu_c_classInit, daObjKm3_Dorifu_c_classInit and daObjRc_Dorifu_c_classInit
 * (historical alias TrickyTriangles_Spawn), one per
 * descendant -- pass 0xdcc to fBase_c::operator new, and each of them then
 * constructs the Model[5] and dBgW_KcMbg[5] in place at 0x320 and 0x4b0
 * with the same strides the destructor uses.
 *
 * 0xd98..0xdc8 IS UNOBSERVED, not inherited. Nothing this class's own methods
 * touch lands in there and no descendant declares anything there, so it is left
 * as padding rather than described.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "Model.h"
#include "SharedFilePtr.h"
#include "dBgW_KcMbg.h"

struct daObjDorifuResources {
    SharedFilePtr *modelFile;
    SharedFilePtr *clsnFile;
    CLPS_Block *clps;
};

struct daObjDorifu_c : dBgActor_c {
    /* Field NAMES are placeholders. Offsets, widths and types are observed. */
    Model mPlankModels[5];                  /* 0x320 */
    dBgW_KcMbg mPlankClsn[5];       /* 0x4b0 */
    Matrix4x3 mPlatformMat;                 /* 0xd98 */
    u8  mActivePlank;       /* 0xdc8 - 0..4, which collider is enabled */
    u8  mStepTimer;         /* 0xdc9 - frames until the next step */
    u8  mState;             /* 0xdca - 0..3, see Behavior */
    u8  mEventBit;          /* 0xdcb - index passed to Event::GetBit */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN13daObjDorifu_cD1Ev (which does exist out of line at ov002 0x020b4af8).
       An out-of-line declaration here would
       make each descendant emit a `bl` the ROM does not have. */
    virtual ~daObjDorifu_c() {}

    /* The base is abstract in the cartridge: these two inherited slots are
       null in _ZTV13daObjDorifu_c. Each concrete descendant supplies the
       no-argument override and forwards to the resource-table overloads. */
    virtual s32 InitResources() = 0;        /* slot 0 */
    virtual s32 CleanupResources() = 0;     /* slot 3 */

    /* Slot 6, ov002 0x020b4bfc -- the state machine the banner above describes.
       An override of the virtual fBase_c already declared, so it occupies a
       slot that exists rather than adding one, and no field moves. Spelled
       without `virtual` for the same reason include/daObjFloatBoard_c.h spells
       its own slot-3 override that way: virtualness is inherited, and the
       destructor above stays the first virtual DECLARED in the class. */
    s32 Behavior();

    /* Slot 9, ov002 0x020b4bc4. Indexes mPlankModels with mActivePlank and
       calls Model::Render(NULL) -- the shadow-struct recovery's `Base::m(0)`
       at vtable slot 5, which is Model's own Render(const Vector3*) taking
       a null scale pointer (spelled 0, matching every other Render(0) call
       in the tree). */
    s32 Render();

    /* Shared implementations used by all three concrete descendants. */
    s32 InitResources(daObjDorifuResources *resources);
    s32 CleanupResources(daObjDorifuResources *resources);
};

typedef char daObjDorifu_c_size_must_be_0xdcc[sizeof(daObjDorifu_c) == 0xdcc ? 1 : -1];
typedef char daObjDorifu_Resource_size_must_be_0xc[
    sizeof(daObjDorifuResources) == 0xc ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base sub-object to
   inherit dBgActor_c's fields from and so spells the layout flat. Same
   arrangement as include/dBgActor_c.h. */
struct daObjDorifu_c {
    u8  pad_000[0xdc8];
    u8  mActivePlank;       /* 0xdc8 */
    u8  mStepTimer;         /* 0xdc9 */
    u8  mState;             /* 0xdca */
    u8  mEventBit;          /* 0xdcb */
};

#endif /* __cplusplus */

#endif /* DAOBJDORIFU_C_H */
