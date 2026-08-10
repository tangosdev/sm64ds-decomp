/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class BowserShockwaves: 5 matched functions, 16 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef BOWSERSHOCKWAVES_H
#define BOWSERSHOCKWAVES_H
#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "MaterialChanger.h"
#include "TextureTransformer.h"

struct BowserShockwaves {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x6c];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_0dc (+0x8 = data),
       mAnimation1 (+0x50 = the Animation base), which the header declared separately
       inside it. */
    ModelAnim mModelAnim1;            /* 0x0d4 */
    /* TextureSequence member, named by the class's own destructor calling
       TextureSequence's D1 at +0x138 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN16BowserShockwavesD1Ev.c] */
    TextureSequence mTextureSequence1;            /* 0x138 */
    /* MaterialChanger member, named by the class's own destructor calling
       MaterialChanger's D1 at +0x14c -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN16BowserShockwavesD1Ev.c] */
    MaterialChanger mMaterialChanger1;            /* 0x14c */
    /* TextureTransformer member, named by the class's own destructor calling
       TextureTransformer's D1 at +0x160 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN16BowserShockwavesD1Ev.c] */
    TextureTransformer mTextureTransformer1;            /* 0x160 */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x174 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       stopped short of the object, so the member also takes over unk_17c (+0x8 = data),
       mAnimation2 (+0x50 = the Animation base), which the header declared separately
       inside it. */
    ModelAnim mModelAnim2;            /* 0x174 */
    /* TextureSequence member, named by the class's own destructor calling
       TextureSequence's D1 at +0x1d8 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN16BowserShockwavesD1Ev.c] */
    TextureSequence mTextureSequence2;            /* 0x1d8 */
    /* MaterialChanger member, named by the class's own destructor calling
       MaterialChanger's D1 at +0x1ec -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN16BowserShockwavesD1Ev.c] */
    MaterialChanger mMaterialChanger2;            /* 0x1ec */
    /* TextureTransformer member, named by the class's own destructor calling
       TextureTransformer's D1 at +0x200 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN16BowserShockwavesD1Ev.c] */
    TextureTransformer mTextureTransformer2;            /* 0x200 */
    s16 unk_214;            /* 0x214 */
#ifdef __cplusplus
    /* methods */
    int CleanupResources();
    int Render();
    int Behavior();
    int InitResources();
#endif
};

#endif
