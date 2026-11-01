//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

/* The watchdog alarm at data_0209d574; layout and names are func_0201aa18's. */
typedef struct {
    char alarm[0x2c]; /* 0x00 -- OSAlarm */
    char unread_2c[4];
    u64 limit;      /* 0x30 */
    char base[0x8]; /* 0x38 -- u64 tick, reset by func_0201a9fc */
    u8 suspended;   /* 0x40 */
} Struct_0201a96c;

typedef void (*OSAlarmHandler)(void *);
extern void func_0201aa50(void *, long long, OSAlarmHandler, void *);
extern void func_0201aaa4(void *);

extern void func_0201a8b4(void *);

/* Set the suspended flag and return the old one. Clearing it re-arms the
   alarm for the full limit with func_0201a8b4 as the handler; setting it
   cancels the alarm through func_0201aaa4. */
u8 func_0201a96c(Struct_0201a96c *self, u8 newVal) {
    u32 saved = IRQ::Disable();
    u8 oldVal = self->suspended;
    self->suspended = newVal;

    if (oldVal != 0 && newVal == 0) {
        func_0201aa50(self, self->limit, func_0201a8b4, self);
    } else if (oldVal == 0 && newVal != 0) {
        func_0201aaa4(self);
    }

    IRQ::Restore(saved);
    return oldVal;
}
}

/* File-scope watchdog. mwcc emits __sinit_func_0201a96c.cpp: the
   constructor calls func_0201aa18, then registers the destructor.
   ~WatchdogAlarm is not defined here. It is aliased onto func_0201aaec,
   which stays in func_0201aaec.c. The registration node is the 12 bytes
   at data_0209d528. The main loop's 32-entry ring data_0209d534 sits
   between that node and the alarm, so this TU's one .bss run owns it
   too. The alarm is 0x44 bytes; 0x0209d5b8 is the next symbol. */
extern "C" void *func_0201aa18(char *alarm);

struct WatchdogAlarm {
    unsigned char raw[0x44];

    WatchdogAlarm() {
        func_0201aa18((char *)this);
    }
    ~WatchdogAlarm();
};

typedef char WatchdogAlarm_must_be_0x44[sizeof(WatchdogAlarm) == 0x44 ? 1 : -1];

unsigned short data_0209d534[32];
WatchdogAlarm data_0209d574;
