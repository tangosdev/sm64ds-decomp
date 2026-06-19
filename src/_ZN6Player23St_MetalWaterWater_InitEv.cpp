//cpp
extern "C" {
extern void func_ov002_020bf2d8(void*,int);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned,void*);
int _ZN6Player23St_MetalWaterWater_InitEv(char* c){
  *(char*)(c+0x712)=1;
  *(char*)(c+0x706)=1;
  *(char*)(c+0x6de)=1;
  *(char*)(c+0x6df)=0;
  if(*(unsigned char*)(c+0x6e3)==0){
    func_ov002_020bf2d8(c,0x20000);
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x53,0x40000000,0x1000,0);
  }else{
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x54,0x40000000,0x1000,0);
  }
  _ZN5Sound9PlayBank0EjRK7Vector3(0xa8,c+0x74);
  *(char*)(c+0x6e5)=0;
  return 1;
}
}
