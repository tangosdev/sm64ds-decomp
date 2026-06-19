//cpp
extern "C" {
extern int func_ov002_020e2b6c(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int func_ov002_020e2ad0(void*);
extern int func_ov002_020e25f0(void*,int);
int _ZN6Player17St_SlopeJump_InitEv(char* c){
  *(unsigned char*)(c+0x71b)=0;
  if(func_ov002_020e2b6c(c)) return 1;
  *(unsigned char*)(c+0x712)=1;
  *(unsigned char*)(c+0x70d)=0;
  *(unsigned char*)(c+0x6e1)=0;
  *(unsigned char*)(c+0x6de)=1;
  *(unsigned char*)(c+0x6df)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x53,0x40000000,0x1000,0);
  *(int*)(c+0xa8)=0x2a000;
  func_ov002_020e2ad0(c);
  if(*(int*)(c+0x98)>0){
    *(int*)(c+0x98)=(int)(((long long)*(int*)(c+0x98)*0xc00+0x800)>>12);
  }
  func_ov002_020e25f0(c,0);
  return 1;
}
}
