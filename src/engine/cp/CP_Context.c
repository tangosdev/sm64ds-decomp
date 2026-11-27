/* NitroSDK cp_context.c: save and restore the divider and square-root
 * coprocessor registers. mwccarm emits the bodies in reverse source order. */
#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/cp/context.h"

/* Captures the divider's and square-root unit's operands and modes. */
void CP_SaveContext(CPContext *context) {
    context->div_numer = *(volatile u64 *)REG_DIV_NUMER_ADDR;
    context->div_denom = *(volatile u64 *)REG_DIV_DENOM_ADDR;
    context->div_mode = (u16)(REG_DIVCNT & REG_CP_DIVCNT_MODE_MASK);
    context->sqrt = *(volatile u64 *)REG_SQRT_PARAM_ADDR;
    context->sqrt_mode = (u16)(REG_SQRTCNT & REG_CP_SQRTCNT_MODE_MASK);
}

/* Puts back what CP_SaveContext captured, both divider operands and its mode,
   then the square-root parameter and its mode. */
void CPi_RestoreContext(const CPContext *context) {
    *(volatile u64 *)REG_DIV_NUMER_ADDR = context->div_numer;
    *(volatile u64 *)REG_DIV_DENOM_ADDR = context->div_denom;
    REG_DIVCNT = context->div_mode;
    *(volatile u64 *)REG_SQRT_PARAM_ADDR = context->sqrt;
    REG_SQRTCNT = context->sqrt_mode;
}
