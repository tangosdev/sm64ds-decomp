//cpp
/* IRQ VBlank setup. func_0201a4e4 arms the handler; VBlankHandler counts
 * frames and wakes the two waiters.
 *
 * leftovers:
 * - Source order is VBlankHandler then func_0201a4e4. Deferred codegen emits
 *   .text backwards, which is the ROM order. Do not reorder.
 * - data_0209d4fc and data_0209d500 are halfwords. OS_WakeupThread takes int *,
 *   so the handler casts their addresses. A word type turns the setup stores
 *   into str.
 * - The globals keep their linker names. Nothing in this file recovers one.
 */

#include "types.h"
#include "IRQ.h"

extern "C" {
extern int data_0209d514;
extern int data_0208ee44;
extern u8 data_0209d4f0;
extern u16 data_0209d500;
extern u16 data_0209d4fc;
extern char data_023c0000[];
extern void OS_WakeupThread(int *p);
extern void func_02019144(void);
extern void func_02019100(void);
extern u8 data_0209d4dc;
}

// 0x0201a534 (0x98)
// @symbol _ZN3IRQ13VBlankHandlerEv
namespace IRQ {

void VBlankHandler(void)
{
  int *waiter;
  data_0209d514 = data_0209d514 + 1;
  if ((data_0209d514 >= data_0208ee44) && (data_0209d4f0 != 0))
  {
    OS_WakeupThread((int *)&data_0209d500);
    data_0209d514 = 0;
    func_02019144();
  }
  waiter = (int *)&data_0209d4fc;
  OS_WakeupThread(waiter);
  *(int *)(((int)data_023c0000 + 0x3ff8)) |= 1;
  func_02019100();
}

}

// 0x0201a4e4 (0x50)
// @symbol func_0201a4e4
extern "C" void func_0201a4e4(void)
{
    data_0209d4fc = 0;
    data_0209d500 = 0;
    IRQ::SetIRQHandler(1, IRQ::VBlankHandler);
    data_0209d4dc = 1;
}
