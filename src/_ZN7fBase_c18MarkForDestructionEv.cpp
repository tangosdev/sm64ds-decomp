//cpp
// @symbol _ZN7fBase_c18MarkForDestructionEv
/* recovered: named members + shared header, real C++ method */
#include "fBase_c.h"


void fBase_c::MarkForDestruction()
{
  if(shouldBeKilled!=0) return;
  unsigned char b = (aliveState==2);
  if(b!=0) return;
  shouldBeKilled=1;
  OnPendingDestroy();   /* vtable+0x30 = slot 12 */
}
