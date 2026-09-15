//cpp
// @symbol _ZN7daTrs_c13OnYoshiTryEatEv
/* daTrs_c::OnYoshiTryEat (slot 18): the 0xd1 TERESA refuses Yoshi's bite with
   7, anything else (the 0xd2 boss) with 0. Same two-step shape as the sibling
   daKrb_c::OnYoshiTryEat. */
#include "daTrs_c.h"

int daTrs_c::OnYoshiTryEat() {
    unsigned short v = actorID;
    int r;
    if (v == 0xd1) r = 1; else r = 0;
    if (r != 0) r = 7; else r = 0;
    return r;
}
