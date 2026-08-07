/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Flag: 5 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FLAG_H
#define FLAG_H
#include "types.h"
#include "ModelAnim.h"

struct Flag {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_114 (+0x40 =
       mat4x3.t.x), unk_118 (+0x44 = mat4x3.t.y), unk_11c (+0x48 = mat4x3.t.z), mAnimation
       (+0x50 = the Animation base), which the header declared separately inside it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
