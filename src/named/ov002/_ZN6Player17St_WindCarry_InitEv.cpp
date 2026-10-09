//cpp
// @symbol _ZN6Player17St_WindCarry_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020da9d4(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int data_0209f318[];
}

int Player::St_WindCarry_Init()
{
  func_ov002_020da9d4(((char*)this));
  _ZN6Player7SetAnimEji5Fix12IiEj(((char*)this),0x4a,0x40000000,0x1000,0);
  mVertSpeed=0;
  mIsAirborne=1;
  mLandSoundPlayed=0;
  func_0200d63c((void*)data_0209f318[0], mPlayerNo);
  return 1;
}
