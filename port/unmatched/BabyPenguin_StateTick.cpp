/* HOST COPY of src/func_ov072_02121cdc.cpp -- the "tick" half of
 * BabyPenguin's state dispatch (see BabyPenguin_StateEnter.cpp's header
 * for the full PMF-disease derivation; this is the same pp+1 shape
 * MrBlizzard_Behavior.cpp already uses for the analogous gate-192 seam).
 * Called every frame from _ZN11BabyPenguin8BehaviorEv.cpp's else-branch
 * (matched src, stays in the slice -- it only calls this function by
 * name, it does not dispatch the PMF itself).
 */
#include <cstddef>   /* size_t (pointer-width casts) */
extern "C" {
struct PortBabyPenguinPair { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
typedef void (*PortBpFn)(void *);

/* No null guard, matching the matched src exactly (see
 * BabyPenguin_StateEnter.cpp's header). */
void func_ov072_02121cdc(char *c)
{
    PortBabyPenguinPair *p = *(PortBabyPenguinPair **)(c + 0x35c);
    ((PortBpFn)(size_t)p->tick_fn)(c);
}
}
