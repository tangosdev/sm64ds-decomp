//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
extern "C" int _ZN7Seaweed6RenderEv(Derived *d) { Base *b = &d->base; b->m(0); return 1; }
