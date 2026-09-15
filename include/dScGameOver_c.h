#ifndef DSCGAMEOVER_C_H
#define DSCGAMEOVER_C_H

#include "dScene_c.h"

/* One of dScene_c's ten direct children -- the game-over screen.
 *
 * BASE dScene_c, confirmed by the RTTI edge (dScGameOver_c ov003:0x020b1758
 * -> dScene_c arm9:0x020914d4) and the vtable: 18 slots like dScene_c's own,
 * overriding the same seven every direct child gets (0, 3, 6, 9, 12, 16,
 * 17). No new virtual is added.
 *
 * SIZE 0x98, read off the factory's `_ZN7fBase_cnwEj(0x98)`. The factory
 * (dScGameOver_c_classInit, src/d_s_game_over.cpp) sits outside this TU.
 * Members below 0x50 are all inherited. Field names are proven by this TU's
 * own uses: the glyph arrays InitResources lays out and Render/060c read
 * back, and the five cursor bytes Behavior/0730/Render share.
 */
struct dScGameOver_c : dScene_c {
    s16 mGlyphX[8];               /* 0x050 -- per-glyph x; 060c slides it to the 020b1774 target */
    s16 mGlyphY[8];               /* 0x060 -- per-glyph y; fixed 0x60, read by Render */
    s16 mGlyphDelay[8];           /* 0x070 -- per-glyph drop-in delay; counted down by 060c */
    s16 unk_080[8];               /* 0x080 -- zeroed by InitResources, never read back */
    u8  mCursorState;             /* 0x090 -- 0 = pick, 1 = countdown-and-commit */
    u8  mSelection;               /* 0x091 -- 0 = yes (continue), 1 = no */
    u8  mCommitTimer;             /* 0x092 -- counts down, then commits the selection */
    u8  mFlashTimer;              /* 0x093 -- while nonzero 0730 holds both boxes unselected */
    u8  mIntroPhase;              /* 0x094 -- 0 = drop-in, 1 = settle, 2 = cursor live */
    u8  pad_095[0x3];    /* 0x095 -- trailing alignment */

    /* Declared first and DEFINED INLINE, both deliberately -- the same form
       dScene_c.h and dBase_c.h use one and two levels up. Inline-in-class is
       the only source form that makes mwccarm emit D1 before D0, and the
       cartridge puts D1 at 0x020b0580 below D0 at 0x020b05bc; out of line it
       emits D2, D0, D1 plus a D2 the ROM never carried. An inline virtual
       cannot be the key function, so InitResources (the first DECLARED
       non-inline virtual) is -- and defining it here is what emits the
       vtable and, through slots 16/17, this destructor pair. The empty body
       still reproduces all 0x8c bytes: three inlined vptr stores plus
       fBase_c::~fBase_c, and D0's inherited operator delete. */
    virtual ~dScGameOver_c() {}                          /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScGameOver_c_size_must_be_0x98[sizeof(dScGameOver_c) == 0x98 ? 1 : -1];
#endif

#endif
