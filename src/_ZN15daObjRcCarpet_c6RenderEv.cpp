//cpp
// @symbol _ZN15daObjRcCarpet_c6RenderEv
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0x428]; unsigned short fld; char pad2[0x450-0x42a]; Base base; };
extern "C" int _ZN15daObjRcCarpet_c6RenderEv(Derived *d) {
    if (d->fld < 0x5a && (d->fld & 1)) return 1;
    Base *b = &d->base; b->m(0);
    return 1;
}
