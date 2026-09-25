/* daFRing_c -- Bowser's shockwaves (registry profile FIRERING).
 * Two rings, each a model plus a texture sequence, a material changer
 * and a texture transformer. mFrame counts up from 0; Behavior shocks
 * the player while it is still below 70.
 *
 * BASE: dActor_c, direct. _ZTI9daFRing_c at ov060 0x0211ab14 reads
 * [__si_class_type_info, _ZTS9daFRing_c (0x0211ab08), _ZTI8dActor_c], and the
 * vtable at 0x0211ab54 carries that _ZTI at its address point minus 4. The
 * tree previously called the class BowserShockwaves (coined).
 *
 * EIGHT sub-objects in two identical groups of four, and every one closes
 * exactly on the next -- no padding anywhere between 0x0d4 and 0x214:
 *
 *     dActor_c            0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     ModelAnim           0x0d4 + 0x064 = 0x138   -> mTextureSequence1
 *     TextureSequence     0x138 + 0x014 = 0x14c   -> mMaterialChanger1
 *     MaterialChanger     0x14c + 0x014 = 0x160   -> mTextureTransformer1
 *     TextureTransformer  0x160 + 0x014 = 0x174   -> mModelAnim2
 *     ModelAnim           0x174 + 0x064 = 0x1d8   -> mTextureSequence2
 *     TextureSequence     0x1d8 + 0x014 = 0x1ec   -> mMaterialChanger2
 *     MaterialChanger     0x1ec + 0x014 = 0x200   -> mTextureTransformer2
 *     TextureTransformer  0x200 + 0x014 = 0x214   -> mFrame
 *
 * Each member is named by the class's own destructor, which calls that type's
 * D1 (not D2, so a member and not an inlined base) at its literal offset --
 * relocations the ROM build checks. The calls come in exactly reverse
 * declaration order, 0x200, 0x1ec, 0x1d8, 0x174, 0x160, 0x14c, 0x138, 0x0d4,
 * then dActor_c's base destructor.
 *
 * THE TWO GROUPS ARE NOT AN ARRAY, and the destructor is what says so. A
 * member array is torn down through the runtime's array-cleanup helper; this
 * class's destructor makes eight separate D1 calls at eight literal offsets.
 * (Whether the four-member group was a named type in the original source is
 * not decidable from the bytes -- the layout and the teardown are identical
 * either way.)
 *
 * sizeof is 0x218, which is not inferred from the fields: daFRing_c_classInit
 * asks fBase_c::operator new for 536 bytes.
 */
#ifndef DAFRING_C_H
#define DAFRING_C_H
#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "MaterialChanger.h"
#include "TextureTransformer.h"

#ifdef __cplusplus

#include "dActor_c.h"

struct daFRing_c : dActor_c {
    u8 pad_0d0[0x4];
    ModelAnim mModelAnim1;                     /* 0x0d4 */
    TextureSequence mTextureSequence1;         /* 0x138 */
    MaterialChanger mMaterialChanger1;         /* 0x14c */
    TextureTransformer mTextureTransformer1;   /* 0x160 */
    ModelAnim mModelAnim2;                     /* 0x174 */
    TextureSequence mTextureSequence2;         /* 0x1d8 */
    MaterialChanger mMaterialChanger2;         /* 0x1ec */
    TextureTransformer mTextureTransformer2;   /* 0x200 */
    u16 mFrame;                                /* 0x214 */
    u8 pad_216[0x2];

    /* Out of line in daFRing_c.cpp: the key function, so that TU emits
     * _ZTV/_ZTI/_ZTS9daFRing_c. vtable slots 16 (D1) and 17 (D0). */
    virtual ~daFRing_c();

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. */
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
typedef char daFRing_c_size_must_be_0x218[
    sizeof(daFRing_c) == 0x218 ? 1 : -1];
#endif

#else

/* The same object for a C translation unit, which has no base class to
   inherit dActor_c's fields from and so spells the whole layout flat. */
struct daFRing_c {
    u8 pad_000[0x5c];
    s32 mPosX;                                 /* 0x05c */
    s32 mPosY;                                 /* 0x060 */
    s32 mPosZ;                                 /* 0x064 */
    u8 pad_068[0x6c];
    ModelAnim mModelAnim1;                     /* 0x0d4 */
    TextureSequence mTextureSequence1;         /* 0x138 */
    MaterialChanger mMaterialChanger1;         /* 0x14c */
    TextureTransformer mTextureTransformer1;   /* 0x160 */
    ModelAnim mModelAnim2;                     /* 0x174 */
    TextureSequence mTextureSequence2;         /* 0x1d8 */
    MaterialChanger mMaterialChanger2;         /* 0x1ec */
    TextureTransformer mTextureTransformer2;   /* 0x200 */
    u16 mFrame;                                /* 0x214 */
    u8 pad_216[0x2];
};

#endif /* __cplusplus */

#endif
