//cpp
/* IRQ VBlank setup and the two VBlank waits. func_0201a4e4 arms the
 * handler; VBlankHandler counts frames and wakes the two thread queues;
 * func_0201a4d0 and func_0201a4bc are the threads' side, sleeping on them.
 *
 * - data_0209d4fc is woken on every VBlank: func_0201a4d0 sleeps on it.
 * - data_0209d500 is woken every data_0208ee44 VBlanks while data_0209d4f0
 *   is up: func_0201a4bc, the main loop's frame wait, sleeps on it.
 *
 * Edges: the two queues are referenced by these four functions and nothing
 * else in arm9. func_0201a490 before the run calls a hook on another
 * object; func_0201a5cc after it installs the scene's actor create hooks.
 *
 * leftovers:
 * - Deferred codegen emits .text backwards, so the file is written highest
 *   address first and comes out in ROM order. Do not reorder.
 * - The queues are OSThreadQueue halfwords (nitro/os/thread.h); a word type
 *   turns the setup stores into str.
 * - The globals keep their linker names. Nothing in this file recovers one.
 */

#include "types.h"
#include "nitro/os/interrupt.h"
#include "nitro/os/interrupt_check.h"
#include "nitro/os/thread.h"

extern int data_0209d514;
extern int data_0208ee44;
extern u8 data_0209d4f0;
extern u16 data_0209d500;
extern u16 data_0209d4fc;
extern void func_02019144(void);
extern "C" {

extern void func_02019100(void);
}

extern u8 data_0209d4dc;

// 0x0201a534 (0x98)
namespace IRQ {

void VBlankHandler(void) {
    u16 *waiter;
    data_0209d514 = data_0209d514 + 1;
    if ((data_0209d514 >= data_0208ee44) && (data_0209d4f0 != 0)) {
        OS_WakeupThread(&data_0209d500);
        data_0209d514 = 0;
        func_02019144();
    }
    waiter = &data_0209d4fc;
    OS_WakeupThread(waiter);
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
    func_02019100();
}
}

// 0x0201a4e4 (0x50)
void func_0201a4e4(void) {
    data_0209d4fc = 0;
    data_0209d500 = 0;
    OS_SetIrqFunction(OS_IE_V_BLANK, IRQ::VBlankHandler);
    data_0209d4dc = 1;
}

// 0x0201a4d0 (0x14)
/* Sleep until the next VBlank. */
void func_0201a4d0(void) {
    OS_SleepThread(&data_0209d4fc);
}

// 0x0201a4bc (0x14)
/* The main loops' frame wait (func_020197b8 and func_020199a4 raise
 * data_0209d4f0 around it). */
void func_0201a4bc(void) {
    OS_SleepThread(&data_0209d500);
}
