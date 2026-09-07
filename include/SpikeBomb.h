/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * class SpikeBomb: 5 matched functions.
 *
 * Two sub-objects, each checked twice -- once by that class's own size assertion,
 * once by closing exactly on the next named field:
 *
 *     dActor_c                      0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     Model                      0x0d4 + 0x050 = 0x124   -> mdCcAcPos_c
 *     dCcAcPos_c  0x124 + 0x040 = 0x164   -> padding, then mStateIndex
 *
 * mdCcAcPos_c was a `u8` marker with 0x4b bytes of pad behind it.
 * The pad was the object: the class's own destructor calls
 * dCcAcPos_c's D1 at +0x124, and 0x40 lands exactly where the next
 * declared field starts.
 *
 * sizeof is 0x1b0, and it IS the field span: the last field mOpacity ends at 0x1af.
 *
 * It used to say 0x32c, "not inferred from the fields", on the authority of
 * daKpa3Bg_c_classInit -- a DIFFERENT CLASS's factory. That is the pair-by-name
 * error: daKirai_c_classInit is the factory that stores _ZTV9SpikeBomb, and it asks
 * fBase_c::operator new for 0x1b0. BowserSkyPlatform's own assert is 0x32c and is
 * correct; this class simply inherited its number, and 0x17d bytes of tail padding
 * were invented to reach it.
 *
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef SPIKEBOMB_H
#define SPIKEBOMB_H
#include "types.h"
#include "Model.h"
#include "dCcAcPos_c.h"

#ifdef __cplusplus

#include "dActor_c.h"

struct SpikeBomb : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by the class's own destructor calling Model's D1 at +0x0d4 -- a
       relocation the ROM build checks. */
    Model mModel;                                               /* 0x0d4 */
    dCcAcPos_c mdCcAcPos_c;       /* 0x124 */
    u8  pad_164[0xc];
    s32 mStateIndex;            /* 0x170 */
    s32 mHomePosX;            /* 0x174 */
    s32 mHomePosY;            /* 0x178 */
    s32 mHomePosZ;            /* 0x17c */
    /* Both taken in InitResources. mHomeHorzDist is Vec3_HorzLen of the spawn
       position -- how far out in XZ the bomb starts from the world origin.
       mHomeYOffset is the constant 0x2ee000, and mHomePosY is raised by
       mHomeYOffset >> 3 right after the home triple is copied.
       [_ZN9SpikeBomb13InitResourcesEv.cpp] */
    s32 mHomeHorzDist;            /* 0x180 */
    s32 mHomeYOffset;            /* 0x184 */
    u8  pad_188[0x20];
    s32 mSlotIndex;            /* 0x1a8 */
    u8  pad_1ac[0x2];
    u8  mOpacity;            /* 0x1ae */
    u8  pad_1af[0x1];       /* 0x1af, closing on the ROM's 0x1b0 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~SpikeBomb();       /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int CleanupResources();
    int Behavior();
    int InitResources();
    int Render();
};

typedef char SpikeBomb_size_must_be_0x1b0[sizeof(SpikeBomb) == 0x1b0 ? 1 : -1];

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
    dCcAcPos_c mdCcAcPos_c;       /* 0x124 */
    u8  pad_164[0xc];
    s32 mStateIndex;            /* 0x170 */
    s32 mHomePosX;            /* 0x174 */
    s32 mHomePosY;            /* 0x178 */
    s32 mHomePosZ;            /* 0x17c */
    /* Both taken in InitResources. mHomeHorzDist is Vec3_HorzLen of the spawn
       position -- how far out in XZ the bomb starts from the world origin.
       mHomeYOffset is the constant 0x2ee000, and mHomePosY is raised by
       mHomeYOffset >> 3 right after the home triple is copied.
       [_ZN9SpikeBomb13InitResourcesEv.cpp] */
    s32 mHomeHorzDist;            /* 0x180 */
    s32 mHomeYOffset;            /* 0x184 */
    u8  pad_188[0x20];
    s32 mSlotIndex;            /* 0x1a8 */
    u8  pad_1ac[0x2];
    u8  mOpacity;            /* 0x1ae */
    u8  pad_1af[0x1];       /* 0x1af, closing on the ROM's 0x1b0 */
};

#endif /* __cplusplus */

#endif
