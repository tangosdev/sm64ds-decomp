#ifndef DABGSNWMN_C_H
#define DABGSNWMN_C_H
#include "dActor_c.h"
#include "Model.h"
#include "TextureSequence.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* An dActor_c-side class (da* prefix, not a scene class) -- a background
 * snowman actor. It had a FLAT header before this pass, with no base
 * clause; seven resolvable vtable slots were blocked on it. The flat
 * header's own field table was not just incomplete but WRONG on size (see
 * SIZE below).
 *
 * BASE. dActor_c DIRECTLY -- not dBgActor_c, despite the da* prefix
 * suggesting a Platform-family straggler. Two independent readings agree:
 *   - RTTI: daBgSnwmn_c's __si_class_type_info (ov072:0x02122938, name
 *     "daBgSnwmn_c" at 0x02122944) names its single base dActor_c
 *     (build/rtti.json edge, resolved_by "arm9").
 *   - Vtable: tools/rtti_vtables.py --own daBgSnwmn_c reports
 *     "daBgSnwmn_c : dActor_c   31 slots (base 31)" -- daBgSnwmn_c's own
 *     table is exactly as long as dActor_c's own (31 slots, 0..30), which
 *     is only possible if dActor_c is the immediate base: dBgActor_c adds
 *     its own Kill override at slot 31 (see notes/handoff and the Platform
 *     family census), so a dBgActor_c child's table would be 32 slots, not
 *     31. daBgSnwmn_c overrides exactly seven of dActor_c's 31 -- 0, 3, 6,
 *     9, 12, 16, 17.
 *
 * CONSTRUCTION. daBgSnwmn_c_classInit is the reconstructed source-style
 * factory name. The function at 0x02120c00 opens with
 * `_ZN7fBase_cnwEj(496)` -- fBase_c::operator new(0x1f0). SIZE 0x1f0 (496
 * bytes) comes directly off the allocator call, and the same factory
 * constructs five typed sub-objects after the dActor_c base and the
 * derived vptr store:
 *     _ZN5ModelC1Ev(p + 0x0d4)                    -- Model, sizeof 0x50
 *     _ZN5ModelC1Ev(p + 0x124)                    -- Model, sizeof 0x50
 *     _ZN15TextureSequenceC1Ev(p + 0x174)         -- TextureSequence, 0x14
 *     _ZN11ShadowModelC1Ev(p + 0x188)             -- ShadowModel, 0x28
 *     _ZN10dCcAcPos_cC1Ev(p + 0x1b0) -- dCcAcPos_c, 0x40
 * THE LAST MEMBER CLOSES EXACTLY ON THE ALLOCATION LITERAL:
 * 0x1b0 + 0x40 = 0x1f0. Nothing is left over. The destructor
 * pair emitted from src/actors/d_a_bg_snwmn.cpp tears the same five down in
 * exactly the reverse order -- dCcAcPos_c, ShadowModel, TextureSequence,
 * Model, Model -- which is what a compiler-generated body emits for typed
 * members declared in ascending-offset order, so they are declared typed
 * below rather than left as opaque storage.
 *
 * THE OLD FLAT HEADER'S SIZE WAS WRONG: it asserted nothing explicitly but
 * its last field sat at 0x368 -- 0x178 bytes past the real 0x1f0 allocation.
 * Every field it listed below 0xd0 (0x05c, 0x060, 0x064, 0x080, 0x084,
 * 0x088, 0x08e, 0x094, 0x098, 0x09c, 0x0a0, 0x0a8, 0x0cc, 0x0d0) is really
 * dActor_c's OWN field, misattributed by the flat generator exactly the way
 * dScEntry_c's old header misattributed everything below fBase_c's 0x50
 * (see dScEntry_c.h) -- dActor_c.h already declares mPosX/Y/Z at 0x05c,
 * mScaleX/Y/Z at 0x080, mAngleX at 0x08c, mHorzSpeed at 0x098,
 * mVertAccel at 0x09c, mTerminalVelocity at 0x0a0, mVertSpeed at 0x0a8, and
 * mAreaId at 0x0cc -- and this class's own InitResources reads
 * self->unk_0cc, confirming it is reading the INHERITED mAreaId, not a
 * field of its own. Nothing daBgSnwmn_c's seven slot functions touch sits
 * above dActor_c's own 0xd0 except through the five typed members above and
 * one untouched 4-byte gap at 0x0d0..0x0d4 (see below) -- the flat header's
 * 0x368 field does not correspond to anything any of the seven functions
 * reads or writes and could not have come from this class.
 *
 * dActor_c ends at 0x0d0 (include/dActor_c.h). Nothing in any of the seven
 * slot functions reads or writes 0x0d0..0x0d3 -- the first typed member
 * (mModel1) starts at 0x0d4, four bytes later, so those four bytes are real
 * (they hold up the rest of the layout, which closes exactly on 0x1f0) but
 * their content is not evidenced by anything this pass read; kept as
 * opaque padding pending further recovery.
 *
 * VTABLE ORDER follows dActor_c's; daBgSnwmn_c adds no new virtual. The
 * destructor pair is at slots 16/17 (the fBase_c/dActor_c convention),
 * confirmed directly against the _ZN8dActor_cD1Ev / _ZN8dActor_cD0Ev
 * anchors.
 */
struct daBgSnwmn_c : dActor_c {
    u8  unk_0d0[0x4];              /* 0x0d0 -- opaque; see the note above */
    Model mModel1;                 /* 0x0d4 */
    Model mModel2;                 /* 0x124 */
    TextureSequence mTexSeq;       /* 0x174 */
    ShadowModel mShadow;           /* 0x188 */
    dCcAcPos_c mCylClsn; /* 0x1b0 */

    /* --- overrides, in dActor_c's own vtable order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    /* Declared last and inline so class instantiation emits the retail D1/D0
       pair in that order, with no separate D2 body. */
    virtual ~daBgSnwmn_c() {}                            /* slots 16 (D1), 17 (D0) */
};

typedef char daBgSnwmn_c_size_must_be_0x1f0[sizeof(daBgSnwmn_c) == 0x1f0 ? 1 : -1];

#endif
