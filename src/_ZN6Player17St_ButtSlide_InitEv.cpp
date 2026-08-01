//cpp
// @symbol _ZN6Player17St_ButtSlide_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
struct Camera;
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern struct Camera* data_0209f318;
extern void func_0200d544(struct Camera* thiz, unsigned char playerID);
}

int Player::St_ButtSlide_Init()
{
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this), 0x41, 0x40000000, 0x1000, 0);
  mStateStep=0;
  if(mSlideType){
    func_0200d544(data_0209f318, mPlayerNo);
  }
  unk_6e6=0;
  mStateArg=mSlideType;
  *(short*)(((char*)this)+0x600+0xa6)=6;
  mIsSlidingOnGround=0;
  return 1;
}
