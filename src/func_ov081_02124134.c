struct V3{int x,y,z;};
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern int RandomIntInternal(int* seed);
extern void func_02012790(int);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, struct V3*, void*, int, int);
extern int data_0209e650;

void func_ov081_02124134(char* c){
  char* a;
  unsigned int id = *(unsigned int*)(c+0x3fc);
  if (id) {
    a = _ZN5Actor10FindWithIDEj(id);
    if (a) {
      *(int*)(a+0x9c) = -0x2000;
      *(int*)(a+0xa0) = -0x28000;
      *(int*)(c+0x3fc) = 0;
    }
  }
  id = *(unsigned int*)(c+0x400);
  if (id) {
    a = _ZN5Actor10FindWithIDEj(id);
    if (a) {
      unsigned int rv;
      *(int*)(a+0x9c) = -0x2000;
      rv = (unsigned int)RandomIntInternal(&data_0209e650);
      rv = (rv >> 8) & 0xf;
      *(short*)(a+0x92) = 0;
      *(short*)(a+0x94) = rv << 0xc;
      *(short*)(a+0x96) = 0;
      *(int*)(a+0xa4) = 0;
      *(int*)(a+0xa8) = 0x14000;
      *(int*)(a+0xac) = 0;
      *(int*)(a+0x98) = 0xa000;
      *(int*)(a+0xc8) = 0;
    }
    func_02012790(0xa);
    *(int*)(c+0x400) = 0;
  }
  if (*(int*)(c+0x41c) == 2) {
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xdf, 0x300, (struct V3*)(c+0x44c), 0, *(signed char*)(c+0xcc), -1);
  }
}
