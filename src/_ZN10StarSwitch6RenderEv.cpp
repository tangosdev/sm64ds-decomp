//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };
struct Sub : Base { };
struct C { char p1[0xd4]; Sub sub; };
extern "C" int _ZN10StarSwitch6RenderEv(C* c){
  c->sub.M((char*)c+0x320);
  return 1;
}
