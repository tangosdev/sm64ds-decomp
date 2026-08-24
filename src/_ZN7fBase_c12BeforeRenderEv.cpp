//cpp
#include "types.h"
// @symbol _ZN7fBase_c12BeforeRenderEv
/* recovered: named members + shared header, real C++ method */
#include "fBase_c.h"
int fBase_c::BeforeRender()
{
  if(shouldBeKilled!=0) goto ret0;
  if((pauseFlags&8)==0) goto ret1;
ret0:
  return 0;
ret1:
  return 1;
}
