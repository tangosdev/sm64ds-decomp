//cpp
// @symbol _ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv
/* recovered: named members + shared header, real C++ method */
#include "StarMarker.h"
struct Vec3 { int x, y, z; };
extern "C" signed char NumRedCoins(void);
extern "C" void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int p, struct Vec3* pos, void* rot, int a, int b);
extern "C" void _ZN9PowerStar13AddStarMarkerEv(void* p);

void StarMarker::SpawnRedCoinStarIfNecessary()
{
  struct Vec3 v;
  char* star;
  int y;
  if(((unsigned int)mFlags << 29) >> 31) return;
  if(NumRedCoins() != 8) return;
  v.x = mPosX;
  y = mPosY;
  v.y = y;
  v.z = mPosZ;
  v.y = y + 0x78000;
  star = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xb2, mStarID, &v, 0, mAreaId, -1);
  if(star == 0) return;
  _ZN9PowerStar13AddStarMarkerEv(star);
  *(unsigned short*)(((int)star + 0x4a2)) |= 0x1000;
  *(int*)(star+0x434) = unk_004;
  *(unsigned char*)(((int)((char*)this) + 0x1db)) |= 4;
}
