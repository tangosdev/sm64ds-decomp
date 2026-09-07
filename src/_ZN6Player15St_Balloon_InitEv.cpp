//cpp
// @symbol _ZN6Player15St_Balloon_InitEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int Player_ReleaseHeldActor(void*);
extern int data_0209f318[];
extern int data_ov002_0210e750[];
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
}

int Player::St_Balloon_Init()
{
  Player_ReleaseHeldActor(((char*)this));
  mIsInAirState=1;
  mIsFallScreaming=0;
  if(mVertSpeed >= 0x10000) mVertSpeed=0x10000;
  if(mVertSpeed <= -0x10000) mVertSpeed=-0x10000;
  mHorzSpeed=0;
  unk_690=0x64000;
  func_0200d63c((void*)data_0209f318[0], mPlayerNo);
  mAngleYSpeed=0x100;
  unk_69e=0x100;
  mPrevVertSpeed=mVertSpeed;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)((char*)&mModelAnim3), (void*)data_ov002_0210e750[1], 0x40000000, 0x1000, 0);
  mModelAnim3.currFrame = 0;
  return 1;
}
