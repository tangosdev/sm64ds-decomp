//cpp
// @symbol _ZN19FirePiranhaPlantBig13OnYoshiTryEatEv
#include "FirePiranhaPlantBig.h"
/* recovered: renamed to Class_Method */
s32 FirePiranhaPlantBig::OnYoshiTryEat() {
    void * c = (void *)this;
    unsigned short v = *(unsigned short*)((char*)c + 0xc);
    int r;
    if (v == 0xfc) r = 1; else r = 0;
    if (r != 0) r = 4; else r = 0;
    return r;
}
