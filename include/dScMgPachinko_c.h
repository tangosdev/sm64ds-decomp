#ifndef DSCMGPACHINKO_C_H
#define DSCMGPACHINKO_C_H
#include "dScMgBase_c.h"

/* dScMgPachinko_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
   (no RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper). rtti_vtables.py --own also
   reports slots 36/38, but both are spurious: the true vtable ends at 36
   words (address-gap math against the neighbouring MgLakituLaunch_SpawnInfo
   table, and relocs.txt cross-references confirm it belongs to a wholly
   different actor's spawn descriptor, not this class) -- excluded, along
   with func_ov006_021019e0 and MgLakituLaunch_Spawn, from this migration.
   Fields below dScMgBase_c's own 0x4660 are INHERITED, not this class's
   own -- accessed via raw offsets on a char* cast of `this` (0xa4, 0xc0,
   0xc3, 0xc4 all touched here, all already dScMgBase_c's own, same as
   dScMgPachinko2_c's). */
struct dScMgPachinko_c : dScMgBase_c {
    virtual ~dScMgPachinko_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    u8  pad_4660[0x15b0];
    s32 unk_5c10;            /* 0x5c10 */
    u8  pad_5c14[0x4];
    u16 unk_5c18;            /* 0x5c18 */
    u8  pad_5c1a[0x2];
    u16 unk_5c1c;            /* 0x5c1c */
    u8  pad_5c1e[0x6];
    u16 unk_5c24;            /* 0x5c24 */
    u8  pad_5c26[0x4];
    u16 unk_5c2a;            /* 0x5c2a */
    /* trailing extent the ROM's `new dScMgPachinko_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_5c2c[0xc];
};

typedef char dScMgPachinko_c_size_must_be_0x5c38[sizeof(struct dScMgPachinko_c) == 0x5c38 ? 1 : -1];

#endif
