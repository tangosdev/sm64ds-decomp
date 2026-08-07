#ifndef DAOBJFALLBLOCK_C_H
#define DAOBJFALLBLOCK_C_H

#include "types.h"

/* daObjFallBlock_c -- an intermediate class the ROM's RTTI names and the tree
 * did not.  Base: dBgActor_c (the tree calls it Platform).
 *
 * typeinfo 0x02114874, vtable 0x0213c5bc, ov015, 32 slots (base has 32).
 * Abstract -- pure-virtual (null) slots: 0, 3
 * Own overrides at slots: 6, 9, 16, 17, 27, 31
 * 4 known descendant(s): daObjBk_Fall_Block_c, daObjFl_Fall_Block_c, daObjKm2_Fall_Block_c, daObjTh_Fall_Block_c
 *
 * The fields below are the offsets this class's OWN vtable overrides
 * touch that no named ancestor declares.  A method of a class can
 * reach its own members and its ancestors', never a descendant's, so
 * an offset seen here and owned by no ancestor belongs to this class.
 * Read from these byte-matched functions:
 *   func_ov098_0213a36c
 *   func_ov098_0213a314
 *   func_ov098_02139fc8
 *   func_ov098_02139f70
 *   func_ov098_0213a284
 *   func_ov098_0213a17c
 *
 * Everything below 0x320 is the base's and is left as padding: this pass
 * knows the offsets, not sizeof(base), so the struct is flat like the
 * rest of the generated corpus rather than inheriting.
 * Regenerate: python tools/rtti_vtables.py --emit-headers */

struct daObjFallBlock_c {
    u8  pad_000[0x320];
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    s32 unk_328;            /* 0x328 */
    s32 unk_32c;            /* 0x32c */
    s32 unk_330;            /* 0x330 */
    s16 unk_334;            /* 0x334 */
    s16 unk_336;            /* 0x336 */
    u8  pad_338[0x2];
    s16 unk_33a;            /* 0x33a */
    u8  unk_33c;            /* 0x33c */
    u8  pad_33d[0x1];
    u8  unk_33e;            /* 0x33e */
    u8  unk_33f;            /* 0x33f */
    u8  unk_340;            /* 0x340 */
    u8  pad_341[0x1];
    u8  unk_342;            /* 0x342 */
    u8  pad_343[0x1];
    s32 unk_344;            /* 0x344 */
};

#endif
