extern int func_ov002_020c9e40(void* c);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int f, unsigned int g);
int _ZN6Player12St_Talk_InitEv(char* c){
  *(unsigned char*)(c+0x6f6)=1;
  func_ov002_020c9e40(c);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x47,0,0x1000,0);
  *(short*)(c+0x69c)=0;
  *(int*)(c+0x98)=0;
  *(unsigned char*)(c+0x70a)=0;
  *(int*)(c+0xa8)=0;
  *(int*)(c+0x9c)=*(int*)(c+0xa8);
  *(unsigned char*)(c+0x716)=1;
  *(unsigned char*)(c+0x713)=0;
  *(int*)(c+0x5c)=*(int*)(c+0x548);
  *(int*)(c+0x60)=*(int*)(c+0x54c);
  *(int*)(c+0x64)=*(int*)(c+0x550);
  *(unsigned char*)(c+0x6e5)=0;
  *(unsigned char*)(c+0x70b)=0;
  return 1;
}
