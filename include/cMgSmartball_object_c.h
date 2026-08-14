/* class cMgSmartball_object_c, real ROM name confirmed by
 * tools/rtti_extract.py (build/rtti.json). Own vtable ov006:0x0213ed10.
 *
 * A ROOT. It has no RTTI base at all -- this is a separate hierarchy from
 * the ActorBase/Scene tree that the rest of this directory models, and the
 * first class in it anyone has touched. Eleven direct children:
 * ball, pakkun, dokan, kinoko, pushswitch, propeller, wing, ana, board,
 * spring, slot.
 *
 * NO VIRTUAL DESTRUCTOR. Its vtable is exactly THREE slots and none of them
 * is a structor -- all three are ordinary virtual methods. That makes this
 * family a different shape from every other class in this tree, where the
 * destructor pair is what gets migrated first. There is nothing to migrate
 * here but real methods, and no operator delete is involved.
 *
 * CONSTRUCTED BY func_ov006_0211474c, which is this class's C1/C2 in all
 * but name: it writes this vtable and then every field below. It is left as
 * a free function -- this tree has migrated zero constructors (langmode
 * audit: "C1: UNPROVEN, 0 migrated"), and proving that route is its own
 * piece of work, not a rider on this one.
 *
 * SIZE 0x34. The constructor's last write is the u8 at 0x30, which alone
 * would only give 0x31 rounded to 0x34; the upper bound comes from a child,
 * whose own constructor writes 0x34 and 0x4c. Base ends where the first
 * derived field begins.
 *
 * THE THREE 2-WORD PAIRS ARE THE INTERESTING PART, and their RELATIONSHIP
 * is what is evidenced, not their meaning. The constructor writes the same
 * incoming pair to BOTH 0x08 and 0x18. Slot 0 copies 0x08/0x0c -> 0x10/0x14.
 * Slot 2 copies 0x18/0x1c -> 0x08/0x0c. So 0x18 holds a value that 0x08 can
 * be restored from, and 0x10 holds a snapshot of 0x08. The field names below
 * say that much and no more.
 *
 * METHOD NAMES ARE COINED FROM THE BODIES, not recovered from the ROM.
 * Nothing in this hierarchy has a named slot map to inherit from -- it is a
 * root, so there is no base whose names could settle them. Each name below
 * describes exactly what its body does; none of them is a claim about what
 * the original called it. Renaming later costs one commit. */
#ifndef CMGSMARTBALL_OBJECT_C_H
#define CMGSMARTBALL_OBJECT_C_H
#include "types.h"

struct cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 -- mSnapshot = mCurrent */
    virtual void Update();         /* slot 1 -- empty in this class */
    virtual void RestoreInitial(); /* slot 2 -- mCurrent = mInitial */

    /* The C spelling of this class gets no explicit vtable member because
       no .c file declares it; every reader reaches these fields by raw
       offset today. */
    s32 unk_004;      /* 0x004 -- the constructor's first argument */
    s32 mCurrent0;    /* 0x008 -- restored from mInitial by RestoreInitial */
    s32 mCurrent1;    /* 0x00c */
    s32 mSnapshot0;   /* 0x010 -- written from mCurrent by SaveSnapshot */
    s32 mSnapshot1;   /* 0x014 */
    s32 mInitial0;    /* 0x018 -- constructor writes the same pair here and
                          at mCurrent */
    s32 mInitial1;    /* 0x01c */
    s32 unk_020;      /* 0x020 -- constructor zeroes it */
    s32 unk_024;      /* 0x024 -- constructor zeroes it */
    s32 unk_028;      /* 0x028 -- constructor zeroes it; ten other files in
                          this family read or write it, the most-touched
                          field of the class */
    s32 unk_02c;      /* 0x02c -- the constructor's second argument */
    u8  unk_030;      /* 0x030 -- constructor sets it to 1 */
    u8  pad_031[0x3]; /* 0x031 -- rounds up to the 0x34 boundary */
};

typedef char cMgSmartball_object_c_size_must_be_0x34[sizeof(cMgSmartball_object_c) == 0x34 ? 1 : -1];

#endif
