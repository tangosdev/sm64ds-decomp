//cpp
// @symbol _ZN13SnowmanBreath6RenderEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SnowmanBreath.h"
extern unsigned char data_0209f2d8[];

int SnowmanBreath::Render()
{
  int b = data_0209f2d8[0];
  b = (b == 2);
  if(b) return 1;
  {
    int i;
    char* p = ((char*)this) + 0xd4;
    for(i=0;i<0x32;i++){
      data_ov043_02112424(p);
      p += 0x60;
    }
  }
  return 1;
}
