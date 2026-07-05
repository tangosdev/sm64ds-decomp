//cpp
struct C;
typedef int (C::*PMF)();
struct C {
  char pad138[0x138];
  PMF* pp;
  char pad13c[0x154-0x13c];
  unsigned int flags;
};
extern "C" int func_0200cae4(C* c){
  if(c->flags & 0x4000){
    volatile unsigned int* flags = (volatile unsigned int*)(((int)c + 0x154) & 0xFFFFFFFFFFFFFFFF);
    *flags &= ~0x4000u;
    *flags |= 0x8000u;
  }
  PMF* p = c->pp;
  if(*(int*)p == 0) return 1;
  return (c->**p)();
}
