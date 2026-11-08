#include "types.h"
/* func_0201a9ec at 0x0201a9ec - thunk: func_0201a96c(self, 1) */
struct WatchdogAlarm;
extern u8 func_0201a96c(struct WatchdogAlarm *context, int suspended);

u8 func_0201a9ec(void* self)
{
    return func_0201a96c(self, 1);
}
