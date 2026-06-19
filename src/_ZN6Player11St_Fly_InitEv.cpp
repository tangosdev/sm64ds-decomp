//cpp
extern "C" {
extern int data_0209f318[];
extern void func_ov002_020dab14(char*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern void func_0200d6f0(void*,unsigned char);
int _ZN6Player11St_Fly_InitEv(char* c){
  *(short*)(c+0x69e)=0;
  *(short*)(c+0x69c)=0;
  *(short*)(c+0x92)=0;
  *(short*)(c+0x96)=0;
  *(short*)(c+0x94)=*(short*)(c+0x8e);
  func_ov002_020dab14(c);
  if(*(unsigned char*)(c+0x6e3)==0){
    *(int*)(c+0xa8)=0x5d000;
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x4a,0x40000000,0x1000,0);
    *(int*)(c+0xa0)=-0x4000;
  } else {
    *(int*)(c+0x9c)=0;
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x49,0,0x1000,0);
  }
  func_0200d6f0((void*)data_0209f318[0], *(unsigned char*)(c+0x6d8));
  return 1;
}
}
