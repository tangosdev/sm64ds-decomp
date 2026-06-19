//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int,void*);
extern int func_ov002_020e25f0(void*,int);
int _ZN6Player17St_WaterJump_InitEv(char* c){
  *(char*)(c+0x71b)=0;
  *(char*)(c+0x712)=1;
  *(char*)(c+0x70d)=0;
  *(char*)(c+0x6e1)=0;
  *(char*)(c+0x6de)=1;
  *(char*)(c+0x6df)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x53,0x40000000,0x1000,0);
  *(int*)(c+0xa8)=0x2a000;
  *(int*)(c+0x98)=0x12000;
  _ZN5Sound9PlayBank0EjRK7Vector3(0x18,(char*)c+0x74);
  func_ov002_020e25f0(c,0);
  return 1;
}
}
