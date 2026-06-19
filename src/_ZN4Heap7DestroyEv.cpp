//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void m(); };
extern "C" void _ZN4Heap10DeallocateEPv(void*, void*);
extern "C" void _ZN4Heap7DestroyEv(Base *c){
  c->m();
  *(int*)((char*)c+4)=0;
  *(int*)((char*)c+8)=0;
  void* p = *(void**)((char*)c+0xc);
  if (p==0) return;
  _ZN4Heap10DeallocateEPv(p, c);
  *(int*)((char*)c+0xc)=0;
}
