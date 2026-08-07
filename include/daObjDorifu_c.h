#ifndef DAOBJDORIFU_C_H
#define DAOBJDORIFU_C_H

#include "types.h"

/* daObjDorifu_c -- an intermediate class the ROM's RTTI names and the tree
 * did not.  Base: dBgActor_c (the tree calls it Platform).
 *
 * typeinfo 0x02108d70 (ov002), vtable 0x02108d94 (ov002), 32 slots (base has 32).
 * Abstract -- pure-virtual (null) slots: 0, 3
 * Own overrides at slots: 6, 9, 16, 17
 * 3 known descendant(s): daObjKm1_Dorifu_c, daObjKm3_Dorifu_c, daObjRc_Dorifu_c
 *
 * The fields below are the offsets this class's OWN vtable overrides
 * touch that no named ancestor declares.  A method of a class can
 * reach its own members and its ancestors', never a descendant's, so
 * an offset seen here and owned by no ancestor belongs to this class.
 * Read from these byte-matched functions:
 *   func_ov002_020b4bfc
 *   func_ov002_020b4bc4
 *   func_ov002_020b4af8
 *   func_ov002_020b4a70
 *
 * The space below 0xdc8 is left as padding.  It is NOT all the base's:
 * this class's own subobjects live in there too -- daObjDorifu_c's
 * destructor destroys a Model[5] at 0x320 and a MeshCollider[5] at
 * 0x4b0, both its own.  Padding means unobserved, not inherited.
 * The struct is flat like the rest of the generated corpus rather than
 * inheriting because this pass knows offsets, not sizeof(base).
 * Regenerate: python tools/rtti_vtables.py --emit-headers */

struct daObjDorifu_c {
    u8  pad_000[0xdc8];
    u8  unk_dc8;            /* 0xdc8 */
    u8  unk_dc9;            /* 0xdc9 */
    u8  unk_dca;            /* 0xdca */
    u8  unk_dcb;            /* 0xdcb */
};

#endif
