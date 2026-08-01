//cpp
// @symbol func_ov027_02111e00
// recovered name: daPgDfdr_c_Render
/* recovered: renamed to Class_Method */
/* daPgDfdr_c::Render - recovered from vtable slot identity */
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base { char pad[0x320]; Sub sub; };
extern "C" void _ZN15TextureSequence6UpdateER15ModelComponents(void *, void *);
extern "C" int func_ov027_02111e00(Base *c) { _ZN15TextureSequence6UpdateER15ModelComponents((char *)c + 0x384, (char *)c + 0x328); Sub *b = &c->sub; b->m(0); return 1; }
