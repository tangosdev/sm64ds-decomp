//cpp
extern "C" {
struct A { char pad[0x320]; };
struct B { virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4(); virtual void m5(bool); };
int func_ov010_021115a8(char* c){
  if(*(unsigned char*)(c+0x3ab) == 0){
    ((B*)(c+0x320))->m5(false);
  }
  return 1;
}
}
