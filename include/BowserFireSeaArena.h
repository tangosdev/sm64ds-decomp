/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * class BowserFireSeaArena: 6 matched functions.
 *
 * A Platform subclass -- the only one in ov060 -- which is what the destructor
 * shows: it tears down its own two members, then REWRITES THE VPTR to _ZTV8Platform
 * and tears down two more at Platform's offsets, then chains to Actor. That middle
 * vptr store is a base-class destructor running inline, and it is the evidence for
 * the base.
 *
 * Two sub-objects of its own, on top of Platform's two:
 *
 *     Platform            0x000 + 0x324 = 0x324   -> mModel2
 *     Model               0x324 + 0x050 = 0x374   -> mMovingMeshCollider2
 *     MovingMeshCollider  0x374 + 0x1c8 = 0x53c   -> padding, then unk_56c
 *
 * mMovingMeshCollider2 was a `u8` marker with 0x1f7 bytes of pad behind it; the pad
 * was the object.
 *
 * sizeof is 0x570, which is not inferred from the fields: BowserFireSeaArena_Spawn
 * asks ActorBase::operator new for 1392 bytes.
 *
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef BOWSERFIRESEAARENA_H
#define BOWSERFIRESEAARENA_H
#include "types.h"
#include "Model.h"
#include "MovingMeshCollider.h"

#ifdef __cplusplus

#include "Platform.h"

struct BowserFireSeaArena : Platform {
    /* THIS CLASS'S OWN, not Platform's -- Platform ends at 0x31e. They are what
       push mModel2 from 0x320 to 0x324, and the first two sit in the base's
       tail padding. */
    s16 unk_31e;                                /* 0x31e */
    s16 unk_320;                                /* 0x320 */
    s16 unk_322;                                /* 0x322 */
    Model mModel2;                              /* 0x324 */
    MovingMeshCollider mMovingMeshCollider2;    /* 0x374 */
    u8  pad_53c[0x30];
    s32 unk_56c;            /* 0x56c */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~BowserFireSeaArena();      /* slots 16 (D1), 17 (D0) */

    /* --- non-virtual --- */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char BowserFireSeaArena_size_must_be_0x570[
    sizeof(BowserFireSeaArena) == 0x570 ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base class to inherit
   Platform's and Actor's fields from and so spells the whole layout flat. */
struct BowserFireSeaArena {
    u8  pad_000[0x8e];
    s16 unk_08e;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN18BowserFireSeaArenaD1Ev.c] */
    Model mModel1;            /* 0x0d4 */
    MovingMeshCollider mMovingMeshCollider1;    /* 0x124 */
    u8  pad_2ec[0x32];
    s16 unk_31e;            /* 0x31e */
    s16 unk_320;            /* 0x320 */
    s16 unk_322;            /* 0x322 */
    /* Model member, named by _ZN5ModelD1Ev at +0x324 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel2;            /* 0x324 */
    MovingMeshCollider mMovingMeshCollider2;    /* 0x374 */
    u8  pad_53c[0x30];
    s32 unk_56c;            /* 0x56c */
};

#endif /* __cplusplus */

#endif
