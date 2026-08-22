//cpp
// @symbol _ZN11CastleWater6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * Pushes the texture transform into the model's components, then draws through
 * the model's own vtable slot 5. The components are not a separate sub-object:
 * 0x0dc is Model::data, +0x8 inside the Model at 0x0d4 -- the cartridge's own
 * ~CastleWater proves the Model's extent (tools/dtor_members.py), so the header
 * no longer declares a marker there and this is the model's own field.
 */
#include "CastleWater.h"

struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

extern "C" void _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);

int CastleWater::Render()
{
    _ZN18TextureTransformer6UpdateER15ModelComponents(&mTexTransformer, &mModel.data);
    Sub *b = (Sub *)&mModel;
    b->m(0);
    return 1;
}
