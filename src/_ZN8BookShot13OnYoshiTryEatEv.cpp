//cpp
// @symbol _ZN8BookShot13OnYoshiTryEatEv
#include "BookShot.h"
/* recovered: renamed to Class_Method */
s32 BookShot::OnYoshiTryEat() {
    char* c = (char*)this;
  unsigned int b = *(unsigned short*)(c+0xc)==0x147; return b ? 2 : 0;
}
