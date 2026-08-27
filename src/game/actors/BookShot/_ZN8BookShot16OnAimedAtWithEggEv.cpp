//cpp
// @symbol _ZN8BookShot16OnAimedAtWithEggEv
#include "BookShot.h"
// recovered name: BookShot_OnAimedAtWithEgg
/* recovered: renamed to Class_Method */
/* daBook_c::OnAimedAtWithEgg - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
extern "C" {
s32 BookShot::OnAimedAtWithEgg() {
    char* c = (char*)this;
  int eq = (*(unsigned short*)(c+0xc)==0x147);
  if(eq) return 0;
  return 0x19000;
}
}
