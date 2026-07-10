//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct D { char pad[0x6b4]; Base b; };
extern "C" int _ZN14QuestionSwitch6RenderEv(D *d) {
  d->b.m(0);
  return 1;
}
