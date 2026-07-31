/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class UnknownVsEntry: 6 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef UNKNOWNVSENTRY_H
#define UNKNOWNVSENTRY_H
#include "types.h"

struct UnknownVsEntry {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x44];
    u8  mParticle;            /* 0x050 */
    u8  pad_051[0x81b];
    u8  mModel;            /* 0x86c */
    u8  pad_86d[0x1b];
    u8  unk_888;            /* 0x888 */
    u8  pad_889[0x33];
    u8  mModelAnim;            /* 0x8bc */
    u8  pad_8bd[0x4f];
    u8  mAnimation;            /* 0x90c */
    u8  pad_90d[0x573];
    u8  unk_e80;            /* 0xe80 */
    u8  pad_e81[0xa7];
    s32 unk_f28;            /* 0xf28 */
    s32 unk_f2c;            /* 0xf2c */
    s32 unk_f30;            /* 0xf30 */
    s32 unk_f34;            /* 0xf34 */
    s32 unk_f38;            /* 0xf38 */
    s32 unk_f3c;            /* 0xf3c */
    u8  unk_f40;            /* 0xf40 */
    u8  unk_f41;            /* 0xf41 */
    u8  unk_f42;            /* 0xf42 */
    u8  unk_f43;            /* 0xf43 */
    u8  unk_f44;            /* 0xf44 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
