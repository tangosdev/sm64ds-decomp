/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Goomba: 6 matched functions, 57 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef GOOMBA_H
#define GOOMBA_H
#include "types.h"

struct Goomba {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x2];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x4c];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0x6];
    u8  unk_107;            /* 0x107 */
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x3];
    s32 mDeathType;            /* 0x10c */
    u8  pad_110[0x1];
    u8  unk_111;            /* 0x111 */
    u8  unk_112;            /* 0x112 */
    u8  unk_113;            /* 0x113 */
    u8  pad_114[0x6c];
    u8  mMovingCylinderClsn;            /* 0x180 */
    u8  pad_181[0x33];
    u8  mWithMeshClsn;            /* 0x1b4 */
    u8  pad_1b5[0x1bb];
    u8  mModelAnim;            /* 0x370 */
    u8  pad_371[0x4f];
    u8  mAnimation;            /* 0x3c0 */
    u8  pad_3c1[0xb];
    s32 unk_3cc;            /* 0x3cc */
    s32 unk_3d0;            /* 0x3d0 */
    u8  mShadowModel;            /* 0x3d4 */
    u8  pad_3d5[0x27];
    u8  mMaterialChanger;            /* 0x3fc */
    u8  pad_3fd[0x13];
    s32 unk_410;            /* 0x410 */
    s32 unk_414;            /* 0x414 */
    s32 unk_418;            /* 0x418 */
    s32 unk_41c;            /* 0x41c */
    s32 unk_420;            /* 0x420 */
    s32 unk_424;            /* 0x424 */
    s32 unk_428;            /* 0x428 */
    s32 unk_42c;            /* 0x42c */
    s32 unk_430;            /* 0x430 */
    s32 mState;            /* 0x434 */
    s32 unk_438;            /* 0x438 */
    s32 unk_43c;            /* 0x43c */
    s32 unk_440;            /* 0x440 */
    s32 unk_444;            /* 0x444 */
    u8  pad_448[0x4];
    s32 unk_44c;            /* 0x44c */
    s16 unk_450;            /* 0x450 */
    u8  pad_452[0x2];
    s16 unk_454;            /* 0x454 */
    u16 mStuckTimer;            /* 0x456 */
    u16 unk_458;            /* 0x458 */
    s16 unk_45a;            /* 0x45a */
    u8  pad_45c[0x4];
    s32 mGoombaType;            /* 0x460 */
    u8  unk_464;            /* 0x464 */
    s8  unk_465;            /* 0x465 */
    u8  unk_466;            /* 0x466 */
    u8  unk_467;            /* 0x467 */
    u8  unk_468;            /* 0x468 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

#endif
