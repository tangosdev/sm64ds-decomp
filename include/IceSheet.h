#ifndef ICESHEET_H
#define ICESHEET_H

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
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct IceSheet : dBgActor_c {
    /* no fields of its own */

    /* --- vtable --- */
    /* DECLARED FIRST, AND IT STAYS FIRST. Out of line, so it is this class's key
       function, and src/_ZN8IceSheetD1Ev.cpp / D0Ev.cpp define it as a real
       method. Kill below must not displace it. */
    virtual ~IceSheet();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    virtual void OnGroundPounded(dActor_c &other);  /* slot 21 */
    virtual void OnHitByMegaChar(Player &player);    /* slot 27 */

    /* Slot 31, dBgActor_c's own new virtual (include/dBgActor_c.h). ATTRIBUTED BY
       THE VTABLE: _ZTV8IceSheet (ov018 0x02113b34) carries 0x02112880 at
       +31*4 = 0x02113bb0, and _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv at the
       same slot, so this is this class's own override. An override adds no slot
       and no field; the size assert below is unaffected. */
    virtual void Kill();                /* slot 31 */
};

typedef char IceSheet_size_must_be_0x320[sizeof(IceSheet) == 0x320 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct IceSheet {
    u8  pad_000[0x8e];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x44];
    /* Model member, named by _ZN5ModelD1Ev at +0xd4 -- a relocation the ROM build checks.
       D1 and not D2, so it is this type and not an inlined base. Was a u8 marker. */
    Model mModel;            /* 0x0d4 */
    /* dBgW_KcMbg member. The cartridge's own ~IceSheet calls _ZN10dBgW_KcMbgD1Ev at
       +0x124 (D0/D1), a relocation the ROM build checks; recovered by
       tools/dtor_members.py. D1 and not D2, so it is this type and not an inlined base. */
    dBgW_KcMbg mMovingMeshCollider;            /* 0x124 */
};

#endif /* __cplusplus */

#endif /* ICESHEET_H */
