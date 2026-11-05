#ifndef NITRO_OS_ALARM_H
#define NITRO_OS_ALARM_H

/*
 * The alarm handler type of the Nitro SDK's os_alarm.c, under the SDK's own
 * name (nitro/os/common/alarm.h). The bodies are in src/engine/os/OS_Alarm.cpp.
 *
 * The parameter is unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

/* Called with the alarm's arg when the alarm fires. */
typedef void (*OSAlarmHandler)(void *);

#endif /* NITRO_OS_ALARM_H */
