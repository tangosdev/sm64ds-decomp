//cpp
/* The hardware screen wipe (arm9 .text 0x0202ec9c..0x0202fc98): dWipe_c, a
 * dFdColor_c child that closes or opens the screen through a star (type 0)
 * or a circle (type 2) instead of a plain colour fade (type 1). The shape is
 * the two hardware windows: each scanline's WIN0H/WIN1H come from a 4-byte
 * table entry that the HBlank IRQ copies in, and outside the windows only the
 * backdrop colour shows. A single static instance lives in BSS at
 * data_0209f61c; scenes pick the shape with SetType. The CPU builds the next
 * frame's table in one of the two 0x300-byte tables at data_0209f648 while
 * the IRQ reads the other. mwccarm emits .text in reverse source order, so
 * the definitions below run ROM-descending.
 *
 * ~dWipe_c() is the key function: mwcc emits D2, D0 and D1 with the
 * definition, the cartridge keeps D0 (0x0202fbc8) and D1 (0x0202fc08), and
 * the vtable/RTTI records land in .data at 0x020926d0..0x02092718.
 *
 * leftovers:
 * - the data_0209f5f8..data_0209f648 table
 *   state (shared with the Luigi minigame's iris, d_s_mg_luigi.cpp) and the
 *   0x400xxxx registers stay address-named.
 */
#include "types.h"
#include "dWipe_c.h"
#include "nitro/os/interrupt.h"
#include "nitro/os/interrupt_check.h"
#include "decl_common.h"
#include "cstd.h"
#include "nitro/os/cache.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/g2.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/gx.h"
#include "nitro/mi/memory.h"

extern "C" {
void Wipe_HBlankHandler(void);
}

extern u8           data_0209f5f8;   /* which display engine the wipe covers */
extern volatile u8  data_0209f5fc;   /* the next HBlank has a fresh line */
extern volatile u32 data_0209f608;   /* table index the IRQ reads */
extern volatile u32 data_0209f60c;   /* table index the CPU fills */
extern u8           data_0209f648[]; /* two 0x300-byte scanline tables */

/* NitroSDK's inline IME helpers (os/common/interrupt.h): each returns the
   previous master enable. File-local because an inline body in a shared
   header renumbers every includer's @NNN symbols. */
static inline int OS_DisableIrq(void) {
    u16 prep = REG_IME;
    REG_IME = 0;
    return prep;
}

static inline int OS_EnableIrq(void) {
    u16 prep = REG_IME;
    REG_IME = 1;
    return prep;
}

dWipe_c::dWipe_c() : dFdColor_c(0)
{
    Reset();
}

/* The destructor's one real obligation: if the windows are still armed,
   disarm them before the object goes away. */
dWipe_c::~dWipe_c()
{
    if (needsCleanup == 1)
        DisableWindows();
}

/* Disarm the windows: mask IME, drop the HBlank IRQ handler, clear the
   WIN0/WIN1/OBJWIN enables in both engines' DISPCNT, and if a type change
   was stashed mid-transition, commit it now. */
void dWipe_c::DisableWindows() {
    int enabled = OS_DisableIrq();
    OS_DisableIrqMask(OS_IE_H_BLANK);
    GX_HBlankIntr(0);
    OS_SetIrqFunction(OS_IE_H_BLANK, 0);
    if (enabled)
        OS_EnableIrq();
    GX_SetVisibleWnd(GX_WNDMASK_NONE);
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    if (typePending == 1) {
        type = pendingType;
        typePending = 0;
    }
}

/* Opening half of a wipe. Type 1 is the plain colour fade and the base owns
   it. `extra` is forwarded and not read; callers pass 0. */
int dWipe_c::SetBackwardTime(u32 frames, u32 extra) {
    if (type == 1)
        return dFdBrightness_c::SetBackwardTime(frames, extra);

    if (state == 0 || state == 4) {
        int t;

        if (frames == 0) {
            wipeSpeed = 0x200000;
            wipeAccel = 0;
        } else {
            t = (type == 0) ? 0x2d : 0x3c;
            wipeSpeed = 0x200000 / t;
            wipeAccel = (wipeSpeed << 1) / t;
            wipeSpeed = 0;
        }

        if (type == 2) {
            DC_FlushRange(data_0209f604, 2);
            GX_LoadBGPltt(data_0209f604, 0, 2);
            GXS_LoadBGPltt(data_0209f604, 0, 2);
        } else {
            DC_FlushRange(data_020926cc, 2);
            GX_LoadBGPltt(data_020926cc, 0, 2);
            GXS_LoadBGPltt(data_020926cc, 0, 2);
        }

        wipeInterp = 0;

        G2_SetWnd0Position(0x7f, 0, 0x7f, 0xc0);
        G2S_SetWnd0Position(0x7f, 0, 0x7f, 0xc0);
        G2_SetWnd1Position(0x80, 0, 0x80, 0xc0);
        G2S_SetWnd1Position(0x80, 0, 0x80, 0xc0);

        state = 1;
        EnableWindows();

        {
            int enabled = OS_DisableIrq();
            OS_SetIrqFunction(OS_IE_H_BLANK, Wipe_HBlankHandler);
            OS_EnableIrqMask(OS_IE_H_BLANK);
            GX_HBlankIntr(1);
            if (enabled)
                OS_EnableIrq();
        }

        needsCleanup = 1;
        return 0;
    }

    if (state == 2)
        return 1;
    state = 1;
    return 0;
}

/* Vtable slot 4 -- starts the closing half of a wipe. Only meaningful from a
   resting state (0 or 2); type 1 is the plain colour fade and the base owns
   it. Loads the wipe's backdrop colour on both engines, arms the windows,
   then hands the per-scanline work to the HBlank IRQ with IME briefly masked
   around the install. */
int dWipe_c::SetForwardTime(u32 frames, u32 extra) {
    if (state == 0 || state == 2) {
        int t;
        int enabled;

        if (type == 1)
            return dFdBrightness_c::SetForwardTime(frames, extra);

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
            DC_FlushRange(data_0209f600, 2);
            GX_LoadBGPltt(data_0209f600, 0, 2);
            GXS_LoadBGPltt(data_0209f600, 0, 2);
        } else {
            DC_FlushRange(data_020926c8, 2);
            GX_LoadBGPltt(data_020926c8, 0, 2);
            GXS_LoadBGPltt(data_020926c8, 0, 2);
        }

        wipeInterp = 0x200000;
        G2_SetWnd0Position(0, 0, 0x7f, 0xc0);
        G2S_SetWnd0Position(0, 0, 0x7f, 0xc0);
        G2_SetWnd1Position(0x80, 0, 0xff, 0xc0);
        G2S_SetWnd1Position(0x80, 0, 0xff, 0xc0);

        state = 3;
        EnableWindows();

        enabled = OS_DisableIrq();
        OS_SetIrqFunction(OS_IE_H_BLANK, Wipe_HBlankHandler);
        OS_EnableIrqMask(OS_IE_H_BLANK);
        GX_HBlankIntr(1);
        if (enabled)
            OS_EnableIrq();

        needsCleanup = 1;
        return 0;
    }

    if (state == 4)
        return 1;
    state = 3;
    return 0;
}

/* Arm the windows: everything shows inside WIN0/WIN1 (WININ), nothing but
   the backdrop outside them (WINOUT), and DISPCNT enables WIN0|WIN1 on the
   main engine, and on the sub engine unless data_0209f5f8 is set. Then fill
   both tables back to back so the first HBlank reads a finished one, and
   clear BLDCNT if a brightness effect (bit 7) is still selected. */
void dWipe_c::EnableWindows() {
    unk_00e = 1;

    G2_SetWnd0InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 |
                              GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
                          1);
    G2S_SetWnd0InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 |
                               GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
                           1);
    G2_SetWnd1InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 |
                              GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
                          1);
    G2S_SetWnd1InsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 |
                               GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
                           1);

    G2_SetWndOutsidePlane(GX_WND_PLANEMASK_NONE, 0);
    G2S_SetWndOutsidePlane(GX_WND_PLANEMASK_NONE, 0);

    GX_SetVisibleWnd(GX_WNDMASK_W0 | GX_WNDMASK_W1);

    if (data_0209f5f8 != 0) {
        GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    } else {
        GXS_SetVisibleWnd(GX_WNDMASK_W0 | GX_WNDMASK_W1);
    }

    data_0209f608 = 0;
    data_0209f60c = 0;
    BuildTable();

    data_0209f608 = 1;
    data_0209f60c = 1;
    data_0209f5fc = 0;
    BuildTable();

    if ((REG_BLDCNT & 0x80) == 0x80)
        G2_BlendNone();
    if ((REG_DB_BLDCNT & 0x80) == 0x80)
        G2S_BlendNone();
}

/* Vtable slot 2. Type 1 is the plain colour fade and the base owns it.
   Otherwise this steps the wipe's own 20.12 ramp: state 1 runs it up to the
   halfway mark and hands over to state 2, state 3 runs it back down to zero
   and pins the blend brightness on both engines before settling in state 4.
   States 0/2/4 are resting and return early. */
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
            DisableWindows();
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
            G2_SetBlendBrightness(0x3f, b);
            G2S_SetBlendBrightness(0x3f, b);
            DisableWindows();
            state = 4;
        }
        break;
    case 4:
    default:
        return;
    }
    BuildTable();
}

/* Latch the staged table into the IRQ-visible index and seed both engines'
   line-0 windows from it. */
extern "C" void Wipe_LatchTable(void) {
    data_0209f608 = data_0209f60c;
    MI_CpuCopy32(data_0209f648 + data_0209f608 * 0x300u, (int *)0x4000040, 4);
    MI_CpuCopy32(data_0209f648 + data_0209f608 * 0x300u, (int *)0x4001040, 4);
    data_0209f5fc = 1;
}

/* The HBlank handler, installed for IRQ bit 2 while a wipe runs. Past the
   last visible line it latches the next table, once per frame; on a visible
   line it copies that scanline's 4-byte entry into both engines' WIN0H and
   WIN1H. */
extern "C" void Wipe_HBlankHandler(void) {
    int line;

    OS_SetIrqCheckFlag(OS_IE_H_BLANK);

    line = REG_VCOUNT + 1;

    if (line >= 0xc0) {
        if (data_0209f5fc != 1)
            Wipe_LatchTable();
        return;
    }

    if ((REG_DISPSTAT & 2) == 0)
        return;

    MI_CpuCopy32(data_0209f648 + data_0209f608 * 0x300u + line * 4, (int *)0x4000040, 4);
    MI_CpuCopy32(data_0209f648 + data_0209f608 * 0x300u + line * 4, (int *)0x4001040, 4);

    data_0209f5fc = 0;
}

void dWipe_c::BuildTable() {
    if (type == 2)
        BuildCircleTable();
    else
        BuildStarTable();
}

/* The type-0 table: a five-pointed star, one window-edge entry per scanline
   (192), built into whichever 0x300 table the IRQ is not reading. After the
   empty-window fill (0x80807F7F), four line segments step down the star; their
   ends sit at 0.382, 0.618, 0.764 and 1.0 of its height, which are a regular
   star's vertex heights. Each writes a left edge clamped to 0..0x7F and its
   mirror 0x100 - edge; the last segment has two spans per line (the legs,
   WIN1 and WIN0). The step is 2^32/wipeInterp; then flush the pair. The
   fill value goes through a stack slot in the ROM (as MI_CpuFill32's does in
   OS_CreateThread), hence the volatile. */
void dWipe_c::BuildStarTable() {
    volatile int fill;
    int idx;
    int i;
    int pos;
    int step;
    struct WindowEdges {
        u8 win0Right, win0Left, win1Right, win1Left;
    } *buf;
    int dur;
    int v, w;
    int a, b;
    s64 d64;

    idx = (data_0209f608 == 0) ? 1 : 0;
    buf = (struct WindowEdges *)(data_0209f648 + idx * 0x300);
    fill = 0x80807F7F;
    i = 0;
    MI_CpuFill32(buf, fill, 0x300);

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
            buf[i].win1Left = a;
            buf[i].win0Right = 0x100 - buf[i].win1Left;
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
            buf[i].win1Left = a;
            buf[i].win0Right = 0x100 - buf[i].win1Left;
            pos += step;
            i++;
        }

        while (pos < 0x161D00) {
            if (i >= 0xC0) break;
            v += 0x53;
            a = (0x8000 - v) / 256;
            if (a < 0) a = 0;
            if (a > 0x7F) a = 0x7F;
            buf[i].win1Left = a;
            buf[i].win0Right = 0x100 - buf[i].win1Left;
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
            buf[i].win1Left = a;
            buf[i].win1Right = b;
            buf[i].win0Left = 0x100 - buf[i].win1Right;
            buf[i].win0Right = 0x100 - buf[i].win1Left;
            pos += step;
            i++;
        }
    }

    DC_FlushRange(data_0209f648, 0x600);
    data_0209f60c = idx;
}

/* The type-2 table: a circle. Lines inside the radius get edges at
   0x80 -/+ the chord's half-width (a square root); the bottom half mirrors
   the top. */
void dWipe_c::BuildCircleTable() {
    int idx;
    int radius;
    int i;
    int start, diameter;
    struct WindowEdges {
        u8 win0Right, win0Left, win1Right, win1Left;
    } *buf;
    int src;
    int j;
    volatile int fill;

    idx = (data_0209f608 == 0) ? 1 : 0;
    fill = 0x80807f7f;
    buf = (struct WindowEdges *)(data_0209f648 + idx * 0x300);

    MI_CpuFill32(buf, fill, 0x300);

    radius = wipeInterp >> 0xc;
    start = 0x60 - radius;
    diameter = radius << 1;
    for (i = 0; i < 0x60; i++) {
        if (start <= i) {
            int a = i - 0x60 + radius;
            int s = cstd::sqrt((a * (diameter - a)));
            int lo = 0x80 - s;
            int hi = s + 0x80;
            if (lo < 0)
                lo = 0;
            if (hi > 0xff)
                hi = 0;
            buf[i].win1Left = (u8)lo;
            buf[i].win0Right = (u8)hi;
        }
    }

    src = 0x5f;
    for (j = 0; j < 0x60; j++) {
        buf[j + 0x60].win0Right = buf[src].win0Right;
        buf[j + 0x60].win1Left = buf[src].win1Left;
        src--;
    }

    DC_FlushRange(data_0209f648, 0x600);
    data_0209f60c = idx;
}

/* Vtable slot 5. Type 1 is the plain brightness fade, which the base answers.
   Otherwise a wipe is "at start" whenever its state machine is idle, or its
   own ramp has not passed the halfway mark. */
int dWipe_c::IsAtStart()
{
    if (type == 1)
        return dFdBrightness_c::IsAtStart();
    return state == 0 || wipeInterp >= 0x200000;
}

/* Vtable slot 6. Mirror of IsAtStart: an idle state machine counts as
   finished, and otherwise the wipe is done once its ramp has run to zero. */
int dWipe_c::IsAtEnd()
{
    if (type == 1)
        return dFdBrightness_c::IsAtEnd();
    return state == 0 || wipeInterp <= 0;
}

/* Vtable slot 7. `type == 1` defers straight back to the base through a
   qualified call -- an unqualified one would dispatch through this object's
   own vptr and land back here. Otherwise the answer composes this object's
   own slots 5 and 6, which stay virtual dispatches. */
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

/* The wipe singleton. A namespace-scope object with a constructor and a
   destructor, so mwcc emits __sinit_d_wipe.cpp to build it and register
   ~dWipe_c() (was the handwritten __sinit_02074f80). The minigame
   overlays still reach it by this address name. */
dWipe_c data_0209f61c;

/* Scene-side wipe reset: disarm live windows, then return the singleton to
   its idle state. */
extern "C" void Wipe_Reset(void) {
    if (data_0209f61c.needsCleanup == 1)
        data_0209f61c.DisableWindows();
    data_0209f61c.Reset();
}

void dWipe_c::Reset() {
    needsCleanup = 0;
    unk_00e = 1;
    state = 0;
    wipeInterp = 0;
    wipeSpeed = 0;
    wipeAccel = 0;
    type = 0;
    pendingType = type;
    typePending = 0;
}

/* Vtable slot 8. A pure veneer to the base: the wipe has no end state of its
   own. The qualified call keeps it a direct `bl` instead of a virtual
   dispatch back into this same slot. */
void dWipe_c::SetToEnd()
{
    dFdBrightness_c::SetToEnd();
}

/* Vtable slot 9, same shape as slot 8. */
void dWipe_c::SetToStart()
{
    dFdBrightness_c::SetToStart();
}

/* Request a new wipe `type`. A mid-transition request is stashed into
   pendingType/typePending and committed by DisableWindows when the current leg ends;
   outside a transition it lands directly. */
int dWipe_c::SetType(int arg) {
    if (arg == type) {
        typePending = 0;
        return 0;
    }
    if (IsBetweenStartAndEnd()) {
        pendingType = arg;
        typePending = 1;
        return 1;
    }
    type = arg;
    typePending = 0;
    return 0;
}
