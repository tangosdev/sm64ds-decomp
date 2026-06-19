//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
int _ZN6Player21St_JumpQuicksand_InitEv(char* c){
  *(unsigned char*)(c+0x71b)=1;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x53,0x40000000,0x1000,0);
  *(unsigned char*)(c+0x6e5)=0;
  *(int*)(c+0xa8)=0;
  *(unsigned char*)(c+0x6de)=1;
  *(unsigned char*)(c+0x6df)=0;
  return 1;
}
}
