/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Butterfly: 5 matched functions, 23 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BUTTERFLY_H
#define BUTTERFLY_H
#include "types.h"
#include "ModelAnim.h"
#include "Model.h"

struct Butterfly {
    u8  pad_000[0x80];
    s32 unk_080;            /* 0x080 */
    s32 unk_084;            /* 0x084 */
    s32 unk_088;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x2];
    u16 unk_092;            /* 0x092 */
    u16 unk_094;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    u8  pad_09c[0x8];
    s32 unk_0a4;            /* 0x0a4 */
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x24];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_0f0 (+0x1c = mat4x3),
       mAnimation (+0x50 = the Animation base), which the header declared separately inside
       it. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* Model member, named by _ZN5ModelD1Ev at +0x138 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. The marker's pad stopped
       short of the object, so the member also takes over unk_154 (+0x1c = mat4x3), which
       the header declared separately inside it. */
    Model mModel;            /* 0x138 */
    u8  mShadowModel1;            /* 0x188 */
    u8  pad_189[0x27];
    u8  mShadowModel2;            /* 0x1b0 */
    u8  pad_1b1[0x27];
    u8  mWithMeshClsn;            /* 0x1d8 */
    u8  pad_1d9[0x1bb];
    u8  mMovingCylinderClsnWithPos;            /* 0x394 */
    u8  pad_395[0x4b];
    s32 unk_3e0;            /* 0x3e0 */
    s32 unk_3e4;            /* 0x3e4 */
    u8  pad_3e8[0x8];
    u8  unk_3f0;            /* 0x3f0 */
    u8  unk_3f1;            /* 0x3f1 */
#ifdef __cplusplus
    /* methods */
    int InitResources();
    int Render();
#endif
};

#endif
