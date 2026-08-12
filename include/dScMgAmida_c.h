#ifndef DSCMGAMIDA_C_H
#define DSCMGAMIDA_C_H
#include "dScMgBase_c.h"

/* __destroy_arr / func_ov006_020d116c / NullDestructor_0203d47c: the same
   __destroy_arr(p, count, elemSize, dtor) idiom dScMgBase_c's own D1/D0 use
   for touchIcon_0f4 (see dScMgBase_c.h's file banner and
   src/_ZN11dScMgBase_cD1Ev.cpp) -- declared here, not per-destructor-file,
   matching dScMgHanachan_c.h's own placement. */
extern "C" void __destroy_arr(void *arr, int count, int elemSize, void *dtor);
extern "C" void func_ov006_020d116c(void);
extern "C" void NullDestructor_0203d47c(void);

/* dScMgAmida_c's own 0x80-element array at 0x4768 (destroyed via
   __destroy_arr in D1/D0, see the class banner below). Each 0x18-byte
   element is walked with individual field accesses in Behavior AND
   Render, not passed opaquely to a helper, so it gets a real named shape
   instead of raw bytes: position (+0x00/+0x04) accumulates velocity
   (+0x08/+0x0c) once per Behavior tick; Vec2_Len/func_0203d434/
   func_0203d630 read and rescale the velocity; a per-element timer
   (+0x10) counts up and gates the u8 active flag (+0x14) off past a
   threshold; Render reads position/timer/active directly for fixed-point
   (>>12) screen-space drawing. The trailing 3 bytes (+0x15..+0x17) are
   plain struct tail padding to reach the observed 0x18 stride -- nothing
   ever reads or writes them, so they are not modelled as a field. */
struct dScMgAmida_c_Piece {
    s32 posX;    /* +0x00 */
    s32 posY;    /* +0x04 */
    s32 velX;    /* +0x08 */
    s32 velY;    /* +0x0c */
    s32 timer;   /* +0x10 */
    u8  active;  /* +0x14 */
};
typedef char dScMgAmida_c_Piece_size_must_be_0x18[sizeof(dScMgAmida_c_Piece) == 0x18 ? 1 : -1];

/* dScMgAmida_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (zero
   RTTI edges name dScMgAmida_c as a base).

   Own vtable slots (python tools/rtti_vtables.py --own dScMgAmida_c): 0
   (InitResources), 5 (AfterCleanupResources -- the recovered source locally
   declared the base override as returning void*, which is WRONG; the real
   dScMgBase_c.h override returns void, so this now calls the base method as
   a plain statement instead of returning it, same fix dScMgLuigi_c's own
   slot 5 needed), 6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new
   slot, not yet named -- stays a raw extern "C" helper,
   src/func_ov006_020d52f0.cpp, same precedent as every other dScMgBase_c
   leaf's slot 18; it no longer includes this header at all -- its one
   inherited-field access at 0xbc is dScMgBase_c's own pad_0bc, not a named
   field there either, so it now reaches it via a raw char* offset, the same
   idiom dScMgPachinko_c's own slot 18 helper already uses), 31 ("Kill",
   src/func_ov006_020d11a0.cpp -- takes no parameters and never touches
   `this` at all, pure hardware-register/global reset; left as a raw helper,
   untouched by this migration), 34 (src/func_ov006_020d14c0.c -- takes
   three extra parameters (y, x, arg3) beyond `this` and draws a HUD
   digit/glyph into its own private Ctx-shaped state; same multi-argument
   shape dScMgTeresa_c's own slot 34 documents, left as a raw helper,
   untouched by this migration -- it never includes this header either).

   rtti_vtables.py --own ALSO reports slot 35 (func_ov006_020d1170, a
   one-line `((*(int*)((char*)c+8))&0xff)==1` check, same shape as slot 36
   below but for a different constant) -- unlike slot 36, nothing in
   InitResources/AfterCleanupResources/Behavior/Render/D1/D0 calls it (no
   cross-reference anywhere in src/ besides its own file), so it is left
   completely alone: not declared, not renamed, not even touched.

   SLOT 36 IS DIFFERENT FROM EVERY SIBLING'S SLOT 18+: it is a brand-new
   own slot (dScMgBase_c's own vtable is 36 slots, 0-35; Amida's is 37,
   0-36) that IS genuinely called, virtually, from three of this class's
   own real methods (InitResources, Behavior, Render), each through its own
   hand-rolled vtable-shim struct (`Obj::v90()` / `VtObj::m36()` /
   `Base::m_90()` -- the shim's exact shape varies per pre-migration file,
   all landing on the same byte offset, vtable_ptr+0x90 = 36*4). Declared
   here as a real virtual method (`Unk36`, a placeholder name -- its true
   ROM identity is not tree-wide reconstructed) -- but MEASURED, not
   assumed, that calling it as a normal `this->Unk36()` does NOT reproduce
   the ROM: it compiled Render 0xc bytes larger (0x2ac vs 0x2a0), because
   mwcc's own compiled slot for Unk36 (appended after dScMgBase_c's
   compiler-visible virtuals, which stop at Scene/ActorBase's own count --
   dScMgBase_c.h leaves slots 18-35 undeclared) does NOT land on true ROM
   slot 36 the way it accidentally does for a class's first-and-only new
   slot when that slot is 18 (dScMgCoin_c's OnYoshiTryEat, etc.) -- Amida's
   slot 36 is 18 slots further out, past all of dScMgBase_c's undeclared
   ones, so the coincidence that makes slot 18 "just work" does not extend
   here. That single size delta cascaded through the rest of the module
   (dsd does not hard-fail a declared-vs-compiled size mismatch, ~1400
   unrelated-looking functions afterward all mismatched) -- caught by
   rombuild, traced via final_link.o.xMAP. So all three call sites keep the
   pre-migration source's exact vtable-shim dispatch (see each method's own
   file), even though Unk36 is declared as a real virtual method here --
   unlike dScMgSlot1_c's OnHitByMegaChar/OnHitFromUnderneath, whose
   compiler-computed slot also doesn't match their true ROM position 27/28
   but is harmless there because neither is called from within Slot1's own
   migrated methods.

   THE DESTRUCTOR IS NON-TRIVIAL: unlike most siblings, this class explicitly
   destroys FOUR arrays via __destroy_arr, in this exact order, in BOTH D1
   and D0 (src/_ZN12dScMgAmida_cD1Ev.cpp and .../_D0Ev.cpp carry an identical
   body, same shape dScMgHanachan_c's own D1/D0 pair uses): the 0x80x0x18
   dScMgAmida_c_Piece array at 0x4768 (own per-element dtor
   func_ov006_020d116c, a no-op -- the element type needs no real cleanup),
   then the three NullDestructor_0203d47c-based 4x8-byte arrays at 0x4744,
   0x4724, and 0x4660 in that order (their own per-element dtor is also a
   no-op). The base-D2 call and own-vtable-write are compiler generated;
   D0's extra Memory::Deallocate comes for free from dScMgBase_c's own
   operator delete (its immediate base), no per-class copy needed. The
   tree-wide OnYoshiTryEat mislabel lands on THIS class's D0 too (its source
   comment said "recovered name: dScMgAmida_c_OnYoshiTryEat") -- same
   textbook D0 shape (vtable write, four array destroys, base D2,
   Deallocate) as every other mislabeled D0 in the family, not gameplay
   logic.

   arr4724/arr4744 (0x4724, 0x4744): two more of the three
   NullDestructor_0203d47c-based 4x8-byte arrays the destructor tears down.
   Unlike 0x4660 (never touched individually -- see below), BOTH are
   genuinely field-accessed, not just passed to a helper: Behavior reads and
   writes both elements' two 4-byte sub-fields directly (indexed 0..3, not
   just element 0), and Render reads arr4724's same two sub-fields as
   fixed-point (>>12) screen coordinates -- so both get a real {x,y}-shaped
   layout (a 2-D int array reproduces the true interleaved 8-byte stride
   exactly) instead of raw bytes. arr4744's own two fields are added into
   arr4724's and one of them is decremented by a fixed step every Behavior
   tick, consistent with a decaying delta/velocity feeding a position --
   named accordingly, though the exact game-side meaning (this is the
   Amida/ghost-leg-lottery minigame) is not otherwise confirmed. 0x4660,
   the third such array, is NEVER touched individually by any of the six
   real methods (only its base address is threaded through the generic
   construct/destroy helpers), so it stays raw bytes, matching the
   guidance that only genuinely field-accessed ranges get a granular
   layout.

   unk_470c/unk_4710: two 0x15800-byte buffers, confirmed in both
   directions -- Memory::Allocate'd in InitResources, Memory::Deallocate'd
   in AfterCleanupResources (only when vfSuccess==2; the pre-migration
   source's early `if (r1!=2) return c` means the base-class call is ALSO
   skipped in that case, preserved exactly). Typed as pointers, not s32,
   because slot 34's own raw Ctx-shaped view of this class (not migrated,
   see above) indexes through both of them as byte buffers
   (`ctx->unk470c + y*0x158`), and AfterCleanupResources itself reads them
   through a `*(void**)` cast.

   Fields below dScMgBase_c's own last-observed field (0x465c, DATA_SIZE
   0x465d per tools/check_header_offsets.py's own convention -- see
   dScMgBase_c.h) are genuinely THIS class's own, drawn only from what
   InitResources/AfterCleanupResources/Behavior/Render/D1/D0 directly touch.
   Fields touched only by the raw slot 18/31/34 helpers (0x46d8..0x4700's
   digit-drawing state, 0x4704..0x470b's flag bytes, and dScMgBase_c's own
   inherited 0xb4/0xbc slot 18 touches) are NOT represented here -- they
   stay inside a pad, same discipline as every prior sibling. The class's
   true allocation size, 0x53fc, is independently confirmed (not just the
   last-named-field's rounded end) by the untouched constructor's own
   `_ZN9ActorBasenwEj(0x53fc)` call (src/func_ov006_020d5974.cpp), which
   also independently corroborates all four destroyed-array offsets/sizes
   via its own construction-side func_020733a8 calls. */
struct dScMgAmida_c : dScMgBase_c {
    virtual ~dScMgAmida_c();
    virtual s32  InitResources();                       /* slot  0 */
    virtual void AfterCleanupResources(u32 vfSuccess);   /* slot  5 */
    virtual s32  Behavior();                             /* slot  6 */
    virtual s32  Render();                               /* slot  9 */
    virtual int  Unk36();                                /* slot 36 */

    u8  pad_465d[0x3];
    u8  unk_4660[4][8];      /* 0x4660 -- see banner */
    u8  pad_4680[0x50];
    s32 unk_46d0;             /* 0x46d0 -- Behavior's state switch */
    u8  unk_46d4;             /* 0x46d4 */
    u8  unk_46d5;             /* 0x46d5 */
    u8  pad_46d6[0x2];
    u8  pad_46d8[0x28];       /* 0x46d8 -- slot 34's own digit-drawing state;
                                  see banner */
    s32 unk_4700;             /* 0x4700 */
    u8  pad_4704[0x8];        /* 0x4704 -- slot 34's own flag bytes; see banner */
    u8 *unk_470c;             /* 0x470c -- see banner */
    u8 *unk_4710;             /* 0x4710 -- see banner */
    s32 unk_4714[4];          /* 0x4714 */
    s32 arr4724[4][2];        /* 0x4724 -- {x,y} pairs; see banner */
    s32 arr4744[4][2];        /* 0x4744 -- {dx,dy} pairs; see banner */
    s32 unk_4764;             /* 0x4764 */
    dScMgAmida_c_Piece arr4768[0x80]; /* 0x4768 -- see banner */
    s32 unk_5368;              /* 0x5368 */
    s32 unk_536c;              /* 0x536c */
    u8  pad_5370[0x4];
    s32 unk_5374;              /* 0x5374 */
    u8  pad_5378[0x24];
    s32 unk_539c[4];           /* 0x539c -- Render-only counters */
    s32 unk_53ac[4];           /* 0x53ac -- Render-only counters */
    u16 unk_53bc;              /* 0x53bc -- Render's BG2 scroll-offset index */
    u8  pad_53be[0x2];
    s32 unk_53c0;               /* 0x53c0 */
    s32 unk_53c4;               /* 0x53c4 */
    u8  pad_53c8[0x8];
    s32 unk_53d0;               /* 0x53d0 */
    s32 unk_53d4;               /* 0x53d4 */
    u8  pad_53d8[0x4];
    u8  unk_53dc;                /* 0x53dc */
    u8  unk_53dd;                /* 0x53dd */
    u8  pad_53de[0x2];
    s32 unk_53e0;                /* 0x53e0 */
    s32 unk_53e4;                /* 0x53e4 */
    s32 unk_53e8;                /* 0x53e8 */
    u8  pad_53ec[0x10];          /* tail padding to the true allocation size;
                                     see banner */
};

typedef char dScMgAmida_c_size_must_be_0x53fc[sizeof(dScMgAmida_c) == 0x53fc ? 1 : -1];

#endif
