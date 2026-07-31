//cpp
// @symbol func_ov064_02117cfc
// @emits daObjFl_Amilift_c_Render
/* recovered: renamed to Class_Method */
/* daObjFl_Amilift_c::Render - recovered from vtable slot identity */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
extern "C" int daObjFl_Amilift_c_Render(Derived *d) { Base *b = &d->base; b->m(0); return 1; }
