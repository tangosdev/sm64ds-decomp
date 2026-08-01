/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ShadowModel: 7 matched functions, 7 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SHADOWMODEL_H
#define SHADOWMODEL_H
#include "types.h"

struct ShadowModel {
    u8  pad_000[0x8];
    u8  unk_008;            /* 0x008 */
    u8  pad_009[0x3];
    u8  unk_00c;            /* 0x00c */
    u8  pad_00d[0x3];
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x3];
    u8  unk_014;            /* 0x014 */
    u8  pad_015[0x3];
    u8  unk_018;            /* 0x018 */
    u8  pad_019[0x3];
    u8  unk_01c;            /* 0x01c */
    u8  pad_01d[0x7];
    u8  unk_024;            /* 0x024 */
#ifdef __cplusplus
    /* methods */
    int InitCuboid();
    int InitCylinder();
#endif
};

#endif
