//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
int _ZN6Player21St_HeadstandJump_InitEv(char* c){
  *(char*)(c+0x71b)=0;
  *(char*)(c+0x712)=1;
  *(char*)(c+0x70d)=0;
  *(char*)(c+0x6e1)=0;
  *(char*)(c+0x6de)=1;
  *(char*)(c+0x6df)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x1c,0x40000000,0x1000,0);
  *(int*)(c+0xa8)=0x3e000;
  *(int*)(c+0x98)=0x18000;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9),0xc,(char*)c+0x74);
  return 1;
}
}
