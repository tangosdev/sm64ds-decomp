//cpp
// @symbol _ZN20daObjCannonShutter_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daObjCannonShutter_c.h"
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
struct Sub {
  virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
  virtual void v10(); virtual void m14(int arg);
};

int daObjCannonShutter_c::Render()
{
  if (data_0209f2f8 == 6 && data_0209f220 == 1 && (int)(param1 & 0xff) == 1)
    return 1;
  if (mCannonOpen != 0)
    return 1;
  Sub* b = (Sub*)&mModel;
  b->m14(0);
  return 1;
}
