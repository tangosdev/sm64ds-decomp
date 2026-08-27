//cpp
// @symbol _ZN13SnowmanBreath6RenderEv
#include "SnowmanBreath.h"
extern unsigned char data_0209f2d8[];

int SnowmanBreath::Render()
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
