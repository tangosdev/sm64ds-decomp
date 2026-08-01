//cpp
// @symbol _ZN22ClockPaintingHandShort8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ClockPaintingHandShort.h"
extern int IsAreaShowing(int);
extern signed char data_02092110[];
extern unsigned char data_0209f2c0[];

int ClockPaintingHandShort::Behavior()
{
  if (data_02092110[0] <= 0) {
    short* p = (short*)(((int)((char*)this) + 0x90));
    *p = *p + data_ov013_021116ac[mHandIndex];
  } else if (IsAreaShowing(unk_0cc) && mHandIndex == 0) {
    unsigned short d = (unsigned short)(-unk_090);
    if (d < 0x2000) data_0209f2c0[0] = 3;
    else if (d < 0x6000 && d >= 0x2000) data_0209f2c0[0] = 0;
    else if (d < 0xa000 && d >= 0x6000) data_0209f2c0[0] = 2;
    else if (d < 0xe000 && d >= 0xa000) data_0209f2c0[0] = 1;
    else if (d >= 0xe000) data_0209f2c0[0] = 3;
  }
  func_ov013_02111430(((char*)this));
  return 1;
}
