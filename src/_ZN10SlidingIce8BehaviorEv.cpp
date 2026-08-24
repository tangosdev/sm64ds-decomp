//cpp
// @symbol _ZN10SlidingIce8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "SlidingIce.h"
struct V3 { int x,y,z; };
struct V316 { short x,y,z; };
extern "C" {
extern int DecIfAbove0_Short(void*);
extern int DecIfAbove0_Byte(void*);
extern int _Z14ApproachLinearRiii(int*, int, int);
extern void _ZN7fBase_c18MarkForDestructionEv(void*);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void*, void*);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int, unsigned int, unsigned int, void*, unsigned int);
extern int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, void*, void*, int, int);
}

int SlidingIce::Behavior()
{
  int isType = (actorID == 0x5d);
  if(isType){
    if(DecIfAbove0_Short((char *)&mDelayTimer) == 0){
      _Z14ApproachLinearRiii((int*)((char *)&mHorzSpeed), 0, 0x3000);
      if(_Z14ApproachLinearRiii((int*)((char *)&mPosY), mMinPosY, 0xa000) != 0){
        _ZN7fBase_c18MarkForDestructionEv(((char *)this));
      }
    }
    _ZN8dActor_c9UpdatePosEP5dCc_c(((char *)this), 0);
    mSoundID = _ZN5Sound8PlayLongEjjjRK7Vector3s(mSoundID, 3, 0x98, ((char *)this)+0x74, 0);
  } else {
    if(DecIfAbove0_Short((char *)&mDelayTimer) == 0){
      V3 pos;
      pos.x = mPosX;
      pos.y = mPosY;
      pos.z = mPosZ;
      int spawnType = 1;
      if(DecIfAbove0_Byte((char *)&mNumToBigIce) == 0){
        mNumToBigIce = 5;
        spawnType = 2;
      } else {
        pos.y -= 0x50000;
      }
      unsigned char cnt = mNumToBigIce;
      mDelayTimer = (cnt + 1) * 0x14;
      _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x5d, spawnType, &pos, ((char *)this)+0x8c, mAreaId, -1);
    }
  }
  return 1;
}
