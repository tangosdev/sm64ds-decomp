//cpp
extern "C" int _ZN11CommonModel6RenderEPK7Vector3(void*, void*);
struct Flags { unsigned char b0:1; };
extern "C" int _ZN4Coin6RenderEv(char* c){
  int f;
  int b;
  if (!((struct Flags*)(c+0x3ae))->b0) return 1;
  f = *(int*)(c+0xb0);
  b = (f & 0x40000) != 0;
  if (b) return 1;
  {
    unsigned short x = *(unsigned short*)(c+0x3a8);
    if (x < 0x2d && (x & 1)) return 1;
  }
  if (!(f & 0x10))
    _ZN11CommonModel6RenderEPK7Vector3(c+0xd8, 0);
  else
    _ZN11CommonModel6RenderEPK7Vector3(c+0x114, 0);
  return 1;
}
