/* class dScMgMCarlo2_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgMCarlo2_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). English Spawn-function name is MgPairAGoneAndOn_Spawn;
 * kept as the symbol name for the factory (already attributed), but the
 * class itself takes its real ROM identity, matching this tree's current
 * convention.
 *
 * SIZE 0x5930, from MgPairAGoneAndOn_Spawn.cpp's own
 * `_ZN7fBase_cnwEj(0x5930)`.
 *
 * SHARED TABLE at 0x4f38, size 0x270 (func_ov006_020c1d80/020c1c64), same
 * as five siblings -- see include/dScMgMemory_c.h's own note. TWO fields
 * fall WITHIN it and are real matched access (src/func_ov006_020fa4d4.cpp):
 * unk_4f52 and unk_511e are literally the ctor's own `*(short*)(t+0x1a)=0`
 * and `*(short*)(t+0x1e6)=0` writes (func_ov006_020c1d80's own body) --
 * named here instead of folded into the opaque table, everything else in
 * the table stays pad.
 *
 * ONE ARRAY at 0x51a8, 40 elements of 0x30 bytes each -- the destructor
 * calls `__destroy_arr(c+0x51a8, 0x28, 0x30, func_ov006_020f8ff0)` and the
 * factory constructs it with `func_020733a8(p+0x51a8, 0x28, 0x30, ...)`.
 * Element type not evidenced by any direct field access -- raw bytes.
 *
 * OWN TAIL, 0x5928..0x5930: three fields are real matched access
 * (src/func_ov006_020f9ffc.cpp, .../020fa4d4.cpp, .../020fa56c.cpp).
 *
 * THE DESTRUCTOR IS DEFINED INLINE, in the class body, and the ROM's own
 * emission order is why. Written out of line, mwcc emits the synthesized D0
 * AHEAD of the written D1; the cartridge has D1 first (0x020f8ef4) and D0
 * second (0x020f8f68), and linkcheck's pre-link audit refuses a TU whose
 * licensed .text is not in ROM address order. Inline, the destructor cannot
 * be the key function; InitResources -- the next virtual declared, and
 * non-inline -- takes that role, and the TU that defines it emits
 * _ZTV14dScMgMCarlo2_c. Slots 16 and 17 name D1 then D0, odr-using both, so
 * the compiler emits the pair for us in cartridge order. It also removes the
 * homeless D2 entirely -- a base-object variant byte-identical to D1, with no
 * ROM address to claim and no inbound relocation. The two calls below are
 * ordinary reverse-declaration member destruction, spelled out only because
 * the members are raw bytes. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGMCARLO2_C_H
#define DSCMGMCARLO2_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020f8ff0(int *p); /* the array's element dtor */

struct dScMgMCarlo2_c : dScMgSingle3DBase_c {
    virtual ~dScMgMCarlo2_c() {
        __destroy_arr(mArray, 0x28, 0x30, (void *)func_ov006_020f8ff0);
        func_ov006_020c1c64((char *)this + 0x4f38);
    }

    u8  pad_4f38[0x1a];   /* 0x4f38 -- shared table start, see file banner */
    s16 unk_4f52;          /* 0x4f52 -- within shared table */
    u8  pad_4f54[0x1ca];   /* 0x4f54 */
    s16 unk_511e;          /* 0x511e -- within shared table */
    u8  pad_5120[0x88];    /* 0x5120 -- rest of shared table (ends 0x51a8) */
    u8  mArray[0x780];     /* 0x51a8 -- 0x28 * 0x30, elem dtor func_ov006_020f8ff0 */
    s16 unk_5928;           /* 0x5928 */
    s16 unk_592a;           /* 0x592a */
    u8  pad_592c[0x2];      /* 0x592c */
    s16 unk_592e;           /* 0x592e */

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and neither adds a field, so the size assert below
       is untouched. InitResources, the first of these and the first
       non-inline virtual declared after the inline destructor, is this
       class's KEY FUNCTION -- so the TU that defines it is the one that
       emits _ZTV14dScMgMCarlo2_c.
       Signatures are include/fBase_c.h's and include/dScMgBase_c.h's own,
       copied unchanged. --- */
    s32 InitResources();      /* slot 0 -- src/_ZN14dScMgMCarlo2_c13InitResourcesEv.cpp */
    s32 Behavior();           /* slot 6 -- ov006 0x020fa13c */
    s32 Render();             /* slot 9 -- src/_ZN14dScMgMCarlo2_c6RenderEv.cpp */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  OnTurnIntoEgg(int mode);              /* slot 19 */
    s32 CleanupResources();   /* slot 3 -- ov006 0x020f9fe0 */
};

typedef char dScMgMCarlo2_c_size_must_be_0x5930[sizeof(dScMgMCarlo2_c) == 0x5930 ? 1 : -1];

#endif
