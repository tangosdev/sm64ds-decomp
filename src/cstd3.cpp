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
 * - The card-ROM bus pokes at 0x9fe2ffe / 0x9fe20f0 and the WRAM latency
 *   mirror at 0x27fff72 stay raw addresses: func_0206cf7c's card-wait gate
 *   is the only consumer and no header names them.
 */

#include "types.h"
#include "nitro/hw/registers.h"

extern "C" {
extern char data_020868a0;
extern char data_020868d4;
void func_0206ccd8(int fmt, ...);
int func_0206cf7c(int mask);
/* local extern: no header declares it; func_0206ccd8 formats through it */
void func_0206a928(void *a, int b, void *c, void *d);
/* local extern: no header declares it; func_0206ccd8 stores it as the print callback */
void func_0206cbc0(void);
}

struct CardBusState {
    unsigned short exmemcnt;
    unsigned short latency;
    unsigned short ready;
};

extern "C" {
#pragma optimize_for_size on
// @symbol func_0206ce20
void func_0206ce20(struct CardBusState* s){
  unsigned short v = REG_EXMEMCNT;
  s->exmemcnt = v;
  REG_EXMEMCNT = (v & 0xffe3) | 0xf;
  if(func_0206cf7c(1) != 0){
    s->ready = 1;
    s->latency = *(volatile unsigned short*)0x27fff72;
    *(volatile unsigned short*)0x9fe2ffe = 0x20;
    *(volatile unsigned short*)0x27fff72 = 0x20;
  }
}
}

extern "C" {
#pragma optimize_for_size on
#pragma opt_common_subs off
// @symbol func_0206cd9c
void func_0206cd9c(struct CardBusState *s)
{
    volatile unsigned short scratch;

    if (func_0206cf7c(1) != 0 && s->ready != 0) {
        volatile unsigned short *p27;
        volatile unsigned short *p9fe;
        unsigned short v2;

        p27 = (volatile unsigned short *)0x27fff72;
        p9fe = (volatile unsigned short *)0x9fe2ffe;
        v2 = s->latency;
        *p27 = v2;
        v2 = s->latency;
        *p9fe = v2;
        scratch = *(volatile unsigned short *)0x9fe20f0;
        scratch = *(volatile unsigned short *)0x9fe20f0;
        scratch = *(volatile unsigned short*)0x9fe20f0;
    }
    REG_EXMEMCNT = s->exmemcnt;
}
}

namespace cstd {

void __builtin_trap();

// @symbol _ZN4cstd8__assertEPKcS1_S1_i
// Assertion failure path for the game's cstd helpers: with a nonzero `line`
// print through the data_020868a0 format and trap forever; with zero, print
// through data_020868d4 and return.
void __assert(const char* file, const char* expr, const char* func, int line)
{
    if (line) {
        func_0206ccd8((int)&data_020868a0, file, expr, func);
        __builtin_trap();
        for (;;) {
        }
    } else {
        func_0206ccd8((int)&data_020868d4, file, expr, func);
    }
}

}

/* The print helper __assert calls: card-wait gate, then formats the varargs
 * through func_0206a928 with a print-argument struct and the func_0206cbc0
 * console callback. The print struct stays function-local and the pragmas
 * stay scoped push/pop so opt_common_subs is restored on, undoing the off
 * state the register helper above leaks. Lowest address in the TU, so it
 * goes last for reverse-source emission. */
#pragma push
#pragma optimize_for_size on
#pragma opt_common_subs on
// @symbol func_0206ccd8
extern "C" void func_0206ccd8(int fmt, ...)
{
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
#pragma pop
