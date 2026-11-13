//cpp
#include "types.h"
#include "private/watchdog_alarm.h"
#include "nitro/os/system.h"
#include "nitro/os/alarm.h"



/* Set the suspended flag and return the old one. Clearing it re-arms the
   alarm for the full limit with func_0201a8b4 as the handler; setting it
   cancels the alarm through func_0201aaa4. */
u8 func_0201a96c(WatchdogAlarm *self, int suspended) {
    OSIntrMode saved = OS_DisableInterrupts();
    u8 oldVal = self->suspended;
    self->suspended = suspended;

    if (oldVal != 0 && suspended == 0) {
        func_0201aa50(&self->alarm, self->limit, func_0201a8b4, self);
    } else if (oldVal == 0 && suspended != 0) {
        func_0201aaa4(&self->alarm);
    }

    OS_RestoreInterrupts(saved);
    return oldVal;
}


/* File-scope watchdog. mwcc emits __sinit_func_0201a96c.cpp: the
   constructor calls func_0201aa18, then registers the destructor.
   ~WatchdogAlarm is not defined here. It is aliased onto func_0201aaec,
   which stays in func_0201aaec.c. The registration node is the 12 bytes
   at data_0209d528. The main loop's 32-entry ring data_0209d534 sits
   between that node and the alarm, so this TU's one .bss run owns it
   too. The alarm is 0x44 bytes; 0x0209d5b8 is the next symbol. */
inline WatchdogAlarm::WatchdogAlarm() {
    func_0201aa18(this);
}

typedef char WatchdogAlarm_must_be_0x44[sizeof(WatchdogAlarm) == 0x44 ? 1 : -1];

unsigned short data_0209d534[32];
WatchdogAlarm data_0209d574;
