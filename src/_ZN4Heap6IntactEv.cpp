//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual bool m(); };
extern "C" unsigned char data_020a0e98;
extern "C" bool _ZN4Heap6IntactEv(Base *c){
  bool r = c->m();
  if (r) return r;
  if (!data_020a0e98) data_020a0e98 = 1;
  return r;
}
