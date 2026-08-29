/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class dBgCh_Lin: 4 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * HAND-EXTENDED 2026-08-06 from dBgW_Kc::DetectClsn(dBgCh_Lin&) at ITCM
 * 0x01ffb0fc. Do not regenerate over this. Two of the generated names were
 * contradicted by the ROM and are corrected below (mPosX was lineEnd.z, mPosY was
 * clsnDist); 0x50 is newly named. Widths are unchanged -- the three s32 at
 * 0x54..0x5c became one Vector3 covering the same twelve bytes, and the struct
 * still spans 0x68. Byte-gated: _ZN9dBgCh_LinD1Ev (0x02037764) still MATCHes.
 * The class family this belongs to is written up in
 * notes/collision-query-classes.md.
 *
 * PROMOTED 2026-08-23 to a real MI declaration (notes/ctor-migration.md item
 * 2 -- the first in the tree). The ROM's own __vmi_class_type_info record
 * states the base list outright:
 *
 *     dBgCh    @ 0x00   polymorphic   (vptr store at +0)
 *     dBgPi    @ 0x10   polymorphic   (vptr store at +0x10; secondary block
 *                                named VTable_dBgPi_dBgCh_LinThunk)
 *     dM3dGLin @ 0x38   NON-polymorphic (no vptr, no store)
 *
 * The C++ branch declares exactly that; the C branch below stays field-for-
 * field IDENTICAL to what this header carried before promotion, because .c
 * translation units (the destructor among them) reach into these interiors by
 * the old member names and their code must not move.
 */
#ifndef DBGCH_LIN_H
#define DBGCH_LIN_H
#include "types.h"

#ifdef __cplusplus

#include "dBgCh.h"
#include "dBgPi.h"
#include "dM3dGLin.h"
#include "dM3dGSph.h"

struct dBgCh_Lin : dBgCh, dBgPi, dM3dGLin {
    /* Set to 1 on the hit path by dBgW_Kc::DetectClsn(dBgCh_Lin&)
       (`strb r0,[r1,#0x50]`); the role dBgCh_Gnd already names at its 0x48. */
    u8  hasClsn;            /* 0x050 */
    u8  pad_051[0x3];

    /* Was unk_054/unk_058/mPosX. ITCM DetectClsn materialises `add r5,r1,#0x54'
       once and reads [r5], [r5,#4], [r5,#8], so these three words are one
       Vector3 -- and the old `mPosX` at 0x5c was its z.

       THIS WORD HAS TWO ROLES and the name only covers the first. Checked
       2026-08-19 because a survey claimed the field was misnamed and should
       be `clsnPos'; it is not misnamed, it is dual-purpose:

         IN   func_02037608 seeds it with the line END --
              func_ov002_020fea4c(&b, thiz+0x38) reads b[3..5], i.e. 0x44, the
              dM3dGLin's second Vector3. It then sets 0x60 = Vec3_Dist(0x54,
              start), the full segment length. dBgW_KcMbg::DetectClsn
              relies on this, transforming 0x38 and 0x54 as the two endpoints.

         OUT  on a hit, func_020375ec(d, s) does d[21..23] = s[0..2], and
              d[21] is 0x54 -- the world-space collision point overwrites the
              end point. dBgCh_Lin::GetClsnPos (0x020375d0) reads it back and
              calls it clsnPos, which is correct AFTER a hit and wrong before.

       So both names are right at different times. Keep `lineEnd', which is the
       value the field is initialised with and the one callers pass in. */
    Vector3 lineEnd;        /* 0x054 -- in: line end. out: collision point. */

    /* Was mPosY. Read as the search seed on entry (`ldr r1,[r0,#0x60]`) and
       written with the winning distance on exit (`str r1,[r0,#0x60]`). */
    Fix12i clsnDist;        /* 0x060 */

    /* The segment's BOUNDING SPHERE, and the struct does not end at 0x65.
       Proven three ways, all matched code:
         C1            constructs it -- _ZN8dM3dGSphC1Ev(thiz + 0x64) stores
                       _ZTV8dM3dGSph
         D1            destroys it   -- dM3dGSph::~dM3dGSph(&self->unk_064)
         SetObjAndLine fills it      -- func_0203abd4(thiz + 0x64, &midpoint,
                       (clsnDist >> 1) + 0x1000)
       and dBgCh_Lin::DetectClsn culls candidate colliders against it, reading
       the centre at 0x68 and the radius at 0x74. dM3dGSph is 0x14, so this
       member alone carries the struct to 0x78. */
    dM3dGSph mBoundSphere;  /* 0x064 */

    u8  pad_078[0xc];       /* 0x078 through 0x083. PINNED AT 0x84 by the
                               embedding: dBgCh_Actr holds this class at 0x134
                               and its next word at 0x1b8, which is treated as
                               Actr's own field (Init sets 0x1000 there; the
                               alternative reading -- that 0x1b8 is this
                               class's last member, size 0x88 -- is byte-safe
                               either way because every access to that word
                               today goes through Actr). The standalone stack
                               footprints measured in ov029 (0x7c) and the
                               hand shadows elsewhere are slot-sharing
                               artefacts, not sizeof evidence. */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* Defined as real C++ in separate D1/D0 source files. Dedicated TUs with
     * that same definition enroll the compiler-emitted -0x10 adjustment
     * thunks; objisolate retains one ABI artifact per source and binds its
     * vptr stores to ROM data. Slots: primary D1/D0 at _ZTV9dBgCh_Lin,
     * secondary D1/D0 at VTable_dBgPi_dBgCh_LinThunk. */
    virtual ~dBgCh_Lin();

    /* DECLARED, defined out of line in src/_ZN9dBgCh_LinC1Ev.cpp as real C++
     * -- complete-object context for every ROM caller, hence C1.
     */
    dBgCh_Lin();

    /* ITS OWN COPY, TO RESOLVE AN AMBIGUITY THE MULTIPLE INHERITANCE CREATES.
       dBgCh_Lin derives from both dBgCh and dBgPi, and each of those declares
       an inline operator delete, so an inherited one is "ambiguous access to
       name found: dBgCh::operator delete and dBgPi::operator delete".
       Declaring it here picks the same deallocator both bases name
       (Memory::operator_delete2, 0x0203cbcc) and satisfies the rule in
       include/dActor_c.h that mwcc only inlines the member when it is in the
       class or its immediate base. This is load-bearing for the D0 at
       0x02037710, which ends at Memory::operator_delete2; a non-virtual inline
       member adds no field and no vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

    /* methods */
    Vector3 GetClsnPos();
};

#else

struct dBgCh_Lin {
    u8  pad_000[0x10];
    u8  mBgPiBase;            /* 0x010 - the dBgPi base starts here */
    u8  pad_011[0x27];
    /* NOT a bare Vector3, despite DetectClsn reading three words here: the
       destructor destroys something at 0x38 via _ZN8dM3dGLinD2Ev, so a
       sub-object lives here whose first member is the line start.

       RECOVERED 2026-08-19. The type is the cartridge's own `dM3dGLin', named
       in the ROM's RTTI (_ZTS8dM3dGLin at 0x02099284) and listed as a base of
       dBgCh_Lin -- this class -- at offset 56 = 0x38. It is 0x18 bytes, two
       Vector3: start at 0x38, end at 0x44. Non-polymorphic, so no vptr, and
       _ZN8dM3dGLinD2Ev is a 4-byte `bx lr' -- the trivial base destructor. Its
       RTTI lives in arm9, so the TYPE is not overlay-resident even though
       those three method bodies are. See notes/collision-system.md.

       Its two accessors are named BACKWARDS in some notes. Read the bodies:
         func_ov002_020fea4c  a[0..2] = b[3..5]  -> reads 0x44  -> GetEnd
         func_ov002_020fea68  a[0..2] = b[0..2]  -> reads 0x38  -> GetStart */
    u8  mLineBase;            /* 0x038 -- dM3dGLin base: start 0x38, end 0x44 */
    u8  pad_039[0x17];
    u8  hasClsn;            /* 0x050 */
    u8  pad_051[0x3];
    Vector3 lineEnd;        /* 0x054 -- in: line end. out: collision point. */
    Fix12i clsnDist;        /* 0x060 */
    u8  mBoundSphere[0x14]; /* 0x064 - a dM3dGSph, see include/dM3dGSph.h.
                               Sized bytes rather than the type because
                               check_header_offsets cannot size a struct-typed
                               member and goes UNPARSED, which blinds it to the
                               rest of the header. */
    u8  pad_078[0xc];       /* 0x078 - same pin as the C++ branch; no member
                               offsets above this move */
};

#endif /* __cplusplus */

/* SIZE PINNED AT 0x84 on the C++ branch (see pad_078 above). The C branch
   still ends at mBoundSphere because .c translation units reach into these
   interiors by the old member names; it may only trail while nothing embeds
   the class by value through the C view. Spelled struct-tag-style so the
   assert holds in both languages. */
typedef char dBgCh_Lin_size_must_be_0x84[
    sizeof(struct dBgCh_Lin) == 0x84 ? 1 : -1];

#endif /* DBGCH_LIN_H */
