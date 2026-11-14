/* func_0201aaa4 @ 0x201aaa4 (arm9) -- tail-call veneer to OS_CancelAlarm (0x2059950).
 * ldr ip, [pc]; bx ip; .word 0x2059950
 * The veneer never touches r0: the alarm it is called with (the watchdog's)
 * rides straight into OS_CancelAlarm.
 */
#include "private/watchdog_alarm.h"

void func_0201aaa4(OSAlarm *alarm) {
    _Z14OS_CancelAlarmP8OSiAlarm(alarm);
}
