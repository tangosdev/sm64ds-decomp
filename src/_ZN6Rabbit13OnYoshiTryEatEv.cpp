//cpp
// @symbol _ZN6Rabbit13OnYoshiTryEatEv
#include "Rabbit.h"
/* recovered: renamed to Class_Method */
s32 Rabbit::OnYoshiTryEat() {
    unsigned char * c = (unsigned char *)this;
  unsigned char v = c[0x107];
  if (v != 0) return 0;
  return 7;
}
