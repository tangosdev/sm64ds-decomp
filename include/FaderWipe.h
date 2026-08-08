#ifndef FADERWIPE_H
#define FADERWIPE_H

#include "FaderColor.h"
#include "Model.h"

/* Wipe transition: a FaderColor that also draws a 3D model scaled by the fade
 * level, so the screen is covered by geometry rather than by a blend register.
 *
 * This header used to be a flat generated struct with no base class, and its
 * own comment recorded the confusion honestly: `mFadeAmount` at 0x004 was
 * annotated "Fader::currInterp", and 0x008 "Fader::speed", with a note that
 * "FaderWipe is one of 7 retyped classes the hierarchy pass cannot place, so
 * 'no ancestor declares this offset' was never checkable for it." It is
 * checkable now, and the answer is that everything up to 0x10 belongs to
 * FaderColor and its bases.
 *
 * DERIVATION. _ZN9FaderWipeC1Ev (0x02017480) writes four vtables into
 * [this+0x0] in chain order -- data_0208eafc, data_0208eacc,
 * data_0208eb2c, _ZTV9FaderWipe -- one per sub-object constructor. The ROM's
 * __si_class_type_info records say the same: dFdWipe_c's single base is
 * dFdColor_c.
 *
 * THE MODEL AT 0x10 is read out of that same constructor. Its last act before
 * returning is `add r0, r4, #0x10` followed by `bl 0x02016d58`, and 0x02016d58
 * is _ZN5ModelC1Ev. include/Model.h asserts sizeof(Model) == 0x50, so the whole
 * object is 0x10 + 0x50 = 0x60. FaderWipe::LoadAndSetFile confirms the offset
 * independently: it forwards to Model::LoadAndSetFile on `this + 0x10`.
 *
 * VTABLE. _ZTV9FaderWipe (0x0208ea9c) is ten slots and overrides exactly one,
 * slot 2 -- AdvanceFade. Everything else is still FaderBrightness's.
 * LoadAndSetFile appears in no slot and is non-virtual.
 */
#ifdef __cplusplus
struct FaderWipe : FaderColor {
    Model model;                    /* 0x10 -- constructed last by FaderWipe::FaderWipe */

    virtual ~FaderWipe();           /* key function; only defined in D0Ev.c / D1Ev.c */
    virtual void AdvanceFade();     /* slot 2 -- the only override */

    void LoadAndSetFile(u16 ov0ID);
};

typedef char FaderWipe_size_must_be_0x60[sizeof(FaderWipe) == 0x60 ? 1 : -1];
#else
struct FaderWipe {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 (from Fader) */
    Fix12i speed;       /* 0x08 (from Fader) */
    u16    unk_00c;     /* 0x0c (from FaderColor) */
    u8     pad_00e[0x2];
    u8     model[0x50]; /* 0x10 */
};
#endif

#endif /* FADERWIPE_H */
