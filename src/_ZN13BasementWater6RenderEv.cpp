//cpp
// @symbol _ZN12SwitchPillar6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "SwitchPillar.h"
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base { char pad[0xd4]; Sub sub; };
extern "C" void _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);

int SwitchPillar::Render()
{
 _ZN18TextureTransformer6UpdateER15ModelComponents((char *)((Base *)this) + 0x320, (char *)((Base *)this) + 0xdc); Sub *b = &((Base *)this)->sub; b->m(0); return 1;
}
