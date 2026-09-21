/* class dScMgSound_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgSound_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). The coined English Spawn-function name was
 * MgBoomBox_Spawn; the factory now carries the reconstructed
 * dScMgSound_c_classInit spelling, and the class itself takes its
 * real ROM identity, matching this tree's current convention.
 *
 * SIZE 0x562c, from d_s_mg_sound.cpp's own `_ZN7fBase_cnwEj(0x562c)`.
 *
 * TAIL STORAGE begins at 0x4f38 and occupies 0x6f4 bytes in this layout.
 * The factory calls func_ov006_020c33dc there and the destructor calls
 * func_ov006_020c3288 at the same address. These establish a component's
 * start and lifetime, not its full extent. The constructor builds nested
 * objects through +0x178 and writes +0x1a4/+0x1a8; those accesses do not
 * prove that the component fills the tail. Matched scene state also uses
 * 0x50e0, 0x5608, 0x5616, 0x5618 and tail bytes through 0x5628.
 * mTable retains raw storage containing the component and live state;
 * the component boundary and typed fields remain to be established under
 * issue #2492.
 *
 * THE DESTRUCTOR IS DEFINED INLINE AND DECLARED FIRST. Its class TU emits
 * _ZTV/_ZTI/_ZTS and the destructor variants. The inline form reproduces the
 * cartridge's destructor ORDER: mwccarm 2004/b56 emits D1 then D0 for an in-class body
 * and D2/D0/D1 for an out-of-line one, and ov006 puts D1 at 0x02119904
 * BELOW D0 at 0x02119958. The body is the one the two pre-migration
 * one-function destructor shards each carried, unchanged; both are now
 * absorbed into src/actors/dScMgSound_c.cpp. No separate operator delete is
 * needed -- dScMgBase_c, two levels up, already provides one.
 *
 * SM64DS RTTI names the implementation dScMgSound_c. The reconstructed factory
 * dScMgSound_c_classInit (historical alias MgBoomBox_Spawn) installs this class's
 * cartridge vtable for the MG_SOUND registry profile.
 */
#ifndef DSCMGSOUND_C_H
#define DSCMGSOUND_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c3288(char *t); /* decl_common.h's own signature */

/* A 0x10-byte animated sprite. Two of these sit at 0x55e8 and 0x55f8, and the
 * evidence they are the same shape is that func_ov006_02119b00 and
 * func_ov006_02119a18 are the SAME function against the two field groups:
 *
 *     if (!active) return;
 *     if (frame >= LIMIT) return;      // 2 for the first, 3 for the second
 *     if (++timer < TABLE[frame]) return;   // 0212ee28 / 0212ee38
 *     timer = 0;
 *     frame++;
 *
 * and func_ov006_0211bd18 / func_ov006_0211bdb4 likewise both read `visible`,
 * then draw at (x, y). The two differ only in their frame limit and table, so
 * they are two instances rather than a two-element array -- nothing indexes
 * between them. */
struct dMgSoundSprite_c {
    s32 x;            /* +0x00 -- 20.12 screen x */
    s32 y;            /* +0x04 -- 20.12 screen y */
    u16 timer;        /* +0x08 -- ticks elapsed in the current frame */
    u8  pad_0a[2];
    u8  active;       /* +0x0c -- gates the frame advance */
    u8  visible;      /* +0x0d -- gates the draw */
    u8  frame;        /* +0x0e -- animation frame; indexes its own table */
    u8  pad_0f;
};

#ifndef SM64DS_PLATFORM_PC
typedef char dMgSoundSprite_c_size_must_be_0x10[sizeof(dMgSoundSprite_c) == 0x10 ? 1 : -1];
#endif

struct dScMgSound_c : dScMgSingle3DBase_c {
    virtual ~dScMgSound_c() { func_ov006_020c3288((char *)mTable); }
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual void Virtual50();                          /* slot 20 */

    /* --- the 0x4f38 tail, partially resolved (was one u8 mTable[0x6f4]) -----
       The component boundary is now established at mTable+0x1a8. Two things
       agree on it: the constructor's own writes into the component stop at
       +0x1a4/+0x1a8, and 0x50e0 is the LOWEST address in the whole tail that
       this TU's scene code touches -- nothing reads 0x4f38..0x50e0 at all.
       func_ov006_020c33dc and func_ov006_020c3288 still take mTable by
       address, which is the only way they ever used it.

       What remains raw is the array region 0x50e4..0x55e8. That is not
       unexamined, it is contested: the TU carries three shadow views of it
       that disagree (BallObj says 4 elements at 0x51b4, BigObj says 16 at the
       same base, Mgr says 30 at 0x51b0), plus a 0x14-stride view at 0x50e8.
       Those views are load-bearing for codegen, so resolving them is issue
       #2492's job, not this one. Likewise 0x5610..0x561f, which is indexed at
       runtime through +0x5625.

       Every field named below is reached only by a constant offset and at one
       consistent width -- across all 193 offset accesses in this TU, no
       address is ever read at two different widths. */
    u8  mTable[0x1a8];        /* 0x4f38 -- opaque component; used by address only */
    s32 unk_50e0;             /* 0x50e0 -- set to 1 once the component reports ready */
    u8  raw_50e4[0x504];      /* 0x50e4 -- contested array region; see #2492 */
    dMgSoundSprite_c mSpriteA;/* 0x55e8 -- frame limit 2, table 0212ee28 */
    dMgSoundSprite_c mSpriteB;/* 0x55f8 -- frame limit 3, table 0212ee38 */
    s32 mState;               /* 0x5608 -- the scene state machine's own switch */
    u32 unk_560c;             /* 0x560c -- handle from 0212efb0, passed to func_02012174 */
    u8  raw_5610[0xf];        /* 0x5610 -- u16 queue indexed by unk_5625 */
    u8  mLanes[3];            /* 0x561f -- three rotating lanes, each prev+1 mod 3 */
    u8  pad_5622[2];
    u8  unk_5624;             /* 0x5624 */
    u8  unk_5625;             /* 0x5625 -- write index into raw_5610 */
    u8  unk_5626;             /* 0x5626 -- counts down from 3 */
    u8  mPattern;             /* 0x5627 -- current pattern 0..4; indexes 0212ee10/ee18 */
    u8  mPrevPattern;         /* 0x5628 -- saved before a new pattern is drawn */
    u8  pad_5629[3];

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and none adds a field, so the size assert below is
       untouched. The existing class TU retains its destructor/RTTI
       ownership; these declarations do not create a new owner.
       Signatures are include/fBase_c.h's and include/dScMgBase_c.h's own,
       copied unchanged.

       Their accesses at 0x50e0, 0x5608, 0x5616, 0x5618 and 0x5626 lie
       within the declared mTable storage. Their location alone does not
       establish whether they belong to the component or to scene state.
       Everything else they touch is inherited (unk_0b4 from dScMgBase_c,
       +0x4660 from dScMgSingle3DBase_c). --- */
    s32 InitResources();      /* slot 0 -- ov006 0x0211c984 */
    s32 Behavior();           /* slot 6 -- ov006 0x0211c720 */
    s32 Render();             /* slot 9 -- ov006 0x0211c6c4 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgSound_c_size_must_be_0x562c[sizeof(dScMgSound_c) == 0x562c ? 1 : -1];
#endif

#endif
