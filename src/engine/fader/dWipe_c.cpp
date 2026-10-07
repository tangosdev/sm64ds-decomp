//cpp
/* The hardware screen wipe (arm9 .text 0x0202ec9c..0x0202fc98): dWipe_c, a
 * dFdColor_c child that sweeps the screen with an IRQ-driven per-scanline
 * blend instead of a plain brightness ramp. A single static instance lives
 * in BSS at data_0209f61c; scene state changes go through func_0202ec9c and
 * the two hardware helpers drive a pair of HBlank-time capture tables at
 * data_0209f648. mwccarm emits .text in reverse source order, so the
 * definitions below run ROM-descending.
 *
 * ~dWipe_c() is the key function: mwcc emits D2, D0 and D1 with the
 * definition, the cartridge keeps D0 (0x0202fbc8) and D1 (0x0202fc08), and
 * the vtable/RTTI records land in .data at 0x020926d0..0x02092718.
 */
#include "types.h"
#include "dWipe_c.h"
#include "IRQ.h"
#include "decl_common.h"

extern "C" {
/* The ROM's SetBackwardTime entry point reads a third argument in r2 and
   forwards it untouched; the header's 2-param member call would drop the
   forwarded register.
   local extern: byte-required arity, banked in decl-agreement-baseline.json */
int  _ZN15dFdBrightness_c15SetBackwardTimeEj(dFdBrightness_c *self, u32 time, u32 extra);
/* local extern: the ROM passes the blend target in r2 with no sign-extend;
   the header's s16 parameter emits sxth at this call site. */
void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16 *p, u16 a, int b);
void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 len);
void _ZN2GX10LoadBGPlttEPKvjj(const void *src, u32 offset, u32 len);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *src, u32 offset, u32 len);
void MultiStore_Int(int val, int *dst, int len);
void func_0202f2c4(void);
}

extern dWipe_c      data_0209f61c;   /* the wipe singleton */
extern u8           data_0209f5f8;   /* which display engine the wipe covers */
extern volatile u8  data_0209f5fc;   /* the next HBlank has a fresh line */
extern volatile u32 data_0209f608;   /* table index the IRQ reads */
extern volatile u32 data_0209f60c;   /* table index the CPU fills */
extern u8           data_0209f648[]; /* two 0x300-byte scanline tables */
extern short        data_023c0000[];

// @symbol _ZN7dWipe_cC1Ev
dWipe_c::dWipe_c() : dFdColor_c(0)
{
    func_0202ed14();
}

/* The destructor's one real obligation: if the per-scanline capture is still
   armed, cancel it before the object goes away. */
// @symbol _ZN7dWipe_cD1Ev
dWipe_c::~dWipe_c()
{
    if (needsCleanup == 1)
        func_0202fb30();
}

/* Disarm the capture: mask IME, drop the HBlank IRQ handler, clear both
   engines' blend windows, and if a type change was stashed mid-transition,
   commit it now. */
// @symbol _ZN7dWipe_c13func_0202fb30Ev
void dWipe_c::func_0202fb30()
{
    u16 ime = *(volatile u16 *)0x4000208;
    *(volatile u16 *)0x4000208 = 0;
    IRQ::DisableIRQs(2);
    func_02053c10(0);
    IRQ::SetIRQHandler(2, 0);
    if (ime != 0) {
        u16 dead = *(volatile u16 *)0x4000208;
        *(volatile u16 *)0x4000208 = 1;
        (void)dead;
    }
    *(volatile u32 *)0x4000000 &= ~0xe000;
    *(volatile u32 *)0x4001000 &= ~0xe000;
    if (unk_028 == 1) {
        type = unk_018;
        unk_028 = 0;
    }
}

/* Vtable slot 3, spelled as the ROM built it: a two-argument entry point under
   the member's mangled name. The extra parameter arrives in r2 and is
   forwarded to the base call with zero instructions, which is what keeps the
   cached `type` in r3 as in the cartridge; a real one-argument member fixes
   the arity and misses by three words (measured, notes/scene-provenance.md).
   The header still declares slot 3 so the emitted vtable binds here. */
// @symbol _ZN7dWipe_c15SetBackwardTimeEj
extern "C" int _ZN7dWipe_c15SetBackwardTimeEj(dWipe_c *self, u32 frames, u32 extra)
{
    s32 type = self->type;
    s32 state;

    if (type == 1)
        return _ZN15dFdBrightness_c15SetBackwardTimeEj(self, frames, extra);

    state = self->state;
    if (state == 0 || state == 4) {
        if (frames == 0) {
            self->wipeSpeed = 0x200000;
            self->wipeAccel = 0;
        } else {
            type = (type == 0) ? 0x2d : 0x3c;
            self->wipeSpeed = 0x200000 / type;
            self->wipeAccel = (self->wipeSpeed << 1) / type;
            self->wipeSpeed = 0;
        }

        if (self->type == 2) {
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_0209f604, 2);
            _ZN2GX10LoadBGPlttEPKvjj(data_0209f604, 0, 2);
            _ZN3GXS10LoadBGPlttEPKvjj(data_0209f604, 0, 2);
        } else {
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_020926cc, 2);
            _ZN2GX10LoadBGPlttEPKvjj(data_020926cc, 0, 2);
            _ZN3GXS10LoadBGPlttEPKvjj(data_020926cc, 0, 2);
        }

        self->wipeInterp = 0;

        *(volatile u16 *)0x4000040 = 0x7f7f;
        *(volatile u16 *)0x4000044 = 0xc0;
        *(volatile u16 *)0x4001040 = 0x7f7f;
        *(volatile u16 *)0x4001044 = 0xc0;
        *(volatile u16 *)0x4000042 = 0x8080;
        *(volatile u16 *)0x4000046 = 0xc0;
        *(volatile u16 *)0x4001042 = 0x8080;
        *(volatile u16 *)0x4001046 = 0xc0;

        self->state = 1;
        self->func_0202f58c();

        {
            u16 ime = *(volatile u16 *)0x4000208;
            *(volatile u16 *)0x4000208 = 0;
            IRQ::SetIRQHandler(2, func_0202f2c4);
            IRQ::EnableIRQs(2);
            func_02053c10(1);
            if (ime != 0) {
                u16 dead = *(volatile u16 *)0x4000208;
                *(volatile u16 *)0x4000208 = 1;
                (void)dead;
            }
        }

        self->needsCleanup = 1;
        return 0;
    }

    if (state == 2)
        return 1;
    self->state = 1;
    return 0;
}

/* Vtable slot 4 -- starts the closing half of a wipe. Only meaningful from a
   resting state (0 or 2); type 1 is the plain colour fade and the base owns
   it. Loads the wipe's palette on both engines, arms the blend registers,
   then hands the per-scanline work to the HBlank IRQ with IME briefly masked
   around the install. */
// @symbol _ZN7dWipe_c14SetForwardTimeEj
int dWipe_c::SetForwardTime(u32 frames)
{
    if (state == 0 || state == 2) {
        int t;
        u16 saved;

        if (type == 1)
            return dFdBrightness_c::SetForwardTime(frames);

        if (frames == 0) {
            wipeSpeed = -0x200000;
            wipeAccel = 0;
        } else {
            t = (type == 0) ? 0x2d : 0x3c;
            wipeSpeed = -0x200000 / t;
            wipeAccel = (-wipeSpeed << 1) / t;
            wipeSpeed = wipeSpeed << 1;
        }

        if (type == 2) {
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_0209f600, 2);
            _ZN2GX10LoadBGPlttEPKvjj(data_0209f600, 0, 2);
            _ZN3GXS10LoadBGPlttEPKvjj(data_0209f600, 0, 2);
        } else {
            _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_020926c8, 2);
            _ZN2GX10LoadBGPlttEPKvjj(data_020926c8, 0, 2);
            _ZN3GXS10LoadBGPlttEPKvjj(data_020926c8, 0, 2);
        }

        wipeInterp = 0x200000;
        *(volatile u16 *)0x4000040 = 0x7f;
        *(volatile u16 *)0x4000044 = 0xc0;
        *(volatile u16 *)0x4001040 = 0x7f;
        *(volatile u16 *)0x4001044 = 0xc0;
        *(volatile u16 *)0x4000042 = 0x80ff;
        *(volatile u16 *)0x4000046 = 0xc0;
        *(volatile u16 *)0x4001042 = 0x80ff;
        *(volatile u16 *)0x4001046 = 0xc0;

        state = 3;
        func_0202f58c();

        saved = *(volatile u16 *)0x4000208;
        *(volatile u16 *)0x4000208 = 0;
        IRQ::SetIRQHandler(2, func_0202f2c4);
        IRQ::EnableIRQs(2);
        func_02053c10(1);
        if (saved != 0) {
            u16 dead = *(volatile u16 *)0x4000208;
            *(volatile u16 *)0x4000208 = 1;
            (void)dead;
        }

        needsCleanup = 1;
        return 0;
    }

    if (state == 4)
        return 1;
    state = 3;
    return 0;
}

/* Arm the blend path: BLDCNT windows on both engines, capture enabled in
   DISPCNT, then fill both table slots back to back so the next flip lands on
   a finished image. */
// @symbol _ZN7dWipe_c13func_0202f58cEv
void dWipe_c::func_0202f58c()
{
    unk_00e = 1;

    { u32 v = *(volatile u16 *)0x4000048; v = (v & ~0x3f) | 0x1f; v = v | 0x20; *(volatile u16 *)0x4000048 = v; }
    { u32 v = *(volatile u16 *)0x4001048; v = (v & ~0x3f) | 0x1f; v = v | 0x20; *(volatile u16 *)0x4001048 = v; }
    { u32 v = *(volatile u16 *)0x4000048; v = (v & ~0x3f00) | 0x1f00; v = v | 0x2000; *(volatile u16 *)0x4000048 = v; }
    { u32 v = *(volatile u16 *)0x4001048; v = (v & ~0x3f00) | 0x1f00; v = v | 0x2000; *(volatile u16 *)0x4001048 = v; }

    *(volatile u16 *)0x400004a &= ~0x3f;
    *(volatile u16 *)0x400104a &= ~0x3f;

    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0xe000) | 0x6000;

    if (data_0209f5f8 != 0) {
        *(volatile u32 *)0x4001000 &= ~0xe000;
    } else {
        *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0xe000) | 0x6000;
    }

    data_0209f608 = 0;
    data_0209f60c = 0;
    func_0202f290();

    data_0209f608 = 1;
    data_0209f60c = 1;
    data_0209f5fc = 0;
    func_0202f290();

    if ((*(volatile u16 *)0x4000050 & 0x80) == 0x80) *(volatile u16 *)0x4000050 = 0;
    if ((*(volatile u16 *)0x4001050 & 0x80) == 0x80) *(volatile u16 *)0x4001050 = 0;
}

/* Vtable slot 2. Type 1 is the plain colour fade and the base owns it.
   Otherwise this steps the wipe's own 20.12 ramp: state 1 runs it up to the
   halfway mark and hands over to state 2, state 3 runs it back down to zero
   and pins the blend brightness on both engines before settling in state 4.
   States 0/2/4 are resting and return early. */
// @symbol _ZN7dWipe_c11AdvanceFadeEv
void dWipe_c::AdvanceFade()
{
    if (type == 1) {
        dFdColor_c::AdvanceFade();
        return;
    }
    switch (state) {
    case 0:
        return;
    case 1:
        wipeInterp += wipeSpeed;
        wipeSpeed += wipeAccel;
        if (wipeInterp >= 0x200000) {
            wipeInterp = 0x200000;
            needsCleanup = 0;
            func_0202fb30();
            state = 2;
        }
        break;
    case 2:
        return;
    case 3:
        wipeInterp += wipeSpeed;
        wipeSpeed += wipeAccel;
        if (wipeInterp <= 0) {
            int b;
            wipeInterp = 0;
            needsCleanup = 0;
            b = (type == 0) ? 0x10 : -0x10;
            _ZN3G2x18SetBlendBrightnessEPVtts((u16 *)0x4000050, 0x3f, b);
            _ZN3G2x18SetBlendBrightnessEPVtts((u16 *)0x4001050, 0x3f, b);
            func_0202fb30();
            state = 4;
        }
        break;
    case 4:
    default:
        return;
    }
    func_0202f290();
}

/* Latch the staged table into the IRQ-visible index and seed both engines'
   first line from it. */
// @symbol func_0202f3a4
extern "C" void func_0202f3a4(void)
{
    data_0209f608 = data_0209f60c;
    MultiCopy_Int((int *)(data_0209f648 + data_0209f608 * 0x300u), (int *)0x4000040, 4);
    MultiCopy_Int((int *)(data_0209f648 + data_0209f608 * 0x300u), (int *)0x4001040, 4);
    data_0209f5fc = 1;
}

/* The HBlank handler, installed for IRQ bit 2 while a wipe runs. Past the
   last visible line it only refreshes the table index; on a visible line it
   copies that scanline's 4-byte entry into both engines' blend windows. */
// @symbol func_0202f2c4
extern "C" void func_0202f2c4(void)
{
    int line;

    *(int *)((char *)data_023c0000 + 0x3ff8) |= 2;

    line = *(volatile u16 *)0x4000006 + 1;

    if (line >= 0xc0) {
        if (data_0209f5fc != 1)
            func_0202f3a4();
        return;
    }

    if ((*(volatile u16 *)0x4000004 & 2) == 0)
        return;

    MultiCopy_Int((int *)(data_0209f648 + data_0209f608 * 0x300u + line * 4), (int *)0x4000040, 4);
    MultiCopy_Int((int *)(data_0209f648 + data_0209f608 * 0x300u + line * 4), (int *)0x4001040, 4);

    data_0209f5fc = 0;
}

// @symbol _ZN7dWipe_c13func_0202f290Ev
void dWipe_c::func_0202f290()
{
    if (type == 2)
        func_0202ee94();
    else
        func_0202efa0();
}

/* Build a 192-entry (one per scanline) 4-byte blend gradient into whichever
   0x300 slot the IRQ is not reading. After the neutral 0x80807F7F fill, four
   fixed-point ramp segments write clamped 0..0x7F coefficients and their
   0x100-complements, stepped by 2^32/wipeInterp; then flush the pair. */
/* opt_strength_reduction off for the rest of the file -- the two table
   builders keep their per-line index arithmetic un-factored, and a later
   `reset` undoes the option for functions defined BEFORE it, so it cannot
   be restored between them (measured). The remaining functions emit
   identically either way. */
#pragma opt_strength_reduction off
// @symbol _ZN7dWipe_c13func_0202efa0Ev
void dWipe_c::func_0202efa0()
{
    volatile int fill;
    int idx;
    int i;
    int pos;
    int step;
    u8 *buf;
    int dur;
    int v, w;
    int a, b;
    u8 *p;
    s64 d64;

    idx = (data_0209f608 == 0) ? 1 : 0;
    buf = data_0209f648 + idx * 0x300;
    fill = 0x80807F7F;
    i = 0;
    MultiStore_Int(fill, (int *)buf, 0x300);

    dur = wipeInterp;
    if (dur > 0) {
        pos = (int)(-0x6000000000LL / dur) + 0x100000;
        d64 = dur;
        step = (int)(0x100000000LL / d64);
        while (pos < 0) {
            if (i >= 0xC0) break;
            pos += step;
            i++;
        }

        v = (int)(d64 * ((s64)pos * 0x532DLL) / 0x10000000000LL);
        while (pos < 0xB0E00) {
            if (i >= 0xC0) break;
            v += 0x53;
            a = (0x8000 - v) / 256;
            if (a < 0) a = 0;
            if (a > 0x7F) a = 0x7F;
            p = buf + i * 4;
            p[3] = a;
            buf[i * 4] = 0x100 - p[3];
            pos += step;
            i++;
        }

        v = (int)((s64)wipeInterp * 0x0F37292CLL / 0x100000000LL);
        while (pos < 0x11E200) {
            if (i >= 0xC0) break;
            v -= 0x160;
            a = (0x8000 - v) / 256;
            if (a < 0) a = 0;
            if (a > 0x7F) a = 0x7F;
            p = buf + i * 4;
            p[3] = a;
            buf[i * 4] = 0x100 - p[3];
            pos += step;
            i++;
        }

        while (pos < 0x161D00) {
            if (i >= 0xC0) break;
            v += 0x53;
            a = (0x8000 - v) / 256;
            if (a < 0) a = 0;
            if (a > 0x7F) a = 0x7F;
            p = buf + i * 4;
            p[3] = a;
            buf[i * 4] = 0x100 - p[3];
            pos += step;
            i++;
        }

        w = 0;
        while (pos < 0x1CF200) {
            if (i >= 0xC0) break;
            v += 0x53;
            w += 0x160;
            a = (0x8000 - v) / 256;
            b = (0x8000 - w) / 256;
            if (a < 0) a = 0;
            if (a > 0x7F) a = 0x7F;
            if (b < a) b = a;
            if (b > 0x7F) b = 0x7F;
            p = buf + i * 4;
            p[3] = a;
            p[2] = b;
            p[1] = 0x100 - p[2];
            buf[i * 4] = 0x100 - p[3];
            pos += step;
            i++;
        }
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_0209f648, 0x600);
    data_0209f60c = idx;
}

/* The type-2 table: a circular mask. Lines inside the radius get a
   square-root blended edge, the second half mirrors the first. */
// @symbol _ZN7dWipe_c13func_0202ee94Ev
void dWipe_c::func_0202ee94()
{
    int fp;
    int sl;
    int i;
    int r8, r7;
    struct Px { u8 a, b, c, d; } *buf;
    int r5, r4;
    int j;
    volatile int val;

    fp = (data_0209f608 == 0) ? 1 : 0;
    val = 0x80807f7f;
    buf = (struct Px *)(data_0209f648 + fp * 0x300);

    MultiStore_Int(val, (int *)buf, 0x300);

    sl = wipeInterp >> 0xc;
    r8 = 0x60 - sl;
    r7 = sl << 1;
    r5 = 0;
    r4 = 0;
    for (i = 0; i < 0x60; i++) {
        if (r8 <= i) {
            int a = i - 0x60 + sl;
            int s = _ZN4cstd4sqrtEy((u64)(long long)(a * (r7 - a)));
            int lo = 0x80 - s;
            int hi = s + 0x80;
            if (lo < 0) lo = r5;
            if (hi > 0xff) hi = r4;
            buf[i].d = (u8)lo;
            buf[i].a = (u8)hi;
        }
    }

    r4 = 0x5f;
    for (j = 0; j < 0x60; j++) {
        buf[j + 0x60].a = buf[r4].a;
        buf[j + 0x60].d = buf[r4].d;
        r4--;
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)data_0209f648, 0x600);
    data_0209f60c = fp;
}

/* Vtable slot 5. Type 1 is the plain brightness fade, which the base answers.
   Otherwise a wipe is "at start" whenever its state machine is idle, or its
   own ramp has not passed the halfway mark. */
// @symbol _ZN7dWipe_c9IsAtStartEv
int dWipe_c::IsAtStart()
{
    if (type == 1)
        return dFdBrightness_c::IsAtStart();
    return (state == 0 || wipeInterp >= 0x200000) ? 1 : 0;
}

/* Vtable slot 6. Mirror of IsAtStart: an idle state machine counts as
   finished, and otherwise the wipe is done once its ramp has run to zero. */
// @symbol _ZN7dWipe_c7IsAtEndEv
int dWipe_c::IsAtEnd()
{
    if (type == 1)
        return dFdBrightness_c::IsAtEnd();
    return (state == 0 || wipeInterp <= 0) ? 1 : 0;
}

/* Vtable slot 7. `type == 1` defers straight back to the base through a
   qualified call -- an unqualified one would dispatch through this object's
   own vptr and land back here. Otherwise the answer composes this object's
   own slots 5 and 6, which stay virtual dispatches. */
// @symbol _ZN7dWipe_c20IsBetweenStartAndEndEv
int dWipe_c::IsBetweenStartAndEnd()
{
    if (type == 1)
        return dFdBrightness_c::IsBetweenStartAndEnd();

    if (!IsAtStart()) {
        if (IsAtEnd() == 0)
            return 1;
    }
    return 0;
}

/* Scene-side wipe reset: cancel a live capture, then return the singleton to
   its idle state. */
// @symbol func_0202ed48
extern "C" void func_0202ed48(void)
{
    if (data_0209f61c.needsCleanup == 1)
        data_0209f61c.func_0202fb30();
    data_0209f61c.func_0202ed14();
}

// @symbol _ZN7dWipe_c13func_0202ed14Ev
void dWipe_c::func_0202ed14()
{
    needsCleanup = 0;
    unk_00e = 1;
    state = 0;
    wipeInterp = 0;
    wipeSpeed = 0;
    wipeAccel = 0;
    type = 0;
    unk_018 = type;
    unk_028 = 0;
}

/* Vtable slot 8. A pure veneer to the base: the wipe has no end state of its
   own. The qualified call keeps it a direct `bl` instead of a virtual
   dispatch back into this same slot. */
// @symbol _ZN7dWipe_c8SetToEndEv
void dWipe_c::SetToEnd()
{
    dFdBrightness_c::SetToEnd();
}

/* Vtable slot 9, same shape as slot 8. */
// @symbol _ZN7dWipe_c10SetToStartEv
void dWipe_c::SetToStart()
{
    dFdBrightness_c::SetToStart();
}

/* Request a new wipe `type`. A mid-transition request is stashed into
   unk_018/unk_028 and committed by func_0202fb30 when the current leg ends;
   outside a transition it lands directly. */
// @symbol _ZN7dWipe_c13func_0202ec9cEi
int dWipe_c::func_0202ec9c(int arg)
{
    if (arg == type) {
        unk_028 = 0;
        return 0;
    }
    if (IsBetweenStartAndEnd()) {
        unk_018 = arg;
        unk_028 = 1;
        return 1;
    }
    type = arg;
    unk_028 = 0;
    return 0;
}
