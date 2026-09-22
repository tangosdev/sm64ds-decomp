//cpp
// @symbol _ZN10KoopaShell13OnYoshiTryEatEv
#include "KoopaShell.h"
/* recovered: renamed to Class_Method */
s32 KoopaShell::OnYoshiTryEat() {
    unsigned char * p = (unsigned char *)this;
    return p[0x3c4] == 0 ? 6 : 5;
}
