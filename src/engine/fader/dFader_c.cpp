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
 * GX/CP15/MMIO writers and the Model-/filesystem-touching dFdWipe_c members
 * stay NDS-only behind _MSC_VER, exactly the subset port/slice_gate1.txt and
 * port/hal/shims.cpp already arrange.
 */
#include "types.h"
#include "dFdWipe_c.h"
#include "dFdDummy_c.h"
#include "decl_common.h"

extern "C" {
Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);
void _Z14ApproachLinearRiii(Fix12i *value, Fix12i target, Fix12i step);
void func_02053a90(u16 *reg, int value);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 len);
void _ZN2GX10LoadBGPlttEPKvjj(const void *src, u32 offset, u32 len);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *src, u32 offset, u32 len);
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16 *p, u16 a, int b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToScale(void *m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
void _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(void *thiz, void *mtx, void *vec);
}
struct Matrix4x3;
extern Matrix4x3 data_020a0e68;

#ifndef _MSC_VER
// @symbol _ZN8dFader_cD0Ev
// @symbol _ZN8dFader_cD1Ev
// @symbol _ZN8dFader_cD2Ev
dFader_c::~dFader_c()
{
}

// @symbol _ZN15dFdBrightness_cD0Ev
// @symbol _ZN15dFdBrightness_cD1Ev
// @symbol _ZN15dFdBrightness_cD2Ev
dFdBrightness_c::~dFdBrightness_c()
{
}

/* One frame of the brightness fade. Nothing reaches the hardware unless the
   interpolator actually moved, so a settled fade costs two loads. 0x400006c
   and 0x400106c are MASTER_BRIGHT for the main and sub engines; the level is
   negated and rescaled from 20.12 into the register's 5-bit field. Past full
   black the backdrop palette is reloaded as well. */
// @symbol _ZN15dFdBrightness_c11AdvanceFadeEv
void dFdBrightness_c::AdvanceFade()
{
    Fix12i before = currInterp;
    int level;

    AdvanceInterp();
    if (currInterp == before)
        return;

    level = -(currInterp << 4) >> 0xc;
    func_02053a90((u16*)0x400006c, level);
    func_02053a90((u16*)0x400106c, level);
    if (level > -0x10)
        return;

    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_0209d3ac, 2);
    _ZN2GX10LoadBGPlttEPKvjj(data_0209d3ac, 0, 2);
    _ZN3GXS10LoadBGPlttEPKvjj(data_0209d3ac, 0, 2);
}
#endif

/* Fade out over `frames` frames: the same rate with the sign flipped. */
// @symbol _ZN15dFdBrightness_c15SetBackwardTimeEj
int dFdBrightness_c::SetBackwardTime(u32 frames)
{
    if (frames == 0) {
        speed = -(Fix12i)0x1000;
    } else {
        speed = _ZN4cstd4fdivEii(-(Fix12i)0x1000, (Fix12i)(frames << 12));
    }
    return IsAtStart();
}

/* Fade in over `frames` frames: speed = 1.0 / frames. 0 means "instant". */
// @symbol _ZN15dFdBrightness_c14SetForwardTimeEj
int dFdBrightness_c::SetForwardTime(u32 frames)
{
    if (frames == 0) {
        speed = (Fix12i)0x1000;
    } else {
        speed = _ZN4cstd4fdivEii(0x1000, (Fix12i)(frames << 12));
    }
    return IsAtEnd();
}

// @symbol _ZN15dFdBrightness_c9IsAtStartEv
int dFdBrightness_c::IsAtStart()
{
    return currInterp == 0;
}

// @symbol _ZN15dFdBrightness_c7IsAtEndEv
int dFdBrightness_c::IsAtEnd()
{
    return currInterp == 0x1000;
}

/* True while the fade is in motion: neither snapped to 0.0 nor to 1.0. Both
   calls go through the vtable (slots 5 and 6), which is how the ROM reaches
   them and how the slot assignment in dFader_c.h was established. */
// @symbol _ZN15dFdBrightness_c20IsBetweenStartAndEndEv
int dFdBrightness_c::IsBetweenStartAndEnd()
{
    if (IsAtStart() == 0 && IsAtEnd() == 0)
        return 1;
    return 0;
}

/* Snaps the fade to its end: 1.0 in 20.12 fixed point. */
// @symbol _ZN15dFdBrightness_c8SetToEndEv
void dFdBrightness_c::SetToEnd()
{
    currInterp = 0x1000;
}

/* Snaps the fade to its start: 0.0 in 20.12 fixed point. */
// @symbol _ZN15dFdBrightness_c10SetToStartEv
void dFdBrightness_c::SetToStart()
{
    currInterp = 0;
}

/* Step currInterp one frame toward its target. A positive speed fades toward
   1.0, a negative one toward 0.0; the helper takes an unsigned step. */
// @symbol _ZN8dFader_c13AdvanceInterpEv
void dFader_c::AdvanceInterp()
{
    Fix12i step = speed;
    Fix12i target = step >= 0 ? 0x1000 : 0;
    if (step < 0)
        step = -step;
    _Z14ApproachLinearRiii(&currInterp, target, step);
}

#ifndef _MSC_VER
// @symbol _ZN10dFdColor_cD0Ev
// @symbol _ZN10dFdColor_cD1Ev
// @symbol _ZN10dFdColor_cD2Ev
dFdColor_c::~dFdColor_c()
{
}

// @symbol _ZN10dFdColor_c11AdvanceFadeEv
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
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4000050, 0x3f, r);
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4001050, 0x3f, r);
        } else {
            *(unsigned short*)0x4000050 = 0;
            *(unsigned short*)0x4001050 = 0;
        }
    }
}

/* The empty C1 is the clearest single statement of the chain's shape:
   dFader_c's vptr, then dFdBrightness_c's plus its two field stores, then
   dFdColor_c's plus its field store, then this class's own, then
   Model::Model() at +0x10 -- all base ctors inline in their headers, which
   is why the whole chain flattens into this one body. */
// @symbol _ZN9dFdWipe_cC1Ev
dFdWipe_c::dFdWipe_c()
{
}

/* ~dFdWipe_c: store this class's vptr, destroy the Model member (out of
   line), run the dFdColor_c sub-object destructor, and -- for D0 -- dFader_c's
   class operator delete inlined to Memory::operator_delete2. */
// @symbol _ZN9dFdWipe_cD0Ev
// @symbol _ZN9dFdWipe_cD1Ev
dFdWipe_c::~dFdWipe_c()
{
}

// @symbol _ZN9dFdWipe_c11AdvanceFadeEv
void dFdWipe_c::AdvanceFade()
{
    Fix12i old = *(Fix12i*)((char*)&currInterp);
    Fix12i cur;
    AdvanceInterp();
    cur = *(Fix12i*)((char*)&currInterp);
    if (cur == 0 && cur == old) return;
    if (cur == 0x1000) {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16*)0x4000050, 0x3f, -0x10);
    } else {
        if (*(Fix12i*)((char*)&speed) != 0) {
            if (old != 0x1000) *(u16*)0x4000050 = 0;
        }
        if (*(Fix12i*)((char*)&currInterp) != 0) {
            Fix12i scale = (Fix12i)(((long long)(0x1000 - *(Fix12i*)((char*)&currInterp)) * 0x20000 + 0x800) >> 12);
            Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, -0x1000);
            Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, scale, scale, scale);
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, 0x4000);
            _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(((char*)this) + 0x18, &data_020a0e68, 0);
        }
    }
    if (*(Fix12i*)((char*)&currInterp) == old) return;
    {
        u16 color = this->color;
        int m = color ? 0x10 : -0x10;
        int prod = *(Fix12i*)((char*)&currInterp) * m;
        int r = prod >> 12;
        if (r != 0) {
            _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16*)0x4001050, 0x3f, r);
        } else {
            *(u16*)0x4000050 = 0;
            *(u16*)0x4001050 = 0;
        }
    }
}

/* This used to reach the model as `(char*)this + 0x10` with Model declared
   nowhere. Going through the member instead is the same bytes and turns the
   offset into a checkable claim. */
// @symbol _ZN9dFdWipe_c14LoadAndSetFileEt
void dFdWipe_c::LoadAndSetFile(u16 ov0ID)
{
    model.LoadAndSetFile(ov0ID, 0, -1);
}

/* Host build has no dFdDummy_c structor coverage: port/hal/shims.cpp stands
   in for the fader-family destructors it exercises. */
// @symbol _ZN10dFdDummy_cC1Ev
dFdDummy_c::dFdDummy_c()
{
}

// @symbol _ZN10dFdDummy_cD0Ev
// @symbol _ZN10dFdDummy_cD1Ev
dFdDummy_c::~dFdDummy_c()
{
}
#endif

/* Vtable slot 2. The whole body is a tail call into the base chain's own
   AdvanceInterp -- `this' rides through in r0 untouched, which is exactly
   what the inherited implementation wants. */
// @symbol _ZN10dFdDummy_c11AdvanceFadeEv
void dFdDummy_c::AdvanceFade()
{
    AdvanceInterp();
}

/* dFdDummy_c::SetBackwardTime -- writes speed = -0x1000, then a genuine
   virtual call through slot 5 (same inherited bodies as below). */
// @symbol _ZN10dFdDummy_c15SetBackwardTimeEj
int dFdDummy_c::SetBackwardTime(u32)
{
    speed = -0x1000;
    return IsAtStart();
}

/* dFdDummy_c::SetForwardTime -- writes speed = 0x1000, then makes a genuine
   virtual call through slot 6 (dFdDummy_c overrides none of slots 5-9, so
   this lands on dFdColor_c/dFdBrightness_c's own inherited body). */
// @symbol _ZN10dFdDummy_c14SetForwardTimeEj
int dFdDummy_c::SetForwardTime(u32)
{
    speed = 0x1000;
    return IsAtEnd();
}
