//cpp
extern "C" {
extern void func_ov002_020c9e40(char*c);
extern int data_ov002_0210a424[];
extern char* CAMERA;
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern void func_0200d89c(char*p);
int _ZN6Player15St_DeadHit_InitEv(char*c){
  func_ov002_020c9e40(c);
  *(unsigned char*)(c+0x713)=0;
  int idx=*(unsigned char*)(c+0x6e3)&1;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,data_ov002_0210a424[idx],0x40000000,0x1000,0);
  *(unsigned char*)(c+0x70c)=0;
  *(unsigned char*)(c+0x6e5)=0;
  func_0200d89c(CAMERA);
  return 1;
}
}
