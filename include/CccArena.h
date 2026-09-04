/* class CccArena : dBgActor_c. Real C++ form.
 *
 * Base and size from the factory (src/daObjEwbIce_c_classInit_EWB_ICE_A.c):
 * fBase_c::operator new(828) -- 0x33c -- then dBgActor_c::dBgActor_c(),
 * then stores _ZTV8CccArena. No intermediate: the destructor stores only
 * its own vptr and dBgActor_c's, with zero extra member-destructor calls
 * in between (src/_ZN8CccArenaD1Ev.c), so every field below 0x320 is a
 * scalar, not a sub-object.
 *
 * dBgActor_c ends at 0x320; CccArena adds 0x1c more bytes, closing exactly
 * on 0x33c (the operator new literal), matching the old AUTO-GENERATED
 * header's own field span -- kept as-is, just re-parented.
 *
 * SM64DS RTTI names the implementation daObjEwbIce_c. The reconstructed
 * factory daObjEwbIce_c_classInit_EWB_ICE_A (historical alias
 * CccArena_Spawn) constructs it for the EWB_ICE_A
 * registry profile.
 */
#ifndef CCCARENA_H
#define CCCARENA_H
#include "types.h"
#include "dBgW_KcMbg.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

struct CccArena : dBgActor_c {
    /* dBgActor_c's own last named fields (unk_31c/unk_31d) are u8, so its
       sizeof (0x320) includes 2 bytes of pure tail padding to reach 4-byte
       alignment. mwcc reuses that tail padding for a derived class's first
       member when that member's OWN alignment does not require the full
       round-up -- a byte field placed first lands at 0x31e, not 0x320,
       which the ROM disagrees with (Behavior reads [this, #0x320]).
       Claiming it explicitly here forces unk_320 to its real offset. */
    u8  pad_31e[0x2];
    u8  mState;             /* 0x320 -- read as a pointer to the current state descriptor */
    u8  pad_321[0xb];
    u8  mVariant;           /* 0x32c -- 0/1/2 from actorID 0xaa/0xab/0xac; indexes the three ov073 file tables */
    u8  mSpawnIndex;        /* 0x32d -- the per-variant global counter's value at spawn */
    u8  pad_32e[0x2];
    u16 unk_330;            /* 0x330 */
    u8  pad_332[0x2];
    s32 unk_334;            /* 0x334 */
    s32 unk_338;            /* 0x338 */

    /* --- vtable --- */
    virtual ~CccArena();

    /* Slot 31, dBgActor_c's own new virtual (include/dBgActor_c.h).
       Attributed by the vtable: _ZTV8CccArena + 4*31 = 0x021231e8 + 0x7c =
       0x02123264; config/arm9/overlays/ov073/relocs.txt confirms
       0x02123264 -> 0x021223f4. NOT the key function: the destructor is
       declared out of line and defined identically in both D1Ev.cpp and
       D0Ev.cpp, so those two TUs keep emitting the vtable -- checked with
       objisolate, not assumed. */
    virtual void Kill();              /* slot 31 */

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char CccArena_size_must_be_0x33c[sizeof(CccArena) == 0x33c ? 1 : -1];

#else

#include "Model.h"

/* The C spelling of the same object, flat. Retained for any leftover C
   translation unit, same arrangement as include/ShadowModel.h. */
struct CccArena {
    u8  pad_000[0xc];
    u16 mActorID;            /* 0x00c */
    u8  pad_00e[0x4e];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0x42];
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~CccArena calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMeshCollider;            /* 0x124 */
    u8  pad_2ec[0x34];
    u8  mState;             /* 0x320 -- read as a pointer to the current state descriptor */
    u8  pad_321[0xb];
    u8  mVariant;           /* 0x32c -- 0/1/2 from actorID 0xaa/0xab/0xac; indexes the three ov073 file tables */
    u8  mSpawnIndex;        /* 0x32d -- the per-variant global counter's value at spawn */
    u8  pad_32e[0x2];
    u16 unk_330;            /* 0x330 */
    u8  pad_332[0x2];
    s32 unk_334;            /* 0x334 */
    s32 unk_338;            /* 0x338 */
};

#endif /* __cplusplus */

#endif
