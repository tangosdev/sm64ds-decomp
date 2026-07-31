//cpp
// @symbol _ZN17SlidingPlatformWf8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SlidingPlatformWf.h"
typedef int Fix12;
typedef short s16;
typedef unsigned short u16;
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* c, void* cc);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(char* c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char* c, Fix12 a, Fix12 b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char* c);

int SlidingPlatformWf::Behavior()
{
  if (DecIfAbove0_Byte((unsigned char*)((char*)&unk_31e)) == 0) {
    if (DecIfAbove0_Short((unsigned short*)((char*)&unk_320)) == 0) {
      s16* a = (s16*)(((int)((char*)this) + 0x94) & 0xffffffffffffffff);
      s16 v = data_ov091_02134504[unk_322];
      unk_320 = v;
      *a += 0x8000;
      unk_31e = 0xf;
    } else {
      _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), 0);
    }
  }
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0, 0) != 0) {
    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  }
  return 1;
}
