/* class dScMgMCarlo_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgMCarlo_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0213d664; the RTTI
 * pointer one word below it is _ZTI13dScMgMCarlo_c, which is what ties
 * that table to this class rather than to a neighbour.
 *
 * SIZE 0x60b0, from the factory's own `_ZN7fBase_cnwEj(0x60b0)`
 * (src/func_ov006_020f8e44.cpp).
 *
 * SHARED TABLE at 0x4f38, size 0x270, constructed by func_ov006_020c1d80
 * and destroyed by func_ov006_020c1c64 -- the same pair dScMgCard_c and
 * dScMgMCarlo2_c use at this same offset; see include/dScMgCard_c.h's own
 * note. The SAME two fields fall within it here as in both of those
 * siblings, unk_4f52 and unk_511e, and they are the same thing there too:
 * literally the shared constructor's own `*(short*)(t+0x1a)=0` and
 * `*(short*)(t+0x1e6)=0` writes. Named here rather than folded into the
 * opaque table; everything else in the table stays pad.
 *
 * ONE ARRAY, 0x50 elements of 0x30 bytes, at 0x51a8: the factory builds it
 * with element ctor func_ov006_020f8ed8 and the destructor destroys it
 * with element dtor func_ov006_020f7730, ending exactly on 0x60a8 where
 * the own tail begins. Element type is not evidenced by any direct field
 * access, so it stays raw bytes -- same treatment as dScMgCard_c's own
 * two arrays.
 *
 * OWN TAIL, 0x60a8..0x60b0: three fields carried over verbatim from this
 * header's previous auto-generated form, real matched access out of
 * dScMgMCarlo_c's own methods, and the last of them closes exactly on the
 * factory's allocation literal. The two fields the old header declared
 * below 0x4f38 (0x0a8, 0x0ac) are dropped: both are dScMgBase_c's own and
 * are already declared there.
 *
 * THE DESTRUCTOR IS DEFINED INLINE, in the class body, and the ROM's own
 * emission order is why. Written out of line, mwcc emits the synthesized D0
 * AHEAD of the written D1; the cartridge has D1 first (0x020f7634) and D0
 * second (0x020f76a8), and linkcheck's pre-link audit refuses a TU whose
 * licensed .text is not in ROM address order. Inline, the destructor cannot
 * be the key function; InitResources -- the next virtual declared, and
 * non-inline -- takes that role, and the TU that defines it emits
 * _ZTV13dScMgMCarlo_c. Slots 16 and 17 name D1 then D0, odr-using both, so
 * the compiler emits the pair for us in cartridge order. It also removes the
 * homeless D2 entirely. The two calls below are ordinary reverse-declaration
 * member destruction, spelled out only because the members are raw bytes.
 * No separate operator delete is needed -- dScMgBase_c, two levels up,
 * already provides one. */
#ifndef DSCMGMCARLO_C_H
#define DSCMGMCARLO_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  func_ov006_020c1c64(char *t); /* decl_common.h's own signature */
extern "C" void __destroy_arr(void *base, int count, int stride, void *dtor);
extern "C" void func_ov006_020f7730(int *p); /* mArray's element dtor;
                                                 signature is the definition's,
                                                 in this class's own TU */

struct dScMgMCarlo_c : dScMgSingle3DBase_c {
    virtual ~dScMgMCarlo_c() {
        __destroy_arr(mArray, 0x50, 0x30, (void *)func_ov006_020f7730);
        func_ov006_020c1c64((char *)this + 0x4f38);
    }

    /* This class's own override, read off the ROM's vtable: the one slot where
       the table differs from dScMgSingle3DBase_c's. Spelled WITHOUT the `virtual`
       keyword, the way include/daObjMarioCap_c.h and include/daObjRc_Dorifu_c.h
       spell theirs -- an override of a virtual an ancestor already declares is
       implicitly virtual either way, so it reuses slot 6 rather than adding one
       and adds no field, and the 0x60b0 assert below still holds. InitResources,
       the first of these and the first non-inline virtual declared after the
       inline destructor, is this class's KEY FUNCTION -- so the TU that defines
       it is the one that emits _ZTV13dScMgMCarlo_c. */
    s32 InitResources();   /* slot  0 -- ov006 0x020f8d08 */
    s32 Behavior();        /* slot  6 -- ov006 0x020f869c */
    s32 Render();          /* slot  9 -- ov006 0x020f85b0 */

    u8  pad_4f38[0x1a];    /* 0x4f38 -- shared table start, see file banner */
    s16 unk_4f52;          /* 0x4f52 -- within shared table */
    u8  pad_4f54[0x1ca];   /* 0x4f54 */
    s16 unk_511e;          /* 0x511e -- within shared table */
    u8  pad_5120[0x88];    /* 0x5120 -- rest of shared table (ends 0x51a8) */
    u8  mArray[0xf00];     /* 0x51a8 -- 0x50 * 0x30, elem dtor func_ov006_020f7730 */
    s16 unk_60a8;          /* 0x60a8 */
    s16 unk_60aa;          /* 0x60aa */
    u8  pad_60ac[0x2];     /* 0x60ac */
    s16 unk_60ae;          /* 0x60ae */
};

typedef char dScMgMCarlo_c_size_must_be_0x60b0[sizeof(dScMgMCarlo_c) == 0x60b0 ? 1 : -1];

#endif
