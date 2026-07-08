//cpp
extern "C" {
extern int CAMERA[];
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern void func_0200d6b4(void*,unsigned char);
extern void func_ov002_020bd928(char*,unsigned int);
int _ZN6Player11St_Owl_InitEv(char* c){
  *(int*)(c+0xa0)=-0x4b000;
  *(int*)(c+0x9c)=0;
  *(unsigned char*)(c+0x6e3)=0;
  *(int*)(c+0x98)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x5b,0x40000000,0x1000,0);
  func_0200d6b4((void*)CAMERA[0], *(unsigned char*)(c+0x6d8));
  func_ov002_020bd928(c,0x2f);
  return 1;
}
}
