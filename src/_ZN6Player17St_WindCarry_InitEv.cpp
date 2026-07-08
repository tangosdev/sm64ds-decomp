//cpp
extern "C" {
extern int func_ov002_020da9d4(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int CAMERA[];
extern void func_0200d63c(void*,unsigned char);
int _ZN6Player17St_WindCarry_InitEv(char* c){
  func_ov002_020da9d4(c);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x4a,0x40000000,0x1000,0);
  *(int*)(c+0xa8)=0;
  *(unsigned char*)(c+0x6de)=1;
  *(unsigned char*)(c+0x6df)=0;
  func_0200d63c((void*)CAMERA[0], *(unsigned char*)(c+0x6d8));
  return 1;
}
}
