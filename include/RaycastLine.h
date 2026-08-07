/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RaycastLine: 4 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * HAND-EXTENDED 2026-08-06 from MeshCollider::DetectClsn(RaycastLine&) at ITCM
 * 0x01ffb0fc. Do not regenerate over this. Two of the generated names were
 * contradicted by the ROM and are corrected below (mPosX was lineEnd.z, mPosY was
 * clsnDist); 0x50 is newly named. Widths are unchanged -- the three s32 at
 * 0x54..0x5c became one Vector3 covering the same twelve bytes, and the struct
 * still spans 0x68. Byte-gated: _ZN11RaycastLineD1Ev (0x02037764) still MATCHes.
 * The class family this belongs to is written up in
 * notes/collision-query-classes.md. */
#ifndef RAYCASTLINE_H
#define RAYCASTLINE_H
#include "types.h"

struct RaycastLine {
    u8  pad_000[0x10];
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x27];
    /* NOT a bare Vector3, despite DetectClsn reading three words here: the
       destructor destroys something at 0x38 via func_ov002_020feab8, so a
       sub-object lives here whose first member is the line start. Left unnamed
       until that type is recovered -- see notes/collision-query-classes.md. */
    u8  unk_038;            /* 0x038 */
    u8  pad_039[0x17];
    /* Set to 1 on the hit path by MeshCollider::DetectClsn(RaycastLine&)
       (`strb r0,[r1,#0x50]`); the role RaycastGround already names at its 0x48. */
    u8  hasClsn;            /* 0x050 */
    u8  pad_051[0x3];
    /* Was unk_054/unk_058/mPosX. ITCM DetectClsn materialises `add r5,r1,#0x54`
       once and reads [r5], [r5,#4], [r5,#8], so these three words are one
       Vector3 -- and the old `mPosX` at 0x5c was its z. */
    Vector3 lineEnd;        /* 0x054 */
    /* Was mPosY. Read as the search seed on entry (`ldr r1,[r0,#0x60]`) and
       written with the winning distance on exit (`str r1,[r0,#0x60]`). */
    Fix12i clsnDist;        /* 0x060 */
    u8  unk_064;            /* 0x064 */
};

#endif
