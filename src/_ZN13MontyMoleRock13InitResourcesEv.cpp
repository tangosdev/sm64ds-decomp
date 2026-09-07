//cpp
// @symbol _ZN13MontyMoleRock13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "MontyMoleRock.h"
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
int _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void*,void*,int,int,unsigned,unsigned);
int _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void*,void*,int,int,int,int);
extern int data_ov080_021283c8[];
}

s32 MontyMoleRock::InitResources()
{
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov080_021283c8);
  if(_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0x110, m, 1, -1) == 0) return 0;
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this)+0x160, ((char*)this), 0x1e000, 0x1e000, 0x200004, 0);
  mIsSmall = (*(s32 *)&param1) & 1;
  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this)+0x194, ((char*)this), 0x1e000, 0x1e000, 0, 0);
  mVertAccel = -0x2000;
  mTerminalVelocity = -0x3c000;
  if(mIsSmall == 0){
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
  } else {
    mScaleX = 0x800;
    mScaleY = 0x800;
    mScaleZ = 0x800;
  }
  return 1;
}
