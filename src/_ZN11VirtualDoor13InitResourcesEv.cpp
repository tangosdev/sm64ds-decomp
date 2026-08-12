//cpp
// @symbol _ZN9CameraTag13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "CameraTag.h"


int CameraTag::InitResources()
{
  mScaleX=(((mParam&0xf)+1)*0x64000)>>1;
  mScaleY=(((mParam>>4&0xf)+1)*0x64000);
  mAngleY=-mAngleY;
  return 1;
}
