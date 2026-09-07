//cpp
// @symbol _ZN6Bowser6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Bowser.h"
struct Obj { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void*); };
extern "C" {
extern void _ZN9Animation7AdvanceEv(void* a);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void* a, void* b);
}

int Bowser::Render()
{
  if(mOpacity < 8) return 1;
  _ZN9Animation7AdvanceEv((char*)&mTextureSequence);
  _ZN15TextureSequence6UpdateER15ModelComponents(((char*)this)+0x138, ((char*)this)+0xdc);
  ((Obj*)((char*)&mModelAnim))->m((char*)&mScaleX);
  return 1;
}
