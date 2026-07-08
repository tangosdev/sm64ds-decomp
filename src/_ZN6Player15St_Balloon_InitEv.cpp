//cpp
extern "C" {
extern int func_ov002_020dab14(void*);
extern int CAMERA[];
extern void func_0200d63c(void*,unsigned char);
extern int data_ov002_0210e750[];
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
int _ZN6Player15St_Balloon_InitEv(char* c){
  func_ov002_020dab14(c);
  *(unsigned char*)(c+0x712)=1;
  *(unsigned char*)(c+0x70d)=0;
  if(*(int*)(c+0xa8) >= 0x10000) *(int*)(c+0xa8)=0x10000;
  if(*(int*)(c+0xa8) <= -0x10000) *(int*)(c+0xa8)=-0x10000;
  *(int*)(c+0x98)=0;
  *(int*)(c+0x690)=0x64000;
  func_0200d63c((void*)CAMERA[0], *(unsigned char*)(c+0x6d8));
  *(short*)(c+0x69c)=0x100;
  *(short*)(c+0x69e)=0x100;
  *(int*)(c+0x640)=*(int*)(c+0xa8);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c+0xf0), (void*)data_ov002_0210e750[1], 0x40000000, 0x1000, 0);
  *(int*)(c+0x148)=0;
  return 1;
}
}
