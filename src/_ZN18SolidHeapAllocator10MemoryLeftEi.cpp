//cpp
// @symbol _ZN18SolidHeapAllocator10MemoryLeftEi
#include "SolidHeapAllocator.h"

/* SolidHeapAllocator::MemoryLeft(int align) at 0x0204eaf0 -- uses `this`.
 *
 * How many bytes a forward allocation at this alignment could still get: round the
 * free region's begin up to `align`, subtract from its end, and clamp at 0. `align`
 * goes through cstd::abs, so the direction convention Allocate uses (negative means
 * backwards) is accepted here and ignored -- the answer is the same either way.
 *
 * THE SCAFFOLDING IS LOad-BEARING AND STAYS VERBATIM. `new_var`/`new_var2`/`new_var3`,
 * the empty `if (!(a - 1u)) {}`, the re-read of begin in the return expression instead
 * of reusing `aligned`, and the `(long long)(int)` round-trips are all register-pressure
 * steering: they are how this recovered source reproduces the ROM's scheduling under
 * -O4,p. Tidying any of them changes the bytes. Only the `this` substitution and the
 * `extern "C"` are new here.
 *
 * `_ZN4cstd3absEi` is cstd::abs(int), a real ROM symbol spelled by hand -- so in a C++
 * TU it MUST be extern "C" or it mangles a second time to a name that exists nowhere.
 * build_pin.verify would still pass: a call is a relocation and match.compare wildcards
 * every relocated word (runbook-reference-repair.md section 1). Only the link sees it.
 */
extern "C" int _ZN4cstd3absEi(int x);

int SolidHeapAllocator::MemoryLeft(int align)
{
    u32 a;
    char *new_var2;
    u32 mask;
    u32 new_var;
    int **new_var3;
    int *fb;
    u32 aligned;
    u32 end;

    a = (u32)_ZN4cstd3absEi(align);
    mask = a - 1u;
    fb = (int *)((char *)this + 0x24);
    aligned = (mask + (u32)*(int *)((long long)(int)((char *)this + 0x24))) & ~(a - 1u);
    end = (u32)fb[1];
    if (!(a - 1u)) {
    }
    new_var = end;
    if (aligned > new_var) {
        return 0;
    }
    new_var2 = (char *)this;
    new_var3 = &fb;
    fb = *new_var3;
    return (int)(end - ((mask + (u32)*(int *)((long long)(int)(new_var2 + 0x24))) & ~(a - 1u)));
}
