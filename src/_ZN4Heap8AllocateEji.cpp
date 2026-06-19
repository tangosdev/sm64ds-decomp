//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual int m(unsigned int,int); };
extern "C" void Crash();
extern "C" int _ZN4Heap8AllocateEji(Base *c, unsigned int a, int b){
  int r = c->m(a,b);
  if (r == 0 && (*(int*)((char*)c+0x10) & 0x4000)) Crash();
  return r;
}
