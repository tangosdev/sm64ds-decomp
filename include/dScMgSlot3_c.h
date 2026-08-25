/* class dScMgSlot3_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgSlot3_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). Its own vtable sits at ov006:0x0213eaa8 -- that word
 * is the vptr value, and the RTTI pointer one word below it is
 * _ZTI12dScMgSlot3_c, which is what ties the table to this class rather
 * than to a neighbour.
 *
 * SIZE 0x5044, from the factory's own `_ZN7fBase_cnwEj(0x5044)`
 * (src/func_ov006_0210c120.c). The evidenced field span stops at 0x5043,
 * so the trailing byte is alignment, not unclaimed space.
 *
 * TABLE at 0x4f38, constructed by func_ov006_020c221c and destroyed by
 * func_ov006_020c21e4 -- the same construct/destroy pair shape five
 * siblings use at this same offset (see include/dScMgCard_c.h's own note).
 * Nothing reads inside the table itself, so its size is bounded only by
 * what follows it, and that bound is 0xac, not the 0xb8 the first draft of
 * this header claimed: a THREE-ELEMENT s32 array sits at 0x4fe4, indexed by
 * five of this class's own files (_ZN12dScMgSlot3_c6RenderEv.cpp's own local
 * `struct T4fe4 { char pad[0x4fe4]; int vals[3]; }` names it outright, and
 * 0210bdb0.cpp, 0210af64.c, 0210adac.c and 0210ab08.c all index it as
 * `[i]` or `+ i*4`). It closes exactly on unk_4ff0. Left opaque up to
 * there rather than guessed at, as dScMgCard_c.h leaves its own.
 *
 * OWN TAIL, 0x4ff0..0x5043: seventeen fields carried over verbatim from
 * this header's previous auto-generated form, all real matched access out
 * of dScMgSlot3_c's own methods. Offsets and widths are unchanged by this
 * migration -- the base contributes exactly 0x4f38 bytes, so every one of
 * them lands on the byte it landed on before. The four fields the old
 * header declared BELOW 0x4f38 (0x0a8, 0x0ac, 0x0b4, 0x0bc) are dropped:
 * all four are dScMgBase_c's own and are already declared there.
 *
 * THE DESTRUCTOR IS NOT DEFINED INLINE -- a leaf, no RTTI descendants of
 * its own. Defined for real in src/_ZN12dScMgSlot3_cD1Ev.cpp; D0Ev.cpp
 * carries an identical copy. No separate operator delete is needed --
 * dScMgBase_c, two levels up, already provides one. */
#ifndef DSCMGSLOT3_C_H
#define DSCMGSLOT3_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c21e4(char *t); /* decl_common.h's own signature */

struct dScMgSlot3_c : dScMgSingle3DBase_c {
    virtual ~dScMgSlot3_c();

    /* This class's own overrides, read off the ROM's vtable: the slots where the
       table differs from dScMgSingle3DBase_c's. Spelled WITHOUT the `virtual`
       keyword, the way include/daObjMarioCap_c.h and include/daObjRc_Dorifu_c.h
       spell theirs -- an override of a virtual an ancestor already declares is
       implicitly virtual either way, so each reuses an existing slot and adds no
       field, and the 0x5044 assert below still holds. The destructor above is
       declared first and out of line, so it stays this class's KEY FUNCTION and
       neither of these translation units emits _ZTV12dScMgSlot3_c. */
    s32 InitResources();   /* slot  0 -- src/_ZN12dScMgSlot3_c13InitResourcesEv.cpp */
    s32 Behavior();        /* slot  6 -- src/_ZN12dScMgSlot3_c8BehaviorEv.cpp */
    s32 Render();          /* slot  9 -- src/_ZN12dScMgSlot3_c6RenderEv.cpp */

    u8  pad_4f38[0xac];     /* 0x4f38 -- opaque table, see file banner */
    s32 mReelPos[3];        /* 0x4fe4 -- Fix12 scroll of each reel; seeded to a
                               random symbol * 0x50000 (0x50 px a symbol) */
    s32 unk_4ff0;           /* 0x4ff0 */
    s32 mReelWinPos[3];     /* 0x4ff4 -- read instead of mReelPos during the
                               state-6 win-line pass */
    s32 mState;             /* 0x5000 -- index into the pointer-to-member table
                               data_ov006_02142bdc that Behavior dispatches */
    s32 unk_5004;           /* 0x5004 -- seeded from dScMgBase_c's own 0xbc */
    u8  pad_5008[0x4];      /* 0x5008 */
    s32 mReelDrawY;         /* 0x500c -- while positive the marker rows are drawn
                               at +0x10 and +0x60 instead of a single 0x60 */
    s32 mWinColumn;         /* 0x5010 -- column of the payout caption
                               (n * 0x50 + 0x20); negative means none */
    s32 unk_5014;           /* 0x5014 */
    u16 mLamp1Angle;        /* 0x5018 -- -0x200 a tick while mState is 1 */
    u16 mLamp2Angle;        /* 0x501a -- -0x400 a tick */
    u8  mReelStrip[3][5];   /* 0x501c -- the symbol on each stop of each reel;
                               Render walks it modulo mStripLength */
    u8  pad_502b[0x3];      /* 0x502b */
    u8  mLineActive[3];     /* 0x502e -- which of the three pay lines is bet */
    u8  mResultSymbols[3][3]; /* 0x5031 -- the 3x3 window the reels stopped on */
    u8  mStripLength;       /* 0x503a -- number of stops per reel */
    u8  mWinSymbol;         /* 0x503b -- matched against mResultSymbols; seeded
                               from 0x503c at InitResources */
    u8  unk_503c;           /* 0x503c -- drawn as the second marker row */
    u8  unk_503d;           /* 0x503d */
    u8  unk_503e;           /* 0x503e */
    u8  mFrameCounter;      /* 0x503f -- +1 a Behavior tick; the win chime fires
                               on (n & 0x3f) == 0x20 */
    u8  unk_5040;           /* 0x5040 */
    u8  unk_5041;           /* 0x5041 */
    u8  unk_5042;           /* 0x5042 */
    u8  pad_5043[0x1];      /* 0x5043 -- rounds up to the 0x5044 boundary */
};

typedef char dScMgSlot3_c_size_must_be_0x5044[sizeof(dScMgSlot3_c) == 0x5044 ? 1 : -1];

#endif
