//cpp
// @symbol _ZN13OneUpMushroom16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "OneUpMushroom.h"
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void* fp);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
}
extern void* data_ov002_0210d9d8;
extern void* data_ov002_0210da30;

int OneUpMushroom::CleanupResources()
{
  int s = mMushroomType;
  if (s != 0xb && s != 0xc){
    int b = (mActorID == 0x114);
    if (b != 0) _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9d8);
    else _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da30);
  }
  if ((unsigned int)(mMushroomType - 0xb) <= 1)
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY + 0x28000, mPosZ);
  return 1;
}
