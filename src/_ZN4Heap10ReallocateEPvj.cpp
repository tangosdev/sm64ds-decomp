//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void m(void*,unsigned int); };
extern "C" void _ZN4Heap10ReallocateEPvj(Base *c, void *a, unsigned int b){ c->m(a,b); }
