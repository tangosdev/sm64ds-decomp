//cpp
// @symbol _ZN10daSnmBth_c6RenderEv
#include "daSnmBth_c.h"
extern unsigned char data_0209f2d8[];

int daSnmBth_c::Render()
{
  int b = data_0209f2d8[0];
  b = (b == 2);
  if(b) return 1;
  {
    int i;
    for(i=0;i<0x32;i++){
      mParticles[i].Render();
    }
  }
  return 1;
}
