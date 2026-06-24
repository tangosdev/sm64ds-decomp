//cpp
extern "C" {
struct Sub { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(void*); };
extern char data_ov089_021328b4[];

int func_ov089_021320f0(char* c)
{
  int b = (int)((*(unsigned int*)(c+0xb0) & 0x40000) != 0);
  if (b) return 1;
  if (*(int*)(c+0x448) != 0) {
    ((Sub*)(c+0x114))->m(0);
  } else {
    ((Sub*)(c+0x114))->m(c+0x80);
    if (*(int*)((char*)data_ov089_021328b4 + (*(int*)(c+0x444) << 2)) != 0 && *(int*)(c+0x448) == 0) {
      ((Sub*)(c+0x178))->m(0);
    }
  }
  return 1;
}
}
