/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class CylinderClsn: 7 matched functions, 10 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CYLINDERCLSN_H
#define CYLINDERCLSN_H
#include "types.h"

struct CylinderClsn {
    u8  unk_000;            /* 0x000 */
    u8  pad_001[0xb];
    s32 unk_00c;            /* 0x00c */
    s32 unk_010;            /* 0x010 */
    s32 unk_014;            /* 0x014 */
    s32 unk_018;            /* 0x018 */
    u8  unk_01c;            /* 0x01c */
    u8  pad_01d[0x3];
    s32 unk_020;            /* 0x020 */
    s32 unk_024;            /* 0x024 */
    u8  unk_028;            /* 0x028 */
    u8  pad_029[0x3];
    u8  unk_02c;            /* 0x02c */
#ifdef __cplusplus
    /* methods */
    void Clear();
    void Update();
#endif
};

#endif
