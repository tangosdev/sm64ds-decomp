//cpp
struct Sub { virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3(); virtual int v4(); virtual int m(void*); };
extern "C" int _ZN8Moneybag6RenderEv(char* c){
  bool b = *(int*)(c+0xb0) & 0x40000;
  if(b != 0) return 1;
  if(*(unsigned char*)(c+0x3f0) > 1){ ((Sub*)(c+0xd4))->m(0); }
  if(*(unsigned char*)(c+0x3f0) <= 0x1f){ ((Sub*)(c+0x138))->m(0); }
  return 1;
}
