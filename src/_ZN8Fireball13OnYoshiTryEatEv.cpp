//cpp
// @symbol _ZN8Fireball13OnYoshiTryEatEv
#include "Fireball.h"
/* recovered: renamed to Class_Method */
s32 Fireball::OnYoshiTryEat() {
    char * p = (char *)this;
    unsigned char b = *(unsigned char *)(p + 0x36d);
    if (b != 0 && b != 4)
        return 5;
    return 0;
}
