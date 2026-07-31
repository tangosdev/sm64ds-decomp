//cpp
// @symbol _ZN13RacingPenguin6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "RacingPenguin.h"
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void *); };

extern "C" void _ZN15TextureSequence6UpdateER15ModelComponents(void *, void *);

int RacingPenguin::Render()
{
    _ZN15TextureSequence6UpdateER15ModelComponents(((char *)this) + 0x138, ((char *)this) + 0xdc);
    Sub *s = (Sub *)((char *)&mModelAnim);
    s->m((char *)&unk_080);
    return 1;
}
