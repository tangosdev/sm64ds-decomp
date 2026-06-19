//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void m(void*); };
extern "C" void _ZN4Heap10DeallocateEPv(Base *c, void *a){ c->m(a); }
