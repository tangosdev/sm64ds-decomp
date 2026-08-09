/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Minimap: 9 matched functions, 25 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MINIMAP_H
#define MINIMAP_H
#include "types.h"

struct Minimap {
    u8  pad_000[0x50];
    s32 unk_050;            /* 0x050 */
    s32 unk_054;            /* 0x054 */
    s32 unk_058;            /* 0x058 */
    s32 mPosX;            /* 0x05c */
    s32 unk_060;            /* 0x060 */
    s32 unk_064;            /* 0x064 */
    u8  pad_068[0x8];
    /* Minimap::Behavior carries a shadow struct that declares these ranges as
       ARRAYS and indexes them; the header had them as flat padding, which is
       why the shadow existed at all. Sizes are the shadow's own, and the
       reconstruction is offset-neutral -- the struct still spans 0x256. */
    s32 unk_070[4];            /* 0x070 */
    s32 unk_080[4];            /* 0x080 */
    s32 unk_090;            /* 0x090 */
    s32 unk_094;            /* 0x094 */
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0[12];            /* 0x0a0 */
    s32 unk_0d0[12];            /* 0x0d0 */
    s32 unk_100[9];            /* 0x100 */
    s32 unk_124[9];            /* 0x124 */
    u8  pad_148[0x30];
    s32 unk_178;            /* 0x178 */
    s32 unk_17c;            /* 0x17c */
    s32 unk_180[8];            /* 0x180 */
    s32 unk_1a0[8];            /* 0x1a0 */
    u8  pad_1c0[0x14];
    s32 unk_1d4;            /* 0x1d4 */
    s32 unk_1d8;            /* 0x1d8 */
    s32 unk_1dc;            /* 0x1dc */
    /* 0x1e0 and 0x1f4 are each a Vector3 in the shadow -- three consecutive
       words handed to Vec3 helpers as one object. Kept as scalars here so the
       header does not assert a type the other eight matched functions never
       see, and taken as `*(Vector3*)&unk_1e0` at the two sites that need it. */
    s32 unk_1e0;            /* 0x1e0 */
    s32 unk_1e4;            /* 0x1e4 */
    s32 unk_1e8;            /* 0x1e8 */
    s32 unk_1ec;            /* 0x1ec */
    s32 unk_1f0;            /* 0x1f0 */
    s32 unk_1f4;            /* 0x1f4 */
    s32 unk_1f8;            /* 0x1f8 */
    s32 unk_1fc;            /* 0x1fc */
    s32 unk_200;            /* 0x200 */
    s32 unk_204;            /* 0x204 */
    s32 unk_208;            /* 0x208 */
    s32 unk_20c;            /* 0x20c */
    s32 unk_210;            /* 0x210 */
    s32 unk_214;            /* 0x214 */
    s32 unk_218;            /* 0x218 */
    /* Stays u16. Behavior's shadow spelled it s16 and then cast EVERY read to
       (u16) -- `(u16)ang >= 0x8000` and `(s32)(u16)ang >> 4` -- so the reads
       want unsigned and the two spellings mean the same thing. */
    u16 unk_21c;            /* 0x21c */
    s8  unk_21e[4];            /* 0x21e */
    s8  unk_222[12];            /* 0x222 */
    u8  pad_22e[0xc];
    s8  unk_23a[9];            /* 0x23a */
    u8  pad_243[0x5];
    s8  unk_248;            /* 0x248 */
    s8  unk_249[8];            /* 0x249 */
    u8  unk_251;            /* 0x251 */
    u8  pad_252[0x2];
    u8  unk_254;            /* 0x254 */
    u8  unk_255;            /* 0x255 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    /* STATIC -- no `this`. The ROM keeps r0 (the Vector3) in r4 and clobbers r1
       before any use, so the only incoming pointer is the argument. Staticness
       is not part of the mangling, so the symbol is unchanged either way; the
       evidence is the register use. */
    static void FixTHIPaintingRoomPos(Vector3 & v_);
    int InitResources();
    s32 CleanupResources();
    void OnPendingDestroy();
#endif
};

#endif
