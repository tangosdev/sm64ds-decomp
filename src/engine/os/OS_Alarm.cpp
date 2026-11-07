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
#include "IRQ.h"
#include "nitro/os/alarm.h"

#define REG_TM1CNT_L (*(volatile u16 *)0x04000104)
#define REG_TM1CNT_H (*(volatile u16 *)0x04000106)

#define OS_IE_TIMER1 0x10
#define OSi_TICK_TIMERCONTROL 0xc1 /* enable, IRQ, prescaler /64 */

extern "C" {

struct OSAlarm {
    OSAlarmHandler handler; /* 0x00 */
    void *arg;              /* 0x04 */
    u32 tag;                /* 0x08 */
    u64 fire;               /* 0x0c */
    OSAlarm *prev;          /* 0x14 */
    OSAlarm *next;          /* 0x18 */
    u64 period;             /* 0x1c */
    u64 start;              /* 0x24 */
};

typedef struct OSiAlarmQueue {
    OSAlarm *head;
    OSAlarm *tail;
} OSiAlarmQueue;

extern long long OS_GetTick(void);
extern void OSi_SetTimerReserved(int timerNum);
extern void OSi_EnterTimerCallback(u32 idx, u32 handler, u32 arg);
extern void Crash(void); /* OS_Terminate */

extern u16 data_020a6440;           /* OSi_UseAlarm */
extern OSiAlarmQueue data_020a6444; /* OSi_AlarmQueue */
extern short data_023c0000[];       /* HW_DTCM */

void OSi_AlarmHandler(void);
void OSi_InsertAlarm(OSAlarm *alarm, u64 fire);
void OSi_ArrangeTimer(void);

void OSi_SetTimer(OSAlarm *alarm) {
    long long tick;
    long long delta;
    u16 timerCount;
    tick = OS_GetTick();
    REG_TM1CNT_H = 0;
    delta = alarm->fire - tick;
    OSi_EnterTimerCallback(1, (u32)OSi_AlarmHandler, 0);
    if (delta < 0)
        timerCount = 0xfffe;
    else if (delta < 0x10000)
        timerCount = (u16)~(u32)delta;
    else
        timerCount = 0;
    REG_TM1CNT_L = timerCount;
    REG_TM1CNT_H = OSi_TICK_TIMERCONTROL;
    IRQ::EnableIRQs(OS_IE_TIMER1);
}

void OS_InitAlarm(void) {
    if (data_020a6440 != 0)
        return;
    data_020a6440 = 1;
    OSi_SetTimerReserved(1);
    data_020a6444.head = 0;
    data_020a6444.tail = 0;
    IRQ::DisableIRQs(OS_IE_TIMER1);
}

int OS_IsAlarmAvailable(void) {
    return data_020a6440;
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

    next = data_020a6444.head;
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
                data_020a6444.head = alarm;
                OSi_SetTimer(alarm);
                return;
            }
            next = next->next;
        } while (next);
    }

    alarm->next = 0;
    {
        OSAlarm *prev = data_020a6444.tail;
        data_020a6444.tail = alarm;
        alarm->prev = prev;
        if (prev) {
            prev->next = alarm;
            return;
        }
    }
    data_020a6444.tail = alarm;
    data_020a6444.head = alarm;
    OSi_SetTimer(alarm);
}

void OS_SetAlarm(OSAlarm *alarm, long long tick, OSAlarmHandler handler, void *arg) {
    u32 saved;
    if (alarm == 0 || alarm->handler != 0)
        Crash();
    saved = IRQ::Disable();
    alarm->period = 0;
    alarm->handler = handler;
    alarm->arg = arg;
    OSi_InsertAlarm(alarm, OS_GetTick() + tick);
    IRQ::Restore(saved);
}

void OS_CancelAlarm(OSAlarm *alarm) {
    u32 saved = IRQ::Disable();
    OSAlarm *next;
    if (alarm->handler == 0) {
        IRQ::Restore(saved);
        return;
    }
    next = alarm->next;
    if (next == 0)
        data_020a6444.tail = alarm->prev;
    else
        next->prev = alarm->prev;
    if (alarm->prev != 0) {
        alarm->prev->next = next;
    } else {
        data_020a6444.head = next;
        if (next != 0)
            OSi_SetTimer(next);
    }
    alarm->handler = 0;
    alarm->period = 0;
    IRQ::Restore(saved);
}

/* The timer 1 IRQ entry. It pushes lr alone and calls out with no stack pad,
 * which mwccarm never emits for C: the SDK writes this one as asm. */
asm void OSi_AlarmHandler(void) {
    stmdb sp!, {lr}
    bl OSi_ArrangeTimer
    ldmia sp!, {lr}
    bx lr
}

void OSi_ArrangeTimer(void) {
    OSAlarm *alarm;
    OSAlarmHandler handler;
    OSAlarm *next;
    u64 tick;
    REG_TM1CNT_H = 0;
    IRQ::DisableIRQs(OS_IE_TIMER1);
    /* OS_SetIrqCheckFlag: HW_INTR_CHECK_BUF, DTCM + 0x3ff8. */
    *(int *)((int)data_023c0000 + 0x3ff8) |= OS_IE_TIMER1;
    tick = OS_GetTick();
    alarm = data_020a6444.head;
    if (alarm == 0)
        return;
    if (tick < alarm->fire) {
        OSi_SetTimer(alarm);
        return;
    }
    next = alarm->next;
    data_020a6444.head = next;
    if (next == 0)
        data_020a6444.tail = 0;
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
    if (data_020a6444.head != 0)
        OSi_SetTimer(data_020a6444.head);
}
}
