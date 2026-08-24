//cpp
#include "types.h"
// @symbol _ZN17SlidingPlatformWf8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SlidingPlatformWf.h"
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, void* cc);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(char* c);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(char* c, Fix12i a, Fix12i b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(char* c);
}

int SlidingPlatformWf::Behavior()
{
  if (DecIfAbove0_Byte((unsigned char*)((char*)&mPauseTimer)) == 0) {
    if (DecIfAbove0_Short((unsigned short*)((char*)&mMoveTimer)) == 0) {
      s16* a = (s16*)(((int)((char*)this) + 0x94));
      s16 v = data_ov091_02134504[mVariant];
      mMoveTimer = v;
      *a += 0x8000;
      mPauseTimer = 0xf;
    } else {
      _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), 0);
    }
  }
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
  if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0) != 0) {
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
  }
  return 1;
}
