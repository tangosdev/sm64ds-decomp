#include "private/watchdog_alarm.h"

/* Cancel the alarm (func_0201aaa4, OS_CancelAlarm) and return it. */
OSAlarm *func_0201aab0(OSAlarm *alarm) {
    func_0201aaa4(alarm);
    return alarm;
}
