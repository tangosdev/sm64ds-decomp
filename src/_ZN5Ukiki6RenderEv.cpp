//cpp
// @symbol _ZN13RollingLogTtm6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "RollingLogTtm.h"
extern "C" {
extern int _ZN5Model6RenderEPK7Vector3(void*, void*);
}

int RollingLogTtm::Render()
{
  int b = (unk_0b0 & 0x40000) != 0;
  if(b) return 1;
  _ZN5Model6RenderEPK7Vector3(((char*)this)+0xd4, 0);
  return 1;
}
