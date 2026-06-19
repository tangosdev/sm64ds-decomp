//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual int m(); };
extern "C" int _ZN4Heap6RescueEv(Base *c){ return c->m(); }
