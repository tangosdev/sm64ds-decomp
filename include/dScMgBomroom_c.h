#ifndef DSCMGBOMROOM_C_H
#define DSCMGBOMROOM_C_H
#include "dScMgBase_c.h"

/* dScMgBomroom_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper, not a declared method). Fields
   below dScMgBase_c's own 0x465d are INHERITED, not this class's own --
   accessed via raw offsets on a char* cast of `this`, same as every other
   dScMgBase_c leaf. Own fields observed only from slots 0/6/9/16/17/18
   directly; the large block of fields touched only by non-virtual helper
   functions (func_ov006_020d5b00 and friends, still raw extern "C" calls,
   not migrated methods) is not represented here -- it stays inside the
   leading pad. */
struct dScMgBomroom_c : dScMgBase_c {
    virtual ~dScMgBomroom_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */

    u8  pad_465d[0x1c73];
    s32 unk_62d0;            /* 0x62d0 */
    u8  pad_62d4[0x1a];
    u16 unk_62ee;            /* 0x62ee */
    u16 unk_62f0;            /* 0x62f0 */
};

#endif
