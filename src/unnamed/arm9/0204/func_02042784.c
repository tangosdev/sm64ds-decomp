/* The OS tick count in microseconds: ticks * 64 * 1000 / (HW_SYSTEM_CLOCK /
 * 1000). A tick is 64 system-clock cycles (OSi_TICK_TIMERCONTROL runs timer
 * 0 at prescaler 64), and HW_SYSTEM_CLOCK is the SDK's 33514000.
 * The quotient goes through _ll_udiv. */
#include "types.h"
#include "nitro/os/tick.h"

#define HW_SYSTEM_CLOCK 33514000

u64 func_02042784(void)
{
    return OS_GetTick() * 64000ULL / (HW_SYSTEM_CLOCK / 1000);
}
