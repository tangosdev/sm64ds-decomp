#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/cp/context.h"

/* NitroSDK's CP_SaveContext: captures the divider's and square-root unit's
   operands and modes. */
void CP_SaveContext(CPContext *context) {
    context->div_numer = *(volatile u64 *)REG_DIV_NUMER_ADDR;
    context->div_denom = *(volatile u64 *)REG_DIV_DENOM_ADDR;
    context->div_mode = (u16)(REG_DIVCNT & REG_CP_DIVCNT_MODE_MASK);
    context->sqrt = *(volatile u64 *)REG_SQRT_PARAM_ADDR;
    context->sqrt_mode = (u16)(REG_SQRTCNT & REG_CP_SQRTCNT_MODE_MASK);
}
