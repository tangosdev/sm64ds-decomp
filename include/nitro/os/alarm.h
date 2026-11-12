#ifndef NITRO_OS_ALARM_H
#define NITRO_OS_ALARM_H

/*
 * The alarms of the Nitro SDK's os_alarm.c, under the SDK's own names
 * (pret pokediamond's OS_alarm_shared.h for struct OSiAlarm,
 * OS_thread_shared.h for its OSAlarm typedef, and
 * arm9/lib/NitroSDK/include/OS_alarm.h for the calls). The typedef is
 * written on the struct here, not ahead of it as the SDK does. The bodies
 * are in src/engine/os/OS_Alarm.cpp.
 *
 * The SDK's OSTick is a u64. OS_SetAlarm's tick is spelled long long and
 * OS_IsAlarmAvailable answers int (the SDK's BOOL), as OS_Alarm.cpp
 * defines them.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Called with the alarm's arg when the alarm fires. */
typedef void (*OSAlarmHandler)(void *);

/* One alarm in the queue OS_Alarm.cpp keeps sorted by fire tick. handler is
 * 0 while the alarm is not set; a nonzero period re-queues it from start. */
typedef struct OSiAlarm {
    OSAlarmHandler handler; /* 0x00 */
    void *arg;              /* 0x04 */
    u32 tag;                /* 0x08 */
    u64 fire;               /* 0x0c */
    struct OSiAlarm *prev;  /* 0x14 */
    struct OSiAlarm *next;  /* 0x18 */
    u64 period;             /* 0x1c */
    u64 start;              /* 0x24 */
} OSAlarm;

void OS_InitAlarm(void);
int OS_IsAlarmAvailable(void);
void OS_CreateAlarm(OSAlarm *);
void OS_SetAlarm(OSAlarm *, long long, OSAlarmHandler, void *);
void OS_CancelAlarm(OSAlarm *);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_ALARM_H */
