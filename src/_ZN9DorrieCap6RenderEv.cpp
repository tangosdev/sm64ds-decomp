//cpp
// @symbol _ZN9DorrieCap6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "DorrieCap.h"
struct V { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };

int DorrieCap::Render()
{
  unsigned int b = unk_0ef;
  if((b<<30)>>31){
    int buf[3];
    buf[0]=0x2c00; buf[1]=0x2c00; buf[2]=0x2c00;
    ((V*)((char*)&mModel))->g5(buf);
  }
  return 1;
}
