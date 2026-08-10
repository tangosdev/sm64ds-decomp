/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base by hand.
 *
 * class BowserShockwaves: 5 matched functions.
 *
 * EIGHT sub-objects in two identical groups of four, and every one closes exactly
 * on the next -- no padding anywhere between 0x0d4 and 0x214:
 *
 *     Actor               0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     ModelAnim           0x0d4 + 0x064 = 0x138   -> mTextureSequence1
 *     TextureSequence     0x138 + 0x014 = 0x14c   -> mMaterialChanger1
 *     MaterialChanger     0x14c + 0x014 = 0x160   -> mTextureTransformer1
 *     TextureTransformer  0x160 + 0x014 = 0x174   -> mModelAnim2
 *     ModelAnim           0x174 + 0x064 = 0x1d8   -> mTextureSequence2
 *     TextureSequence     0x1d8 + 0x014 = 0x1ec   -> mMaterialChanger2
 *     MaterialChanger     0x1ec + 0x014 = 0x200   -> mTextureTransformer2
 *     TextureTransformer  0x200 + 0x014 = 0x214   -> unk_214
 *
 * THE TWO GROUPS ARE NOT AN ARRAY, and the destructor is what says so. A member
 * array is torn down through the runtime's array-cleanup helper; this class's
 * destructor makes eight separate D1 calls, at eight literal offsets, in reverse
 * order. Eight members it is. (Whether the four-member group was a named type in
 * the original source is not decidable from the bytes -- the layout and the
 * teardown are identical either way.)
 *
 * sizeof is 0x218, which is not inferred from the fields: BowserShockwaves_Spawn
 * asks ActorBase::operator new for 536 bytes.
 *
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef BOWSERSHOCKWAVES_H
#define BOWSERSHOCKWAVES_H
#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "MaterialChanger.h"
#include "TextureTransformer.h"

#ifdef __cplusplus

#include "Actor.h"

struct BowserShockwaves : Actor {
    u8  pad_0d0[0x4];
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
    u8  pad_216[0x2];

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~BowserShockwaves();        /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int CleanupResources();
    int Render();
    int Behavior();
    int InitResources();
};

typedef char BowserShockwaves_size_must_be_0x218[
    sizeof(BowserShockwaves) == 0x218 ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base class to inherit
   Actor's fields from and so spells the whole layout flat. Every current includer
   is a .cpp; this half is kept so that a future C one gets the right offsets
   rather than a parse error. */
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
    u8  pad_216[0x2];
};

#endif /* __cplusplus */

#endif
