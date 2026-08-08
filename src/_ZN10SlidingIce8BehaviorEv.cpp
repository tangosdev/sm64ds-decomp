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
extern void _ZN9ActorBase18MarkForDestructionEv(void*);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int, unsigned int, unsigned int, void*, unsigned int);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, void*, void*, int, int);
}

int SlidingIce::Behavior()
{
  int isType = (mActorID == 0x5d);
  if(isType){
    if(DecIfAbove0_Short((char *)&unk_31e) == 0){
      _Z14ApproachLinearRiii((int*)((char *)&unk_098), 0, 0x3000);
      if(_Z14ApproachLinearRiii((int*)((char *)&mPosY), unk_324, 0xa000) != 0){
        _ZN9ActorBase18MarkForDestructionEv(((char *)this));
      }
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(((char *)this), 0);
    unk_328 = _ZN5Sound8PlayLongEjjjRK7Vector3s(unk_328, 3, 0x98, ((char *)this)+0x74, 0);
  } else {
    if(DecIfAbove0_Short((char *)&unk_31e) == 0){
      V3 pos;
      pos.x = mPosX;
      pos.y = mPosY;
      pos.z = mPosZ;
      int spawnType = 1;
      if(DecIfAbove0_Byte((char *)&unk_320) == 0){
        unk_320 = 5;
        spawnType = 2;
      } else {
        pos.y -= 0x50000;
      }
      unsigned char cnt = unk_320;
      unk_31e = (cnt + 1) * 0x14;
      _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x5d, spawnType, &pos, ((char *)this)+0x8c, mAreaId, -1);
    }
  }
  return 1;
}
