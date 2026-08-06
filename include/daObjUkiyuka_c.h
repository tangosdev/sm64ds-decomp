#ifndef DAOBJUKIYUKA_C_H
#define DAOBJUKIYUKA_C_H

#include "types.h"

/* daObjUkiyuka_c -- an intermediate class the ROM's RTTI names and the tree
 * did not.  Base: dBgActor_c (the tree calls it Platform).
 *
 * typeinfo 0x02109104, vtable 0x0210912c, ov002, 32 slots (base has 32).
 * Abstract -- pure-virtual (null) slots: 0, 3
 * Own overrides at slots: 6, 9, 16, 17
 * 2 known descendant(s): daObjFl_Ukiyuka_c, daObjKm2_Ukishima_c
 *
 * The fields below are the offsets this class's OWN vtable overrides
 * touch that no named ancestor declares.  A method of a class can
 * reach its own members and its ancestors', never a descendant's, so
 * an offset seen here and owned by no ancestor belongs to this class.
 * Read from these byte-matched functions:
 *   func_ov002_020b6494
 *   func_ov002_020b646c
 *   func_ov002_020b63e0
 *   func_ov002_020b6388
 *
 * Everything below 0x320 is the base's and is left as padding: this pass
 * knows the offsets, not sizeof(base), so the struct is flat like the
 * rest of the generated corpus rather than inheriting.
 * Regenerate: python tools/rtti_vtables.py --emit-headers */

struct daObjUkiyuka_c {
    u8  pad_000[0x320];
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    s16 unk_328;            /* 0x328 */
    s16 unk_32a;            /* 0x32a */
};

#endif
