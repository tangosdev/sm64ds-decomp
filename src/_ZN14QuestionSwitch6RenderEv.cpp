//cpp
// @symbol _ZN14QuestionSwitch6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "QuestionSwitch.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct D { char pad[0x6b4]; Base b; };

int QuestionSwitch::Render()
{
  ((D *)this)->b.m(0);
  return 1;
}
