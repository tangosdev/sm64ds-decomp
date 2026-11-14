//cpp
/* arm9/cstd3 -- cstd::__assert plus the two card-wait register helpers at
 * .text 0x0206ccd8..0x0206ce90. The run is bounded by func_0206cbc0 ending
 * at 0x0206ccd8 below and an undelinked ROM gap starting at 0x0206ce90
 * above; written in descending address order so reverse-source emission
 * lands the ROM-ascending run.
 *
 * leftovers:
 * - func_0206a928's local extern keeps its 4-parameter spelling even
 *   though src/unnamed/arm9/0206/func_0206a928.c defines it with three
 *   (ctx, fmt, ap): func_0206ccd8's call passes the trailing `p` in r3,
 *   and dropping it diffs.
 * - The debugger bus pokes at 0x9fe2ffe / 0x9fe20f0 stay raw addresses:
 *   these two helpers are their only consumers and no header names them.
 *   The value written there is mirrored in the debugger block
 *   (DebuggerBlock::busMirror, private/debugger_block.h).
 */

#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/hw/mmap_shared.h"
#include "private/debugger_block.h"

extern "C" {
extern const char data_020868a0[52];
extern char data_020868d4;
int func_0206cf7c(int mask);
/* local extern: no header declares it; func_0206ccd8 formats through it */
void func_0206a928(void *a, int b, void *c, void *d);
/* local extern: no header declares it; func_0206ccd8 stores it as the print callback */
void func_0206cbc0(struct Csl *sl, u8 *sb, int count);
}
void func_0206ccd8(int fmt, ...);

struct CardBusState {
    unsigned short exmemcnt;
    unsigned short busMirror;
    unsigned short ready;
};

extern "C" {
/* optimize_for_size is this library's own build setting, not a matching lever:
   the ISD debug console and its soft-float helpers were shipped size-optimized. */
#pragma optimize_for_size on
void func_0206ce20(struct CardBusState* s){
  unsigned short v = REG_EXMEMCNT;
  s->exmemcnt = v;
  REG_EXMEMCNT = (v & 0xffe3) | 0xf;
  if(func_0206cf7c(1) != 0){
    s->ready = 1;
    s->busMirror = ((struct DebuggerBlock *)HW_ISD_RESERVED)->busMirror;
    *(volatile unsigned short*)0x9fe2ffe = 0x20;
    ((struct DebuggerBlock *)HW_ISD_RESERVED)->busMirror = 0x20;
  }
}
}

extern "C" {
#pragma optimize_for_size on
void func_0206cd9c(struct CardBusState *s)
{
    volatile unsigned short scratch;

    if (func_0206cf7c(1) != 0 && s->ready != 0) {
        volatile unsigned short *mirror;
        volatile unsigned short *busReg;
        unsigned short v2;

        mirror = &((struct DebuggerBlock *)HW_ISD_RESERVED)->busMirror;
        busReg = (volatile unsigned short *)0x9fe2ffe;
        v2 = s->busMirror;
        *mirror = v2;
        v2 = s->busMirror;
        *busReg = v2;
        scratch = *(volatile unsigned short *)0x9fe20f0;
        scratch = *(volatile unsigned short *)0x9fe20f0;
        scratch = *(volatile unsigned short*)0x9fe20f0;
    }
    REG_EXMEMCNT = s->exmemcnt;
}
}

namespace cstd {

void __builtin_trap();

// The IS-NITRO debugger library's assert, the twin of the IS-AGB library's
// AGBAssert (pret's pokeemerald/src/libisagbprn.c) down to its two format
// strings: with stop set, print "ASSERTION FAILED  FILE=[%s] LINE=[%d]
// EXP=[%s]" (data_020868a0) and trap forever; otherwise print the "WARING"
// line (data_020868d4) and return. isdoverlay.c is its only caller.
void __assert(const char *file, int line, const char *exp, int stop) {
    if (stop) {
        func_0206ccd8((int)&data_020868a0, file, line, exp);
        __builtin_trap();
        for (;;) {
        }
    } else {
        func_0206ccd8((int)&data_020868d4, file, line, exp);
    }
}
}

/* The print helper __assert calls: card-wait gate, then formats the varargs
 * through func_0206a928 with a print-argument struct and the func_0206cbc0
 * console callback. The print struct stays function-local. Lowest address
 * in the TU, so it goes last for reverse-source emission. */
void func_0206ccd8(int fmt, ...) {
    struct S { int f0, f1, f2, f3; void *cb; };
    S s;
    int *p;

    if (func_0206cf7c(0x20000) != 0) {
        s.f0 = 0;
        s.f3 = 0;
        s.f1 = -1;
        s.cb = (void *)func_0206cbc0;

        p = (int *)(((int)&fmt) & ~3) + 1;
        func_0206a928(&s, fmt, &p, p);
    }
}
