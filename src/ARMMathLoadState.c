#include "types.h"
#include "nitro/hw/registers.h"
#include "private/arm_math_state.h"

/* Puts back what ARMMathSaveState captured: both divider operands and its
   mode, then the square-root parameter and its mode (NitroSDK's
   CPi_RestoreContext). Register stores throughout, the parameter included,
   with no IME bracket: this runs with interrupts already off. */
void ARMMathLoadState(const ARMMathState *s) {
    *(volatile u64 *)REG_DIV_NUMER_ADDR = s->div_numer;
    *(volatile u64 *)REG_DIV_DENOM_ADDR = s->div_denom;
    REG_DIVCNT = s->div_mode;
    *(volatile u64 *)REG_SQRT_PARAM_ADDR = s->sqrt;
    REG_SQRTCNT = s->sqrt_mode;
}
