#ifndef DSCMGLUIGI_C_H
#define DSCMGLUIGI_C_H
#include "dScMgBase_c.h"

/* dScMgLuigi_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   5 (AfterCleanupResources), 6 (Behavior), 9 (Render), 16 (D1), 17 (D0),
   18 (own new slot, not yet named -- stays a raw extern "C" helper, not a
   declared method). Fields below dScMgBase_c's own 0x465d are INHERITED,
   not this class's own -- accessed via raw offsets on a char* cast of
   `this`, same as every other dScMgBase_c leaf. Own fields observed only
   from slots 0/5/6/9/16/17/18 directly; fields touched only by non-virtual
   helper functions (still raw extern "C" calls, not migrated methods) are
   not represented here. */
struct dScMgLuigi_c : dScMgBase_c {
    virtual ~dScMgLuigi_c();
    virtual s32 InitResources();          /* slot 0 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot 5 */
    virtual s32 Behavior();               /* slot 6 */
    virtual s32 Render();                 /* slot 9 */

    u8  pad_465d[0x91b];
    s32 unk_4f78;            /* 0x4f78 */
    u8  pad_4f7c[0x1f6];
    s16 unk_5172;            /* 0x5172 */
    u8  pad_5174[0x2e3];
    u8  unk_5457;            /* 0x5457 */
    u8  pad_5458[0x1];
    u8  unk_5459;            /* 0x5459 */
};

#endif
