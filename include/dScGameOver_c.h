#ifndef DSCGAMEOVER_C_H
#define DSCGAMEOVER_C_H

#include "dScene_c.h"

/* One of dScene_c's ten direct children (see the census in dScene_c.h) --
 * the game-over screen.
 *
 * BASE. dScene_c, confirmed by the RTTI edge (dScGameOver_c
 * ov003:0x020b1758 -> dScene_c arm9:0x020914d4, kind "si") and
 * independently by the vtable: _ZTV13dScGameOver_c (0x020b179c, ov003) is
 * 18 slots, matching dScene_c's own 18, and dScGameOver_c overrides exactly
 * the same seven dScene_c gives every other direct child -- 0, 3, 6, 9, 12,
 * 16, 17 (rtti_vtables.py --own dScGameOver_c). No new virtual is added.
 *
 * CONSTRUCTION. dScGameOver_c_classInit is the factory: it opens with
 * `_ZN7fBase_cnwEj(0x98)`, i.e. fBase_c::operator new(0x98). SIZE 0x98 is
 * read directly off the allocator call. No nested-object construction is
 * visible in this TU (unlike dScEntry_c/dScStarSel_c); every own field
 * below is a plain scalar.
 *
 * MEMBERS BELOW 0x50 are all inherited (dScene_c/dBase_c add none of their
 * own -- see dScene_c.h). InitResources (func_ov003_020b0b3c) fills four
 * back-to-back 8-entry s16 arrays at 0x050/0x060/0x070/0x080 (a "GAME
 * OVER" letter-drop layout: x/y/scale/rotation per glyph, the same four
 * offsets Render (func_ov003_020b0814) reads back for OAM::Render, and the
 * same 0x050/0x060 pair CleanupResources implicitly bounds by never
 * touching them), then zeroes five state bytes at 0x090..0x094. Behavior
 * (func_ov003_020b0894, itself vtable slot 6) is the second witness for
 * those five: `self[0x90]` is a 2-state cursor FSM, `self[0x91]` the
 * left/right selection, `self[0x92]`/`self[0x93]` countdown timers, and
 * `self[0x94]` an input-gate counter that Render also reads (>= 2 unlocks
 * the bouncing-arrow overlay). 0x98 - 0x90 - 5 = 3 bytes of trailing
 * padding to the class's own 4-byte alignment, matching the allocator
 * literal exactly.
 *
 * VTABLE ORDER follows dScene_c's (unchanged slot indices; dScGameOver_c
 * adds no new virtual). The destructor pair is at slots 16/17, same
 * convention as dScEntry_c and every other fBase_c-family class
 * (dScene_c.h/dScEntry_c.h): func_ov003_020b0580 (D1) writes
 * _ZTV13dScGameOver_c then dScene_c's/dBase_c's vptrs before
 * fBase_c::~fBase_c; func_ov003_020b05bc (D0) does the same then calls
 * Memory::Deallocate(this, GAME_HEAP_PTR) -- dScGameOver_c declares no
 * operator delete of its own, so this finds dScene_c's inline copy on its
 * immediate base, same as dScEntry_c.
 */
struct dScGameOver_c : dScene_c {
    s16 unk_050[8];                 /* 0x050 -- per-glyph x, InitResources/Render */
    s16 unk_060[8];                 /* 0x060 -- per-glyph y, InitResources/Render */
    s16 unk_070[8];                 /* 0x070 -- per-glyph scale, InitResources */
    s16 unk_080[8];                 /* 0x080 -- per-glyph rotation, InitResources */
    u8  unk_090;                    /* 0x090 -- cursor FSM state, Behavior */
    u8  unk_091;                    /* 0x091 -- left/right selection, Behavior */
    u8  unk_092;                    /* 0x092 -- countdown timer, Behavior */
    u8  unk_093;                    /* 0x093 -- countdown timer, Behavior */
    u8  unk_094;                    /* 0x094 -- input-gate counter, Behavior/Render */
    u8  pad_095[0x3];    /* 0x095 -- trailing alignment */

    /* Declared first and DEFINED INLINE, both deliberately -- the same form
       dScene_c.h and dBase_c.h use one and two levels up.

       INLINE, because inline-in-class is the only admissible source form that
       makes mwccarm 2004/b56 emit D1 before D0, and the cartridge puts D1 at
       0x020b0580 below D0 at 0x020b05bc. Out of line the compiler emits
       D2, D0, D1 -- the wrong order, plus a D2 that exists nowhere in the ROM.

       NOT the key function, despite being declared first: an inline virtual
       cannot be one. The key function is InitResources, the first DECLARED
       non-inline virtual below, and src/actors/dScGameOver_c.cpp defines it --
       which is what emits _ZTV13dScGameOver_c and, through its slots 16 and
       17, odr-uses and emits this destructor pair out of line. That is the
       mechanism that put 0x020b0580 and 0x020b05bc in the cartridge at all.

       The body is empty and still reproduces all 0x3c + 0x50 bytes: D1's three
       vptr stores are this class's own plus dScene_c's and dBase_c's inlined
       (both of those are inline in class too), then fBase_c::~fBase_c; every
       own field is a scalar, so there is no member destruction to write. D0
       adds the operator delete dScGameOver_c inherits from its immediate base
       dScene_c. */
    virtual ~dScGameOver_c() {}                          /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */
};

typedef char dScGameOver_c_size_must_be_0x98[sizeof(dScGameOver_c) == 0x98 ? 1 : -1];

#endif
