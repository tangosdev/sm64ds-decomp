//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
int _ZN6Player21St_SmallLaunchUp_InitEv(char* c){
  *(unsigned char*)(c+0x71b)=0;
  *(unsigned char*)(c+0x712)=1;
  *(unsigned char*)(c+0x70d)=0;
  *(unsigned char*)(c+0x6e1)=0;
  *(unsigned char*)(c+0x6de)=1;
  *(unsigned char*)(c+0x6df)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x56,0,0x1000,0);
  *(int*)(c+0xa8)=0x2a000;
  *(int*)(c+0x98)=0;
  *(short*)(c+0x8c)=*(short*)(c+0x92);
  *(short*)(c+0x8e)=*(short*)(c+0x94);
  *(short*)(c+0x90)=*(short*)(c+0x96);
  return 1;
}
}
