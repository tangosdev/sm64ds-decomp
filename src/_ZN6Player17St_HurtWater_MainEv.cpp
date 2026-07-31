//cpp
// @symbol _ZN6Player17St_HurtWater_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020cec2c(void*);
extern void _Z14ApproachLinearRiii(int*,int,int);
extern int func_ov002_020ceaf4(void*);
extern int _ZN6Player12FinishedAnimEv(void*);
extern int _ZN6Player9GetHealthEv(void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern void func_ov007_020c5dec(void*,int);
extern int func_ov002_020ceb7c(void*);
extern int func_ov002_020bedd4(void*);
extern int data_ov002_0211067c[];
extern int data_ov002_021106ac[];
extern int data_0209f32c[];
}

int Player::St_HurtWater_Main()
{
  if(func_ov002_020cec2c(((char*)this))) return 1;
  _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed),0,0x800);
  int t=func_ov002_020ceaf4(((char*)this));
  _Z14ApproachLinearRiii((int*)((char*)&mVertSpeed),t,0x800);
  if(_ZN6Player12FinishedAnimEv(((char*)this))){
    if((mStateStep&1)==0){
      mTargetAngleY=mTargetAngleY+0x8000;
      mAngleY=mTargetAngleY;
    }
    if(_ZN6Player9GetHealthEv(((char*)this))){
      if(mIsMetal==0)
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_0211067c);
      else
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_021106ac);
      mIsTakingDamage=0;
      if(mHurtDamage)
        mInvincibleTimer=0x24;
    }else{
      func_ov007_020c5dec(((char*)this),8);
      return 1;
    }
  }
  int lim=*(int*)data_0209f32c-0x50000;
  if(mPosY>=lim && (mClsnFlags&1)==0){
    if(mVertSpeed>=0){
      mPosY=lim;
      mVertSpeed=0;
    }
  }
  func_ov002_020ceb7c(((char*)this));
  func_ov002_020bedd4(((char*)this));
  return 1;
}
