//cpp
struct Mdl {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void slot5(void* p);
};
extern "C" {
extern void _ZN5Model12ShowMaterialEii(void*, int, int);
extern void _ZN5Model12HideMaterialEii(void*, int, int);
struct V3 { int x, y, z; };
int func_ov062_02118f84(char* c){
  volatile struct V3 saved;
  int b = (*(int*)(c + 0xb0) & 0x40000) != 0;
  if (b) return 1;
  if (*(int*)(c + 0x390) == 1) {
    _ZN5Model12ShowMaterialEii(c + 0x300, 0, 1);
    _ZN5Model12HideMaterialEii(c + 0x300, 0, 2);
  } else {
    _ZN5Model12HideMaterialEii(c + 0x300, 0, 1);
    _ZN5Model12ShowMaterialEii(c + 0x300, 0, 2);
  }
  saved.x = *(int*)(c + 0x80);
  saved.y = *(int*)(c + 0x84);
  saved.z = *(int*)(c + 0x88);
  if (*(int*)(c + 0x10c) == 1 && *(int*)(c + 0x390) == 2) {
    *(int*)(c + 0x80) = (int)(((long long)*(volatile int*)(c + 0x80) * 0x800 + 0x800) >> 12);
    *(int*)(c + 0x84) = (int)(((long long)*(volatile int*)(c + 0x84) * 0x800 + 0x800) >> 12);
    *(int*)(c + 0x88) = (int)(((long long)*(volatile int*)(c + 0x88) * 0x800 + 0x800) >> 12);
  }
  ((struct Mdl*)(c + 0x300))->slot5(c + 0x80);
  *(int*)(c + 0x80) = saved.x;
  *(int*)(c + 0x84) = saved.y;
  *(int*)(c + 0x88) = saved.z;
  return 1;
}
}
