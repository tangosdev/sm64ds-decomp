#include "nitro/os/alarm.h"

struct OSAlarm;

extern int OS_IsAlarmAvailable(void);
extern void OS_SetAlarm(struct OSAlarm *alarm, long long tick, OSAlarmHandler handler, void *arg);

/* OS_SetAlarm, but only once the alarm system is up (OS_IsAlarmAvailable).
   The watchdog alarm (func_0201a8b4, func_0201a96c), which begins with its
   OSAlarm, arms itself through it. */
void func_0201aa50(void *alarm, long long tick, OSAlarmHandler handler, void *arg) {
    if (OS_IsAlarmAvailable() == 0)
        return;
    OS_SetAlarm(alarm, tick, handler, arg);
}
