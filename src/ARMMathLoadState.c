#include "private/arm_math_state.h"

void ARMMathLoadState(const ARMMathState *s) {
    *(volatile u64 *)0x4000290 = s->div_numer;
    *(volatile u64 *)0x4000298 = s->div_denom;
    *(volatile u16 *)0x4000280 = s->div_mode;
    *(volatile u64 *)0x40002b8 = s->sqrt;
    *(volatile u16 *)0x40002b0 = s->sqrt_mode;
}
