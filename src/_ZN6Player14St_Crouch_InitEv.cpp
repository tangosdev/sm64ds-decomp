//cpp
// @symbol _ZN6Player14St_Crouch_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern unsigned char data_020a0e40;
extern unsigned char data_0209f49c[];
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,struct Vector3*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

int Player::St_Crouch_Init()
{
  if(unk_6ed){
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(mCharacter,0x2e,(struct Vector3*)((char*)&mCamSpacePosX));
  }
  mStateTimer=0x1e;
  if(*(unsigned short*)(data_0209f49c + data_020a0e40*0x18) & 2){
    if(func_ov002_020d1204(((char*)this))) return 1;
  }
  if(mStateStep==0){
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x2d,0x40000000,0x1000,0);
    mPrevAngleY=mAngleY;
  } else if(mStateStep==1){
    _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x2c,0,0x1000,0);
  }
  return 1;
}
