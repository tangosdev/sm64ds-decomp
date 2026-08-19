//cpp
// @symbol _ZN3Boo16OnAimedAtWithEggEv
/* Boo::OnAimedAtWithEgg (slot 29, include/dActor_c.h) - name recovered from
   the vtable slot it fills. The body is a decompilation verified against the
   ROM, not an inferred stub. Reads a word 8 bytes into
   mMovingCylinderClsnWithPos (include/Boo.h, +0x184), i.e. inside that
   member's own MovingCylinderClsn base -- not a separate Boo field. Body
   unchanged from the recovered free function -- only the symbol and self
   access moved to a real method, the same shape as Goomba::OnAimedAtWithEgg
   (src/_ZN6Goomba16OnAimedAtWithEggEv.cpp). */
#include "Boo.h"

int Boo::OnAimedAtWithEgg() {
    return *(int *)((char *)this + 0x18c) / 2;
}
