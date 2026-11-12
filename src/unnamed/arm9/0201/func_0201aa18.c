#include "private/watchdog_alarm.h"

/* The watchdog's constructor (private/watchdog_alarm.h): create the alarm
 * (func_0201aad4, which reaches OS_CreateAlarm and clears its handler and
 * tag), start suspended with no limit, and answer the watchdog. */
#ifndef SM64DS_PLATFORM_PC
typedef char
    WatchdogAlarm_suspended_must_be_at_0x40[(int)&((WatchdogAlarm *)0)->suspended == 0x40 ? 1 : -1];
#endif

WatchdogAlarm *func_0201aa18(WatchdogAlarm *w) {
    func_0201aad4(&w->alarm);
    w->limit = 0;
    w->suspended = 1;
    w->unread_2c = 0;
    w->base = 0;
    return w;
}
