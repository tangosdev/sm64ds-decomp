//cpp
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };
struct Base2 { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void*); };
struct D { char pad[0xd4]; Base b1; char pad2[0x138-0xd8]; Base2 b2; };
extern "C" int func_ov100_021419d4(D* d){
  char* c=(char*)d;
  if(*(int*)(c+0x3e4)==4) return 1;
  if(*(unsigned char*)(c+0x3f1)!=0){
    d->b1.m(0);
  } else {
    d->b2.m(c+0x80);
  }
  return 1;
}
