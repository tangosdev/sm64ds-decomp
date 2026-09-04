#ifndef DADKK_C_H
#define DADKK_C_H

#include "types.h"

/* RTTI names the class directly: _ZTS7daDkk_c / _ZTI7daDkk_c
 * (config/arm9/overlays/ov025/symbols.txt 0x021137e0/0x021137ec). The
 * factory the tree calls daDkk_c_classInit (src/d_a_dkk.c) gives the real
 * size: fBase_c::operator new(928) == 0x3a0.
 *
 * BASE IS daDsnBase_c, NOT dBgActor_c DIRECTLY. daDkk_c_classInit's un-migrated
 * form stores _ZTV11daDsnBase_c as a mid-construction vtable before this
 * class's own -- daDsnBase_c.h documents the whole family (it and daDsn_c /
 * Thwomp share this base). daDsnBase_c's own destructor is defined inline in
 * its class body specifically so descendants inline its teardown rather than
 * emitting a `bl` to it; this class follows that rule and defines its own
 * destructor out of line, matching D1 (func_ov025_021118c8) and D0
 * (func_ov025_02111928): own vptr, then daDsnBase_c's (inlined --
 * ShadowModel@0x338 then TextureSequence@0x324), then dBgActor_c's (also
 * inlined, per its own header's convention -- dBgW_KcMbg@0x124 and
 * Model@0xd4), then dActor_c::~dActor_c.
 *
 * daDsnBase_c ends its own data at 0x360; this class's own storage starts
 * there. THE SPAN ONLY REACHES 0x39c -- daDsnBase_c.h notes 0x360..0x3a0
 * exists in both leaves (daDkk_c_classInit allocates 0x3a0, Thwomp_Spawn 0x3a4)
 * but stays declared per-leaf until a later pass shows both reading the same
 * offsets. Here the two fields are:
 *
 *   0x390  a Fix12 height read/written by InitResources' raycast probe and
 *          nothing else -- coined mProbeHeight.
 *   0x398  a small state selector Behavior switches on (cases 0..7) --
 *          coined mState.
 *
 * Slot 29, OnAimedAtWithEgg, is an override this class supplies beyond the
 * two (InitResources, Behavior) daDsnBase_c leaves abstract; the signature
 * matches dActor_c's own declaration (include/dActor_c.h, slot 29).
 *
 * SM64DS RTTI names the implementation daDkk_c. The reconstructed factory
 * daDkk_c_classInit (historical alias Grindel_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_DONKAKU
 * (historical alias Grindel_SpawnInfo) is its registry descriptor.
 */

#ifdef __cplusplus

#include "daDsnBase_c.h"

struct daDkk_c : daDsnBase_c {
    u8  pad_360[0x30];                /* 0x360 -- unclaimed, within daDsnBase_c's
                                          declared-but-unmodelled 0x360..0x3a0 span */
    s32 mProbeHeight;                 /* 0x390 */
    u8  pad_394[0x4];
    s32 mState;                       /* 0x398 */
    u8  pad_39c[0x4];                 /* 0x39c, to the ROM's 0x3a0 */

    /* --- vtable --- */
    /* Out of line on purpose: daDsnBase_c's destructor is inline, so this
       class must emit its own body directly rather than delegate. */
    virtual ~daDkk_c();

    int InitResources();
    int Behavior();
    int OnAimedAtWithEgg();
};

typedef char daDkk_c_size_must_be_0x3a0[sizeof(daDkk_c) == 0x3a0 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DADKK_C_H */
