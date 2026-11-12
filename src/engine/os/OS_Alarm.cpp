//cpp
/* OS alarms (the Nitro SDK's os_alarm.c), arm9 .text 0x02059824..0x02059cb4.
 *
 * pret/pokediamond's NitroSDK OS_alarm.c defines, in this order:
 * OSi_SetTimer, OS_InitAlarm, OS_IsAlarmAvailable, OS_CreateAlarm,
 * OSi_InsertAlarm, OS_SetAlarm, OS_CancelAlarm, OSi_AlarmHandler (asm) and
 * OSi_ArrangeTimer. mwccarm emits an asm function where it stands and
 * defers the C bodies, which come out in reverse. So OSi_AlarmHandler opens
 * the run at 0x02059824, and the C bodies follow from OSi_ArrangeTimer up to
 * OSi_SetTimer at 0x02059c18.
 *
 * Alarms sit in one queue sorted by fire tick. Timer 1 counts down to the
 * head alarm; its IRQ runs OSi_ArrangeTimer, which fires every alarm that is
 * due and re-queues the periodic ones.
 */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"
#include "nitro/os/interrupt_check.h"
#include "nitro/os/alarm.h"

#define REG_TM1CNT_L (*(volatile u16 *)0x04000104)
#define REG_TM1CNT_H (*(volatile u16 *)0x04000106)

#define OSi_TICK_TIMERCONTROL 0xc1 /* enable, IRQ, prescaler /64 */

extern "C" {

typedef struct OSiAlarmQueue {
    OSAlarm *head;
    OSAlarm *tail;
} OSiAlarmQueue;

extern long long OS_GetTick(void);
extern void OSi_SetTimerReserved(int timerNum);

extern u16 OSi_UseAlarm;
extern OSiAlarmQueue OSi_AlarmQueue;

void OSi_AlarmHandler(void *arg);
void OSi_InsertAlarm(OSAlarm *alarm, u64 fire);
void OSi_ArrangeTimer(void);

void OSi_SetTimer(OSAlarm *alarm) {
    long long tick;
    long long delta;
    u16 timerCount;
    tick = OS_GetTick();
    REG_TM1CNT_H = 0;
    delta = alarm->fire - tick;
    OSi_EnterTimerCallback(1, OSi_AlarmHandler, 0);
    if (delta < 0)
        timerCount = 0xfffe;
    else if (delta < 0x10000)
        timerCount = (u16)~(u32)delta;
    else
        timerCount = 0;
    REG_TM1CNT_L = timerCount;
    REG_TM1CNT_H = OSi_TICK_TIMERCONTROL;
    OS_EnableIrqMask(OS_IE_TIMER1);
}

void OS_InitAlarm(void) {
    if (OSi_UseAlarm != 0)
        return;
    OSi_UseAlarm = 1;
    OSi_SetTimerReserved(1);
    OSi_AlarmQueue.head = 0;
    OSi_AlarmQueue.tail = 0;
    OS_DisableIrqMask(OS_IE_TIMER1);
}

int OS_IsAlarmAvailable(void) {
    return OSi_UseAlarm;
}

void OS_CreateAlarm(OSAlarm *alarm) {
    alarm->handler = 0;
    alarm->tag = 0;
}

void OSi_InsertAlarm(OSAlarm *alarm, u64 fire) {
    OSAlarm *next;

    if (alarm->period != 0) {
        u64 tick = OS_GetTick();
        fire = alarm->start;
        if (fire < tick)
            fire = fire + ((tick - fire) / alarm->period + 1) * alarm->period;
    }
    alarm->fire = fire;

    next = OSi_AlarmQueue.head;
    if (next != 0) {
        do {
            if ((long long)(fire - next->fire) < 0) {
                alarm->prev = next->prev;
                next->prev = alarm;
                alarm->next = next;
                if (alarm->prev) {
                    alarm->prev->next = alarm;
                    return;
                }
                OSi_AlarmQueue.head = alarm;
                OSi_SetTimer(alarm);
                return;
            }
            next = next->next;
        } while (next);
    }

    alarm->next = 0;
    {
        OSAlarm *prev = OSi_AlarmQueue.tail;
        OSi_AlarmQueue.tail = alarm;
        alarm->prev = prev;
        if (prev) {
            prev->next = alarm;
            return;
        }
    }
    OSi_AlarmQueue.tail = alarm;
    OSi_AlarmQueue.head = alarm;
    OSi_SetTimer(alarm);
}

void OS_SetAlarm(OSAlarm *alarm, long long tick, OSAlarmHandler handler, void *arg) {
    OSIntrMode saved;
    if (alarm == 0 || alarm->handler != 0)
        OS_Terminate();
    saved = OS_DisableInterrupts();
    alarm->period = 0;
    alarm->handler = handler;
    alarm->arg = arg;
    OSi_InsertAlarm(alarm, OS_GetTick() + tick);
    OS_RestoreInterrupts(saved);
}

void OS_CancelAlarm(OSAlarm *alarm) {
    OSIntrMode saved = OS_DisableInterrupts();
    OSAlarm *next;
    if (alarm->handler == 0) {
        OS_RestoreInterrupts(saved);
        return;
    }
    next = alarm->next;
    if (next == 0)
        OSi_AlarmQueue.tail = alarm->prev;
    else
        next->prev = alarm->prev;
    if (alarm->prev != 0) {
        alarm->prev->next = next;
    } else {
        OSi_AlarmQueue.head = next;
        if (next != 0)
            OSi_SetTimer(next);
    }
    alarm->handler = 0;
    alarm->period = 0;
    OS_RestoreInterrupts(saved);
}

/* The timer 1 IRQ entry. It pushes lr alone and calls out with no stack pad,
 * which mwccarm never emits for C: the SDK writes this one as asm. */
#ifndef SM64DS_PLATFORM_PC
asm void OSi_AlarmHandler(void *arg) {
    stmdb sp!, {lr}
    bl OSi_ArrangeTimer
    ldmia sp!, {lr}
    bx lr
}
#else
void OSi_AlarmHandler(void *arg);
#endif

void OSi_ArrangeTimer(void) {
    OSAlarm *alarm;
    OSAlarmHandler handler;
    OSAlarm *next;
    u64 tick;
    REG_TM1CNT_H = 0;
    OS_DisableIrqMask(OS_IE_TIMER1);
    OS_SetIrqCheckFlag(OS_IE_TIMER1);
    tick = OS_GetTick();
    alarm = OSi_AlarmQueue.head;
    if (alarm == 0)
        return;
    if (tick < alarm->fire) {
        OSi_SetTimer(alarm);
        return;
    }
    next = alarm->next;
    OSi_AlarmQueue.head = next;
    if (next == 0)
        OSi_AlarmQueue.tail = 0;
    else
        next->prev = 0;
    handler = alarm->handler;
    if (alarm->period == 0)
        alarm->handler = 0;
    if (handler != 0)
        handler(alarm->arg);
    if (alarm->period != 0) {
        alarm->handler = handler;
        OSi_InsertAlarm(alarm, 0);
    }
    if (OSi_AlarmQueue.head != 0)
        OSi_SetTimer(OSi_AlarmQueue.head);
}
}
