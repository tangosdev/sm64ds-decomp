#ifndef DAOBJSWDOOR_C_H
#define DAOBJSWDOOR_C_H

#include "types.h"

/* daObjSwdoor_c -- an intermediate class the ROM's RTTI names and the tree
 * did not.  Base: dBgActor_c (the tree calls it Platform).
 *
 * typeinfo 0x021099c0 (ov002), vtable 0x021099e4 (ov002), 51 slots (base has 32).
 * Abstract -- pure-virtual (null) slots: 0, 3, 6, 33, 35, 37, 39, 41, 43, 45, 47, 49
 * Own overrides at slots: 9, 16, 17, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50
 * 2 known descendant(s): daObjBSwdoor_c, daObjCvShutter_c
 *
 * The fields below are the offsets this class's OWN vtable overrides
 * touch that no named ancestor declares.  A method of a class can
 * reach its own members and its ancestors', never a descendant's, so
 * an offset seen here and owned by no ancestor belongs to this class.
 * Read from these byte-matched functions:
 *   func_ov002_020babf0
 *   func_ov002_020bab64
 *   func_ov002_020bab0c
 *   func_ov002_020bb9fc
 *   func_ov002_020bb9f0
 *   func_ov002_020bbd50
 *   func_ov002_020bba24
 *   func_ov002_020bba28
 *   func_ov002_020bbc78
 *   func_ov002_020bbb14
 *   func_ov002_020bbac8
 *   func_ov002_020bbcb8
 *
 * The space below 0x344 is left as padding.  It is NOT all the base's:
 * this class's own subobjects live in there too -- daObjDorifu_c's
 * destructor destroys a Model[5] at 0x320 and a MeshCollider[5] at
 * 0x4b0, both its own.  Padding means unobserved, not inherited.
 * The struct is flat like the rest of the generated corpus rather than
 * inheriting because this pass knows offsets, not sizeof(base).
 * Regenerate: python tools/rtti_vtables.py --emit-headers */

struct daObjSwdoor_c {
    u8  pad_000[0x344];
    s32 unk_344;            /* 0x344 */
    u8  pad_348[0x245];
    u8  unk_58d;            /* 0x58d */
    u8  pad_58e[0xe];
    s32 unk_59c;            /* 0x59c */
    s32 unk_5a0;            /* 0x5a0 */
};

#endif
