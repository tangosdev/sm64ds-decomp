//cpp
// @symbol _ZN6Player20St_StomachSlide_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct Camera;
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern struct Camera* data_0209f318;
extern void func_0200d544(struct Camera* thiz, unsigned char playerID);
}

int Player::St_StomachSlide_Init()
{
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x40, 0x40000000, 0x1000, 0);
  mStateStep=0;
  if(mSlideType){
    func_0200d544(data_0209f318, mPlayerNo);
  }
  mStateWork=0;
  mStatePhase=0;
  mSlideStoppedTimer=0;
  mStateArg=mSlideType;
  mIsSlidingOnGround=0;
  return 1;
}
