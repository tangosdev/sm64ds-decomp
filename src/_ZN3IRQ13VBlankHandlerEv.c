#include "types.h"
extern int data_0209d514;
extern int data_0208ee44;
extern u8 data_0209d4f0;
extern int data_0209d500;
extern int data_0209d4fc;
extern char data_023c0000[];
extern void OS_WakeupThread(int *p);
extern void func_02019144(void);
extern void func_02019100(void);
void _ZN3IRQ13VBlankHandlerEv(void)
{
  int *new_var;
  data_0209d514 = data_0209d514 + 1;
  if ((data_0209d514 >= data_0208ee44) && (data_0209d4f0 != 0))
  {
    OS_WakeupThread(&data_0209d500);
    data_0209d514 = 0;
    func_02019144();
  }
  new_var = &data_0209d4fc;
  OS_WakeupThread(new_var);
  *(int *)(((int)data_023c0000 + 0x3ff8)) |= 1;
  func_02019100();
}
