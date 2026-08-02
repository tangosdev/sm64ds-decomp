#ifndef BGCH_H
#define BGCH_H

#include "types.h"

/* The collision-query root, vtable at 0x020991d8 (still unnamed in
 * symbols.txt; naming it is a separate change).
 *
 * VTABLE, 2 slots, read out of the ROM:
 *
 *   slot 0  0x02035504  ~BgCh (D1)        - currently func_02035504
 *   slot 1  0x020354e0  ~BgCh (D0)        - currently func_020354e0
 *
 * Exactly two: the word after slot 1 is 0x0209a764, the __si_class_type_info
 * kind pointer that opens WithMeshClsn's typeinfo record, not a third slot.
 * The base-object destructor D2 is 0x020354d0, reached by direct call from
 * derived destructors and present in no vtable -- the #774 shape. The
 * constructor is 0x02035514. All four keep their func_ names here; renaming
 * them is a symbols.txt change and does not belong in a byte-neutral pass.
 *
 * A ROOT. Its typeinfo at 0x020991c8 is the 8-byte __class_type_info kind,
 * which carries no base pointer, and the ROM's own name for the class is
 * dBgCh. WithMeshClsn (dBgCh_Actr) is the __si_class_type_info record based
 * on it.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
 * key-function arrangement from include/ModelBase.h. That is what keeps this
 * translation unit from emitting a vtable of its own.
 *
 * LAYOUT is 0x10 bytes, pinned by the constructor:
 *
 *   str  r2,[r0]        the vptr
 *   strb r1,[r0,#4]     = 1, a BYTE -- the detect-flag bitmask
 *   str  r1,[r0,#0xc]   = 0
 *   mvn  r1,#0 / str    [r0,#8] = -1
 *
 * unk_004 keeps its placeholder name because the four matched BgCh bodies in
 * src/ spell it, and this pass changes no bodies. StartDetectingWater ORs
 * bit 1 into it; the toxic and ordinary calls work the same byte.
 *
 * ShouldPassThroughImpl is STATIC. Its matched definition takes four
 * parameters -- (void *, const CLPS &, const BgCh &, bool) -- and no this,
 * which is what the mangled name spells. Same shape as the MeshColliderBase
 * UpdatePos callbacks.
 */

#ifdef __cplusplus

struct CLPS;

struct BgCh {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    u8 unk_004;             /* 0x04 - detect-flag bitmask, ctor sets it to 1 */
    u8 pad_005[0x3];
    s32 unk_008;            /* 0x08 - ctor sets -1 */
    s32 unk_00c;            /* 0x0c - ctor sets 0 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~BgCh();        /* slots 0 (D1), 1 (D0) */

    /* --- non-virtual --- */
    void StartDetectingToxic();
    void StartDetectingWater();
    void StopDetectingOrdinary();
    void StopDetectingWater();
};

typedef char BgCh_size_must_be_0x10[sizeof(BgCh) == 0x10 ? 1 : -1];

#else

struct BgCh {
    void **vtable;          /* 0x00 */
    u8 unk_004;             /* 0x04 */
    u8 pad_005[0x3];
    s32 unk_008;            /* 0x08 */
    s32 unk_00c;            /* 0x0c */
};

#endif /* __cplusplus */

#endif /* BGCH_H */
