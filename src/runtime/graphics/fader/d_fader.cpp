//cpp
/* The screen-fade family (arm9 .text 0x020171c8..0x0201787c): abstract dFader_c,
 * dFdBrightness_c driving MASTER_BRIGHT, dFdColor_c driving BLDY, dFdWipe_c
 * covering the screen with a scaled model, and the no-op dFdDummy_c embedded
 * in dScDSMT_c. mwccarm emits .text sections in reverse source order, so the
 * definitions below run ROM-descending.
 *
 * ~dFader_c() defines the whole destructor family here: mwccarm emits the
 * synthesized variants with the definition, so D2, D0 and D1 land at
 * 0x02017838..0x0201787c in cartridge order.
 *
 * For the host port only the non-hardware members compile; the structors, the
 * GX/CP15/MMIO writers and the dExtSimpleModel_c-/filesystem-touching dFdWipe_c members
 * stay NDS-only behind _MSC_VER, exactly the subset port/slice_gate1.txt and
 * port/hal/shims.cpp already arrange.
 *
 * leftovers:
 * - The 0x400xxxx MMIO addresses stay literal; no shared register-name
 *   header owns them.
 */
#include "types.h"
#include "nitro/fx/fx.h"
#include "dFdWipe_c.h"
#include "dFdDummy_c.h"
#include "decl_common.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx.h"
#include "dExtModel_c.h"
#include "math/MtxStack.h"
#include "nitro/gx/gx_load.h"
#include "nitro/fx/fx_cp.h"

// These helpers have native definitions but no shared declaration yet.
#include "nitro/os/cache.h"
int ApproachLinear(Fix12i &value, Fix12i target, Fix12i step);

extern "C" {

void Matrix4x3_ApplyInPlaceToScale(Matrix4x3 *m, Fix12i x, Fix12i y, Fix12i z);
}

#ifndef _MSC_VER
dFader_c::~dFader_c()
{
}

dFdBrightness_c::~dFdBrightness_c()
{
}

/* One frame of the brightness fade. Nothing reaches the hardware unless the
   interpolator actually moved, so a settled fade costs two loads. 0x400006c
   and 0x400106c are MASTER_BRIGHT for the main and sub engines; the level is
   negated and rescaled from 20.12 into the register's 5-bit field. Past full
   black the backdrop palette is reloaded as well. */
void dFdBrightness_c::AdvanceFade()
{
    Fix12i before = currInterp;
    int level;

    AdvanceInterp();
    if (currInterp == before)
        return;

    level = -(currInterp << 4) >> 0xc;
    GX_SetMasterBrightness(level);
    GXS_SetMasterBrightness(level);
    if (level > -0x10)
        return;

    DC_FlushRange(data_0209d3ac, 2);
    GX_LoadBGPltt(data_0209d3ac, 0, 2);
    GXS_LoadBGPltt(data_0209d3ac, 0, 2);
}
#endif

/* Fade out over `frames` frames: the same rate with the sign flipped.
   `extra` is not read. Callers still pass it (always 0). */
int dFdBrightness_c::SetBackwardTime(u32 frames, u32 extra) {
    if (frames == 0) {
        speed = -0x1000;
    } else {
        speed = FX_Div(-0x1000, (frames << 12));
    }
    return IsAtStart();
}

/* Fade in over `frames` frames: speed = 1.0 / frames. 0 means "instant". */
int dFdBrightness_c::SetForwardTime(u32 frames, u32 extra) {
    if (frames == 0) {
        speed = 0x1000;
    } else {
        speed = FX_Div(0x1000, (frames << 12));
    }
    return IsAtEnd();
}

int dFdBrightness_c::IsAtStart()
{
    return currInterp == 0;
}

int dFdBrightness_c::IsAtEnd()
{
    return currInterp == 0x1000;
}

/* True while the fade is in motion: neither snapped to 0.0 nor to 1.0. Both
   calls go through the vtable (slots 5 and 6), which is how the ROM reaches
   them and how the slot assignment in dFader_c.h was established. */
int dFdBrightness_c::IsBetweenStartAndEnd()
{
    if (IsAtStart() == 0 && IsAtEnd() == 0)
        return 1;
    return 0;
}

/* Snaps the fade to its end: 1.0 in 20.12 fixed point. */
void dFdBrightness_c::SetToEnd()
{
    currInterp = 0x1000;
}

/* Snaps the fade to its start: 0.0 in 20.12 fixed point. */
void dFdBrightness_c::SetToStart()
{
    currInterp = 0;
}

/* Step currInterp one frame toward its target. A positive speed fades toward
   1.0, a negative one toward 0.0; the helper takes a nonnegative step. */
void dFader_c::AdvanceInterp()
{
    Fix12i step = speed;
    Fix12i target = step >= 0 ? 0x1000 : 0;
    if (step < 0)
        step = -step;
    ApproachLinear(currInterp, target, step);
}

#ifndef _MSC_VER
dFdColor_c::~dFdColor_c()
{
}

void dFdColor_c::AdvanceFade()
{
    int old = currInterp;
    AdvanceInterp();
    if (currInterp == old) return;
    {
        unsigned short color = this->color;
        int m = color ? 0x10 : -0x10;
        int r = (currInterp * m) >> 12;
        if (r != 0) {
            G2_SetBlendBrightness(0x3f, r);
            G2S_SetBlendBrightness(0x3f, r);
        } else {
            G2_BlendNone();
            G2S_BlendNone();
        }
    }
}

/* The empty C1 is the clearest single statement of the chain's shape:
   dFader_c's vptr, then dFdBrightness_c's plus its two field stores, then
   dFdColor_c's plus its field store, then this class's own, then
   dExtSimpleModel_c::dExtSimpleModel_c() at +0x10 -- all base ctors inline in their headers, which
   is why the whole chain flattens into this one body. */
dFdWipe_c::dFdWipe_c()
{
}

/* ~dFdWipe_c: store this class's vptr, destroy the dExtSimpleModel_c member (out of
   line), run the dFdColor_c sub-object destructor, and -- for D0 -- operator
   delete. */
dFdWipe_c::~dFdWipe_c()
{
}

void dFdWipe_c::AdvanceFade()
{
    Fix12i old = currInterp;
    Fix12i cur;
    AdvanceInterp();
    cur = currInterp;
    if (cur == 0 && cur == old) return;
    if (cur == 0x1000) {
        G2_SetBlendBrightness(0x3f, -0x10);
    } else {
        if (speed != 0) {
            if (old != 0x1000)
                G2_BlendNone();
        }
        if (currInterp != 0) {
            Fix12i scale = FX_MUL(0x1000 - currInterp, 0x20000);
            Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, -0x1000);
            Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, scale, scale, scale);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, 0x4000);
            model.data.Render(&data_020a0e68, 0);
        }
    }
    if (currInterp == old)
        return;
    {
        u16 color = this->color;
        int m = color ? 0x10 : -0x10;
        int prod = currInterp * m;
        int r = prod >> 12;
        if (r != 0) {
            G2S_SetBlendBrightness(0x3f, r);
        } else {
            G2_BlendNone();
            G2S_BlendNone();
        }
    }
}

/* This used to reach the model as `(char*)this + 0x10` with dExtSimpleModel_c declared
   nowhere. Going through the member instead is the same bytes and turns the
   offset into a checkable claim. */
void dFdWipe_c::LoadAndSetFile(u16 ov0ID)
{
    model.LoadAndSetFile(ov0ID, 0, -1);
}

/* Host build has no dFdDummy_c structor coverage: port/hal/shims.cpp stands
   in for the fader-family destructors it exercises. */
dFdDummy_c::dFdDummy_c()
{
}

dFdDummy_c::~dFdDummy_c()
{
}
#endif

/* Vtable slot 2. The whole body is a tail call into the base chain's own
   AdvanceInterp -- `this' rides through in r0 untouched, which is exactly
   what the inherited implementation wants. */
void dFdDummy_c::AdvanceFade()
{
    AdvanceInterp();
}

/* dFdDummy_c::SetBackwardTime -- writes speed = -0x1000, then a genuine
   virtual call through slot 5 (same inherited bodies as below). */
int dFdDummy_c::SetBackwardTime(u32, u32) {
    speed = -0x1000;
    return IsAtStart();
}

/* dFdDummy_c::SetForwardTime -- writes speed = 0x1000, then makes a genuine
   virtual call through slot 6 (dFdDummy_c overrides none of slots 5-9, so
   this lands on dFdColor_c/dFdBrightness_c's own inherited body). */
int dFdDummy_c::SetForwardTime(u32, u32) {
    speed = 0x1000;
    return IsAtEnd();
}
