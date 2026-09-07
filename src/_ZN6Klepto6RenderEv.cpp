//cpp
// @symbol _ZN6Klepto6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "Klepto.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

int Klepto::Render()
{
 Base *b = (Base *)&mBlendModelAnim; b->m(0); return 1;
}
