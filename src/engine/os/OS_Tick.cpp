//cpp
/* OS tick counter (the Nitro SDK's os_tick.c), arm9 .text
 * 0x02059640..0x02059824.
 *
 * pret/pokediamond's NitroSDK OS_tick.c defines, in this order: OS_InitTick,
 * OS_IsTickAvailable, OSi_CountUpTick, OS_GetTick and OS_GetTickLo. mwccarm
 * defers C bodies and emits them in reverse, so the ROM runs bottom-up from
 * OS_GetTickLo at 0x02059640 to OS_InitTick at 0x02059788. Nothing calls
 * OS_IsTickAvailable, so the linker dropped it.
 *
 * Timer 0 counts at the system clock / 64. Its overflow IRQ increments the
 * 48-bit high part, OSi_TickCounter, and a tick is (high << 16) | TM0CNT_L.
 */
#include "types.h"
#include "nitro/os/system.h"
#include "nitro/os/interrupt.h"
#include "nitro/hw/registers.h"

#define REG_TM0CNT_H (*(volatile u16 *)0x04000102)

#define OSi_TICK_TIMERCONTROL 0xc1 /* enable, IRQ, prescaler /64 */

extern "C" {

extern void OSi_SetTimerReserved(int timerNum);

extern u16 data_020a6430;          /* OSi_UseTick */
extern int data_020a6434;          /* OSi_NeedResetTimer */
extern volatile u64 data_020a6438; /* OSi_TickCounter */

void OSi_CountUpTick(void);

void OS_InitTick(void) {
    if (data_020a6430 != 0)
        return;
    data_020a6430 = 1;
    OSi_SetTimerReserved(0);
    data_020a6438 = 0;
    REG_TM0CNT_H = 0;
    REG_TM0CNT_L = 0;
    REG_TM0CNT_H = OSi_TICK_TIMERCONTROL;
    OS_SetIrqFunction(OS_IE_TIMER0, OSi_CountUpTick);
    OS_EnableIrqMask(OS_IE_TIMER0);
    data_020a6434 = 0;
}

void OSi_CountUpTick(void) {
    data_020a6438++;
    if (data_020a6434 != 0) {
        REG_TM0CNT_H = 0;
        REG_TM0CNT_L = 0;
        REG_TM0CNT_H = OSi_TICK_TIMERCONTROL;
        data_020a6434 = 0;
    }
    // OSi_CountUpTick is also the timer 0 OSIrqFunction (void(void)), so it
    // is cast to the callback type here, as in the SDK.
    OSi_EnterTimerCallback(0, (void (*)(void *))OSi_CountUpTick, 0);
}

long long OS_GetTick(void) {
    OSIntrMode saved = OS_DisableInterrupts();
    volatile u16 countL = REG_TM0CNT_L;
    volatile u64 countH = data_020a6438 & 0xffffffffffffULL;
    if ((REG_IF & OS_IE_TIMER0) && !(countL & 0x8000))
        countH = countH + 1;
    OS_RestoreInterrupts(saved);
    return ((long long)countH << 16) | (int)countL;
}

int OS_GetTickLo(void) {
    return REG_TM0CNT_L;
}
}
