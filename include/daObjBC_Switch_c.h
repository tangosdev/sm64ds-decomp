#ifndef DAOBJBC_SWITCH_C_H
#define DAOBJBC_SWITCH_C_H

#include "types.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * dBgActor_c.h FIRST so common.h beats Model.h on Matrix4x3 (the two guarded
 * spellings over the same 0x30 bytes). This class's TU does not copy a
 * Matrix4x3, but the header used to pull Model.h ahead of the base and that
 * is the order UpdateClsnPosAndRot refuses.
 *
 * The flat-C `#else` spelling this header used to carry is gone: it existed
 * for the three .c members (D0's neighbours func_ov002_020f1578/020f15b8 and
 * the classInit factory), and the promotion folded all of them into the one
 * C++ translation unit, so nothing includes this header from C any more.
 */

#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daObjBC_Switch_c : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mStopPosY;      /* 0x320 */
    s32 mTickSound;     /* 0x324 */
    u16 mCoinTimer;     /* 0x328 */
    u16 mCoinTimerSeed; /* 0x32a */
    u8  mPressed;       /* 0x32c */
    u8  mEventBit;      /* 0x32d */
    u8  mHomeAreaId;    /* 0x32e */

    /* --- vtable --- */

    /* MEASURED -- INLINE ON PURPOSE, do not move this body out of line.
     * Out of line, mwcc emits D0 before D1 plus a homeless D2; the ROM has
     * D1 at ov002 0x020f11b0 then D0 at 0x020f11f4 and no D2. Production
     * isolation lays .text into the spanning delink in emission order, so
     * the out-of-line spelling fails tubuild linkcheck even though the
     * bytes match 9/9 and objisolate calls the object clean. Inline, the
     * emission is D1, D0, no D2 -- the ROM's order.
     *
     * Safe to do here only because this class is a leaf: no _ZTI in the ROM
     * names it as a base, so no other TU's codegen moves with it.
     * src/game/actors/d_a_obj_bc_switch.cpp carries the leaf proof. */
    virtual ~daObjBC_Switch_c() {}

    s32 Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Leaf operator new until #2570 puts the same allocator on fBase_c.
       Parameter is size_t (unsigned long on this compiler). `return new`
       relocates to `_Znwm` without this. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjBC_Switch_c_size_must_be_0x330[sizeof(daObjBC_Switch_c) == 0x330 ? 1 : -1];

#endif /* DAOBJBC_SWITCH_C_H */
