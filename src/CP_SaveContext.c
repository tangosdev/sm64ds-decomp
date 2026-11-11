#include "types.h"
#include "nitro/hw/registers.h"
#include "nitro/cp/context.h"

/* The 64-bit operand registers are copied as word pairs, one LDMIA/STMIA
   each. */
struct Pair {
    u32 a, b;
};

/* NitroSDK's CP_SaveContext: captures the divider's and square-root unit's
   operands and modes. */
void CP_SaveContext(CPContext *context) {
    *(struct Pair *)&context->div_numer = *(struct Pair *)REG_DIV_NUMER_ADDR;
    *(struct Pair *)&context->div_denom = *(struct Pair *)REG_DIV_DENOM_ADDR;
    context->div_mode = (u16)(REG_DIVCNT & REG_CP_DIVCNT_MODE_MASK);
    *(struct Pair *)&context->sqrt = *(struct Pair *)REG_SQRT_PARAM_ADDR;
    context->sqrt_mode = (u16)(REG_SQRTCNT & REG_CP_SQRTCNT_MODE_MASK);
}
