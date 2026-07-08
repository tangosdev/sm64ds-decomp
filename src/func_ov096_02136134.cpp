//cpp
struct V3{int x,y,z;};
struct Obj {
  virtual int d0();
  virtual int d1();
  virtual int d2();
  virtual int d3();
  virtual int d4();
  virtual int d5();
  virtual int d6();
  virtual int d7();
  virtual int d8();
  virtual int d9();
  virtual int d10();
  virtual int d11();
  virtual int d12();
  virtual int d13();
  virtual int d14();
  virtual int d15();
  virtual int d16();
  virtual int d17();
  virtual int d18();
  virtual int d19();
  virtual int d20();
  virtual int d21();
  virtual int d22();
  virtual int d23();
  virtual int d24();
  virtual int d25();
  virtual int d26();
  virtual int d27();
  virtual int d28();
  virtual int GetY();
};
extern "C" {
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, struct V3*, void*, int, int);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
extern int func_ov096_02135800(char* c);
extern short Vec3_HorzAngle(void*, void*);
extern int RandomIntInternal(void*);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern int RNG_STATE[];
}
extern "C" int func_ov096_02136134(char* c){
  int cond = (*(unsigned short*)(c+0xc) == 0xf0);
  if(cond){
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x122, 2, (struct V3*)(c+0x5c), 0, *(signed char*)(c+0xcc), -1);
  }
  {
    _ZN5Sound9PlayBank0EjRK7Vector3(9, c+0x74);
    func_ov096_02135800(c);
    *(short*)(c+0x94) = Vec3_HorzAngle((char*)*(void**)(c+0x388)+0x5c, c+0x5c);
    int rnd = RandomIntInternal(RNG_STATE);
    *(short*)(c+0x94) = *(short*)(c+0x94) + ((rnd & 0x7fff) - 0x4000);
    *(short*)(c+0x8e) = *(short*)(c+0x94) + 0x8000;
    *(int*)(c+0xa8) = 0x28000;
    *(int*)(c+0x98) = 0xa000;
    *(unsigned char*)(c+0x3ac) = 0x2d;
    Obj* o = (Obj*)c;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, *(int*)(c+0x5c), *(int*)(c+0x60)+o->GetY(), *(int*)(c+0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, *(int*)(c+0x5c), *(int*)(c+0x60)+o->GetY(), *(int*)(c+0x64));
    *(int*)(c+0x38c) = 5;
  }
  return 1;
}
