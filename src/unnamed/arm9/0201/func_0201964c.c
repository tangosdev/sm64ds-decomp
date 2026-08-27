#include "types.h"
/* func_0201964c at 0x0201964c
 * Timer constructor: calls ResetTimer() then returns this.
 * Likely Timer::Timer() C1 constructor.
 */
struct Timer {
    u32 field0;
    u32 field4;
    u8 field8;
};

extern void _ZN5Timer10ResetTimerEv(struct Timer *);

struct Timer *func_0201964c(struct Timer *__this) {
    _ZN5Timer10ResetTimerEv(__this);
    return __this;
}
