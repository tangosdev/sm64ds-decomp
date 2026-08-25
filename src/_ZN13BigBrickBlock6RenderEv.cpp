//cpp
// @symbol _ZN13BigBrickBlock6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "BigBrickBlock.h"
extern "C" int _ZN5Event6GetBitEj(unsigned int bit);

struct V3 { int x, y, z; };
extern V3 data_ov002_021089e0;

struct Sub {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void m(V3* p);
};

int BigBrickBlock::Render()
{
  int b = (actorID == 0x13);
  if (b != 0) {
    if (!(_ZN5Event6GetBitEj(mEventID) != 0 && mBroken == 0)) {
      return 1;
    } else {
      char* o = *(char**)((char*)&mSwitch);
      if (o != 0) {
        unsigned short d = (unsigned short)(*(unsigned short*)(o+0x33a) - *(unsigned short*)(o+0x338));
        if (d < 0x2d) {
          if ((d & 1) != 0) return 1;
        }
      }
    }
  }
  int b2 = (actorID == 0x10);
  if (b2 != 0) {
    V3 v = data_ov002_021089e0;
    ((Sub*)((char*)&mModel))->m(&v);
  } else {
    ((Sub*)((char*)&mModel))->m(0);
  }
  return 1;
}
