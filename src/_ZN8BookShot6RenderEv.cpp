//cpp
struct Sub { virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3(); virtual int v4(); virtual int m(void*); };
extern "C" int _ZN8BookShot6RenderEv(char* c){
  bool b = *(int*)(c+0xb0) & 0x40000;
  if(b != 0) return 1;
  if(*(unsigned char*)(c+0x450) != 0){ ((Sub*)(c+0x110))->m(c+0x80); }
  else { ((Sub*)(c+0x174))->m(c+0x80); }
  return 1;
}
