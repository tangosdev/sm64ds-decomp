#ifndef PRIVATE_WATCHDOG_ALARM_H
#define PRIVATE_WATCHDOG_ALARM_H

/*
 * The game's watchdog: one file-scope object, data_0209d574, built around a
 * Nitro SDK alarm. __sinit_func_0201a96c.cpp constructs it (func_0201aa18)
 * and registers its destructor (func_0201aaec).
 *
 * func_0201a8b4, the alarm handler, re-arms the alarm for what is left of
 * limit since base, and once it is used up enters the fatal-error spin
 * (func_02013f28) unless input bit 0x2000 is held. func_0201a9fc resets base
 * to the current tick; long jobs call it between steps. func_0201a96c sets
 * suspended and answers the old value, cancelling the alarm while it is set
 * and re-arming it for limit when it is cleared; func_0201a9ec suspends.
 *
 * The alarm goes through a thin layer over the SDK calls: func_0201aad4
 * creates it (func_0201aac8, OS_CreateAlarm) and func_0201aab0 cancels it
 * (func_0201aaa4, OS_CancelAlarm), each answering the alarm; func_0201aa50
 * sets it (OS_SetAlarm) once OS_IsAlarmAvailable says the alarm system is up.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"
#include "nitro/os/alarm.h"

typedef struct WatchdogAlarm {
    OSAlarm alarm; /* 0x00 */
    int unread_2c; /* 0x2c -- zeroed by func_0201aa18, never read */
    u64 limit;     /* 0x30 */
    u64 base;      /* 0x38 */
    u8 suspended;  /* 0x40 */

#ifdef __cplusplus
    WatchdogAlarm();
    ~WatchdogAlarm();
#endif
} WatchdogAlarm;

#ifdef __cplusplus
extern "C" {
#endif

extern WatchdogAlarm data_0209d574;

void func_0201a8b4(void *);
u8 func_0201a96c(WatchdogAlarm *, int);
u8 func_0201a9ec(WatchdogAlarm *);
void func_0201a9fc(WatchdogAlarm *);
WatchdogAlarm *func_0201aa18(WatchdogAlarm *);
WatchdogAlarm *func_0201aaec(WatchdogAlarm *);

void func_0201aa50(OSAlarm *, long long, OSAlarmHandler, void *);
void func_0201aaa4(OSAlarm *);
OSAlarm *func_0201aab0(OSAlarm *);
void func_0201aac8(OSAlarm *);
OSAlarm *func_0201aad4(OSAlarm *);

#ifdef __cplusplus
}
#endif

#endif
