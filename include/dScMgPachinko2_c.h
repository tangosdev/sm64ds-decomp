#ifndef DSCMGPACHINKO2_C_H
#define DSCMGPACHINKO2_C_H
#include "dScMgBase_c.h"

/* dScMgPachinko2_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
   (no RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper, not a declared method). Fields
   below dScMgBase_c's own 0x4660 are INHERITED, not this class's own --
   accessed via raw offsets on a char* cast of `this`, same as every other
   dScMgBase_c leaf (0xa4, 0xbc, 0xc0, 0xc3, 0xc4 all touched here, all
   already dScMgBase_c's own). Own fields observed only from slots
   0/6/9/16/17/18 directly; fields touched only by non-virtual helper
   functions (still raw extern "C" calls, not migrated methods) are not
   represented here. */
struct dScMgPachinko2_c : dScMgBase_c {
    virtual ~dScMgPachinko2_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    u8  pad_4660[0x1000];
    s32 unk_5660;            /* 0x5660 */
    u8  pad_5664[0x8];
    u16 unk_566c;            /* 0x566c */
    u16 unk_566e;            /* 0x566e */
    /* trailing extent the ROM's `new dScMgPachinko2_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_5670[0xc];
};

typedef char dScMgPachinko2_c_size_must_be_0x567c[sizeof(struct dScMgPachinko2_c) == 0x567c ? 1 : -1];

#endif
