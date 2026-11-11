#include "types.h"
#include "nitro/hw/registers.h"
#include "private/arm_math_state.h"

/* The 64-bit operand registers are copied as word pairs, one LDMIA/STMIA
   each. */
struct Pair {
    u32 a, b;
};

/* Captures the divider's and square-root unit's operands and modes
   (NitroSDK's CP_SaveContext). */
void ARMMathSaveState(ARMMathState *s) {
    *(struct Pair *)&s->div_numer = *(struct Pair *)REG_DIV_NUMER_ADDR;
    *(struct Pair *)&s->div_denom = *(struct Pair *)REG_DIV_DENOM_ADDR;
    s->div_mode = (u16)(REG_DIVCNT & REG_CP_DIVCNT_MODE_MASK);
    *(struct Pair *)&s->sqrt = *(struct Pair *)REG_SQRT_PARAM_ADDR;
    s->sqrt_mode = (u16)(REG_SQRTCNT & REG_CP_SQRTCNT_MODE_MASK);
}
