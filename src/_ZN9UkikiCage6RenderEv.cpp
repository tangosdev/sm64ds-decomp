//cpp
// @symbol _ZN9UkikiCage6RenderEv
/* recovered: renamed to Class_Method
 *
 * UkikiCage::Render -- vtable slot 9, ov030 0x02111350. Attributed by the
 * vtable: _ZTV9UkikiCage carries this address at slot 9, where
 * _ZTV10dBgActor_c carries fBase_c's generic 0x02043af0.
 *
 * Kept as a free function under the literal mangled name -- the body is the
 * same virtual-dispatch-through-a-shadow-vtable trick the file used before
 * the rename (Model's own Render, called through the Model sub-object at
 * +0xd4), unconverted. */
struct Obj { virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3(); virtual void f4(); virtual void m(int); };
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void*);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern "C" int _ZN9UkikiCage6RenderEv(char *c){
  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
  ((Obj*)(c+0xd4))->m(0);
  return 1;
}
