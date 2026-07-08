//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual bool m(); };
extern "C" unsigned char _ZN6Memory11heapDamagedE;
extern "C" bool _ZN4Heap6IntactEv(Base *c){
  bool r = c->m();
  if (r) return r;
  if (!_ZN6Memory11heapDamagedE) _ZN6Memory11heapDamagedE = 1;
  return r;
}
