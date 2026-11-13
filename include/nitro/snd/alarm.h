#ifndef NITRO_SND_ALARM_H
#define NITRO_SND_ALARM_H
#ifndef __cplusplus
#define SND_AlarmInit _Z13SND_AlarmInitv
#endif

/*
 * The sound alarm callback type of the Nitro SDK's SND_alarm.c
 * (nitro/snd/common/alarm.h), spelled as pret's SND_alarm.h spells it. The
 * bodies are in src/engine/snd/SND_Alarm.cpp. SND_AlarmInit is the SDK's
 * (ntrtwl/NitroSDK nitro/snd/common/alarm.h); SND_Init runs it once.
 *
 * The parameter is unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

/* Run with the user data SNDi_SetAlarmHandler stored beside it when the
 * ARM7 driver reports the alarm. */
typedef void (*SNDAlarmCallback)(void *);


void SND_AlarmInit(void);


#endif /* NITRO_SND_ALARM_H */
