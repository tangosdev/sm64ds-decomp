/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PrincessPeach: 5 matched functions, 6 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PRINCESSPEACH_H
#define PRINCESSPEACH_H
#include "types.h"
#include "ModelAnim.h"

struct PrincessPeach {
    u8  pad_000[0xd4];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over mAnimation (+0x50 = the
       Animation base), which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    u8  mShadowModel;            /* 0x138 */
    u8  pad_139[0x27];
    u8  mMovingCylinderClsn;            /* 0x160 */
    u8  pad_161[0x33];
    u8  mWithMeshClsn;            /* 0x194 */
    u8  pad_195[0x1bf];
    s32 unk_354;            /* 0x354 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
#endif
};

#endif
