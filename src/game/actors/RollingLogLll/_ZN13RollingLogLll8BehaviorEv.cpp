//cpp
// @symbol _ZN13RollingLogLll8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "RollingLogLll.h"
extern "C" {
void func_020393a4(void* p, int v);
unsigned char func_ov080_0212714c(void* a, void* b);
extern int data_ov022_02112c98[];
}

int RollingLogLll::Behavior()
{
  func_020393a4(((char*)this)+0x124, 0x500000);
  return func_ov080_0212714c(((char*)this), data_ov022_02112c98) & 0xff;
}
