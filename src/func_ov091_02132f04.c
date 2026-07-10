//cpp
extern "C" {
struct V3 { int x, y, z; };
int _ZN5Actor15HugeLandingDustEb(void*, int);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned, int, int, int);
int _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void*, struct V3*, int);
int func_0201267c(int, void*);
void func_ov091_02132f04(char* c){
  int *pa8 = (int*)(((int)c + 0xa8) & 0xFFFFFFFFFFFFFFFFLL);
  int a = *pa8;
  int *p60 = (int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
  a = a - 0x4000;
  *pa8 = a;
  a = *p60 + *(int*)(c+0xa8);
  *p60 = a;
  if(*(int*)(c+0x60) > *(int*)(c+0x394)) return;
  *(int*)(c+0x60) = *(int*)(c+0x394);
  int zero = 0;
  *(int*)(c+0xa8) = zero;
  *(int*)(c+0x398) = 3;
  *(unsigned char*)(c+0x39e) = 0xa;
  if(*(unsigned short*)(c+0xc) == 0xa1) zero = 1;

  struct { struct V3 part; struct V3 quake; } sp;
  if(zero != 0){
    _ZN5Actor15HugeLandingDustEb(c, 1);
  } else {
    sp.part.x = *(int*)(c+0x5c);
    sp.part.y = *(int*)(c+0x60);
    sp.part.z = *(int*)(c+0x64);
    sp.part.y = sp.part.y + 0x3c000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x2e, sp.part.x, sp.part.y, sp.part.z);
  }
  sp.quake.x = *(int*)(c+0x5c);
  sp.quake.y = *(int*)(c+0x60);
  sp.quake.z = *(int*)(c+0x64);
  _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &sp.quake, 0x7d0000);
  func_0201267c(0xc7, c+0x74);
}
}
