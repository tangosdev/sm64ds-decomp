//cpp
// @symbol _ZN7daTrs_c16OnAimedAtWithEggEv
/* daTrs_c::OnAimedAtWithEgg (slot 29, include/dActor_c.h) - name recovered from
   the vtable slot it fills. The body is a decompilation verified against the
   ROM, not an inferred stub. Reads a word 8 bytes into
   mdCcAcPos_c (include/daTrs_c.h, +0x184), i.e. inside that
   member's own dCcAc_c base -- not a separate daTrs_c field. Body
   unchanged from the recovered free function -- only the symbol and self
   access moved to a real method, the same shape as daKrb_c::OnAimedAtWithEgg
   (src/_ZN7daKrb_c16OnAimedAtWithEggEv.cpp). */
#include "daTrs_c.h"

int daTrs_c::OnAimedAtWithEgg() {
    return *(int *)((char *)this + 0x18c) / 2;
}
