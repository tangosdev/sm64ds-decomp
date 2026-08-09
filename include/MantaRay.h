/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MantaRay: 5 matched functions, 25 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MANTARAY_H
#define MANTARAY_H
#include "types.h"
#include "ModelAnim.h"

struct MantaRay {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    u8  pad_098[0x4];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    /* 0x0a4..0x0b0 is written as a whole Vector3: Behavior builds a heading
       matrix and hands MulVec3Mat4x3 this address as its destination, so all
       twelve bytes are stored at once. unk_0a4 was declared u8 and is the x of
       that triple -- too narrow, and offset-neutral to widen.
       It OVERLAPS what Actor keeps here (0x0a0 terminal velocity, 0x0a8 vert
       speed), and the block right after the matrix multiply re-derives 0x0a8
       as max(0x0a0, 0x0a8 + 0x09c) -- so the swim direction is written first
       and gravity is then applied to its y. Repurposed, not coincidental. */
    s32 unk_0a4;            /* 0x0a4 */
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x4c];
    /* Was u8 and is two bytes: Behavior's only use hands its address to
       DecIfAbove0_Short, whose own definition takes `unsigned short*` and
       whose ROM body loads with ldrh (0x0203adbc). Same width defect as
       Enemy.h's 0x102, on the same evidence. Offset-neutral to widen. */
    u16 unk_100;            /* 0x100 */
    u8  pad_102[0xe];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x30c -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    ModelAnim mModelAnim;            /* 0x30c */
    u8  unk_370;            /* 0x370 */
    u8  pad_371[0xb];
    s32 unk_37c;            /* 0x37c */
    s32 unk_380;            /* 0x380 */
    s32 mPathNode;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
#endif
};

#endif
