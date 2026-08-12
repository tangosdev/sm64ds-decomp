/* Banner claims tools/deepen_rtti.py generated this file. That tool has never
 * existed in this repo (git log --all --diff-filter=A -- '*deepen_rtti*' is
 * empty; see notes/runbook-type-reconstruction.md section 2) -- so treat every
 * field below except the one this comment documents as an unverified
 * placeholder, not as machine-checked evidence.
 *
 * class dScMgBase_c, recovered from ROM RTTI + vtable slot identity.
 * Offsets/widths are observed. Names are placeholders.
 *
 * dScMgBase_c : Scene, confirmed by tools/rtti_extract.py (single base,
 * dScMgBase_c's __si_class_type_info at ov004:0x020bbf6c points at dScene_c
 * arm9:0x020914d4, offset 0) -- same edge include/Scene.h's own census
 * documents. dScMgBase_c is ALSO a second hierarchy root: 15 direct RTTI
 * children, 32 transitive descendants (the minigame family --
 * notes/dscene-c-siblings-census.md section 2). Its own fields therefore
 * start at file-relative offset 0 == ROM offset 0x50 (sizeof(Scene)), same
 * as every other dScene_c child.
 *
 * THE DESTRUCTOR CASCADES ONE MORE LEVEL, same shape as Scene's own fix
 * (include/Scene.h's KEY FUNCTION note). dScMgBase_c has 32 descendants, so
 * its own D2/D1 must be DEFINED INLINE for them to inline it the way Stage
 * inlines Scene's -- a merely declared destructor can't be. Its own copy of
 * operator delete is for the same reason: mwcc only inlines a D0 route
 * through the class itself or its IMMEDIATE base, and for dScMgBase_c's
 * children that immediate base is dScMgBase_c, not Scene -- Scene's own copy
 * (which already covers dScMgBase_c's own D0, Scene being ITS immediate
 * base) is out of reach two levels down. See include/Actor.h and
 * include/Scene.h for the full mechanics; this is the same rule applied one
 * level further from the root.
 *
 * 0x0f4 IS hand-verified, from _ZN11dScMgBase_cD2Ev (src/_ZN11dScMgBase_cD2Ev.cpp):
 * it calls func_ov004_020b929c(c + 0xf4), which is
 *     __destroy_arr(p, count=8, elem_size=0x24, func_ov004_020b9280)
 * (0x0207328c is __destroy_arr / __cxa_vec_cleanup, config/arm9/symbols.txt:3050-3051).
 * The per-element destructor func_ov004_020b9280 writes two vtables into [r0+0]
 * back to back with no further calls -- 0x020bca7c then 0x020ad494 -- which
 * build/rtti.json identifies as _ZTVN10dMgPsOpt_c11TouchIcon_cE (ov004:0x020bca68,
 * "dMgPsOpt_c::TouchIcon_c") and _ZTV9dThIcon_c (ov001:0x020ad478, "dThIcon_c",
 * a root class) respectively -- i.e. TouchIcon_c : dThIcon_c, single inheritance,
 * offset 0, and both destructors are trivial enough to fully inline into vtable
 * writes. So 0x0f4 is 8 contiguous dMgPsOpt_c::TouchIcon_c, each 0x24 bytes,
 * spanning 0x0f4..0x214. Neither TouchIcon_c nor dThIcon_c has a header yet, so
 * the array stays raw bytes rather than an unverified struct type -- the
 * destructor below calls func_ov004_020b929c on it explicitly instead of
 * relying on member-wise auto-destruction, which is exactly what the ROM's
 * own D2 does and keeps this slice from having to invent two more classes'
 * worth of vtables to get dScMgBase_c's own destructor byte-correct.
 * 0x214..0x21c (8 bytes) has no matched access yet and stays padding.
 *
 * data_ov004_020beb68 is a global singleton pointer to the active
 * dScMgBase_c instance -- 60+ files across ov004/ov006 read it, each having
 * invented its own local type (Base*, Obj*, G*, char*, void*...). Retyping
 * it tree-wide is its own slice; this file only clears it, matching the D2's
 * own `*(int*)data_ov004_020beb68 = 0`, which -- since that file declared it
 * `extern int data_ov004_020beb68[]` -- writes 0 into the GLOBAL'S OWN
 * storage (arrays decay to their own address), not through a pointer it
 * holds. i.e. it is a plain global pointer being nulled, not a target being
 * zeroed through it. */
#ifndef DSCMGBASE_C_H
#define DSCMGBASE_C_H
#include "Scene.h"

extern "C" void func_ov004_020b929c(void *);
extern "C" void *data_ov004_020beb68;

struct dScMgBase_c : Scene {
    /* Declared first, deliberately -- see include/Scene.h's KEY FUNCTION
       note for why. Overrides slots 16 (D1) and 17 (D0).
       NOT DEFINED INLINE -- unlike Scene's, which is trivial (an empty
       body) and unconditionally inlined by every child. This body has
       real work (a global write, a function call), and measured against
       MgBounceAndPounce -- the first real descendant -- mwcc does NOT
       inline it: the ROM's own MgBounceAndPounce D1 (0x38 bytes) calls
       `_ZN11dScMgBase_cD2Ev` as a real `bl`, not a fully-inlined 0x84-byte
       body (measured directly: compiling MgBounceAndPounce's destructor
       against an INLINE-defined dScMgBase_c dtor produced exactly 0x84
       bytes, `999 word(s) differ` against the ROM's 0x38). Defined for
       real in src/_ZN11dScMgBase_cD1Ev.cpp and .../_D0Ev.cpp instead --
       same shape Stage.h documents for a leaf, except dScMgBase_c is not
       a leaf; its descendants simply don't inline a body this size. */
    virtual ~dScMgBase_c();

    /* dScMgBase_c's own copy, for the same reason Scene has one -- see the
       file banner. Unlocks D0 for all 32 descendants below. */
    void operator delete(void *ptr) { _ZN6Memory10DeallocateEPvP4Heap(ptr, data_020a0eac); }

    /* --- overrides of Scene's own virtuals, same signature, in _ZTV order.
           1, 2, 5, 7, 10 re-override slots Scene already gave a body;
           6, 9, 12 are the first override below ActorBase's own default. --- */
    virtual bool BeforeInitResources();                /* slot  1 */
    virtual void AfterInitResources(u32 vfSuccess);    /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual s32  Behavior();                           /* slot  6 */
    virtual int  BeforeBehavior();                     /* slot  7 */
    virtual s32  Render();                             /* slot  9 */
    virtual int  BeforeRender();                       /* slot 10 */
    virtual void OnPendingDestroy();                   /* slot 12 */

    /* Slots 18-35: eighteen further virtuals new at this class, same shape
       as Actor's own 13 new slots over ActorBase. All 18 target addresses
       are already matched source (func_ov004_* under arm9/ov004) -- see
       notes/dscene-c-siblings-census.md section 2 -- but their signatures
       are not yet reconstructed from the bodies, so they are left
       undeclared here rather than guessed. Naming a subset is fine. */

    u8  pad_050[0xc];
    s32 unk_05c;            /* 0x05c */
    u8  pad_060[0x44];
    s32 unk_0a4;            /* 0x0a4 */
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac -- real matched access, e.g. dScMgCard_c's own
                                InitResources (src/func_ov006_020dbaf0.cpp) and
                                dScMgMCarlo2_c's own (func_ov006_020fa56c.cpp) both
                                write `self->unk_0ac = self->unk_0a8;` */
    u8  pad_0b0[0x4];
    s32 unk_0b4;            /* 0x0b4 */
    s32 unk_0b8;            /* 0x0b8 */
    u32 unk_0bc;            /* 0x0bc -- real matched access, dScMg3DEsp_c's own
                                OnYoshiTryEat (src/func_ov006_020e9c20.c) reads
                                and writes it as a 4-byte int */
    u8  pad_0c0[0x3];
    u8  unk_0c3;            /* 0x0c3 */
    u8  pad_0c4[0x4];
    s32 unk_0c8;            /* 0x0c8 */
    u8  pad_0cc[0x24];
    s32 unk_0f0;            /* 0x0f0 */
    u8  touchIcon_0f4[8][0x24]; /* 0x0f4 -- 8x dMgPsOpt_c::TouchIcon_c : dThIcon_c,
                                    destroyed via __destroy_arr in dScMgBase_c's D2;
                                    see the file banner. */
    u8  pad_214[0x8];       /* 0x214 -- unaccounted for, no matched access yet */
    u32 unk_21c;            /* 0x21c */
    u32 unk_220;            /* 0x220 */
    u32 unk_224;            /* 0x224 */
    u8  pad_228[0x4400];
    s32 unk_4628;           /* 0x4628 */
    s32 unk_462c;           /* 0x462c */
    s32 unk_4630;           /* 0x4630 */
    s16 unk_4634;           /* 0x4634 */
    s16 unk_4636;           /* 0x4636 */
    s16 unk_4638;           /* 0x4638 */
    s16 unk_463a;           /* 0x463a */
    s16 unk_463c;           /* 0x463c */
    s16 unk_463e;           /* 0x463e */
    s32 unk_4640;           /* 0x4640 */
    u8  pad_4644[0x2];
    s16 unk_4646;           /* 0x4646 */
    u8  pad_4648[0x14];
    u8  unk_465c;           /* 0x465c */
};

/* NOT a claim that the object ends here -- dScMgBase_c has 32 RTTI
   descendants (notes/dscene-c-siblings-census.md), and MgBounceAndPounce
   (one of them, real ROM class dScMgD3DBase_c) is the first to need a
   number to start its own fields at. 0x465c is the last field any matched
   function has observed; asserting its rounded size (0x4660, 4-byte
   alignment) is the minimum claim that unblocks a derived class -- if it
   is short, MgBounceAndPounce's own fields would land on the wrong bytes
   and build_pin would catch it immediately, the same safety net
   check_header_offsets.py's own DATA_SIZE comment describes. */
typedef char dScMgBase_c_size_must_be_0x4660[sizeof(dScMgBase_c) == 0x4660 ? 1 : -1];

#endif
