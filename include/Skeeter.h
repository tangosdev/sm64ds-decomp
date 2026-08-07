/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Skeeter: 5 matched functions, 22 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SKEETER_H
#define SKEETER_H
#include "types.h"
#include "ModelAnim.h"

struct Skeeter {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x2];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x6];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x64];
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x1];
    u8  unk_10a;            /* 0x10a */
    u8  pad_10b[0x5];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x30c -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       ran 0x4 bytes PAST the end of the object; that space is not evidenced and stays
       explicit padding rather than being folded into the member. */
    ModelAnim mModelAnim;            /* 0x30c */
    u8  pad_370[0x4];
    s32 unk_374;            /* 0x374 */
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
    u8  pad_380[0x1c];
    u8  unk_39c;            /* 0x39c */
    u8  pad_39d[0x7];
    s32 unk_3a4;            /* 0x3a4 */
    s32 unk_3a8;            /* 0x3a8 */
    s32 unk_3ac;            /* 0x3ac */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
