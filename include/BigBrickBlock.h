#ifndef BIGBRICKBLOCK_H
#define BIGBRICKBLOCK_H

#include "types.h"
#include "Platform.h"

/* Derives from Platform: the destructor stores this class's vtable, then
 * Platform's -- inlined -- then destroys the MovingMeshCollider at 0x124 and
 * the Model at 0xd4 before chaining to Actor. All three belong to Platform.
 * Everything this header used to restate below 0x31e was Actor's and
 * Platform's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

struct BigBrickBlock : Platform {
    u8 unk_31e;                       /* 0x31e */
    u8 unk_31f;                       /* 0x31f */
    u8 mEventID;                      /* 0x320 */
    u8  pad_321[0x3];
    Actor *mSwitch;                   /* 0x324 */

    /* --- vtable --- */
    virtual ~BigBrickBlock();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char BigBrickBlock_size_must_be_0x328[sizeof(BigBrickBlock) == 0x328 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct BigBrickBlock {
    u8  pad_000[0xc];
    u16 mActorId;            /* 0x00c */
    u8  pad_00e[0xc6];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN13BigBrickBlockD1Ev.c] */
    Model mModel;            /* 0x0d4 */
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1f9];
    u8  unk_31e;            /* 0x31e */
    u8  unk_31f;            /* 0x31f */
    u8  mEventID;            /* 0x320 */
    u8  pad_321[0x3];
    /* Actor * -- the ROM loads this WORD and passes it to _ZN5Actor15FindWithActorIDEjPS_
       as that function's `this`, which is an object address, so the word is a Actor *. It
       says nothing about the rest of the marker's span, which stays explicit padding. Was
       a u8 marker. */
    Actor *mSwitch;            /* 0x324 */
};

#endif /* __cplusplus */

#endif /* BIGBRICKBLOCK_H */
