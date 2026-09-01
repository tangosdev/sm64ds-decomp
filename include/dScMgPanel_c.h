#ifndef DSCMGPANEL_C_H
#define DSCMGPANEL_C_H
#include "dScMgBase_c.h"

/* dScMgPanel_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper, not a declared method). Fields
   below dScMgBase_c's own 0x4660 are INHERITED, not this class's own --
   accessed via raw offsets on a char* cast of `this`, same as every other
   dScMgBase_c leaf. Own fields observed only from slots 0/6/9/16/17/18
   directly; fields touched only by non-virtual helper functions (still
   raw extern "C" calls, not migrated methods) are not represented here. */
struct dScMgPanel_c : dScMgBase_c {
    virtual ~dScMgPanel_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    u8  pad_4660[0x648];
    s32 unk_4ca8;            /* 0x4ca8 */
    s32 unk_4cac;            /* 0x4cac */
    s32 unk_4cb0;            /* 0x4cb0 */
    s32 unk_4cb4;            /* 0x4cb4 */
    u8  pad_4cb8[0x32a];
    u8  unk_4fe2;            /* 0x4fe2 */
    u8  pad_4fe3[0x7];
    u8  unk_4fea;            /* 0x4fea */
};

typedef char dScMgPanel_c_size_must_be_0x4fec[sizeof(dScMgPanel_c) == 0x4fec ? 1 : -1];

#endif
