//cpp
struct Vector3 { int x,y,z; };
extern "C" {
extern int _ZN6Player12ST_BURN_LAVAE[];
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int func_ov006_020e3078(void*,int*);
extern int func_ov002_020d91e0(void*,int,int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,struct Vector3*);
int _ZN6Player16St_BurnLava_InitEv(char* c){
  *(char*)(c+0x712)=1;
  *(char*)(c+0x6e1)=0;
  *(char*)(c+0x6de)=1;
  *(char*)(c+0x6df)=0;
  *(char*)(c+0x708)=1;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x16,0,0x1000,0);
  *(int*)(c+0x98)=0;
  *(int*)(c+0xa8)=0x64000;
  *(char*)(c+0x6e5)=0;
  *(char*)(c+0x6e3)=0;
  if(func_ov006_020e3078(c,_ZN6Player12ST_BURN_LAVAE)==0){
    *(char*)(c+0x70c)=0;
  }
  func_ov002_020d91e0(c,0x300,1);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9),0x23,(struct Vector3*)(c+0x74));
  return 1;
}
}
