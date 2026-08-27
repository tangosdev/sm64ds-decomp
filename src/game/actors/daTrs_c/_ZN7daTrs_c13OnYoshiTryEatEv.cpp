//cpp
// @symbol _ZN7daTrs_c13OnYoshiTryEatEv
/* daTrs_c::OnYoshiTryEat (slot 18, include/dActor_c.h) - name recovered from the
   vtable slot it fills. Tests the actor-ID "kind" field (fBase_c +0xc) for
   0xd1 -- the same constant daTrs_c::CleanupResources checks -- and returns 7 for
   that specific daTrs_c variant, 0 otherwise. Body unchanged from the recovered
   free function -- only the symbol and self access moved to a real method,
   the same shape as daKrb_c::OnYoshiTryEat (src/_ZN7daKrb_c13OnYoshiTryEatEv.cpp). */
#include "daTrs_c.h"

int daTrs_c::OnYoshiTryEat() {
    unsigned short v = *(unsigned short*)((char*)this + 0xc);
    int r;
    if (v == 0xd1) r = 1; else r = 0;
    if (r != 0) r = 7; else r = 0;
    return r;
}
