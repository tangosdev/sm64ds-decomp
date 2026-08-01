/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Bully: 6 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BULLY_H
#define BULLY_H
#include "types.h"

struct Bully {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x2c];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x36];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x43];
    u8  mModelAnim;            /* 0x110 */
    u8  pad_111[0x63];
    u8  mWithMeshClsn;            /* 0x174 */
    u8  pad_175[0x1bb];
    s32 mFileTable;            /* 0x330 */
    u8  pad_334[0x8];
    u8  mMovingCylinderClsn;            /* 0x33c */
    u8  pad_33d[0x33];
    u8  mShadowModel;            /* 0x370 */
    u8  pad_371[0x8b];
    s32 unk_3fc;            /* 0x3fc */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
    void InitResources();
#endif
};

#endif
