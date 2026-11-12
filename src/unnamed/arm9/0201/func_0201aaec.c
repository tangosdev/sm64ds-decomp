#include "private/watchdog_alarm.h"

/* The watchdog alarm's destructor, which __sinit_func_0201a96c.cpp registers:
   cancel it (func_0201aab0) and return it. */
WatchdogAlarm *func_0201aaec(WatchdogAlarm *w) {
    func_0201aab0(&w->alarm);
    return w;
}
