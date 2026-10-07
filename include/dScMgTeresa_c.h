#ifndef DSCMGTERESA_C_H
#define DSCMGTERESA_C_H
#include "dScMgBase_c.h"

/* dScMgTeresa_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (OnYoshiTryEat),
   20 (Virtual50) and 34 (Virtual88). These overrides are declared members.
   Slot 20 is declared here because dScMgBase_c
   declares it, so leaving it out here would put the BASE's body in this
   class's slot 20 where the cartridge holds its own.  The body is a bare
   FreeGfxSlotsById(8) thunk that never reads `this`, which costs nothing --
   `this` arrives in r0 and is immediately overwritten with 8 either way, so
   the bytes are the same whether it is spelled as a member or not.
   "Virtual50" is a placeholder, not a recovered name; see
   include/dScMgBase_c.h.  Slot 34 IS that multi-argument virtual, and it is
   declared and reconstructed tree-wide now -- `Virtual88(int, int, int, int)`,
   the family's pixel brush.  The guess recorded here, "draws a dMeter_c
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
    virtual void Virtual50();                          /* slot 20 */
    virtual void Virtual88(int cx, int cy, int colour, int size); /* slot 34 */

    /* scene helpers, in ROM order */
    void func_ov006_0211cc2c();
    void func_ov006_0211cc90();
    void func_ov006_0211cca8();
    void func_ov006_0211cd24(int idx);
    void func_ov006_0211ce90();
    void func_ov006_0211ce94(int index);
    void func_ov006_0211cef4(int i);
    void func_ov006_0211d018(int idx);
    void func_ov006_0211d0f8(int i);
    void func_ov006_0211d224(int i);
    void func_ov006_0211d368(int i);
    void func_ov006_0211d4e8(int i);
    void func_ov006_0211d5a8();
    void func_ov006_0211d608();
    void func_ov006_0211d688();
    void func_ov006_0211d69c();
    void func_ov006_0211d75c();
    void func_ov006_0211d7b0();
    void func_ov006_0211d7b4();
    void func_ov006_0211d7d8();
    void func_ov006_0211d7ec();
    void func_ov006_0211d86c(int idx);
    void func_ov006_0211d924(int i);
    void func_ov006_0211dad0(int i);
    void func_ov006_0211db7c(int i);
    void func_ov006_0211dce0(int i);
    void func_ov006_0211dd0c();
    void func_ov006_0211dd6c();
    void func_ov006_0211ddb8();
    void func_ov006_0211ddcc();
    void func_ov006_0211de54();
    int func_ov006_0211de7c();
    void func_ov006_0211dec0();
    void func_ov006_0211e020(int i);
    void func_ov006_0211e0c8();
    void func_ov006_0211e118();
    void func_ov006_0211e184();
    void func_ov006_0211e220(int param);
    void func_ov006_0211e29c();
    void func_ov006_0211e318();
    void func_ov006_0211e3e0();
    void func_ov006_0211e460();
    void func_ov006_0211e4e0();
    void func_ov006_0211e55c(int idx);
    void func_ov006_0211e5cc();
    void func_ov006_0211e658();
    void func_ov006_0211e72c();
    void func_ov006_0211e7d8();
    void func_ov006_0211e8a8(int idx);
    void func_ov006_0211ea70(int idx);
    void func_ov006_0211eb90(int i);
    void func_ov006_0211ebdc(int i);
    void func_ov006_0211ee34(int i);
    void func_ov006_0211f040(int idx);
    void func_ov006_0211f0d0(int idx);
    void func_ov006_0211f1a4(int i);
    void func_ov006_0211f224(int i);
    void func_ov006_0211f34c(int i);
    void func_ov006_0211f454(int i);
    void func_ov006_0211f51c();
    void func_ov006_0211f554(int i);
    void func_ov006_0211f5d4(int idx);
    void func_ov006_0211f664(int i);
    void func_ov006_0211f6fc();
    void func_ov006_0211f77c();
    void func_ov006_0211f9fc();
    void func_ov006_0211fb1c();
    void func_ov006_0211fbf8();
    void func_ov006_0211fd44();
    void func_ov006_0211fe78();
    void func_ov006_02120008();
    void func_ov006_021200a8();
    void func_ov006_021200cc();

    u8  pad_4660[0x588];
    s32 unk_4be8;            /* 0x4be8 -- state index for Behavior's pmf dispatch */
    u8  pad_4bec[0x2a];
    s16 unk_4c16;            /* 0x4c16 */
    u8  pad_4c18[0xb];
    u8  unk_4c23;            /* 0x4c23 */
    /* trailing extent the ROM's `new dScMgTeresa_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_4c24[0x4];
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgTeresa_c_size_must_be_0x4c28[sizeof(struct dScMgTeresa_c) == 0x4c28 ? 1 : -1];
#endif

#endif
