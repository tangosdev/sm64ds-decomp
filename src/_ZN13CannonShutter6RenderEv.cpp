//cpp
// @symbol _ZN13CannonShutter6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "CannonShutter.h"
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
struct Sub {
  virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
  virtual void v10(); virtual void m14(int arg);
};
struct Obj {
  char pad[0xd4];
  Sub sub;
};

int CannonShutter::Render()
{
  if (data_0209f2f8 == 6 && data_0209f220 == 1 && (*(int*)((char*)&param1) & 0xff) == 1)
    return 1;
  if (*(unsigned char*)((char*)&unk_32e) != 0)
    return 1;
  Sub* b = &((Obj*)this)->sub;
  b->m14(0);
  return 1;
}
