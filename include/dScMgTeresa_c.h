#ifndef DSCMGTERESA_C_H
#define DSCMGTERESA_C_H
#include "dScMgBase_c.h"

/* dScMgTeresa_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 20 (Virtual50), plus slot 34,
   still a raw extern "C" helper.  Slot 20 IS declared now: dScMgBase_c
   declares it, so leaving it out here would put the BASE's body in this
   class's slot 20 where the cartridge holds its own.  The body is a bare
   FreeGfxSlotsById(8) thunk that never reads `this`, which costs nothing --
   `this` arrives in r0 and is immediately overwritten with 8 either way, so
   the bytes are the same whether it is spelled as a member or not.
   "Virtual50" is a placeholder, not a recovered name; see
   include/dScMgBase_c.h.  Slot 34 IS that multi-argument virtual, and it is
   declared and reconstructed tree-wide now -- `Virtual88(int, int, int, int)`,
   the family's pixel brush.  The guess recorded here, "draws a HUD
   digit/glyph", was right: this class's override stamps a shape by testing
   data_ov006_0213f9e4[row] one bit per column, so it paints a stipple pattern
   rather than a solid square, and it really does barely touch `this` -- the
   object pointer arrives and is never read, because unlike the base it always
   draws into sub BG0 instead of consulting the layer index at +0x6c.  Fields below
   dScMgBase_c's own 0x4660 are INHERITED, not this class's own -- accessed
   via raw offsets on a char* cast of `this` (0xb4 touched here, already
   dScMgBase_c's own).
 *
 * SM64DS RTTI names the implementation dScMgTeresa_c. The reconstructed factory
 * dScMgTeresa_c_classInit (historical alias MgHideAndBooSeek_Spawn) installs this class's
 * cartridge vtable for the MG_TERESA registry profile.
 */
struct dScMgTeresa_c : dScMgBase_c {
    virtual ~dScMgTeresa_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  Virtual50();                          /* slot 20 */
    virtual void Virtual88(int cx, int cy, int colour, int size); /* slot 34 */

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
