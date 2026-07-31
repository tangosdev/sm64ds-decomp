//cpp
// @symbol _ZN6Player17St_SlopeJump_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern void _Z14ApproachLinearRiii(int*,int,int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern int func_ov002_020bedd4(void*);
extern char data_ov002_0211013c[];
extern char data_ov002_02110034[];
extern char data_ov002_021105bc[];
extern unsigned char data_020a0e40[];
extern unsigned short data_0209f49e[];
}

int Player::St_SlopeJump_Main()
{
  _Z14ApproachLinearRiii((int*)((char*)&mHorzSpeed),0,0x800);
  if(mIsAirborne==0){
    _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_0211013c);
  }
  if(*(unsigned short*)((char*)data_0209f49e + data_020a0e40[0]*0x18) & 1){
    mTargetAngleY=mAngleY;
    if(mIsMega==0){
      if(mParam==3){
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_02110034);
      }else{
        _ZN6Player11ChangeStateERNS_5StateE(((char*)this),data_ov002_021105bc);
      }
    }
  }
  func_ov002_020bedd4(((char*)this));
  return 1;
}
