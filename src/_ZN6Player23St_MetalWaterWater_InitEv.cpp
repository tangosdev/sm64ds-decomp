//cpp
// @symbol _ZN6Player23St_MetalWaterWater_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void func_ov002_020bf2d8(void*,int);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned,void*);
}

int Player::St_MetalWaterWater_Init()
{
  mIsInAirState=1;
  mIsUnderwater=1;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  if(mStateStep==0){
    func_ov002_020bf2d8(((char*)this),0x20000);
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x53,0x40000000,0x1000,0);
  }else{
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x54,0x40000000,0x1000,0);
  }
  _ZN5Sound9PlayBank0EjRK7Vector3(0xa8,((char*)this)+0x74);
  mStateWork=0;
  return 1;
}
