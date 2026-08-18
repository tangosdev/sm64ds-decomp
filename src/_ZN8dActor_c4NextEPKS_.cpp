//cpp
// @symbol _ZN5Actor4NextEPKS_
/* recovered: named members + shared header */
#include "Actor.h"
extern "C" {
struct Actor;
extern struct Actor* data_0209b468;
struct Actor* _ZN5Actor4NextEPKS_(struct Actor *self) {
  struct Actor* p;
  if(((struct Actor*)self)) p = *(struct Actor**)((char*)&self->unk_054);
  else  p = *(struct Actor**)&data_0209b468;
  if(p) return *(struct Actor**)((char*)p+8);
  return 0;
}
}
