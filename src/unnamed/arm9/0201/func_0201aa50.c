#include "private/watchdog_alarm.h"

/* OS_SetAlarm, but only once the alarm system is up (OS_IsAlarmAvailable).
   The watchdog (func_0201a8b4, func_0201a96c) arms its alarm through it. */
void func_0201aa50(OSAlarm *alarm, long long tick, OSAlarmHandler handler, void *arg) {
    if (OS_IsAlarmAvailable() == 0)
        return;
    OS_SetAlarm(alarm, tick, handler, arg);
}
