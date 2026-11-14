/* func_0201aac8 @ 0x201aac8 (arm9) -- tail-call veneer to OS_CreateAlarm (0x2059ba0).
 * ldr ip, [pc]; bx ip; .word 0x2059ba0
 * recovered: the veneer never touches r0, so whatever this function was called
 * with rides straight into OS_CreateAlarm(alarm). func_0201aad4 passes it the
 * alarm, which is why the empty parameter list this once had was byte-exact and
 * wrong.
 */
#include "private/watchdog_alarm.h"

void func_0201aac8(OSAlarm *alarm) {
    _Z14OS_CreateAlarmP8OSiAlarm(alarm);
}
