#ifndef DABGSNWMN_C_H
#define DABGSNWMN_C_H
#include "dActor_c.h"
#include "Model.h"
#include "TextureSequence.h"
#include "ShadowModel.h"
#include "dCcAcPos_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* BIG_SNOWMAN (272) -- ov072/daBgSnwmn_c, the snowman manager/head assembly.
 *
 * BASE. dActor_c DIRECTLY -- not dBgActor_c. Two independent readings agree:
 *   - RTTI: daBgSnwmn_c's __si_class_type_info (ov072:0x02122938, name
 *     "daBgSnwmn_c" at 0x02122944) names its single base dActor_c
 *     (build/rtti.json edge, resolved_by "arm9").
 *   - Vtable: tools/rtti_vtables.py --own daBgSnwmn_c reports
 *     "daBgSnwmn_c : dActor_c   31 slots (base 31)" -- daBgSnwmn_c's own
 *     table is exactly as long as dActor_c's own (31 slots, 0..30), which
 *     is only possible if dActor_c is the immediate base: dBgActor_c adds
 *     its own Kill override at slot 31. daBgSnwmn_c overrides exactly seven
 *     of dActor_c's 31 -- 0, 3, 6, 9, 12, 16, 17.
 *
 * SIZE 0x1f0 (496 bytes) is the literal daBgSnwmn_c_classInit passes to
 * fBase_c::operator new. The factory constructs five typed sub-objects
 * after the dActor_c base:
 *     Model           0x0d4  sizeof 0x50
 *     Model           0x124  sizeof 0x50
 *     TextureSequence 0x174  sizeof 0x14
 *     ShadowModel     0x188  sizeof 0x28
 *     dCcAcPos_c      0x1b0  sizeof 0x40
 * 0x1b0 + 0x40 = 0x1f0. The destructor pair tears the same five down in
 * reverse order.
 *
 * dActor_c ends at 0x0d0. The first typed member (mModel1) starts at 0x0d4.
 * The four bytes at 0x0d0..0x0d3 are real layout (they hold up the rest,
 * which closes exactly on 0x1f0) but nothing in this TU reads or writes
 * them; kept opaque. InitResources' area argument is the inherited
 * mAreaId at 0x0cc, not a field of this class.
 *
 * VTABLE ORDER follows dActor_c's; daBgSnwmn_c adds no new virtual. The
 * destructor pair is at slots 16/17.
 */
struct daBgSnwmn_c : dActor_c {
    u8  unk_0d0[0x4];              /* 0x0d0 -- opaque; see the note above */
    Model mModel1;                 /* 0x0d4 */
    Model mModel2;                 /* 0x124 */
    TextureSequence mTexSeq;       /* 0x174 */
    ShadowModel mShadow;           /* 0x188 */
    dCcAcPos_c mCylClsn;           /* 0x1b0 */

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
