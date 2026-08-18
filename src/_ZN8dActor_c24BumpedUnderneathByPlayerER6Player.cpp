//cpp
// @symbol _ZN8dActor_c24BumpedUnderneathByPlayerER6Player
/* recovered: named members + shared header */
#include "dActor_c.h"
extern "C" {
int _ZN8dActor_c24BumpedUnderneathByPlayerER6Player(struct dActor_c *self, char*player) {
if(*(unsigned char*)(player+0x6de)!=0 && *(int*)(player+0xa8)>0
   && *(int*)(player+0x60) < self->mPosY)
  return 1;
return 0;
}
}
