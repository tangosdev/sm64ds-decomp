#include "private/watchdog_alarm.h"

/* Reset the watchdog's base to the current tick. */
extern s64 OS_GetTick(void);

void func_0201a9fc(WatchdogAlarm *w) {
    w->base = OS_GetTick();
}
