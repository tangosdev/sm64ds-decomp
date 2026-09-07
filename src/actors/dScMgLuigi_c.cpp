//cpp
/* dScMgLuigi_c -- the MG_LUIGI minigame scene class, ov006.
 *
 * Reconstructed translation unit: the contiguous linker run
 * 0x020efc0c..0x020f3834, 58 functions, assembled from the 58 one-function
 * legacy sources.  The class name is the cartridge's own RTTI, not a coined
 * one: ov006 0x0213ce60 holds the NUL-terminated string "12dScMgLuigi_c" and
 * 0x0213ce54 is the __si_class_type_info record that points at it.
 *
 * ---------------------------------------------------------------------------
 * BOUNDARIES
 * ---------------------------------------------------------------------------
 * RIGHT, hard: the last function here is dScMgLuigi_c_classInit at 0x020f3800
 * (0x34 bytes), ending exactly at 0x020f3834, where _ZN13dScMgMemory_cD1Ev
 * begins -- the first byte of the already-promoted single-span TU
 * src/minigames/d_s_mg_memory.cpp.
 *
 * LEFT: the function immediately below, func_ov006_020efc08, is four bytes
 * whose only word is 0xe12fff1e (`bx lr`).  It is its own delink unit, has no
 * caller and no relocation, and nothing in the cartridge attributes it to
 * either neighbour, so it is left out.
 *
 * SELF-CONTAINED, proven twice: filtering
 * config/arm9/overlays/ov006/relocs.txt on `module:overlay(6)`, exactly 70
 * accepted rows point into this range and not one is an arm_call from outside
 * it; decoding every BL word in ov006 .text agrees, set for set.  (An
 * UNfiltered scan of the other modules' relocs.txt over the same addresses
 * yields 164 rows, 163 of which the module filter rejects -- that many phantom
 * callers.)
 *
 * ---------------------------------------------------------------------------
 * `#pragma defer_codegen off` IS LOAD-BEARING TWICE
 * ---------------------------------------------------------------------------
 *   1. it makes mwccarm 2004/b56 emit one .text section per function in SOURCE
 *      order rather than in reverse, which is why the members below run lowest
 *      ROM address first;
 *   2. it makes `opt_strength_reduction`, `opt_common_subs` and
 *      `opt_propagation` bind POSITIONALLY instead of file-global last-wins,
 *      which is what lets the seven pragma-bearing shards live in one TU.
 *      Three families are involved here: opt_strength_reduction (ordinals 5,
 *      46, 49), opt_common_subs (27, 30, 31) and opt_propagation (45), and
 *      ordinal 30 carries two at once.
 * The two halves are ONE decision: source order and the pragma have to flip
 * together or `linkcheck [4b/8]`'s ROM-ascending emission-order audit refuses
 * the result.  Do not reorder the members.
 *
 * ---------------------------------------------------------------------------
 * WHY EVERY MEMBER KEEPS ITS OWN DECLARATIONS
 * ---------------------------------------------------------------------------
 * The 58 shards were recovered independently and disagree about nine symbols --
 * data_0209f608, data_0209f60c, data_0209f648, MultiCopy_Int,
 * func_ov004_020af948, data_ov006_0213ce70, data_ov006_0213abc8,
 * data_ov006_02142254 and data_ov004_020beb68.  Merging those onto one spelling
 * is a codegen hazard, not a naming one, so each member's own view is kept:
 * shadow TYPE tags are uniquified per member with the member's address as a
 * suffix, and the declarations themselves sit at BLOCK scope inside the member
 * that recovered them.  Each member that still carries an auto-generated C name
 * is wrapped in its own file-scope `extern "C" { }` region precisely so those
 * block-scope declarations get C linkage and name the ROM symbol.
 *
 * A class member function may NOT sit inside a linkage-specification region, so
 * every C++-named member is outside one, and what they call is declared in the
 * file-scope `extern "C"` regions instead.  Writing those declarations in their
 * bodies compiles and byte-matches and then fails to link with mangled
 * undefined symbols.
 *
 * Measured at this revision, after the stage-3b naming pass: this file defines
 * 57 functions carrying 58 symbols -- the single `~dScMgLuigi_c` definition
 * emits both D1 and D0.  55 of the 58 are mangled `_ZN12dScMgLuigi_c*` members;
 * exactly 3 keep an auto-generated C name (`func_ov006_020efcf8`,
 * `func_ov006_020efdac` and `dScMgLuigi_c_classInit`).  There are 10 file-scope
 * `extern "C"` regions: 3 wrap those C-named definitions, the other 7 carry
 * only declarations and shadow structs.
 *
 * decl_common.h is deliberately NOT included: it types eight of this TU's own
 * members `void(void *)` / `void(char *)` against byte-matched definitions that
 * take `char*`, which would be eight `illegal function overloading` errors
 * pointed at the definitions rather than at the header.
 */

#pragma defer_codegen off

#include "types.h"
#include "nitro/hw/registers.h"
#include "dScMgLuigi_c.h"
#include "IRQ.h"

/* Same spelling the matched IRQ::EnableIRQs / IRQ::DisableIRQs shards use. */
#define IME (*(volatile u16 *)0x04000208)

/* ---------------------------------------------------------------------------
 * Shadow types, one set per member that recovered one.  The tag suffix is the
 * member's ROM address: two members that describe the same object through
 * different windows keep both windows, because mwccarm's pointer-to-member
 * representation depends on whether the class was complete when the PMF type
 * was formed, and merging them changes it.
 * ------------------------------------------------------------------------- */
/* ordinal 4, func_ov006_020efdac */
struct Record_efdac { char _pad[0x300]; };

/* ordinal 5, dScMgLuigi_c::BuildIrisTable */
struct Px_efdf0 {
    u8 a, b, c2, d2;
};

/* ordinal 8, dScMgLuigi_c::IrisHold */
struct E_f002c { unsigned char d[0x14]; };

/* ordinal 9, dScMgLuigi_c::UpdateIris */
struct C_f0044;
typedef void (C_f0044::*PMF_f0044)(int);
struct Entry_f0044 { PMF_f0044 pmf[1]; };
struct C_f0044 {
    char pad[0x47f4];
    unsigned char guard;
    unsigned char idx;
};

/* ordinal 12, dScMgLuigi_c::UpdateReward */
#define BEB68_f0274 ((char *)data_ov004_020beb68)

/* ordinal 19, dScMgLuigi_c::CurtainUpdate */
struct C_f0ba0;
typedef void (C_f0ba0::*PMF_f0ba0)(int);
struct Entry_f0ba0 { PMF_f0ba0 pmf; };
struct Row_f0ba0 { unsigned char tag; char pad[0x17]; };
struct C_f0ba0 { char pad[0x47b7]; Row_f0ba0 rows[1]; };

/* ordinal 21, dScMgLuigi_c::CurtainsDone */
struct C_f0d58;
typedef void (C_f0d58::*PMF_f0d58)(int);
struct Entry_f0d58 { PMF_f0d58 pmf; };

/* ordinal 38, dScMgLuigi_c::WrapPicture */
typedef struct Obj_f1dbc {
    char _pad0[0x47f8]; /* 0x0000 */
    int mPosX[120];     /* 0x47f8 */
    int mPosY[120];     /* 0x49d8 */
} Obj_f1dbc;

/* ordinal 41, dScMgLuigi_c::UpdatePictures */
struct C_f1e90;
typedef void (C_f1e90::*PMF_f1e90)(int);
struct Entry_f1e90 { PMF_f1e90 pmf; };
struct C_f1e90 { char pad[1]; };

struct Ctx_f2ec0;   /* completed at ordinal 48, below */

/* ---------------------------------------------------------------------------
 * This TU's own members, forward declared once with each member's own
 * definition spelling, plus the one external that ordinal 2 needs before any
 * wrapped member exists to declare it.  Call sites whose caller recovered a
 * different pointer type cast at the call site; a pointer cast is free.
 * ------------------------------------------------------------------------- */
extern "C" {
extern int func_02053c10(int);
/* Externals a C++-NAMED member calls.  A class member function may not sit in a
   linkage-specification region, so a declaration written in its body would get
   C++ linkage and the reference would mangle; these have to be declared with C
   linkage at file scope instead.  Data is different -- mwccarm leaves a
   file-scope variable's name unmangled in C++ -- so every `data_*` declaration
   stays in the body that recovered it. */
extern int RandomIntInternal(int *seed);
extern int _ZN4cstd4sqrtEy(unsigned long long);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, unsigned int len);
extern s32 GetGameLanguage(void);
extern void func_ov004_020af948(void *a, int b, int c, int d);
extern char *func_ov004_020afb20(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
extern void DrawOamSprite(int p, int a, int b, int c);
extern void func_ov004_020b2220(int a, int b, int c, int d, int e, int f, int g);
extern void func_02012790(int x);
extern void func_020127a4(int a0, int a1, int a2, int a3);
extern void Hud_RenderSprite(int a, int b, int c, int d, int e);
extern char *_ZN2G212GetBG3ScrPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern void FreeGfxSlotsById(int arg);
extern void func_ov004_020b0a54(int c);

extern void func_ov006_020efcf8(void);
extern void func_ov006_020efdac(void);
extern int *dScMgLuigi_c_classInit(void);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 0 -- _ZN12dScMgLuigi_cD1Ev, 0x020efc0c, size 0x24 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_cD1Ev
// @symbol _ZN12dScMgLuigi_cD0Ev
/* One definition, two emitted variants.  The cartridge orders D1 (0x020efc0c,
   0x24) BELOW D0 (0x020efc30, 0x38) and carries no dScMgLuigi_c D2 at all;
   out-of-line under `#pragma defer_codegen off` emits D1, D0, D2 in that order,
   so the two ROM symbols land in the ROM's own order and the D2 is a homeless
   extra that the manifest licenses as a plain deadstrip.  Both variants store
   the vptr 0x0213cf10 and then `bl` ov004 0x020b29c0 (dScMgBase_c D2); D0 also
   reaches Memory::Deallocate through dScMgBase_c's own operator delete, which
   is why no body is needed here. */
dScMgLuigi_c::~dScMgLuigi_c()
{
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 2 -- _ZN12dScMgLuigi_c21AfterCleanupResourcesEj, 0x020efc68, size 0x90 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c21AfterCleanupResourcesEj
/* Slot 5 of _ZTV12dScMgLuigi_c.
   The recovered source returned the base call's result as if it were int;
   the real override (dScMgBase_c.h) returns void, so this now calls the
   base method as a plain statement instead. */
void dScMgLuigi_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2 && IRQ::GetIRQHandler(2) == func_ov006_020efcf8) {
        u16 ime;
        do {
            ime = IME;
            IME = 0;
        } while (ime != 0);
        IRQ::DisableIRQs(2);
        func_02053c10(0);
        IRQ::SetIRQHandler(2, 0);
        ime = IME;
        IME = 1;
    }
    dScMgBase_c::AfterCleanupResources(vfSuccess);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 3 -- func_ov006_020efcf8, 0x020efcf8, size 0xb4 */
/* ------------------------------------------------------------------ */
// @symbol func_ov006_020efcf8
/* IRQ 2's handler, installed by ordinal 10 through
   _ZN3IRQ13SetIRQHandlerEjPFvvE, whose parameter mangles PFvvE = void(*)().
   It has to stay a free function: a member function has a different type and
   would not bind. */
extern "C" {
void func_ov006_020efcf8(void)
{
    extern char data_023c0000[];
    extern int data_0209f608;
    extern int data_0209f648[][192];
    extern void MultiCopy_Int(int *dst, int *src, int len);
    int line;
    /* The (int) round-trip is load-bearing here: written as plain array
       arithmetic the whole handler re-codegens.  MEASURED 2026-09-07 -- this TU
       inherited 24 such launders from its shards and this is the ONLY one that
       pays; deleting the other 23 was byte-neutral.  Do not put those back, and
       do not delete this one. */
    *(int *)(((int)data_023c0000 + 0x3ff8)) |= 2;
    line = REG_VCOUNT + 1;
    if (line >= 0xc0) {
        func_ov006_020efdac();
    } else if (line <= 0xc0) {
        if (*(volatile u16 *)0x04000004 & 2) {
            MultiCopy_Int(&data_0209f648[data_0209f608][line], (int *)0x04000040, 4);
        }
    }
}
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 4 -- func_ov006_020efdac, 0x020efdac, size 0x44 */
/* ------------------------------------------------------------------ */
// @symbol func_ov006_020efdac
extern "C" {
/* Latches the pending scanline-table index and pushes that 0x300-byte record
   at 0x04000040. */
void func_ov006_020efdac(void)
{
    extern u32 data_0209f60c;
    extern volatile u32 data_0209f608;
    extern struct Record_efdac data_0209f648[];
    extern void MultiCopy_Int(struct Record_efdac *src, void *dst, u32 count);
    data_0209f608 = data_0209f60c;
    MultiCopy_Int(&data_0209f648[data_0209f608], (void *)0x04000040, 4);
}
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 5 -- _ZN12dScMgLuigi_c14BuildIrisTableEi, 0x020efdf0, size 0x130 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c14BuildIrisTableEi
#pragma push
#pragma opt_strength_reduction off
void dScMgLuigi_c::BuildIrisTable(int i)
{
    char *o = (char *)this;
    extern int data_0209f608;
    extern struct Px_efdf0 data_0209f648[][0xc0];
    extern int data_0209f60c;
    int toggle;
    int y;
    int k, n, m;
    u8 mirrored;

    if (data_0209f608 == 0)
        toggle = 1;
    else
        toggle = 0;
    y = *(int *)(o + i * 0x14 + 0x47ec) >> 12;

    for (k = 0; k < 0x60; k++) {
        if (0x60 - y <= k) {
            int t = k - 0x60 + y;
            int s = _ZN4cstd4sqrtEy((s64)(t * (y * 2 - t)));
            int lo = (*(int *)(o + 0x47e4) >> 12) - s;
            int hi = s + (*(int *)(o + 0x47e4) >> 12);
            if (lo < 0)
                lo = 0;
            if (hi >= 0xff)
                hi = 0xff;
            data_0209f648[toggle][k].b = lo;
            data_0209f648[toggle][k].a = hi;
        } else {
            data_0209f648[toggle][k].a = 0;
            data_0209f648[toggle][k].b = 0;
        }
    }
    for (n = 0x5f, m = 0; m < 0x60; m++, n--) {
        mirrored = data_0209f648[toggle][n].b;
        data_0209f648[toggle][m + 0x60].b = mirrored;
        data_0209f648[toggle][m + 0x60].a = data_0209f648[toggle][n].a;
    }
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(data_0209f648, 0x600);
    data_0209f60c = toggle;
}
#pragma pop

/* ------------------------------------------------------------------ */
/* ROM ordinal 6 -- _ZN12dScMgLuigi_c8IrisStopEi, 0x020eff20, size 0x98 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c8IrisStopEi
extern "C" {
extern void _ZN3IRQ11DisableIRQsEj(unsigned int);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned int, void(*)(void));
extern unsigned char data_0209d454;
}
void dScMgLuigi_c::IrisStop(int idx) {
    char *c = (char *)this;
    volatile unsigned short *ime = &IME;
    unsigned short saved;
    *(unsigned char *)(c + idx * 0x14 + 0x47f4) = 0;
    saved = *ime;
    *ime = 0;
    _ZN3IRQ11DisableIRQsEj(2);
    func_02053c10(0);
    _ZN3IRQ13SetIRQHandlerEjPFvvE(2, 0);
    if (saved != 0) { *ime; *ime = 1; }
    REG_DISPCNT &= ~0xe000;
    REG_DISPCNT_SUB &= ~0xe000;
    data_0209d454 |= 4;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 7 -- _ZN12dScMgLuigi_c8IrisGrowEi, 0x020effb8, size 0x74 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c8IrisGrowEi
extern "C" {
extern unsigned char data_0209d45c;
}

void dScMgLuigi_c::IrisGrow(int idx) {
    char *c = (char *)this;
    *(int *)(c + 0x47ec + idx * 0x14) += 0x2000;
    BuildIrisTable(idx);
    if ((*(int *)(c + 0x47ec + idx * 0x14) >> 12) < 0xa0) return;
    *(unsigned char *)(c + 0x47f5 + idx * 0x14) += 1;
    data_0209d45c &= ~4;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 8 -- _ZN12dScMgLuigi_c8IrisHoldEi, 0x020f002c, size 0x18 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c8IrisHoldEi
void dScMgLuigi_c::IrisHold(int idx) {
    struct E_f002c *base = (struct E_f002c *)this;
  *(unsigned *)((unsigned char *)&base[idx] + 0x47f0) = 0x1000;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 9 -- _ZN12dScMgLuigi_c10UpdateIrisEv, 0x020f0044, size 0x60 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c10UpdateIrisEv
void dScMgLuigi_c::UpdateIris()
{
    C_f0044 *c = (C_f0044 *)this;
    extern Entry_f0044 data_ov006_021421ec[];
    if (c->guard == 0) return;
    int j = c->idx;
    (c->*data_ov006_021421ec[j].pmf[0])(0);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 10 -- _ZN12dScMgLuigi_c9StartIrisEv, 0x020f00a4, size 0x134 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c9StartIrisEv
void dScMgLuigi_c::StartIris()
{
    char *self = (char *)this;
    extern unsigned int data_0209f608;
    extern unsigned char data_0209d460;
    unsigned short saved;
    *(unsigned char *)(self + 0x47f4) = 1;
    *(int *)(self + 0x47e4) = 0x80000;
    *(int *)(self + 0x47e8) = 0x60000;
    *(unsigned char *)(self + 0x47f5) = 0;
    *(int *)(self + 0x47ec) = 0x40000;
    *(int *)(self + 0x47f0) = 0x1000;
    data_0209f608 = 0;
    BuildIrisTable(0);

    data_0209f608 = 1;
    BuildIrisTable(0);

    data_0209f608 = 0;
    saved = IME;
    IME = 0;
    IRQ::SetIRQHandler(2, func_ov006_020efcf8);

    REG_DISPCNT = (REG_DISPCNT & ~0xe000) | 0x2000;
    data_0209d460 = 1;
    { unsigned int v = *(volatile unsigned short *)0x04000048; v = (v & ~0x3f) | 0x18; v = v | 0x20; *(volatile unsigned short *)0x04000048 = v; }
    *(volatile unsigned short *)0x0400004a = (*(volatile unsigned short *)0x0400004a & ~0x3f) | 0x14;
    *(volatile unsigned short *)0x04000040 = 0;
    *(volatile unsigned short *)0x04000044 = 0xc0;
    IRQ::EnableIRQs(2);
    func_02053c10(1);
    if (saved != 0) {
        IME;
        IME = 1;
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 11 -- _ZN12dScMgLuigi_c10DrawRewardEv, 0x020f01d8, size 0x9c */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c10DrawRewardEv
void dScMgLuigi_c::DrawReward()
{
    char *c = (char *)this;
    extern void *data_ov006_0213ce70[];
    extern char data_ov006_02137cd8[];
    if (*(unsigned char *)(c + 0x47e3) == 0) return;
    int x = *(int *)(c + 0x47d0) >> 0xc;
    int y = *(int *)(c + 0x47d4) >> 0xc;
    if (x <= 8) x = 8;
    if (x >= 0xf0) x = 0xf0;
    if (y <= 8) y = 8;
    if (y >= 0xb8) y = 0xb8;
    s32 r = GetGameLanguage();
    func_ov004_020af948(*(void **)((char *)data_ov006_0213ce70[r] + 0x34), x - 8, y, 0);
    func_ov004_020af948(*(void **)(data_ov006_02137cd8 + 0x64), x + 8, y, 0);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 12 -- _ZN12dScMgLuigi_c12UpdateRewardEv, 0x020f0274, size 0x228 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c12UpdateRewardEv
extern "C" {
extern void func_ov004_020adb1c(int self);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
}

void dScMgLuigi_c::UpdateReward()
{
    char *s = (char *)this;
    if (*(u8 *)(s + 0x47e0) == 0)
        return;

    if (*(u8 *)(s + 0x47e1) == 0) {
        *(int *)(s + 0x47d4) += *(int *)(s + 0x47d8);
        *(int *)(s + 0x47d8) -= 0x100;
        if (*(u8 *)(s + 0x47df) != 0) {
            int v;
            (*(u8 *)(s + 0x47df))--;
            v = *(u8 *)(s + 0x47df);
            if (v < 0)
                *(u8 *)(s + 0x47df) = 0;
            return;
        }
        *(u8 *)(s + 0x47df) = 0x40;
        *(u8 *)(s + 0x47e1) += 1;
        return;
    }

    if (*(u8 *)(s + 0x47e1) == 1) {
        if (*(u8 *)(s + 0x47df) != 0) {
            int v;
            (*(u8 *)(s + 0x47df))--;
            v = *(u8 *)(s + 0x47df);
            if (v < 0)
                *(u8 *)(s + 0x47df) = 0;
            return;
        }
        *(u8 *)(s + 0x47e3) = 0;
        *(u8 *)(s + 0x47e1) += 1;
        {
            char *g = BEB68_f0274;
            if (g != 0) {
                if (*(int *)(g + 0xb4) < 0x270f)
                    *(int *)(g + 0xb4) += 1;
                if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
                    *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
            }
        }
        func_ov004_020adb1c(BEB68_f0274 != 0 ? *(int *)(BEB68_f0274 + 0xb4) : 0);
        return;
    }

    if (*(u8 *)(s + 0x47e2) != 0) {
        *(u8 *)(s + 0x47df) += 1;
        if (*(u8 *)(s + 0x47df) < 4)
            return;
        _ZN5Sound12PlayBank2_2DEj(0x1bc);
        *(u8 *)(s + 0x47df) = 0;
        *(u8 *)(s + 0x47e2) -= 1;
        *(u16 *)(s + 0x5172) += 1;
        if (*(u16 *)(s + 0x5172) >= 0x32) {
            *(u16 *)(s + 0x5172) = 0x32;
            *(u8 *)(s + 0x51fb) = 0;
        }
        return;
    }

    *(u8 *)(s + 0x47e0) = 0;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 13 -- _ZN12dScMgLuigi_c11StartRewardEi, 0x020f049c, size 0x50 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c11StartRewardEi
void dScMgLuigi_c::StartReward(int i) {
    char *c = (char *)this;
    char *s = c + i*4;
    *(unsigned char *)(c + 0x47e0) = 1;
    *(int *)(c + 0x47d0) = *(int *)(s + 0x47f8);
    *(int *)(c + 0x47d4) = *(int *)(s + 0x49d8);
    *(unsigned char *)(c + 0x47df) = 16;
    *(unsigned char *)(c + 0x47e1) = 0;
    *(unsigned char *)(c + 0x47e3) = 1;
    *(unsigned char *)(c + 0x47e2) = 5;
    *(int *)(c + 0x47d8) = -3840;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 14 -- _ZN12dScMgLuigi_c12DrawCurtainsEv, 0x020f04ec, size 0xec */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c12DrawCurtainsEv
void dScMgLuigi_c::DrawCurtains()
{
    char *base = (char *)this;
    extern char *data_ov006_021350d8;
    int j;
    for (j = 0; j < 2; j++) {
        if (*(unsigned char *)(base + 0x47b5) != 0) {
            int x = *(int *)(base + 0x47a0) >> 0xc;
            int y = *(int *)(base + 0x47a4) >> 0xc;
            char *e = data_ov006_021350d8;
            for (;;) {
                char *r = func_ov004_020afb20((int)e, x, y, -1, 1, 0x1000, 0);
                if (r != 0) {
                    int v = (int)((unsigned int)(*(int *)(r + 4) << 0x10) >> 0x1c);
                    *(int *)r = (*(int *)r & ~0xc00) | 0x800;
                    *(unsigned short *)(r + 4) = (unsigned short)((*(unsigned short *)(r + 4) & ~0xf000) | (v << 12));
                }
                if (*(unsigned short *)(e + 6) == 0xffff) break;
                e += 8;
            }
        }
        base += 0x18;
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 15 -- _ZN12dScMgLuigi_c16CurtainStateSlowEi, 0x020f05d8, size 0x124 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c16CurtainStateSlowEi
void dScMgLuigi_c::CurtainStateSlow(int i)
{
    char *c = (char *)this;
    int n = i * 0x18;
    unsigned short *ctr = (unsigned short *)(c + 0x47b2 + n);
    int *b;
    int *a;
    int v;
    *ctr = *ctr + 1;
    if (*ctr == 0x1a)
        _ZN5Sound12PlayBank2_2DEj(0x1b9);
    b = (int *)(c + 0x47a8 + n);
    a = (int *)(c + 0x47a0 + n);
    *a = *a + *b;
    v = *a >> 12;
    if (i == 0) {
        *b = *b - 0x140;
        if (v < 0x80) return;
        if (*b <= 0) return;
        *a = 0x80000;
        *(unsigned char *)(c + n + 0x47b4) = 0;
        *(unsigned char *)(c + n + 0x47b7) = 0;
        FreeGfxSlotsById(0xd);
    } else {
        *b = *b + 0x140;
        if (v > 0x80) return;
        if (*b >= 0) return;
        *a = 0x80000;
        *(unsigned char *)(c + n + 0x47b4) = 0;
        *(unsigned char *)(c + n + 0x47b7) = 0;
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 16 -- _ZN12dScMgLuigi_c16CurtainStateFastEi, 0x020f06fc, size 0x190 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c16CurtainStateFastEi
/* The tests on `i` nested inside the branch that already decided `i` are NOT
   redundant: they are what mwccarm 2004/b56 needs to emit this body.  MEASURED
   2026-09-07 -- deleting the four of them (and the code the else arm can then no
   longer reach) makes the member stop reproducing outright. */
void dScMgLuigi_c::CurtainStateFast(int i)
{
    char *c = (char *)this;
    int n = i * 0x18;
    int *pf8 = (int *)(c + 0x47a8 + n);
    int *pf0 = (int *)(c + 0x47a0 + n);
    int v;

    *pf0 = *pf0 + *pf8;
    v = *pf0 >> 12;

    if (i == 0) {
        *pf8 += 0x140;
        if (v >= 0x80 && *pf8 > 0) {
            *pf0 = 0x80000;
            *(unsigned char *)(c + n + 0x47b4) = 0;
            *(unsigned char *)(c + n + 0x47b7) = 0;
            if (i == 0)
                _ZN5Sound12PlayBank2_2DEj(0x1b9);
            return;
        }
        if (i != 0)
            return;
        *(int *)(c + 0x47ac + n) = Sound_PlayIfNotActive(*(int *)(c + 0x47ac + n), 2, 0x1b8, 0);
    } else {
        *pf8 -= 0x140;
        if (v <= 0x80 && *pf8 < 0) {
            *pf0 = 0x80000;
            *(unsigned char *)(c + n + 0x47b4) = 0;
            *(unsigned char *)(c + n + 0x47b7) = 0;
            if (i != 0)
                return;
            _ZN5Sound12PlayBank2_2DEj(0x1b9);
            return;
        }
        if (i != 0)
            return;
        *(int *)(c + 0x47ac + n) = Sound_PlayIfNotActive(*(int *)(c + 0x47ac + n), 2, 0x1b8, 0);
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 17 -- _ZN12dScMgLuigi_c16CurtainStateHoldEi, 0x020f088c, size 0x1e0 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c16CurtainStateHoldEi
void dScMgLuigi_c::CurtainStateHold(int i)
{
    char *c = (char *)this;
    int n = i * 0x18;
    unsigned short *ctr = (unsigned short *)(c + 0x47b2 + n);
    int *b;
    int *a;
    int v;
    *ctr = *ctr + 1;
    if (*ctr == 0x35)
        _ZN5Sound12PlayBank2_2DEj(0x1b9);
    if (i == 0) {
        if (*ctr <= 0x35) {
            int *p = (int *)(c + 0x47ac + n);
            *p = Sound_PlayIfNotActive(*p, 2, 0x1b8, 0);
        }
    }
    {
        unsigned short *q = (unsigned short *)(c + 0x47b0 + n);
        if (*q != 0) {
            *q = *q - 1;
            if ((short)*q < 0) *q = 0;
            return;
        }
    }
    b = (int *)(c + 0x47a8 + n);
    a = (int *)(c + 0x47a0 + n);
    *a = *a + *b;
    v = *a >> 12;
    if (i == 0) {
        *b = *b + 0x140;
        if (v > 0x80) return;
        *a = 0x80000;
        *(unsigned char *)(c + n + 0x47b4) = 0;
        *(unsigned char *)(c + n + 0x47b7) = 0;
        FreeGfxSlotsById(0x1d);
        if (*(unsigned char *)(c + 0xc4) != 0) return;
        *(unsigned char *)(c + 0xc3) = 1;
        *(unsigned char *)(c + 0xc4) = 1;
        *(unsigned short *)(c + 0xc0) = 0;
    } else {
        *b = *b - 0x140;
        if (v < 0x80) return;
        *a = 0x80000;
        *(unsigned char *)(c + n + 0x47b4) = 0;
        *(unsigned char *)(c + n + 0x47b7) = 0;
        FreeGfxSlotsById(0x1d);
        if (*(unsigned char *)(c + 0xc4) == 0) {
            *(unsigned char *)(c + 0xc3) = 1;
            *(unsigned char *)(c + 0xc4) = 1;
            *(unsigned short *)(c + 0xc0) = 0;
        }
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 18 -- _ZN12dScMgLuigi_c18CurtainStateBounceEi, 0x020f0a6c, size 0x134 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c18CurtainStateBounceEi
void dScMgLuigi_c::CurtainStateBounce(int i)
{
    char *c = (char *)this;
    int n = i * 0x18;
    int *pf8 = (int *)(c + 0x47a8 + n);
    int *pf0 = (int *)(c + 0x47a0 + n);
    int v;
    *pf0 = *pf0 + *pf8;
    v = *pf0 >> 12;
    if (i == 0) {
        *pf8 -= 0x100;
        if (v >= 0x140) {
            *pf0 = 0x140000;
            ((unsigned char *)(c + 0x47b7))[n]++;
            *(short *)(c + n + 0x47b0) = 0x10;
            *pf8 = -0x6000;
        }
    } else {
        *pf8 += 0x100;
        if (v <= -0x40) {
            *pf0 = -0x40000;
            ((unsigned char *)(c + 0x47b7))[n]++;
            *(short *)(c + n + 0x47b0) = 0x10;
            *pf8 = 0x6000;
        }
    }
    if (i == 0) {
        *(int *)(c + 0x47ac + n) = Sound_PlayIfNotActive(*(int *)(c + 0x47ac + n), 2, 0x1b8, 0);
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 19 -- _ZN12dScMgLuigi_c13CurtainUpdateEi, 0x020f0ba0, size 0x50 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c13CurtainUpdateEi
void dScMgLuigi_c::CurtainUpdate(int i) {
    C_f0ba0 *c = (C_f0ba0 *)this;
    extern Entry_f0ba0 data_ov006_0214221c[];
    (c->*(data_ov006_0214221c[c->rows[i].tag].pmf))(i);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 20 -- _ZN12dScMgLuigi_c12CurtainStartEi, 0x020f0bf0, size 0x168 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c12CurtainStartEi
void dScMgLuigi_c::CurtainStart(int i) {
    char *c = (char *)this;
    extern unsigned char data_0209d460;
    extern int data_ov006_0212e850[];
    unsigned short t;
    int x;
    int v;

    t = *(unsigned short *)(c + 0x47b0 + i * 0x18);
    if (t != 0) {
        *(short *)(c + 0x47b0 + i * 0x18) = t - 1;
        if (*(short *)(c + 0x47b0 + i * 0x18) < 0)
            *(short *)(c + 0x47b0 + i * 0x18) = 0;
        return;
    }

    x = *(unsigned short *)0x0400004a;
    x &= ~0x3f00;
    x |= 0x1800;
    x &= ~0x2000;
    x |= 0x2000;
    *(unsigned short *)0x0400004a = x;
    x = (*(unsigned short *)0x0400004a & ~0x3f) | 0x14;
    *(unsigned short *)0x0400004a = x;
    *(volatile int *)0x04000000 = (*(volatile int *)0x04000000 & ~0xe000) | 0x8000;
    data_0209d460 = 4;

    *(int *)(c + 0x47a0 + i * 0x18) = data_ov006_0212e850[i] << 12;
    *(int *)(c + 0x47a4 + i * 0x18) = 0x60000;
    *(char *)(c + 0x47b5 + i * 0x18) = 1;
    *(char *)(c + 0x47b6 + i * 0x18) = 1;
    if (i != 0)
        *(int *)(c + 0x47a8 + i * 0x18) = -0x8000;
    else
        *(int *)(c + 0x47a8 + i * 0x18) = 0x8000;

    v = *(int *)(c + 0xbc);
    while (v >= 5)
        v -= 5;
    if (v != 0) {
        *(char *)(c + 0x47b6 + i * 0x18) = 2;
        if (i != 0)
            *(int *)(c + 0x47a8 + i * 0x18) = -0x7000;
        else
            *(int *)(c + 0x47a8 + i * 0x18) = 0x7000;
    }
    *(int *)(c + 0x47ac + i * 0x18) = 0;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 21 -- _ZN12dScMgLuigi_c12CurtainsDoneEv, 0x020f0d58, size 0x80 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c12CurtainsDoneEv
int dScMgLuigi_c::CurtainsDone()
{
    C_f0d58 *c = (C_f0d58 *)this;
    extern Entry_f0d58 data_ov006_02142204[];
    int count = 0;
    int i = 0;
    char *p = (char *)c;
    do {
        if (*(unsigned char *)(p + 0x47b4) != 0) {
            (c->*data_ov006_02142204[*(unsigned char *)(p + 0x47b6)].pmf)(i);
            count++;
        }
        i++;
        p += 0x18;
    } while (i < 2);
    return count == 0;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 22 -- _ZN12dScMgLuigi_c13ResetCurtainsEv, 0x020f0dd8, size 0x50 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c13ResetCurtainsEv
void dScMgLuigi_c::ResetCurtains() {
    char *c = (char *)this;
    int i;
    for (i = 0; i < 2; i++) {
        *(unsigned char *)(c + 0x47b4) = 1;
        *(unsigned char *)(c + 0x47b6) = 0;
        *(short *)(c + 0x47b0) = 0;
        *(short *)(c + 0x47b2) = 0;
        *(unsigned char *)(c + 0x47b5) = 0;
        *(unsigned char *)(c + 0x47b7) = 0;
        c += 0x18;
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 23 -- _ZN12dScMgLuigi_c9DrawTimerEv, 0x020f0e28, size 0x84 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c9DrawTimerEv
void dScMgLuigi_c::DrawTimer() {
    char *self = (char *)this;
    extern int *data_ov006_0213ce70[];
    int idx;
    if (*(int *)(self + 0x4f78) < 2)
        return;
    idx = GetGameLanguage();
    DrawOamSprite(*(int *)((char *)data_ov006_0213ce70[idx] + 0xc), 0x80, 0x10, 0);
    func_ov004_020b2220(0x80, 0x28, *(unsigned short *)(self + 0x5172), 1, -1, 0x800, 0);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 24 -- _ZN12dScMgLuigi_c9TickTimerEv, 0x020f0eac, size 0xd0 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c9TickTimerEv
void dScMgLuigi_c::TickTimer()
{
    char *c = (char *)this;
    if (*(u16 *)(c + 0x5172) != 0) {
        {
            u8 *q = (u8 *)(c + 0x51fb);
            *q = *q + 1;
        }
        if (*(u8 *)(c + 0x51fb) < 0x3c)
            return;
        *(u8 *)(c + 0x51fb) = 0;
        {
            u16 *p = (u16 *)(c + 0x5172);
            *p = *p - 1;
        }
        if (*(s16 *)(c + 0x5172) <= 0)
            *(s16 *)(c + 0x5172) = 0;
        if (*(u16 *)(c + 0x5172) <= 2) {
            _ZN5Sound12PlayBank2_2DEj(0xa6);
            return;
        }
        _ZN5Sound12PlayBank2_2DEj(0xa7);
        return;
    }
    _ZN5Sound12PlayBank2_2DEj((unsigned int)((*(u8 *)(c + 0x545a) << 3) + 0xc0));
    func_02012790(0xe);
    BeginCatch(0);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 25 -- _ZN12dScMgLuigi_c12DrawPicturesEv, 0x020f0f7c, size 0x90 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c12DrawPicturesEv
void dScMgLuigi_c::DrawPictures() {
    char *c = (char *)this;
    extern int data_ov006_0213abc8[];
    int i;
    if (*(int *)(c+0x4f78) == 0)
        return;
    for (i=0;i<0x78;i++) {
        if (*(unsigned char *)(c+i+0x53dd) == 1) {
            Hud_RenderSprite(
                data_ov006_0213abc8[*(unsigned char *)(c+i+0x5365)],
                ((int *)(c+0x47f8))[i] >> 12,
                ((int *)(c+0x49d8))[i] >> 12,
                -1,
                0);
        }
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 26 -- _ZN12dScMgLuigi_c13DrawPenaltiesEv, 0x020f100c, size 0xe0 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c13DrawPenaltiesEv
void dScMgLuigi_c::DrawPenalties()
{
    char *c = (char *)this;
    extern void *data_ov006_0213ce70[];
    extern char data_ov006_02137cd8[];
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char *)(c + 0x4671) != 0) {
            int x = *(s32 *)(c + 0x4660) >> 0xc;
            int y = *(s32 *)(c + 0x4664) >> 0xc;
            s32 r;
            if (x <= 0x10) x = 0x10;
            if (x >= 0xe0) x = 0xe0;
            if (y <= 8) y = 8;
            if (y >= 0xb8) y = 0xb8;
            r = GetGameLanguage();
            func_ov004_020af948(*(void **)((char *)data_ov006_0213ce70[r] + 0x38), x - 0x10, y, 0);
            func_ov004_020af948(*(void **)(data_ov006_02137cd8 + 0xa4), x, y, 0);
            func_ov004_020af948(*(void **)(data_ov006_02137cd8 + 0xa0), x + 0x10, y, 0);
        }
        c += 0x14;
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 27 -- _ZN12dScMgLuigi_c15UpdatePenaltiesEv, 0x020f10ec, size 0x120 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c15UpdatePenaltiesEv
#pragma push
#pragma opt_common_subs off
void dScMgLuigi_c::UpdatePenalties()
{
    char *q = (char *)this;
    int i;
    for (i = 0; i < 0x10; i++, q += 0x14) {
        if (*(u8 *)(q + 0x4670) != 0) {
            if (*(u8 *)(q + 0x4672) == 0) {
                if (*(u16 *)(q + 0x466c) != 0) {
                    *(u16 *)(q + 0x466c) -= 1;
                    if (*(short *)(q + 0x466c) < 0)
                        *(u16 *)(q + 0x466c) = 0;
                    *(int *)(q + 0x4664) += *(int *)(q + 0x4668);
                    *(int *)(q + 0x4668) += 0x100;
                } else {
                    *(u16 *)(q + 0x466c) = 0x40;
                    *(u8 *)(q + 0x4672) += 1;
                }
            } else {
                if (*(u16 *)(q + 0x466c) != 0) {
                    *(u16 *)(q + 0x466c) -= 1;
                    if (*(short *)(q + 0x466c) < 0)
                        *(u16 *)(q + 0x466c) = 0;
                } else {
                    *(u16 *)(q + 0x466c) = 0;
                    *(u8 *)(q + 0x4670) = 0;
                    *(u8 *)(q + 0x4671) = 0;
                }
            }
        }
    }
}
#pragma pop

/* ------------------------------------------------------------------ */
/* ROM ordinal 28 -- _ZN12dScMgLuigi_c10AddPenaltyEi, 0x020f120c, size 0xbc */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c10AddPenaltyEi
/* Arms the first free entry of the sixteen-entry table at +0x4660 at picture
   `idx`'s position, and takes 10 off the counter at +0x5172. */
void dScMgLuigi_c::AddPenalty(int idx)
{
    char *base = (char *)this;
    int i;
    char *p = base;
    for (i = 0; i < 0x10; i++, p += 0x14) {
        if (*(unsigned char *)(p + 0x4670) != 0)
            continue;
        *(base + i * 0x14 + 0x4670) = 1;
        *(unsigned char *)(base + i * 0x14 + 0x4671) = 1;
        *(short *)(base + i * 0x14 + 0x466c) = 0x10;
        *(int *)(base + i * 0x14 + 0x4660) = *(int *)(base + idx * 4 + 0x47f8);
        *(int *)(base + i * 0x14 + 0x4664) = *(int *)(base + idx * 4 + 0x49d8);
        *(int *)(base + i * 0x14 + 0x4668) = 0x1100;
        *(unsigned char *)(base + i * 0x14 + 0x4672) = 0;
        *(unsigned short *)(base + 0x5172) -= 0xa;
        if (*(short *)(base + 0x5172) < 0)
            *(short *)(base + 0x5172) = 0;
        return;
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 29 -- _ZN12dScMgLuigi_c14DrawWantedIconEv, 0x020f12c8, size 0x50 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c14DrawWantedIconEv
void dScMgLuigi_c::DrawWantedIcon() {
    char *c = (char *)this;
    extern void *data_ov006_0213abc8[];
    if (*(unsigned short *)(c + 0x5164) == 0) return;
    func_ov004_020af948(data_ov006_0213abc8[*(unsigned char *)(c + 0x545a)],
                        *(unsigned short *)(c + 0x5166),
                        *(unsigned short *)(c + 0x5168), 0);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 30 -- _ZN12dScMgLuigi_c16TickPictureFlashEi, 0x020f1318, size 0xb4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c16TickPictureFlashEi
#pragma push
#pragma opt_common_subs off
#pragma push
#pragma opt_strength_reduction off
void dScMgLuigi_c::TickPictureFlash(int idx)
{
    char *c = (char *)this;
    char *base = c + 0x506c;
    int twice = idx * 2;
    unsigned short h = *(unsigned short *)(base + twice);
    unsigned char *q;

    *(short *)(base + twice) = (short)(h - 1);

    if (*(short *)(c + (idx << 1) + 0x506c) < 0)
        *(short *)(c + (idx << 1) + 0x506c) = 0;

    /* The goto and the duplicated block are load-bearing.  MEASURED 2026-09-07 --
       folding them into one if/else over a single `p` makes the member stop
       reproducing; mwccarm keeps the two exits distinct. */
    if ((((unsigned short)*(unsigned short *)(c + (idx << 1) + 0x506c) >> 2) & 1) != 0) {
        char *p = c + 0x53dd;
        p[idx] = 0;
        q = (unsigned char *)(p + idx);
        goto after_flag;
    }
    {
        char *p = c + 0x53dd;
        p[idx] = 1;
        q = (unsigned char *)(p + idx);
    }
after_flag:
    if (*(unsigned short *)(c + (idx << 1) + 0x506c) != 0)
        return;
    *(unsigned char *)(c + idx + 0x51fd) = 0;
    *q = 1;
}
#pragma pop
#pragma pop

/* ------------------------------------------------------------------ */
/* ROM ordinal 31 -- _ZN12dScMgLuigi_c17MovePictureBounceEi, 0x020f13cc, size 0x1e0 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c17MovePictureBounceEi
#pragma push
#pragma opt_common_subs off
void dScMgLuigi_c::MovePictureBounce(int i)
{
    char *c = (char *)this;
    extern int data_0209d4b8;
    extern int data_ov006_0212e8b8[];
    extern s16 data_02082214[];
    u8 *started = (u8 *)(c + 0x5275 + i);

    if (*started == 0) {
        u32 r = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        u16 val = (u16)((((r << 4) >> 15) << 12));

        *(u16 *)(c + i * 2 + 0x4f7c) = val;
        *started += 1;
        return;
    }

    {
        u8 *speedLevel = (u8 *)(c + 0x5365);
        int *posX = (int *)(c + 0x47f8);
        int *posY = (int *)(c + 0x49d8);
        u16 phase;
        int a;
        int stepX, stepY;
        int speed;
        s64 prod;
        int round12 = 0x800;

        phase = *(u16 *)(c + i * 2 + 0x4f7c);
        a = phase >> 4;
        stepX = data_02082214[a * 2 + 1];
        speed = data_ov006_0212e8b8[speedLevel[i]];
        prod = (s64)stepX * speed;
        posX[i] += (int)((prod + round12) >> 12);

        phase = *(u16 *)(c + i * 2 + 0x4f7c);
        a = phase >> 4;
        stepY = data_02082214[a * 2];
        speed = data_ov006_0212e8b8[speedLevel[i]];
        prod = (s64)stepY * speed;
        posY[i] += (int)((prod + round12) >> 12);

        {
            int x = *(int *)(c + i * 4 + 0x47f8) >> 0xc;
            int y = *(int *)(c + i * 4 + 0x49d8) >> 0xc;

            if (x > 0xf8) {
                *(u16 *)(c + i * 2 + 0x4f7c) = 0x8000 - *(u16 *)(c + i * 2 + 0x4f7c);
                *(int *)(c + i * 4 + 0x47f8) = 0xf8000;
            } else if (x < 8) {
                *(u16 *)(c + i * 2 + 0x4f7c) = 0x8000 - *(u16 *)(c + i * 2 + 0x4f7c);
                *(int *)(c + i * 4 + 0x47f8) = 0x8000;
            }

            if (y > 0xb8) {
                *(u16 *)(c + i * 2 + 0x4f7c) = -*(u16 *)(c + i * 2 + 0x4f7c);
                *(int *)(c + i * 4 + 0x49d8) = 0xb8000;
                return;
            }

            if (y < 8) {
                *(u16 *)(c + i * 2 + 0x4f7c) = -*(u16 *)(c + i * 2 + 0x4f7c);
                *(int *)(c + i * 4 + 0x49d8) = 0x8000;
                return;
            }

            return;
        }
    }
}
#pragma pop

/* ------------------------------------------------------------------ */
/* ROM ordinal 32 -- _ZN12dScMgLuigi_c15MovePictureSwayEi, 0x020f15ac, size 0x250 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c15MovePictureSwayEi
/* dScMgLuigi_c per-slot mover, ov006 0x020f15ac (592 bytes). One
 * slot of the 120-entry picture table per call. A slot that has not started
 * yet is armed: phase 0, mStarted set, the x speed loaded from the speed-level
 * table (positive for level 0, negated otherwise) and the y speed from its own
 * table, and the call returns. A running slot adds its two speeds to its
 * position, then runs the x phase machine -- phase 0 decelerates toward zero
 * and flips to 2 or 1 at the crossing (the direction it was moving picks the
 * new phase), phases 1 and 2 accelerate toward the level's top speed and drop
 * back to 0 on reaching it -- and finally wraps the position through
 * dScMgLuigi_c::WrapPicture.
 *
 * Plain member access throughout, and that is the whole match: every read of
 * mVelX[idx] takes the `this + idx*4 + 0x4000` base with a #0xbb8 offset,
 * every read-modify-write of it takes the pool-loaded array base with the
 * scaled index, and mwccarm picks those two forms itself. The near-miss draft
 * this replaces (div 70) had forced the first form through a u64 launder and
 * `#pragma opt_common_subs off`, which is what rotated its registers. */


void dScMgLuigi_c::MovePictureSway(int idx)
{
    extern int data_ov006_0212e888[];
    extern int data_ov006_0212e898[];
    extern int data_ov006_0212e8a8[];
    if (mStarted[idx] == 0) {
        mMovePhase[idx] = 0;
        mStarted[idx]++;
        if (mSpeedLevel[idx] == 0) {
            mVelX[idx] = data_ov006_0212e888[mSpeedLevel[idx]];
        } else {
            mVelX[idx] = -data_ov006_0212e888[mSpeedLevel[idx]];
        }
        mVelY[idx] = data_ov006_0212e898[mSpeedLevel[idx]];
        return;
    }
    mPosX[idx] += mVelX[idx];
    mPosY[idx] += mVelY[idx];
    if (mMovePhase[idx] == 0) {
        if (mVelX[idx] > 0) {
            mVelX[idx] -= data_ov006_0212e8a8[mSpeedLevel[idx]];
            if (mVelX[idx] <= 0) {
                mVelX[idx] = 0;
                mMovePhase[idx] = 2;
            }
        } else if (mVelX[idx] < 0) {
            mVelX[idx] += data_ov006_0212e8a8[mSpeedLevel[idx]];
            if (mVelX[idx] >= 0) {
                mVelX[idx] = 0;
                mMovePhase[idx] = 1;
            }
        }
    } else if (mMovePhase[idx] == 1) {
        mVelX[idx] += data_ov006_0212e8a8[mSpeedLevel[idx]];
        if (mVelX[idx] >= data_ov006_0212e888[mSpeedLevel[idx]]) {
            mMovePhase[idx] = 0;
        }
    } else {
        mVelX[idx] -= data_ov006_0212e8a8[mSpeedLevel[idx]];
        if (mVelX[idx] <= -data_ov006_0212e888[mSpeedLevel[idx]]) {
            mMovePhase[idx] = 0;
        }
    }
    WrapPicture(idx);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 33 -- _ZN12dScMgLuigi_c22MovePictureDriftRandomEi, 0x020f17fc, size 0x130 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c22MovePictureDriftRandomEi
/* dScMgLuigi_c per-slot mover, ov006 0x020f17fc (304 bytes). A slot that has not started draws a random phase (one of eight 0x1000 steps).
 * A running slot adds one sine/cosine step (data_02082214, indexed by the
 * slot's phase) scaled by its speed-level entry to mPosX/mPosY, then wraps
 * through dScMgLuigi_c::WrapPicture.
 *
 * Plain member access on the class header is the match under 2004/b56: the
 * twice-read mMovePhase[idx] takes the `this + idx*2 + 0x4f00` base with a
 * #0x7c offset as a compiler temp, and the two RMWs take the pool-loaded array
 * base with the scaled index. The raw char *form this replaces pooled 0x4f7c
 * whole (+8 bytes) and, once that was fixed by hand, still swapped the r4/ip
 * pair in the second update. */


void dScMgLuigi_c::MovePictureDriftRandom(int idx)
{
    extern int data_0209d4b8;
    extern int data_ov006_0212e878[];
    extern s16 data_02082214[];
    if (mStarted[idx] == 0) {
        unsigned int r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        unsigned short val = (unsigned short)(((r << 4) >> 15) << 12);
        mMovePhase[idx] = val;
        mStarted[idx]++;
        return;
    }
    {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e878[mSpeedLevel[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e878[mSpeedLevel[idx]] + 0x800) >> 12);
    }
    WrapPicture(idx);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 34 -- _ZN12dScMgLuigi_c21MovePictureDriftByRowEi, 0x020f192c, size 0x144 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c21MovePictureDriftByRowEi
/* dScMgLuigi_c per-slot mover, ov006 0x020f192c (324 bytes). A slot that has not started is armed at phase 0x8000 or 0 from unk_51f5[idx / 8].
 * A running slot adds one sine/cosine step (data_02082214, indexed by the
 * slot's phase) scaled by its speed-level entry to mPosX/mPosY, then wraps
 * through dScMgLuigi_c::WrapPicture.
 *
 * Member access, not raw offsets, for the reason spelled out on
 * dScMgLuigi_c::MovePictureDriftRandom above. */


void dScMgLuigi_c::MovePictureDriftByRow(int idx)
{
    extern int data_ov006_0212e868[];
    extern s16 data_02082214[];
    int cnt;
    int j;
    if (mStarted[idx] == 0) {
        mStarted[idx]++;
        cnt = 0;
        j = idx;
        if (idx >= 8) {
            do {
                j -= 8;
                cnt++;
            } while (j >= 8);
        }
        if (unk_51f5[cnt] != 0) {
            mMovePhase[idx] = 0x8000;
        } else {
            mMovePhase[idx] = 0;
        }
        return;
    }
    {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e868[mSpeedLevel[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e868[mSpeedLevel[idx]] + 0x800) >> 12);
    }
    WrapPicture(idx);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 35 -- _ZN12dScMgLuigi_c24MovePictureDriftByColumnEi, 0x020f1a70, size 0x128 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c24MovePictureDriftByColumnEi
/* dScMgLuigi_c per-slot mover, ov006 0x020f1a70 (296 bytes). A slot that has not started is armed at phase 0x4000 or 0xc000 from unk_51ed[idx & 7].
 * A running slot adds one sine/cosine step (data_02082214, indexed by the
 * slot's phase) scaled by its speed-level entry to mPosX/mPosY, then wraps
 * through dScMgLuigi_c::WrapPicture.
 *
 * Member access, not raw offsets, for the reason spelled out on
 * dScMgLuigi_c::MovePictureDriftRandom above. */


void dScMgLuigi_c::MovePictureDriftByColumn(int idx)
{
    extern int data_ov006_0212e858[];
    extern s16 data_02082214[];
    if (mStarted[idx] == 0) {
        mStarted[idx]++;
        if (unk_51ed[idx & 7] != 0) {
            mMovePhase[idx] = 0x4000;
        } else {
            mMovePhase[idx] = 0xc000;
        }
        return;
    }
    {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e858[mSpeedLevel[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e858[mSpeedLevel[idx]] + 0x800) >> 12);
    }
    WrapPicture(idx);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 36 -- _ZN12dScMgLuigi_c23MovePictureDriftByLevelEi, 0x020f1b98, size 0x11c */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c23MovePictureDriftByLevelEi
/* dScMgLuigi_c per-slot mover, ov006 0x020f1b98 (284 bytes). A slot that has not started takes its phase from unk_515c[speed level].
 * A running slot adds one sine/cosine step (data_02082214, indexed by the
 * slot's phase) scaled by its speed-level entry to mPosX/mPosY, then wraps
 * through dScMgLuigi_c::WrapPicture.
 *
 * Member access, not raw offsets, for the reason spelled out on
 * dScMgLuigi_c::MovePictureDriftRandom above. */


void dScMgLuigi_c::MovePictureDriftByLevel(int idx)
{
    extern int data_ov006_0212e8d8[];
    extern s16 data_02082214[];
    if (mStarted[idx] == 0) {
        u8 t = mSpeedLevel[idx];
        mMovePhase[idx] = unk_515c[t];
        mStarted[idx]++;
    } else {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e8d8[mSpeedLevel[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e8d8[mSpeedLevel[idx]] + 0x800) >> 12);
        WrapPicture(idx);
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 37 -- _ZN12dScMgLuigi_c21MovePictureDriftFixedEi, 0x020f1cb4, size 0x108 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c21MovePictureDriftFixedEi
/* dScMgLuigi_c per-slot mover, ov006 0x020f1cb4 (264 bytes). A slot that has not started is armed at phase 0x6000.
 * A running slot adds one sine/cosine step (data_02082214, indexed by the
 * slot's phase) scaled by its speed-level entry to mPosX/mPosY, then wraps
 * through dScMgLuigi_c::WrapPicture.
 *
 * Member access, not raw offsets, for the reason spelled out on
 * dScMgLuigi_c::MovePictureDriftRandom above. */


void dScMgLuigi_c::MovePictureDriftFixed(int idx)
{
    extern int data_ov006_0212e8c8[];
    extern s16 data_02082214[];
    if (mStarted[idx] == 0) {
        mMovePhase[idx] = 0x6000;
        mStarted[idx]++;
        return;
    }
    {
        int a = mMovePhase[idx] >> 4;
        mPosX[idx] = mPosX[idx] + (s32)(((s64)data_02082214[a * 2 + 1] * data_ov006_0212e8c8[mSpeedLevel[idx]] + 0x800) >> 12);
        a = mMovePhase[idx] >> 4;
        mPosY[idx] = mPosY[idx] + (s32)(((s64)data_02082214[a * 2] * data_ov006_0212e8c8[mSpeedLevel[idx]] + 0x800) >> 12);
    }
    WrapPicture(idx);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 38 -- _ZN12dScMgLuigi_c11WrapPictureEi, 0x020f1dbc, size 0x84 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c11WrapPictureEi
/* Wraps one picture's 20.12 position round the 0x110 x 0xd0 screen. Leaf. */
void dScMgLuigi_c::WrapPicture(int i) {
    Obj_f1dbc *self = (Obj_f1dbc *)this;
    int x = self->mPosX[i] >> 12;
    int y = self->mPosY[i] >> 12;
    if (x > 0x110) self->mPosX[i] = -0x10000;
    if (x < -0x10) self->mPosX[i] = 0x110000;
    if (y > 0xd0)  self->mPosY[i] = -0x10000;
    if (y < -0x10) self->mPosY[i] = 0xd0000;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 39 -- _ZN12dScMgLuigi_c11StopPictureEi, 0x020f1e40, size 0x18 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c11StopPictureEi
void dScMgLuigi_c::StopPicture(int idx) {
    unsigned *base = (unsigned *)this;
    unsigned *p = base + idx;
    *(unsigned *)((unsigned char *)p + 0x4bb8) = 0;
    *(unsigned *)((unsigned char *)p + 0x4d98) = 0;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 40 -- _ZN12dScMgLuigi_c14RestartPictureEi, 0x020f1e58, size 0x38 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c14RestartPictureEi
void dScMgLuigi_c::RestartPicture(int idx) {
    unsigned char *self = (unsigned char *)this;
    extern unsigned char data_ov006_0213ceac[];
    unsigned short board = *(unsigned short *)(self + 0x5174);
    self += idx;
    self[0x53dd] = 1;
    self[0x5275] = 0;
    self[0x51fd] = data_ov006_0213ceac[board] + 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 41 -- _ZN12dScMgLuigi_c14UpdatePicturesEv, 0x020f1e90, size 0x68 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c14UpdatePicturesEv
void dScMgLuigi_c::UpdatePictures() {
    C_f1e90 *c = (C_f1e90 *)this;
    extern Entry_f1e90 data_ov006_02142254[];
    int i;
    char *base = (char *)c;
    for (i = 0; i < 0x78; i++) {
        char *b = base + i;
        if (*(unsigned char *)(b + 0x52ed) != 0) {
            unsigned char k = *(unsigned char *)(b + 0x51fd);
            (c->*data_ov006_02142254[k].pmf)(i);
        }
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 42 -- _ZN12dScMgLuigi_c10BeginCatchEi, 0x020f1ef8, size 0xd4 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c10BeginCatchEi
void dScMgLuigi_c::BeginCatch(int p1)
{
    char *o = (char *)this;
    int v;
    *(short *)(o + 0x5166) = (short)(((int *)(o + 0x47f8))[*(unsigned char *)(o + 0x5456) - 1] >> 0xc);
    *(short *)(o + 0x5168) = (short)(((int *)(o + 0x49d8))[*(unsigned char *)(o + 0x5456) - 1] >> 0xc);
    *(short *)(o + 0x5164) = 0x60;
    *(int *)(o + 0x4f78) = 3;
    *(short *)(o + 0x516a) = 0xc8;
    v = *(int *)(o + 0xbc);
    while (v >= 5) v -= 5;
    if (v != 4)
        *(unsigned short *)(o + 0x516a) += 8;
    if (p1 == 0)
        *(short *)(o + 0x516a) = 0x80;
    *(unsigned char *)(o + 0x5459) = (unsigned char)p1;
    StartIris();
    *(unsigned char *)(o + 0x47f5) = 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 43 -- _ZN12dScMgLuigi_c10CheckTouchEv, 0x020f1fcc, size 0x258 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c10CheckTouchEv
void dScMgLuigi_c::CheckTouch()
{
    char *c = (char *)this;
    extern unsigned char data_020a0e40;
    extern unsigned char data_020a0de8[][4];
    extern unsigned char data_020a0de9[][4];
    extern unsigned char data_020a0dea[][4];
    extern unsigned char data_020a0deb[][4];
    extern unsigned short data_ov006_0212e848[];
    int idx = data_020a0e40;
    int flag = 0;
    int cur;
    int i;

    if (data_020a0de8[idx][0] != 0) {
        if (data_020a0de9[idx][0] != 0) flag = 1;
    }
    if (flag == 0) return;

    cur = *(u8 *)(c + 0x5456);
    {
        int dx = data_020a0dea[idx][0] - (((int *)(c + 0x47f8))[cur - 1] >> 12);
        int dy = data_020a0deb[idx][0] - (((int *)(c + 0x49d8))[cur - 1] >> 12);
        if (dx <= 0x10 && dx >= -0x10 && dy <= 0x10 && dy >= -0x10) {
            int lvl, cat;

            ((dScMgLuigi_c *)c)->BeginCatch(1);
            cur = *(u8 *)(c + 0x5456);
            ((dScMgLuigi_c *)c)->StartReward(cur - 1);

            lvl = *(int *)(c + 0xbc);
            cat = 0;
            if (lvl >= 0xa) cat = 2;
            else if (lvl >= 5) cat = 1;
            func_02012790(0x26);

            {
                int tbl = data_ov006_0212e848[cat];
                func_02012790(tbl + ((*(u8 *)(c + 0x545a)) << 3));
            }
            return;
        }
    }

    {
        int off = 0x51fd;

        for (i = 0; i < 0x78; i++) {
            if (*(u8 *)(c + i + 0x52ed) == 1) {
                u8 *p = (u8 *)(c + i + off);
                if (*p != 9) {
                    int dx2 = data_020a0de8[idx][2] - (((int *)(c + 0x47f8))[i] >> 12);
                    int dy2 = data_020a0de8[idx][3] - (((int *)(c + 0x49d8))[i] >> 12);
                    if (dx2 <= 0x10 && dx2 >= -0x10 && dy2 <= 0x10 && dy2 >= -0x10) {
                        u16 *arrC;
                        ((dScMgLuigi_c *)c)->AddPenalty(i);
                        *(u8 *)(c + 0x5459) = 0;
                        *p = 9;
                        arrC = (u16 *)(c + 0x506c);
                        arrC[i] = 0x40;
                        if (*(u16 *)(c + 0x5172) != 0) {
                            int cur2 = *(u8 *)(c + 0x5456);
                            int t = (((int *)(c + 0x47f8))[cur2 - 1] >> 12) - 0x80;
                            int pan = (t * 0x30) >> 7;
                            int unk545a;
                            if (pan >= 0x30) pan = 0x30;
                            if (pan <= -0x30) pan = -0x30;
                            unk545a = *(u8 *)(c + 0x545a);
                            func_020127a4(2, (unk545a << 3) + 0xbf, 0xffff, pan);
                            func_02012790(0xe);
                        }
                        return;
                    }
                }
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 44 -- _ZN12dScMgLuigi_c16PlaceBoardLayoutEv, 0x020f2224, size 0x56c */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c16PlaceBoardLayoutEv
/* dScMgLuigi_c board layout, ov006 0x020f2224 (1388 bytes). Called
 * from dScMgLuigi_c::PlaceNextPicture for the boards whose data_ov006_0213ce98 entry is
 * non-zero: instead of scattering pictures at random it lays the whole board
 * out at once. The table entry picks one of four fixed layouts -- 1 is a 2x2
 * block at (0x70,0x50) step 0x20 whose speed levels run consecutively from one
 * random start, 2 a 4x4 at (0x50,0x30), 3 a 6-row x 8-column sheet at
 * (0x10,0x10), and anything else a single row of eight at y = -5 -- each with
 * its own random count in unk_5456 and a per-slot random speed level that is
 * nudged off unk_545a when it collides with it (and forced back onto it for
 * the slot that unk_5456 - 1 names). Then eight plus six random 0/1 bytes and
 * unk_5455++.
 *
 * Two spellings are load-bearing (both measured):
 *   - every RandomIntInternal result goes through the `rnd` local first. Writing
 *     the call inline inside the store expression lets mwccarm hoist the
 *     `this + n + 0x5365` element address ABOVE the call into a callee-saved
 *     register; the ROM computes it after the call and folds it into
 *     `ldrb r0,[r1,r0]!` / `add sl,sl,r1`. Inline cost +8 bytes and 21 extra
 *     instructions (divergence 193/347 -> shape-exact).
 *   - i/j/n are declared INSIDE each layout arm, not once at the top. Sharing
 *     them across the four arms coalesces their spill slots into one order and
 *     rotates every register in the function (58 divergences); per-arm locals
 *     give each arm its own pair of stack slots in the ROM's order. Within an
 *     arm the order is i, n, j (reverse declaration order is the colouring
 *     order: j takes the low register, n the next).
 * The two tail loops keep the INLINE call: naming the result there stops
 * mwccarm hoisting the 0x7fff mask out of the loop (+33 divergences). */


void dScMgLuigi_c::PlaceBoardLayout()
{
    extern int data_0209d4b8;
    extern u8 data_ov006_0213ce98[];
    int mode;
    int rnd;
    int i;

    mode = data_ov006_0213ce98[unk_5174];
    if (mode == 1) {
        int i;
        int n;
        int j;
        int base;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        base = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
        rnd = RandomIntInternal(&data_0209d4b8);
        unk_5456 = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                mPosX[n] = (0x70 + j * 0x20) << 12;
                mPosY[n] = (0x50 + i * 0x20) << 12;
                unk_52ed[n] = 1;
                unk_51fd[n] = 0;
                mSpeedLevel[n] = (base + n) & 3;
                if (unk_545a == mSpeedLevel[n])
                    unk_5456 = n + 1;
                n++;
            }
        }
    } else if (mode == 2) {
        int i;
        int n;
        int j;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        unk_5456 = ((((u32)rnd >> 16) & 0x7fff) * 16 >> 15) + 1;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                u8 step;
                mPosX[n] = (0x50 + j * 0x20) << 12;
                mPosY[n] = (0x30 + i * 0x20) << 12;
                unk_52ed[n] = 1;
                unk_51fd[n] = 0;
                rnd = RandomIntInternal(&data_0209d4b8);
                mSpeedLevel[n] = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
                if (unk_545a == mSpeedLevel[n]) {
                    rnd = RandomIntInternal(&data_0209d4b8);
                    step = (((u32)rnd >> 16) & 0x7fff) * 3 >> 15;
                    mSpeedLevel[n] += step + 1;
                    mSpeedLevel[n] &= 3;
                }
                if (n == unk_5456 - 1)
                    mSpeedLevel[n] = unk_545a;
                n++;
            }
        }
    } else if (mode == 3) {
        int i;
        int n;
        int j;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        unk_5456 = ((((u32)rnd >> 16) & 0x7fff) * 0x30 >> 15) + 1;
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 8; j++) {
                u8 step;
                mPosX[n] = (0x10 + j * 0x20) << 12;
                mPosY[n] = (0x10 + i * 0x20) << 12;
                unk_52ed[n] = 1;
                unk_51fd[n] = 0;
                rnd = RandomIntInternal(&data_0209d4b8);
                mSpeedLevel[n] = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
                if (unk_545a == mSpeedLevel[n]) {
                    rnd = RandomIntInternal(&data_0209d4b8);
                    step = (((u32)rnd >> 16) & 0x7fff) * 3 >> 15;
                    mSpeedLevel[n] += step + 1;
                    mSpeedLevel[n] &= 3;
                }
                if (n == unk_5456 - 1)
                    mSpeedLevel[n] = unk_545a;
                n++;
            }
        }
    } else {
        int n;
        int i;
        n = 0;
        rnd = RandomIntInternal(&data_0209d4b8);
        unk_5456 = ((((u32)rnd >> 16) & 0x7fff) * 8 >> 15) + 1;
        for (i = 0; i < 8; i++) {
            u8 step;
            mPosX[n] = (0x10 + i * 0x20) << 12;
            mPosY[n] = -(5 << 12);
            unk_52ed[n] = 1;
            unk_51fd[n] = 0;
            rnd = RandomIntInternal(&data_0209d4b8);
            mSpeedLevel[n] = (((u32)rnd >> 16) & 0x7fff) * 3 >> 15;
            if (unk_545a == mSpeedLevel[n]) {
                rnd = RandomIntInternal(&data_0209d4b8);
                step = ((((u32)rnd >> 16) & 0x7fff) * 2 >> 15) + 1;
                mSpeedLevel[n] += step;
                if (mSpeedLevel[n] >= 3)
                    mSpeedLevel[n] = 0;
            }
            if (n == unk_5456 - 1)
                mSpeedLevel[n] = unk_545a;
            n++;
        }
    }

    for (i = 0; i < 8; i++)
        unk_51ed[i] = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15;
    for (i = 0; i < 6; i++)
        unk_51f5[i] = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15;
    unk_5455++;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 45 -- _ZN12dScMgLuigi_c16PlaceNextPictureEv, 0x020f2790, size 0x528 */
/* ------------------------------------------------------------------ */
#pragma push
#pragma opt_propagation off
// @symbol _ZN12dScMgLuigi_c16PlaceNextPictureEv
/* dScMgLuigi_c picture placer, ov006 0x020f2790 (1320 bytes). Runs
 * once per frame while the board is still filling. Boards whose
 * data_ov006_0213ce98 entry is set are laid out in one go by
 * dScMgLuigi_c::PlaceBoardLayout instead; the rest place one picture per call. The slot
 * gets a random speed level, nudged off unk_545a when it lands on it, and the
 * very first slot of a board is dropped at a random cell outside the middle
 * block with a small random jitter. Every later slot retries a random cell up
 * to 100 times, then walks the grid linearly until it finds a free one. When
 * unk_5456 reaches the board's capacity the board is marked full (unk_5455)
 * and four random 4-bit values go into unk_515c.
 *
 * Two pointer locals are load-bearing (both measured against the ROM):
 *   - `p` in the speed-level fixup. The ROM stores and re-reads
 *     mSpeedLevel[cur] through the scaled-index form `[r3,r0]` but keeps
 *     `&mSpeedLevel[cur]` in a register for the three accesses inside the
 *     fixup. Declaring p before the `if` (whose test still uses the subscript)
 *     puts the `add` in the pre-branch block exactly where the ROM has it;
 *     spelling the fixup with subscripts costs the address register and
 *     re-derives the base from the literal pool.
 *   - `f` over data_ov006_0213ce84[idx]. mwccarm shares one loaded value
 *     across both tests when both are written as subscripts, which drops the
 *     re-load the ROM performs after `unk_5455 = 1`. Naming a const pointer
 *     and using it for the SECOND test only breaks that: the first test stays
 *     folded into `[base,idx]`, the `add` survives for the pointer, and the
 *     tail re-loads through it.
 * `#pragma opt_propagation off` is worth 219 -> 330 aligned instructions here:
 * without it `this` colours into r6 and the whole callee-saved file permutes. */


void dScMgLuigi_c::PlaceNextPicture()
{
    extern int data_0209d4b8;
    extern u8 data_ov006_0213ce84[];
    extern u8 data_ov006_0213ce98[];
    extern u16 data_ov006_0213cee0[];
    int idx;
    int cur;
    int lim;
    int col;
    int row;
    int tries;
    int rnd;

    if (unk_5455 != 0)
        return;

    idx = unk_5174;
    if (data_ov006_0213ce98[idx] != 0) {
        PlaceBoardLayout();
        return;
    }

    lim = data_ov006_0213cee0[idx];
    cur = unk_5456;

    rnd = RandomIntInternal(&data_0209d4b8);
    mSpeedLevel[cur] = (((u32)rnd >> 16) & 0x7fff) * 4 >> 15;
    {
        u8 *p = &mSpeedLevel[cur];
        if (unk_545a == mSpeedLevel[cur]) {
            rnd = RandomIntInternal(&data_0209d4b8);
            *p += ((((u32)rnd >> 16) & 0x7fff) * 3 >> 15) + 1;
            *p &= 3;
        }
    }

    if (cur == 0) {
        int jx;
        int jy;
        do {
            col = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 13 >> 15;
            row = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15;
        } while (col >= 6 && col <= 10 && row >= 4 && row <= 8);

        jy = -2;
        jx = jy;
        jx += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
        jy += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;

        if (data_ov006_0213ce84[idx] != 0) {
            mPosX[0] = (col * 20 + (jx + 8)) << 12;
            mPosY[0] = (row * 20 + (jy + 16)) << 12;
            unk_52ed[0] = 1;
            unk_51fd[0] = 0;
            mSpeedLevel[0] = unk_545a;
            mGrid[col][row] = 1;
            unk_5456++;
            return;
        } else {
            int n = lim - 1;
            mPosX[n] = (col * 20 + 8) << 12;
            mPosY[n] = (row * 20 + 16) << 12;
            unk_52ed[n] = 1;
            unk_51fd[n] = 0;
            mSpeedLevel[n] = unk_545a;
            mGrid[col][row] = 1;
        }
    }

    tries = 0;
    while (1) {
        u8 *cell;
        if (tries >= 100) {
            col++;
            if (col >= 12) {
                row++;
                col = 0;
                if (row >= 11)
                    row = 0;
            }
        } else {
            col = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 13 >> 15;
            row = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15;
        }
        cell = (u8 *)((unsigned int)((char *)this + col * 9 + row) + 0x5178);
        if (*cell == 0) {
            int jx;
            int jy;
            jy = -2;
            jx = jy;
            jx += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
            jy += (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15;
            mPosX[cur] = (col * 20 + (jx + 8)) << 12;
            mPosY[cur] = (row * 20 + (jy + 16)) << 12;
            *cell = 1;
            break;
        }
        tries++;
    }

    unk_52ed[cur] = 1;
    unk_51fd[cur] = 0;
    unk_5456++;

    const u8 *f = &data_ov006_0213ce84[idx];
    if (unk_5456 >= (data_ov006_0213ce84[idx] != 0 ? lim : lim - 1)) {
        if (idx == 0xe || idx == 0x10 || idx == 0x11 || idx == 0x13) {
            mPosX[cur] = mPosX[lim - 1];
            mPosY[cur] = mPosY[lim - 1] - 0x14000;
        }
        unk_5455 = 1;
        if (*f != 0)
            unk_5456 = 1;
        else
            unk_5456 = lim;
    }

    if (unk_5455 == 0)
        return;

    unk_515c[0] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
    unk_515c[1] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
    unk_515c[2] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
    unk_515c[3] = ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 16 >> 15) << 12;
}
#pragma pop

/* ------------------------------------------------------------------ */
/* ROM ordinal 46 -- _ZN12dScMgLuigi_c12ChooseTargetEv, 0x020f2cb8, size 0x168 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c12ChooseTargetEv
#pragma push
#pragma opt_strength_reduction off
void dScMgLuigi_c::ChooseTarget()
{
    char *c = (char *)this;
    extern int data_0209d4b8;
    extern u8 data_ov006_0213cec0[];
    extern u16 data_ov006_0213cdec[];
    u8 *pb;
    s32 i;
    s32 k;
    u16 *dst;
    u16 idx;
    u16 t;
    s32 j;
    s32 off;
    s32 nbytes;
    volatile u16 v;

    t = *(u16 *)(c + 0x5172);
    k = 0;
    if (t >= 0x14) {
        k = 0x14;
    } else if (t >= 0xa) {
        k = 0xa;
    }

    *(u8 *)(c + 0x545a) = (data_ov006_0213cec0 + k)[((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xa) >> 15];

    t = *(u16 *)(c + 0x5174);
    if (t == 9) {
        *(u8 *)(c + 0x545a) = 1;
    }
    if (t == 0xe || t == 0x10 || t == 0x11 || t == 0x13) {
        if (*(u8 *)(c + 0x545a) == 3) {
            *(u8 *)(c + 0x545a) = (u8)((((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3)) >> 15);
        }
    }

    nbytes = 2;
    for (i = 0, off = 0; i < 8; i++, off += 0x20) {
        pb = (u8 *)(c + 0x5000);
        dst = (u16 *)(_ZN2G212GetBG3ScrPtrEv() + 0x1d8) + off;
        idx = data_ov006_0213cdec[pb[0x45a]];
        idx += off;
        for (j = 0; j < 8; j++) {
            v = ((u16 *)_ZN2G212GetBG3ScrPtrEv())[idx];
            MultiStore16(v, (char *)dst, nbytes);
            idx++;
            dst++;
        }
    }
}
#pragma pop

/* ------------------------------------------------------------------ */
/* ROM ordinal 47 -- _ZN12dScMgLuigi_c11ChooseBoardEv, 0x020f2e20, size 0xa0 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c11ChooseBoardEv
void dScMgLuigi_c::ChooseBoard()
{
    char *c = (char *)this;
    extern int data_0209d4b8;
    int lvl;
    *(unsigned short *)(c + 0x5176) = *(unsigned short *)(c + 0x5174);
    lvl = *(int *)(c + 0xbc);
    if (lvl >= 0x14) {
        int pick = (int)(((((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0xa) >> 0xf);
        if (pick + 0xa == *(unsigned short *)(c + 0x5176)) {
            int step = (int)((((((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9) >> 0xf)) + 1;
            pick += step;
            if (pick >= 0xa) pick -= 0xa;
        }
        lvl = pick + 0xa;
    }
    *(unsigned short *)(c + 0x5174) = lvl;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 48 -- _ZN12dScMgLuigi_c10ResetBoardEv, 0x020f2ec0, size 0x14c */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c10ResetBoardEv
extern "C" {
struct S20_f2ec0
{
    s32 a;      /* +0x00 */
    s32 b;      /* +0x04 */
    u8 pad8[4]; /* +0x08 */
    s16 c;      /* +0x0c */
    u8 padE[2]; /* +0x0e */
    u8 d;       /* +0x10 */
    u8 e;       /* +0x11 */
};

struct S24_f2ec0
{
    u8 pad0[0x14]; /* +0x00 */
    u8 a;          /* +0x14 */
    u8 b;          /* +0x15 */
    u8 pad16[2];
};

struct Ctx_f2ec0
{
    u8 pad0000[0x4660];
    struct S20_f2ec0 arr16[16];    /* 0x4660 */
    struct S24_f2ec0 arr2[2];      /* 0x47a0 */
    u8 pad47d0[0x10];        /* 0x47d0 */
    u8 f47e0;                /* 0x47e0 */
    u8 pad47e1[2];
    u8 f47e3;                /* 0x47e3 */
    s32 f47e4;               /* 0x47e4 */
    s32 f47e8;               /* 0x47e8 */
    s32 f47ec;               /* 0x47ec */
    u8 pad47f0[4];
    u8 f47f4;                /* 0x47f4 */
    u8 f47f5;                /* 0x47f5 */
    u8 pad47f6[2];
    s32 mPosX[120];           /* 0x47f8 */
    s32 mPosY[120];           /* 0x49d8 */
    s32 mVelX[120];           /* 0x4bb8 */
    s32 mVelY[120];           /* 0x4d98 */
    u8 pad4f78[4];
    s16 mMovePhase[120];           /* 0x4f7c */
    s16 arrI[120];           /* 0x506c */
    u8 pad515c[8];
    s16 h5164;               /* 0x5164 */
    s16 h5166;               /* 0x5166 */
    s16 h5168;               /* 0x5168 */
    s16 h516a;               /* 0x516a */
    u8 pad516c[0xc];
    u8 mGrid[13][9];          /* 0x5178 */
    u8 pad51ed[0x10];
    u8 arrE[120];            /* 0x51fd */
    u8 mStarted[120];        /* 0x5275 -- not cleared here */
    u8 arrF[120];            /* 0x52ed */
    u8 mSpeedLevel[120];            /* 0x5365 */
    u8 arrH[120];            /* 0x53dd */
    u8 b5455;                /* 0x5455 */
    u8 b5456;                /* 0x5456 */
    u8 pad5457[1];
    u8 b5458;                /* 0x5458 */
    u8 b5459;                /* 0x5459 */
};
}

void dScMgLuigi_c::ResetBoard()
{
    struct Ctx_f2ec0 *c = (struct Ctx_f2ec0 *)this;
    s32 i;
    s32 j;
    s32 k;
    s32 m;
    s32 n;

    for (i = 0; i < 120; i++)
    {
        c->mPosX[i] = 0;
        c->mPosY[i] = 0;
        c->mVelX[i] = 0;
        c->mVelY[i] = 0;
        c->arrE[i] = 0;
        c->arrF[i] = 0;
        c->mSpeedLevel[i] = 0;
        c->arrI[i] = 0;
        c->mMovePhase[i] = 0;
        c->arrH[i] = 0;
    }

    for (j = 0; j < 16; j++)
    {
        c->arr16[j].a = 0;
        c->arr16[j].b = 0;
        c->arr16[j].c = 0;
        c->arr16[j].d = 0;
        c->arr16[j].e = 0;
    }

    for (k = 0; k < 2; k++)
    {
        c->arr2[k].a = 0;
        c->arr2[k].b = 0;
    }

    c->f47e0 = 0;
    c->f47e3 = 0;
    c->f47f4 = 0;
    c->f47e4 = 0;
    c->f47e8 = 0;
    c->f47f5 = 0;
    c->f47ec = 0;

    for (m = 0; m < 13; m++)
    {
        for (n = 0; n < 9; n++)
        {
            c->mGrid[m][n] = 0;
        }
    }

    c->h5164 = 0;
    c->h5166 = 0x80;
    c->h5168 = 0x60;
    c->h516a = 0;

    c->b5456 = 0;
    c->b5458 = 1;
    c->b5459 = 0;
    c->b5455 = 0;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 49 -- _ZN12dScMgLuigi_c10StateCatchEv, 0x020f300c, size 0x190 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c10StateCatchEv
#pragma push
#pragma opt_strength_reduction off
extern "C" {
/* This member reaches slot 18 of its own object through the dispatch table at
   data_ov006_02142254, so the pointer-to-member has to be formed against a class
   with that many virtuals. The slots below slot 18 are never called from here
   and are placeholders; only the shape matters. */
class C_f300c {
public:
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18(int);
};

typedef void (C_f300c::*PMF_f300c)(int);
}



void dScMgLuigi_c::StateCatch()
{
    char *o = (char *)this;
    extern PMF_f300c data_ov006_02142254[];
    int matches;
    int i;

    UpdateIris();
    UpdatePenalties();
    UpdateReward();

    matches = 0;
    for (i = 0; i < 0x78; i++) {
        if (*(unsigned char *)(o + i + 0x52ed) != 0) {
            int x = *(unsigned char *)(o + i + 0x51fd);
            if (x == 9) {
                matches++;
                (((C_f300c *)o)->*data_ov006_02142254[x])(i);
            }
        }
    }
    if (matches != 0)
        return;

    for (i = 0; i < 0x78; i++) {
        unsigned char *p = (unsigned char *)(o + i + 0x53dd);
        if (*p == 1)
            *p = 0;
    }

    if (*(unsigned short *)(o + 0x516a) == 0)
        return;
    *(unsigned short *)(o + 0x5164) = 1;
    *(unsigned short *)(o + 0x516a) -= 1;
    if (*(short *)(o + 0x516a) > 0)
        return;
    *(unsigned short *)(o + 0x516a) = 0;
    *(unsigned short *)(o + 0x5164) = 0;

    if (*(unsigned char *)(o + 0x5459) != 0) {
        int v = *(int *)(o + 0xbc);
        while (v >= 5)
            v -= 5;
        if (v != 4)
            ((C_f300c *)o)->v18(-1);
        else
            func_ov004_020b0a54(1);
    } else {
        func_ov004_020b0a54(0x12);
    }
    *(unsigned char *)(o + 0xc3) = 0;
}
#pragma pop

/* ------------------------------------------------------------------ */
/* ROM ordinal 50 -- _ZN12dScMgLuigi_c9StatePlayEv, 0x020f319c, size 0x40 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c9StatePlayEv
void dScMgLuigi_c::StatePlay() {
    char *c = (char *)this;
    TickTimer();
    if (*(int *)(c + 0x4f78) == 3) return;
    UpdatePictures();
    CheckTouch();
    UpdatePenalties();
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 51 -- _ZN12dScMgLuigi_c10StatePlaceEv, 0x020f31dc, size 0x84 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c10StatePlaceEv
void dScMgLuigi_c::StatePlace() {
    char *self = (char *)this;
    int i;
    for (i = 0; i < 8; i++)
        PlaceNextPicture();
    if (CurtainsDone() == 0)
        return;
    if (*(unsigned char *)(self + 0x5455) == 0)
        return;
    *(int *)(self + 0x4f78) = 2;
    *(unsigned char *)(self + 0x5458) = 0;
    *(short *)(self + 0x516c) = 0xa;
    *(short *)(self + 0x516e) = 0;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 52 -- _ZN12dScMgLuigi_c10StateSetupEv, 0x020f3260, size 0x34 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c10StateSetupEv
void dScMgLuigi_c::StateSetup()
{
    char *o = (char *)this;
    *(short *)(o + 0x5164) = 0;
    ResetCurtains();
    *(int *)(o + 0x4f78) = 1;
    ChooseTarget();
}

/* ---------------------------------------------------------------------------
 * From here down the members are C++-named, so none of them may sit inside a
 * linkage-specification region and none of them may declare a ROM symbol in its
 * own body -- a block-scope declaration inside a C++-named member gets C++
 * linkage and the reference mangles.  Everything ordinals 53..57 call that this
 * TU does not itself define is therefore declared here, once, at file scope,
 * AFTER the last wrapped member so that none of them can see it.
 * ------------------------------------------------------------------------- */

/* ordinal 55's window on the object: a 0x4f78-deep pad and one PMF, the whole
   reason a class type is needed at all. */
struct C_beh;
typedef void (C_beh::*PMF_beh)();
struct Entry_beh { PMF_beh pmf; };

extern "C" {
/* ordinal 53 -- dScMgLuigi_c::OnYoshiTryEat */
extern void FreeGfxSlotsById(int arg);
extern void func_ov004_020adb1c(int);
extern void func_ov004_020b0cac(int, int, int, int, int, short);
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
/* ordinal 54 -- dScMgLuigi_c::Render */
extern int func_ov004_020b1e34(void *c, int a, int b, int d);
/* ordinal 55 -- dScMgLuigi_c::Behavior */
extern Entry_beh data_ov006_02142234[];
/* ordinal 56 -- dScMgLuigi_c::InitResources */
extern void *data_ov006_0213cfa0;
extern int func_ov004_020adc74(void *p);
extern void DecompressLZ16(int src, void *dst);
extern int LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern char *_ZN3G2S12GetBG3ScrPtrEv(void);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void Ov004_Deallocate(int a);
extern int func_02054d88(void);
extern void func_02056314(void *, u32, u32);
extern void func_02056374(const void *, u32, u32);
extern void Deallocate(void *);
extern void func_ov004_020b04d0(int);
extern int data_ov004_020bc888;
extern int data_ov004_020bc864;
/* ordinal 57 -- dScMgLuigi_c_classInit */
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN11dScMgBase_cC2Ev(void *);
extern int data_ov006_0213cf10[];
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 53 -- _ZN12dScMgLuigi_c13OnYoshiTryEatEi, 0x020f3294, size 0x12c */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c13OnYoshiTryEatEi
/* Slot 18 of _ZTV12dScMgLuigi_c -- an override of dScMgBase_c::OnYoshiTryEat(int).
   The signature must repeat the base declaration exactly, or mwcc appends a slot
   instead of overriding. */
void dScMgLuigi_c::OnYoshiTryEat(int arg1)
{
    char *c = (char *)this;

    char *p;
    int *q;

    if (*(unsigned char *)(c + 0x5459) != 0) {
        *(unsigned char *)(c + 0x5457) += 1;

        q = (int *)(c + 0xbc);
        *q += 1;
        if ((unsigned int)*(int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;
    }

    if (arg1 == 0x12) {
        *(short *)(c + 0x5172) = 0xa;
        *(int *)(c + 0xbc) = 0;
        if ((unsigned int)*(int *)(c + 0xbc) > 0x270e)
            *(int *)(c + 0xbc) = 0x270e;

        if (data_ov004_020beb68 != 0)
            *(int *)((char *)data_ov004_020beb68 + 0xb4) = 0;

        p = (char *)data_ov004_020beb68;
        func_ov004_020adb1c(p != 0 ? *(int *)(p + 0xb4) : 0);
    }

    FreeGfxSlotsById(0x1d);
    ResetBoard();
    ChooseBoard();

    *(int *)(c + 0x4f78) = 0;
    data_0209d45c |= 4;
    data_0209d454 &= ~4;

    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 54 -- _ZN12dScMgLuigi_c6RenderEv, 0x020f33c0, size 0x54 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c6RenderEv
/* Slot 9 of _ZTV12dScMgLuigi_c. */
s32 dScMgLuigi_c::Render()
{
    func_ov004_020b1e34(this, 0xe0, 0x14, 1);
    DrawCurtains();
    DrawReward();
    DrawPenalties();
    DrawWantedIcon();
    DrawTimer();
    DrawPictures();
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 55 -- _ZN12dScMgLuigi_c8BehaviorEv, 0x020f3414, size 0x4c */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c8BehaviorEv
/* Slot 6 of _ZTV12dScMgLuigi_c: one dispatch through the per-state table at
   data_ov006_02142234, indexed by unk_4f78. */
s32 dScMgLuigi_c::Behavior()
{
    struct StateView { char pad[0x4f78]; int state; };
    StateView *c = (StateView *)this;
    int j = c->state;
    (((C_beh *)c)->*data_ov006_02142234[j].pmf)();
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 56 -- _ZN12dScMgLuigi_c13InitResourcesEv, 0x020f3460, size 0x3a0 */
/* ------------------------------------------------------------------ */
// @symbol _ZN12dScMgLuigi_c13InitResourcesEv
/* Slot 0 of _ZTV12dScMgLuigi_c. */
s32 dScMgLuigi_c::InitResources()
{
    char *c = (char *)this;
    char *scr;
    /* Two distinct volatile slots, not one reused: the main-screen and
       sub-screen fills are separate stack words in the ROM's frame. */
    volatile u16 fillMain;
    volatile u16 fillSub;
    int arc;
    int file;
    int objChar;

    arc = func_ov004_020adc74(&data_ov006_0213cfa0);
    if (arc == 0) return 0;

    data_0209d45c |= 8;
    *(volatile u16 *)0x0400000e = (*(volatile u16 *)0x0400000e & ~3) | 2;
    *(volatile u16 *)0x0400000e = (*(volatile u16 *)0x0400000e & 0x43) | 0x1218;

    DecompressLZ16(arc, (void *)func_02054d88());

    file = LoadFile(0x48);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)file, 0x60, 0x1a0);
    Deallocate((void *)file);

    file = LoadFile(0x49);
    func_02056314((void *)file, 0, 0x800);
    Deallocate((void *)file);

    data_0209d45c |= 4;
    *(volatile u16 *)0x0400000c &= ~3;
    *(volatile u16 *)0x0400000c &= ~0x40;
    *(volatile u32 *)0x04000018 = 0;
    *(volatile u16 *)0x0400000c = (*(volatile u16 *)0x0400000c & 0x43) | 0x1410;

    file = LoadFile(0x4b);
    DecompressLZ16(file, (void *)(_ZN2G213GetBG2CharPtrEv() + 0x4000));
    Deallocate((void *)file);

    file = LoadFile(0x4c);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)file, 0x40, 0x20);
    Deallocate((void *)file);

    scr = _ZN2G212GetBG2ScrPtrEv();
    fillMain = 0x23e1;
    MultiStore16(fillMain, scr, 0x800);

    objChar = LoadFile(0xd3);
    file = LoadFile(0xd4);
    DecompressLZ16(objChar, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)file, 0, 0x100);

    *(volatile u16 *)0x0400100c &= ~3;
    *(volatile u16 *)0x0400100c &= ~0x40;
    *(volatile u32 *)0x04001018 = 0;
    *(volatile u16 *)0x0400100c = (*(volatile u16 *)0x0400100c & 0x43) | 0x1210;

    {
        int subFile;
        subFile = LoadFile(0x4b);
        DecompressLZ16(subFile, (void *)(_ZN3G2S13GetBG2CharPtrEv() + 0x4000));
        Deallocate((void *)subFile);

        subFile = LoadFile(0x4c);
        _ZN3GXS10LoadBGPlttEPKvjj((const void *)subFile, 0x1e0, 0x20);
        Deallocate((void *)subFile);

        subFile = LoadFile(0x4d);
        func_02056374((const void *)subFile, 0, 0x800);
        Deallocate((void *)subFile);

        data_0209d454 |= 8;
        *(volatile u16 *)0x0400100e &= ~3;
        *(volatile u16 *)0x0400100e = (*(volatile u16 *)0x0400100e & 0x43) | 0x1410;

        subFile = LoadFile(0x4a);
        scr = _ZN3G2S12GetBG3ScrPtrEv();
        fillSub = 0xf3e1;
        MultiStore16(fillSub, scr, 0x800);
        Deallocate((void *)subFile);
    }

    DecompressLZ16(objChar, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)file, 0, 0x100);
    Deallocate((void *)objChar);
    Deallocate((void *)file);

    *(u8 *)(c + 0x5457) = 0;
    ResetBoard();
    *(volatile u16 *)(c + 0x5174) = 0xff;
    ChooseBoard();

    *(int *)(c + 0x4f78) = 0;
    Ov004_Deallocate(arc);
    func_ov004_020b04d0(0x30);
    *(volatile u16 *)(c + 0x5172) = 0xa;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    data_ov004_020bc888 = 0x80;
    data_ov004_020bc864 = ~0x1b;
    *(int *)(c + 0xb4) = 0;
    return 1;
}

/* ------------------------------------------------------------------ */
/* ROM ordinal 57 -- dScMgLuigi_c_classInit, 0x020f3800, size 0x34 */
/* ------------------------------------------------------------------ */
// @symbol dScMgLuigi_c_classInit
extern "C" {
/* The MG_LUIGI factory: allocates 0x545c, runs dScMgBase_c's constructor and
 * installs this class's vtable, data_ov006_0213cf10.
 *
 * Reconstructed source-style name: SM64DS proves dScMgLuigi_c through RTTI,
 * allocation size, vtable identity, and the MG_LUIGI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgWanted_Spawn. */
int *dScMgLuigi_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(21596);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213cf10;
    }
    return p;
}
}

