/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Number: 6 matched functions, 18 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef NUMBER_H
#define NUMBER_H
#include "types.h"
#include "Model.h"
#include "TextureSequence.h"

struct Number {
    u8  pad_000[0x8];
    s32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x34];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 mVertSpeed;            /* 0x0a8 */
    u8  pad_0ac[0x28];
    /* Model member. The cartridge's own ~Number calls _ZN5ModelD1Ev at +0x0d4 (D0/D1),
       a relocation the ROM build checks; recovered by tools/dtor_members.py. D1 and not
       D2, so it is this type and not an inlined base. */
    Model mModel;            /* 0x0d4 */
    /* TextureSequence member. The cartridge's own ~Number calls
       _ZN15TextureSequenceD1Ev at +0x124 (D0/D1), a relocation the ROM build checks;
       recovered by tools/dtor_members.py. D1 and not D2, so it is this type and not an
       inlined base. */
    TextureSequence mTextureSequence;            /* 0x124 */
    s32 unk_138;            /* 0x138 */
    s32 unk_13c;            /* 0x13c */
    s32 mStartPosY;            /* 0x140 */
    s32 unk_144;            /* 0x144 */
    s32 unk_148;            /* 0x148 */
    u16 unk_14c;            /* 0x14c */
    u8  unk_14e;            /* 0x14e */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

typedef char Number_size_must_be_0x150[sizeof(struct Number) == 0x150 ? 1 : -1];

#endif
