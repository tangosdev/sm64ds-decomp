/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MeshCollider: 7 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MESHCOLLIDER_H
#define MESHCOLLIDER_H
#include "types.h"

/* fwd */
struct CLPS_Block;
struct KCL_File;
struct arg1_;
struct arg2_;
struct f_;
struct MeshCollider {
    u8  pad_000[0x4];
    s32 unk_004;            /* 0x004 */
    s32 unk_008;            /* 0x008 */
    s32 unk_00c;            /* 0x00c */
    u8  pad_010[0x10];
    s32 unk_020;            /* 0x020 */
    u8  unk_024;            /* 0x024 */
    u8  pad_025[0x3];
    s32 unk_028;            /* 0x028 */
    s32 unk_02c;            /* 0x02c */
    s32 unk_030;            /* 0x030 */
    u8  unk_034;            /* 0x034 */
    u8  unk_035;            /* 0x035 */
    u8  pad_036[0x2];
    s32 unk_038;            /* 0x038 */
    s32 unk_03c;            /* 0x03c */
    s32 unk_040;            /* 0x040 */
    s32 unk_044;            /* 0x044 */
    s32 unk_048;            /* 0x048 */
    u8  unk_04c;            /* 0x04c */
    u8  unk_04d;            /* 0x04d */
#ifdef __cplusplus
    /* methods */
    void SetFile(KCL_File * arg1_, CLPS_Block & arg2_);
    void UpdateFileOffsets(KCL_File & f_);
    void Virtual08();
#endif
};

#endif
