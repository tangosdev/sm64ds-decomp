//cpp
struct Args { int a, b, c; };
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(Args*); };
struct Derived { char pad[0xd8]; Base base; };
extern "C" {
extern int func_0203aad0(int *p);
int _ZN8PoleLift6RenderEv(Derived *d) {
  Args s;
  s.a = 0x1000;
  s.b = func_0203aad0((int*)((char*)d+0x158));
  s.c = 0x1000;
  Base *b = &d->base; b->m(&s);
  return 1;
}
}
