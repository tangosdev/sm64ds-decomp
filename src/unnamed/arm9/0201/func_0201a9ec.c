#include "private/watchdog_alarm.h"

/* Suspend the watchdog (func_0201a96c) and answer whether it already was. */
u8 func_0201a9ec(WatchdogAlarm *w) {
    return _Z13func_0201a96cP13WatchdogAlarmi(w, 1);
}
