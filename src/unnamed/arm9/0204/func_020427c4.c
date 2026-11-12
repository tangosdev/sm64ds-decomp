/* The OS tick count in milliseconds: ticks * 64 / (HW_SYSTEM_CLOCK / 1000).
 * A tick is 64 system-clock cycles (OSi_TICK_TIMERCONTROL runs timer 0 at
 * prescaler 64). pret's pokediamond spells the same expression in
 * Timer3CountToMilliSeconds. ShowCrashScreen hands the result to nds_printt
 * with the time template "99h59m59s999".
 * The quotient goes through _ll_udiv. */
#include "types.h"
#include "nitro/os/tick.h"

#define HW_SYSTEM_CLOCK 33514000

u64 func_020427c4(void) {
    return OS_GetTick() * 64ULL / (HW_SYSTEM_CLOCK / 1000);
}
