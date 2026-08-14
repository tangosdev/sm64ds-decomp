/* class cMgSmartball_slot_c, real ROM name confirmed by tools/rtti_extract.py
 * (build/rtti.json). Own vtable ov006:0x0213ed74, RTTI ov006:0x0213ec70
 * (_ZTI19cMgSmartball_slot_c), _ZTS19cMgSmartball_slot_c ov006:0x0213edf0.
 * One of eleven direct children of cMgSmartball_object_c -- see that header
 * for the family's shape (a root, three slots, no virtual destructor).
 *
 * SIZE 0x88, from _Znwj(0x88) in func_ov006_02115b0c. Base ends at 0x34, so
 * this class adds 0x54 bytes. This class's own four functions never touch
 * the base's contested 0x31/0x32 span, so no raw cast is needed anywhere
 * here (unlike wing_c/ana_c).
 *
 * FIELD EVIDENCE. The constructor (func_ov006_021101bc) placement-constructs
 * two 3-element, 8-byte-stride arrays via func_020733a8(this+0x34,3,8,...)
 * and func_020733a8(this+0x4c,3,8,...) -- that call is the source of the
 * two arrays' bounds and 8-byte stride. RestoreInitial is exhaustive over
 * everything from there to the end of the class and is the strongest single
 * source for the rest of the layout: every loop bound and store width below
 * comes from its own zero pass, not a guess. Update corroborates the first
 * array's shape (it already used a `struct {int a,b;} arr[3]` cast at 0x34
 * in the pre-migration matched source -- kept verbatim as a nested member
 * type here). SaveSnapshot corroborates the four scalars from 0x73 to 0x84.
 *
 * unk_034/unk_04c ARE ARRAYS OF (a,b) PAIRS, not flat scalars, because
 * Update indexes them with a real stride-8 loop (`arr[i].a`, `arr[i].b`) in
 * source that already byte-matched before this migration -- the nested
 * struct type is carried over unchanged, only the local cast becomes a real
 * member. unk_034[i].b (0x38/0x40/0x48) is advanced by unk_04c[i].b
 * (0x50/0x58/0x60) inside the out-of-scope helper func_ov006_0210fb58,
 * which this migration does not touch; nothing about that relationship is
 * asserted here beyond what this class's own four functions show.
 *
 * pad_076[2] (0x76-0x77) is a genuine gap: RestoreInitial's own exhaustive
 * zero pass jumps straight from the three flag bytes at 0x73-0x75 to the
 * s32 countdown at 0x78, skipping it entirely. UNMODELLED, NOT UNREAD.
 *
 * CONSTRUCTED BY func_ov006_021101bc, left a free function per the recipe
 * (this tree has migrated zero constructors), with only its vtable-symbol
 * reference renamed. */
#ifndef CMGSMARTBALL_SLOT_C_H
#define CMGSMARTBALL_SLOT_C_H
#include "types.h"
#include "cMgSmartball_object_c.h"

/* NOTE FOR ANYONE EDITING THIS HEADER: tools/check_header_offsets.py CANNOT
 * parse the two anonymous nested-struct arrays below. It reports them as
 * "2 unparsed" and then computes the struct as spanning 0x58 rather than
 * 0x88 -- and still prints "0 mismatched", which reads like a pass. The
 * offsets in this header are therefore NOT machine-checked by that gate.
 * What actually holds them is the compile-time size assert at the bottom
 * plus the byte match itself, since a wrong offset changes the emitted
 * instruction. Do not trust a clean check_header_offsets run on this file. */
struct cMgSmartball_slot_c : cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 */
    virtual void Update();         /* slot 1 */
    virtual void RestoreInitial(); /* slot 2 */

    struct { s32 a; s32 b; } unk_034[3]; /* 0x034 -- placement-constructed by the
                                             ctor; .a/.b read by Update as (dx,dy)
                                             render offsets from mCurrent */
    struct { s32 a; s32 b; } unk_04c[3]; /* 0x04c -- placement-constructed by the
                                             ctor; .b is a per-element speed the
                                             out-of-scope helper func_ov006_0210fb58
                                             adds into unk_034[i].b */
    s32 unk_064[3]; /* 0x064 -- per-element countdown, zeroed by RestoreInitial */
    u8  unk_070[3]; /* 0x070 -- per-element flag, zeroed by RestoreInitial */
    u8  unk_073;    /* 0x073 -- flag; SaveSnapshot gates the spin-start call to
                        func_ov006_0210fb04 on it being clear */
    u8  unk_074;    /* 0x074 */
    u8  unk_075;    /* 0x075 */

    /* Genuine gap -- see the header comment. */
    u8  pad_076[2]; /* 0x076-0x077 */

    s32 unk_078; /* 0x078 -- countdown; SaveSnapshot decrements while >0 and,
                    once clear along with unk_073/unk_07c, calls
                    func_ov006_0210fb04; zeroed by RestoreInitial */
    s32 unk_07c; /* 0x07c -- countdown; SaveSnapshot decrements while >0;
                    zeroed by RestoreInitial */
    s32 unk_080; /* 0x080 -- countdown; SaveSnapshot decrements while >0 and
                    calls func_ov006_0210fa6c when it reaches 0; zeroed by
                    RestoreInitial */
    s32 unk_084; /* 0x084 -- zeroed by RestoreInitial only; this class's own
                    SaveSnapshot/Update never touch it directly (the
                    out-of-scope helper func_ov006_0210fb58 does) */
};

typedef char cMgSmartball_slot_c_size_must_be_0x88[sizeof(cMgSmartball_slot_c) == 0x88 ? 1 : -1];

#endif
