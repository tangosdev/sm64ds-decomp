#include "private/watchdog_alarm.h"

/* Create the alarm (func_0201aac8, OS_CreateAlarm) and return it. The
   watchdog's constructor func_0201aa18 starts with it. */
OSAlarm *func_0201aad4(OSAlarm *alarm) {
    func_0201aac8(alarm);
    return alarm;
}
