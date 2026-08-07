/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SnowmanHead: 5 matched functions, 10 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SNOWMANHEAD_H
#define SNOWMANHEAD_H
#include "types.h"
#include "Model.h"

struct SnowmanHead {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x48];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    u8  mTextureSequence;            /* 0x124 */
    u8  pad_125[0x13];
    u8  mMovingCylinderClsn;            /* 0x138 */
    u8  pad_139[0x33];
    u8  mWithMeshClsn;            /* 0x16c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
