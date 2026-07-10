//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Derived { char pad[0xd4]; Base base; };
extern "C" int _ZN8ShipWing6RenderEv(Derived *d) {
  if(*(unsigned char*)((char*)d+0x4ea)!=2){
    Base *b = &d->base; b->m(0);
  }
  return 1;
}
