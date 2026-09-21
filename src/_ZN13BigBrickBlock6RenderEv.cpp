//cpp
// @symbol _ZN13daObjBlockL_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daObjBlockL_c.h"
#include "daObjSwitch_c.h"

/* Event has no shared namespace declaration yet. */
extern "C" int _ZN5Event6GetBitEj(unsigned int bit);

extern Vector3 data_ov002_021089e0;

int daObjBlockL_c::Render()
{
  int b = (actorID == 0x13);
  if (b != 0) {
    if (!(_ZN5Event6GetBitEj(mEventID) != 0 && mBroken == 0)) {
      return 1;
    } else {
      if (mSwitch != 0) {
        unsigned short d = (unsigned short)(mSwitch->mTimeLimit - mSwitch->mTimer);
        if (d < 0x2d) {
          if ((d & 1) != 0) return 1;
        }
      }
    }
  }
  int b2 = (actorID == 0x10);
  if (b2 != 0) {
    Vector3 v = data_ov002_021089e0;
    mModel.Render(&v);
  } else {
    mModel.Render(0);
  }
  return 1;
}
