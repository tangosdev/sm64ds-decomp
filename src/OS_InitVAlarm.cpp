//cpp
/* OS_InitVAlarm (the Nitro SDK's os_valarm.c). This SDK's version keeps no
 * frame counters: it only empties the alarm queue and masks V-count. */
#include "nitro/os/interrupt.h"

extern volatile u16 data_020a644c;
/* OSi_UseVAlarm */
extern u32 data_020a6450[2];
/* OSi_VAlarmQueue: head, tail */

void OS_InitVAlarm(void) {
    if (data_020a644c != 0)
        return;
    u32 *queue = data_020a6450;
    data_020a644c = 1;
    queue[0] = 0;
    queue[1] = 0;
    OS_DisableIrqMask(OS_IE_V_COUNT);
}
