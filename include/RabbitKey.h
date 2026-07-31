/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class RabbitKey: 5 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef RABBITKEY_H
#define RABBITKEY_H
#include "types.h"

struct RabbitKey {
    u8  pad_000[0x8];
    s32 unk_008;            /* 0x008 */
    u8  pad_00c[0x82];
    u8  unk_08e;            /* 0x08e */
    u8  pad_08f[0xd];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u8  pad_0b0[0x50];
    u8  unk_100;            /* 0x100 */
    u8  pad_101[0xf];
    u8  mModel;            /* 0x110 */
    u8  pad_111[0x4f];
    u8  mShadowModel;            /* 0x160 */
    u8  pad_161[0x27];
    u8  unk_188;            /* 0x188 */
    u8  pad_189[0x7];
    s32 unk_190;            /* 0x190 */
    u8  pad_194[0x8];
    s32 unk_19c;            /* 0x19c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
