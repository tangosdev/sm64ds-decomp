#ifndef DSCMGTERESA_C_H
#define DSCMGTERESA_C_H
#include "dScMgBase_c.h"

/* dScMgTeresa_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), plus two further own slots
   (20, 34) left as raw extern "C" helpers -- slot 20's recovered name
   ("Virtual50") is stale (it's a bare FreeGfxSlotsById(8) thunk with no
   `this` at all), and slot 34 takes four extra parameters and barely
   touches `this` (draws a HUD digit/glyph), same multi-arg shape as
   dScMgAmida_c's own slot 34 -- both point at dScMgBase_c's slot 34 being a
   multi-argument virtual, not reconstructed tree-wide yet. Fields below
   dScMgBase_c's own 0x4660 are INHERITED, not this class's own -- accessed
   via raw offsets on a char* cast of `this` (0xb4 touched here, already
   dScMgBase_c's own). */
struct dScMgTeresa_c : dScMgBase_c {
    virtual ~dScMgTeresa_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */

    u8  pad_4660[0x588];
    s32 unk_4be8;            /* 0x4be8 -- state index for Behavior's pmf dispatch */
    u8  pad_4bec[0x2a];
    s16 unk_4c16;            /* 0x4c16 */
    u8  pad_4c18[0xb];
    u8  unk_4c23;            /* 0x4c23 */
    /* trailing extent the ROM's `new dScMgTeresa_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_4c24[0x4];
};

typedef char dScMgTeresa_c_size_must_be_0x4c28[sizeof(struct dScMgTeresa_c) == 0x4c28 ? 1 : -1];

#endif
