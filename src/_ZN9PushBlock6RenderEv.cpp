//cpp
// @symbol _ZN9PushBlock6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * PushBlock::Render -- vtable slot 9, ov002 0x020b8dac. Attributed by the
 * vtable: _ZTV9PushBlock carries this address at slot 9, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043af0.
 *
 * Draws the block's own Model sub-object at +0xd4 through the Model vtable
 * (slot 5, Model::Render), the same shadow-vtable spelling DonutBlock::Render
 * uses -- the Model class is not yet modelled here as a real member. */
#include "PushBlock.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };

int PushBlock::Render()
{
    Base *b = &((Derived *)this)->base; b->m(0);
    return 1;
}
