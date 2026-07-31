/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Shark: 5 matched functions, 24 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SHARK_H
#define SHARK_H
#include "types.h"

struct Shark {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    u8  unk_05c;            /* 0x05c */
    u8  pad_05d[0x2f];
    s16 unk_08c;            /* 0x08c */
    s16 unk_08e;            /* 0x08e */
    s16 unk_090;            /* 0x090 */
    s16 unk_092;            /* 0x092 */
    s16 unk_094;            /* 0x094 */
    s16 unk_096;            /* 0x096 */
    u8  pad_098[0x4];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  unk_0a4;            /* 0x0a4 */
    u8  pad_0a5[0x3];
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x50];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mMovingCylinderClsnWithPos;            /* 0x110 */
    u8  pad_111[0x3f];
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1bb];
    u8  mModelAnim;            /* 0x30c */
    u8  pad_30d[0x63];
    u8  unk_370;            /* 0x370 */
    u8  pad_371[0x3];
    s32 unk_374;            /* 0x374 */
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
    u8  pad_380[0x8];
    s32 mPathID;            /* 0x388 */
    s32 unk_38c;            /* 0x38c */
    s32 mPathNodeIdx;            /* 0x390 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
