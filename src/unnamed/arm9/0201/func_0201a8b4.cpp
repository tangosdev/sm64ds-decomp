//cpp
#include "private/watchdog_alarm.h"
#include "FatalSpin.h"

/* The watchdog alarm's handler. While the time since base is under limit it
 * re-arms itself for the rest, provided the rest is at least a millisecond
 * (ticks * 64 / (HW_SYSTEM_CLOCK / 1000), as in func_020427c4). Once the
 * limit is used up it enters the fatal-error spin (func_02013f28), unless
 * input bit 0x2000 is held. */
extern "C" long long OS_GetTick(void);
extern "C" unsigned short func_0203da80(unsigned short x);

extern "C" void func_0201a8b4(void *arg) {
    WatchdogAlarm *w = (WatchdogAlarm *)arg;
    unsigned long long elapsed = (unsigned long long)OS_GetTick() - w->base;

    if (elapsed < w->limit) {
        unsigned long long rest = w->limit - elapsed;
        if ((unsigned int)((rest << 6) / 33514) != 0)
            func_0201aa50(&w->alarm, rest, func_0201a8b4, w);
    } else {
        if (func_0203da80(0x2000) == 0)
            FatalSpin::func_02013f28();
    }
}
