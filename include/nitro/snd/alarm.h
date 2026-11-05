#ifndef NITRO_SND_ALARM_H
#define NITRO_SND_ALARM_H

/*
 * The sound alarm callback type of the Nitro SDK's SND_alarm.c
 * (nitro/snd/common/alarm.h), spelled as pret's SND_alarm.h spells it. The
 * bodies are in src/engine/snd/SND_Alarm.cpp.
 *
 * The parameter is unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

/* Run with the user data SNDi_SetAlarmHandler stored beside it when the
 * ARM7 driver reports the alarm. */
typedef void (*SNDAlarmCallback)(void *);

#endif /* NITRO_SND_ALARM_H */
