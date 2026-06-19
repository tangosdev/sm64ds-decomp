//cpp
extern "C" {
extern int func_ov002_020c9e40(void*);
extern int func_ov002_020dab14(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
int _ZN6Player14St_Cannon_InitEv(char* c){
  func_ov002_020c9e40(c);
  *(unsigned char*)(c+0x706)=0;
  *(unsigned char*)(c+0x712)=1;
  *(unsigned char*)(c+0x70d)=0;
  *(unsigned char*)(c+0x6de)=1;
  *(unsigned char*)(c+0x6df)=0;
  func_ov002_020dab14(c);
  *(unsigned char*)(c+0x6e3)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x73,0,0x1000,0);
  *(int*)(c+0x9c)=-0x1200;
  *(unsigned char*)(c+0x6f5)=0;
  *(unsigned char*)(c+0x713)=0;
  *(unsigned char*)(c+0x6e5)=0;
  *(unsigned char*)(c+0x6f6)=1;
  return 1;
}
}
