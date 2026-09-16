//cpp
#include "TextureTransformer.h"
// @symbol _ZN9Whirlpool6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Whirlpool.h"
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base { char pad[0x114]; Sub sub; };

int Whirlpool::Render()
{
 ((TextureTransformer *)((char *)((Base *)this) + 0x178))->Update(*(ModelComponents *)((char *)((Base *)this) + 0x11c)); Sub *b = &((Base *)this)->sub; b->m(0); return 1;
}
