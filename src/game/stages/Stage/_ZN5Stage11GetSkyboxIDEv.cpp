//cpp
/* Stage::GetSkyboxID() at 0x0202a93c. Member, not static: LoadSkybox reaches it
 * with r0 still holding `this` and no argument set up. The body reads the level
 * record through the global at 0x0209f340 rather than through a field, so the
 * bytes cannot show the `this` -- the call site is the evidence. */
#include "Stage.h"

extern "C" { extern int data_0209f340[]; }

int Stage::GetSkyboxID(){
  unsigned int *p=*(unsigned int**)data_0209f340;
  return (p[6] >> 4) & 0x1f;
}
