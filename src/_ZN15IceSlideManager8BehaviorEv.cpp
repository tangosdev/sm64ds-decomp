//cpp
// @symbol _ZN15IceSlideManager8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "IceSlideManager.h"
extern "C" {
extern int _ZN8dActor_c13DistToCPlayerEv(void*);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned,unsigned,unsigned,int,int);
extern int DecIfAbove0_Short(void*);
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
}

int IceSlideManager::Behavior()
{
  switch (mState) {
  case 0:
    if (_ZN8dActor_c13DistToCPlayerEv(((char*)this)) < 0x180000) {
      unsigned char* p = (unsigned char*)(((int)((char*)this) + 0xd6));
      _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x15666, 0);
      *p += 1;
    }
    break;
  case 1:
    if (DecIfAbove0_Short((char*)&mKillTimer) == 0) {
      _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, 0x15666, 0);
      _ZN8dActor_c24KillAndTrackInDeathTableEv(((char*)this));
    }
    break;
  }
  return 1;
}
