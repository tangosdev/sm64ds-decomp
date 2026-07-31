//cpp
// @symbol _ZN5Actor24BumpedUnderneathByPlayerER6Player
/* recovered: named members + shared header */
#include "Actor.h"
extern "C" {
int _ZN5Actor24BumpedUnderneathByPlayerER6Player(struct Actor *self, char*player) {
if(*(unsigned char*)(player+0x6de)!=0 && *(int*)(player+0xa8)>0
   && *(int*)(player+0x60) < self->mPosY)
  return 1;
return 0;
}
}
