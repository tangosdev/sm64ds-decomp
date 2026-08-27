//cpp
// @symbol _ZN5Ukiki6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Ukiki.h"
extern "C" {
extern int _ZN5Model6RenderEPK7Vector3(void*, void*);
}

int Ukiki::Render()
{
  int b = (mFlags & 0x40000) != 0;
  if(b) return 1;
  _ZN5Model6RenderEPK7Vector3(((char*)this)+0xd4, 0);
  return 1;
}
