//cpp
/* Sound alarm callbacks (the Nitro SDK's SND_alarm.c), arm9 .text
 * 0x0205b470..0x0205b554.
 *
 * The ARM7 driver's alarms call back into the ARM9 through the sound PXI
 * FIFO with the alarm index in the low byte and its id in the next.
 * SNDi_SetAlarmHandler installs a callback and bumps the id, and
 * SNDi_IncAlarmId bumps it when an alarm stops. SNDi_CallAlarmHandler runs
 * the callback only if the id still matches, so a late message from a
 * stopped alarm is dropped.
 *
 * pret's pokediamond SND_alarm.c defines these four functions in this
 * order. mwccarm emits the bodies in reverse, so the ROM runs from
 * SNDi_CallAlarmHandler at 0x0205b470 up to SND_AlarmInit at 0x0205b524.
 */
#include "types.h"
#include "nitro/snd/alarm.h"

#define SND_ALARM_COUNT 8

typedef struct AlarmCallback {
    SNDAlarmCallback cb; /* 0x0 */
    void *data;          /* 0x4 */
    u8 id;               /* 0x8 */
    u8 padding[3];       /* 0x9 */
} AlarmCallback;

extern AlarmCallback data_020a7f60[SND_ALARM_COUNT];
/* sCallbackTable */

void SND_AlarmInit(void) {
    for (s32 i = 0; i < SND_ALARM_COUNT; i++) {
        data_020a7f60[i].cb = 0;
        data_020a7f60[i].data = 0;
        data_020a7f60[i].id = 0;
    }
}

void SNDi_IncAlarmId(u32 idx) {
    AlarmCallback *ac = &data_020a7f60[idx];
    ac->id++;
}

u8 SNDi_SetAlarmHandler(u32 idx, SNDAlarmCallback cb, void *data) {
    AlarmCallback *ac = &data_020a7f60[idx];
    ac->cb = cb;
    ac->data = data;
    ac->id++;
    return ac->id;
}

void SNDi_CallAlarmHandler(s32 idx) {
    AlarmCallback *ac = &data_020a7f60[idx & 0xff];
    if (((idx >> 8) & 0xff) != ac->id)
        return;
    if (ac->cb == 0)
        return;
    ac->cb(ac->data);
}
