//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual int m(void*); };
extern "C" void Crash();
extern "C" int _ZN4Heap6SizeofEPv(Base *c, void *a){
  int r = c->m(a);
  if (r == -1 && (*(int*)((char*)c+0x10) & 0x4000)) Crash();
  return r;
}
