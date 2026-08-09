//cpp
// @symbol _ZN11CastleWater6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * Pushes the texture transform into the model's components, then draws through
 * the model's own vtable slot 5. The transformer and the components are
 * separate sub-objects (0x320 and 0x0dc) and this is the only place they meet.
 */
#include "CastleWater.h"

struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

extern "C" void _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);

int CastleWater::Render()
{
    _ZN18TextureTransformer6UpdateER15ModelComponents(&mTexTransformer, &mModelComponents);
    Sub *b = (Sub *)&mModel;
    b->m(0);
    return 1;
}
