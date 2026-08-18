//cpp
// @symbol _ZN9PushBlock6RenderEv
/* recovered: renamed to Class_Method
 *
 * PushBlock::Render -- vtable slot 9, ov002 0x020b8dac. Attributed by the
 * vtable: _ZTV9PushBlock carries this address at slot 9, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043af0.
 *
 * Kept as an extern "C" free function under the literal mangled name -- the
 * body is the same virtual-dispatch-through-a-shadow-vtable trick the file
 * used before the rename (Model's own Render, called through the Model
 * sub-object at +0xd4), unconverted. */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
extern "C" int _ZN9PushBlock6RenderEv(Derived *d) { Base *b = &d->base; b->m(0); return 1; }
