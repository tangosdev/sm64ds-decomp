#ifndef FORTRESSTOWER_H
#define FORTRESSTOWER_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daObjSimpleBg_c. The reconstructed
 * factory daObjSimpleBg_c_classInit_BK_TOWER (historical alias
 * FortressTower_Spawn) constructs it for the BK_TOWER
 * registry profile.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct FortressTower : dBgActor_c {
    /* no fields of its own */

    /* --- vtable --- */
    virtual ~FortressTower();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char FortressTower_size_must_be_0x320[sizeof(FortressTower) == 0x320 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct FortressTower {
    u8  pad_000[0xc];
    u16 actorID;            /* 0x00c */
    u8  pad_00e[0xc6];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~FortressTower calls _ZN10dBgW_KcMbgD1Ev
       at +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
};

#endif /* __cplusplus */

#endif /* FORTRESSTOWER_H */
