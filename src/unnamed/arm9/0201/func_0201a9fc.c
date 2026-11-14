#include "private/watchdog_alarm.h"

/* Reset the watchdog's base to the current tick. */
extern s64 _Z10OS_GetTickv(void);

void func_0201a9fc(WatchdogAlarm *w) {
    w->base = _Z10OS_GetTickv();
}
