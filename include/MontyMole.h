/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MontyMole: 5 matched functions, 11 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MONTYMOLE_H
#define MONTYMOLE_H
#include "types.h"

struct MontyMole {
    u8  pad_000[0x8];
    u32 unk_008;            /* 0x008 */
    u8  pad_00c[0xc8];
    u8  mModelAnim;            /* 0x0d4 */
    u8  pad_0d5[0x63];
    u8  mMovingCylinderClsn;            /* 0x138 */
    u8  pad_139[0x33];
    u8  unk_16c;            /* 0x16c */
    u8  pad_16d[0xf];
    s32 unk_17c;            /* 0x17c */
    u8  unk_180;            /* 0x180 */
    u8  unk_181;            /* 0x181 */
    u8  unk_182;            /* 0x182 */
    u8  unk_183;            /* 0x183 */
    u8  unk_184;            /* 0x184 */
    u8  pad_185[0x3];
    s32 unk_188;            /* 0x188 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
