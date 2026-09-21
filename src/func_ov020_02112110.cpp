//cpp
#include "dActor_c.h"

extern "C" {
void func_ov020_02112110(char* c) {
  if (*(unsigned char*)(c+0x108)) {
    int param = *(signed char*)(c+0xcc);
    dActor_c::Spawn(0x122, 2, *(const Vector3*)(c+0x5c), 0, param, -1);
  }
  ((dActor_c*)c)->PoofDust();
  ((dActor_c*)c)->MarkForDestruction();
}
}
