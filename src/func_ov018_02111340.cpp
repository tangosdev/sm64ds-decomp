//cpp
// @symbol func_ov018_02111340
// recovered name: daObjSm_Lift_c_Render
/* recovered: renamed to Class_Method */
/* daObjSm_Lift_c::Render - verified 2004/b56 byte-match (ov018), strict-reloc */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
extern "C" int func_ov018_02111340(Derived *d) { Base *b = &d->base; b->m(0); return 1; }
