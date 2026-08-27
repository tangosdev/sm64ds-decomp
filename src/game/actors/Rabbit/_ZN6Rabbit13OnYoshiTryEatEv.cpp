//cpp
// @symbol _ZN6Rabbit13OnYoshiTryEatEv
#include "Rabbit.h"
/* recovered: renamed to Class_Method */
s32 Rabbit::OnYoshiTryEat() {
  unsigned char v = mEatenByYoshi;
  if (v != 0) return 0;
  return 7;
}
