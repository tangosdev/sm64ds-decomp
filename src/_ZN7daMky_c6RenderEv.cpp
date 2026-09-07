//cpp
// @symbol _ZN7daMky_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daMky_c.h"
extern "C" {
extern int _ZN5Model6RenderEPK7Vector3(void*, void*);
}

int daMky_c::Render()
{
  int b = (mFlags & 0x40000) != 0;
  if(b) return 1;
  _ZN5Model6RenderEPK7Vector3(((char*)this)+0xd4, 0);
  return 1;
}
