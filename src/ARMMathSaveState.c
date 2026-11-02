#include "types.h"
#include "private/arm_math_state.h"
struct Pair {
    u32 a, b;
};

void ARMMathSaveState(ARMMathState *s) {
    *(struct Pair *)&s->div_numer = *(struct Pair *)0x4000290;
    *(struct Pair *)&s->div_denom = *(struct Pair *)0x4000298;
    s->div_mode = (u16)(*(volatile u16 *)0x4000280 & 3);
    *(struct Pair *)&s->sqrt = *(struct Pair *)0x40002b8;
    s->sqrt_mode = (u16)(*(volatile u16 *)0x40002b0 & 1);
}
