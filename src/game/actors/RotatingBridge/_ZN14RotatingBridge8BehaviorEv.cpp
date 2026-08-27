//cpp
// @symbol _ZN14RotatingBridge8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "RotatingBridge.h"
extern "C" {
extern int DecIfAbove0_Byte(void*);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned,unsigned,unsigned,void*,unsigned);
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern int func_020393a4(int*,int);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void*,int,int);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
}

int RotatingBridge::Behavior()
{
  if (DecIfAbove0_Byte((char*)&mPauseTimer) == 0) {
    short* p = (short*)(((int)((char*)this) + 0x94));
    *p = *p + 0x100;
    mAngleY = mPrevAngleY;
    mTurnSound = _ZN5Sound8PlayLongEjjjRK7Vector3s(mTurnSound, 3, 0x88, (void*)((char*)&mCamSpacePosX), 0);
    if ((mPrevAngleY & 0x7fff) == 0) mPauseTimer = 0x3c;
  }
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  func_020393a4((int*)((char*)&(*(u8 *)&mMeshCollider)), 0x320000);
  if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x320000, 0))
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  return 1;
}
