#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/cp/context.h"

/* NitroSDK's CPi_RestoreContext: puts back what CP_SaveContext captured, both
   divider operands and its mode, then the square-root parameter and its mode.
   Register stores throughout, the parameter included, with no IME bracket:
   this runs with interrupts already off. */
void CPi_RestoreContext(const CPContext *context) {
    *(volatile u64 *)REG_DIV_NUMER_ADDR = context->div_numer;
    *(volatile u64 *)REG_DIV_DENOM_ADDR = context->div_denom;
    REG_DIVCNT = context->div_mode;
    *(volatile u64 *)REG_SQRT_PARAM_ADDR = context->sqrt;
    REG_SQRTCNT = context->sqrt_mode;
}
