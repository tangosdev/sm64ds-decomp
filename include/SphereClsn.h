/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class SphereClsn: 4 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SPHERECLSN_H
#define SPHERECLSN_H
#include "types.h"

/* fwd */
struct ClsnResult;
struct src_;
struct SphereClsn {
    u8  pad_000[0x10];
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x27];
    u8  unk_038;            /* 0x038 */
    u8  pad_039[0x3b];
    u8  mClsnResult1;            /* 0x074 */
    u8  pad_075[0x3];
    s64 unk_078;            /* 0x078 */
    s32 unk_080;            /* 0x080 */
    s32 unk_084;            /* 0x084 */
    s32 unk_088;            /* 0x088 */
    u16 unk_08c;            /* 0x08c */
    u16 unk_08e;            /* 0x08e */
    s32 unk_090;            /* 0x090 */
    s32 unk_094;            /* 0x094 */
    s32 unk_098;            /* 0x098 */
    u8  mClsnResult2;            /* 0x09c */
    u8  pad_09d[0x27];
    u8  mClsnResult3;            /* 0x0c4 */
    u8  pad_0c5[0x27];
    s32 unk_0ec;            /* 0x0ec */
    u8  pad_0f0[0x18];
    s32 unk_108;            /* 0x108 */
#ifdef __cplusplus
    /* methods */
    void SetFloorResult(const ClsnResult & src_);
#endif
};

#endif
