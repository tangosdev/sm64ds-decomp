/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BigBully: 5 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BIGBULLY_H
#define BIGBULLY_H
#include "types.h"

struct BigBully {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x34];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x20];
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x43];
    u8  mModelAnim;            /* 0x110 */
    u8  pad_111[0x63];
    u8  mWithMeshClsn;            /* 0x174 */
    u8  pad_175[0x1bb];
    u8  unk_330;            /* 0x330 */
    u8  pad_331[0xb];
    u8  mMovingCylinderClsn;            /* 0x33c */
    u8  pad_33d[0x33];
    u8  mShadowModel;            /* 0x370 */
    u8  pad_371[0x89];
    u16 mSecretSoundCounter;            /* 0x3fa */
    u8  unk_3fc;            /* 0x3fc */
    u8  unk_3fd;            /* 0x3fd */
    u8  unk_3fe;            /* 0x3fe */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
