#ifndef THWOMP_H
#define THWOMP_H

#include "types.h"

/* The Thwomp. ROM name daDsn_c, from dossun.
 *
 * IT DOES NOT DERIVE FROM dBgActor_c. It derives from daDsnBase_c, which derives
 * from dBgActor_c, and the difference is in the bytes rather than only in the RTTI:
 * its destructor stores THREE vptrs -- its own, daDsnBase_c's, then dBgActor_c's --
 * and destroys daDsnBase_c's ShadowModel at 0x338 and TextureSequence at 0x324
 * between the second and the third. A one-level chain emits two vptr stores and
 * neither member call.
 *
 *   _ZTI7daDsn_c  ov091 0x0213512c
 *   _ZTS7daDsn_c  ov091 0x02135120
 *   _ZTV6Thwomp   ov091 0x02135174  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  daDsnBase_c, ov025 0x021137f8 -- whose vtable is in ov091
 *
 * TWO OF THE METHODS THIS HEADER USED TO DECLARE WERE NOT THIS CLASS'S, and they
 * have been moved. CleanupResources and Render are slots 3 and 9, and the
 * cartridge puts those words in _ZTV11daDsnBase_c; this class does not override
 * either, it inherits both. The ROM spelled the symbols `_ZN6Thwomp*`, which is
 * the only reason they were parked here -- and leaving them here was not free:
 * daDsnBase_c declared neither, so both its vtable and daDkk_c's inherited
 * fBase_c's bodies in those slots and disagreed with the ROM. The config change
 * that note called "its own piece of work" is done; the two are now
 * `_ZN11daDsnBase_c16CleanupResourcesEv` and `_ZN11daDsnBase_c6RenderEv`, and
 * both still reproduce at the same addresses. InitResources and Behavior, slots
 * 0 and 6, really are this class's: the base declares both `= 0`.
 *
 * SIZE 0x3a4, from Thwomp_Spawn's literal 932.
 *
 * FOUR FIELDS OF ITS OWN, all of them read by this class's own two slots:
 *
 *   0x398  the state, 0..4. Behavior is the switch on it; InitResources zeroes it.
 *   0x39e  a 0x5a-frame timer, set when the Thwomp is triggered mid-cycle.
 *   0x3a0  a countdown Behavior decrements while the model's own 0x3a0 counter is
 *          still running.
 *   0x3a2  the trigger flag. Behavior clears it, InitResources zeroes it.
 *
 * WHAT THIS HEADER USED TO SAY, AND WHY MOST OF IT WAS THE BASE'S. The generated
 * body had nine fields over `u8 pad_000[0xd4]`. `Model mModel` at 0xd4 and a
 * `u8 mMovingMeshCollider` marker at 0x124 are dBgActor_c's, twice removed --
 * dBgActor_c calls the second one mMeshCollider. `s32 mFileTable` at 0x320,
 * `u8 mTextureSequence` at 0x324 and `u8 mShadowModel` at 0x338 are
 * daDsnBase_c's, and the last two are whole sub-objects rather than marker bytes:
 * both factories construct them and both destructors destroy them.
 *
 * AND `s32 unk_32c` WAS INSIDE ONE OF THEM. 0x32c is 0x324 + 8, which is
 * Animation::currFrame reached through the TextureSequence -- Behavior treats it
 * as a 20.12 countdown, shifting right by 12 to read it and left by 12 to write
 * it, in the same function that calls Animation::Advance on the same sub-object.
 * It is spelled through the member now, so nothing describes those four bytes
 * twice.
 *
 * SM64DS proves this class as daDsn_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daDsn_c_classInit at 0x02132cb8 (historical alias Thwomp_Spawn) allocates
 * 0x3a4 and installs this class's cartridge vtable. It backs the DOSUN
 * registry profile, whose descriptor at 0x02135150 is reconstructed as
 * g_profile_DOSUN.
 */

#ifdef __cplusplus

#include "daDsnBase_c.h"

struct Thwomp : daDsnBase_c {
    /* UNOBSERVED, not inherited. daDsnBase_c's own methods stop at 0x360 and
       nothing in the tree reads 0x360..0x398 on either leaf. daDkk_c allocates
       0x3a0, so the span exists there too -- see include/daDsnBase_c.h. */
    u8  pad_360[0x38];
    /* Field NAMES are placeholders. Offsets, widths and types are observed. */
    s32 mState;             /* 0x398 */
    u8  pad_39c[0x2];
    u8  mRetrigger;         /* 0x39e */
    u8  pad_39f[0x1];
    u16 mHoldTimer;         /* 0x3a0 */
    u8  mTriggered;         /* 0x3a2 */

    /* --- vtable --- */
    virtual ~Thwomp();                 /* slots 16 (D1), 17 (D0) */

    /* The two the base declares `= 0`; this class supplies both. */
    int Behavior();                    /* slot  6 */
    int InitResources();               /* slot  0 */

    /* Slots 3 and 9 are daDsnBase_c's and are declared there -- see the note
       above. This class inherits both words. */

    virtual void OnHitByMegaChar(Player &player); /* slot 27 */
    virtual int  OnAimedAtWithEgg();              /* slot 29 */
};

typedef char Thwomp_size_must_be_0x3a4[sizeof(Thwomp) == 0x3a4 ? 1 : -1];

#endif /* __cplusplus */

#endif /* THWOMP_H */
