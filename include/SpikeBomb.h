/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * class SpikeBomb: 5 matched functions.
 *
 * Two sub-objects, each checked twice -- once by that class's own size assertion,
 * once by closing exactly on the next named field:
 *
 *     Actor                      0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     Model                      0x0d4 + 0x050 = 0x124   -> mMovingCylinderClsnWithPos
 *     MovingCylinderClsnWithPos  0x124 + 0x040 = 0x164   -> padding, then unk_170
 *
 * mMovingCylinderClsnWithPos was a `u8` marker with 0x4b bytes of pad behind it.
 * The pad was the object: the class's own destructor calls
 * MovingCylinderClsnWithPos's D1 at +0x124, and 0x40 lands exactly where the next
 * declared field starts.
 *
 * sizeof is 0x32c, which is not inferred from the fields: BowserSkyPlatform_Spawn
 * asks ActorBase::operator new for 812 bytes.
 *
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef SPIKEBOMB_H
#define SPIKEBOMB_H
#include "types.h"
#include "Model.h"
#include "MovingCylinderClsnWithPos.h"

#ifdef __cplusplus

#include "Actor.h"

struct SpikeBomb : Actor {
    u8  pad_0d0[0x4];
    /* Named by the class's own destructor calling Model's D1 at +0x0d4 -- a
       relocation the ROM build checks. */
    Model mModel;                                               /* 0x0d4 */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;       /* 0x124 */
    u8  pad_164[0xc];
    s32 unk_170;            /* 0x170 */
    s32 unk_174;            /* 0x174 */
    s32 unk_178;            /* 0x178 */
    s32 unk_17c;            /* 0x17c */
    s32 unk_180;            /* 0x180 */
    s32 unk_184;            /* 0x184 */
    u8  pad_188[0x20];
    s32 unk_1a8;            /* 0x1a8 */
    u8  pad_1ac[0x2];
    u8  unk_1ae;            /* 0x1ae */
    u8  pad_1af[0x17d];

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~SpikeBomb();       /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int CleanupResources();
    int Behavior();
    int InitResources();
    int Render();
};

typedef char BowserSkyPlatform_size_must_be_0x32c[
    sizeof(SpikeBomb) == 0x32c ? 1 : -1];

#else

struct SpikeBomb {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    u8  pad_08c[0x48];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN9SpikeBombD0Ev.c] */
    Model mModel;            /* 0x0d4 */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;       /* 0x124 */
    u8  pad_164[0xc];
    s32 unk_170;            /* 0x170 */
    s32 unk_174;            /* 0x174 */
    s32 unk_178;            /* 0x178 */
    s32 unk_17c;            /* 0x17c */
    s32 unk_180;            /* 0x180 */
    s32 unk_184;            /* 0x184 */
    u8  pad_188[0x20];
    s32 unk_1a8;            /* 0x1a8 */
    u8  pad_1ac[0x2];
    u8  unk_1ae;            /* 0x1ae */
    u8  pad_1af[0x17d];
};

#endif /* __cplusplus */

#endif
