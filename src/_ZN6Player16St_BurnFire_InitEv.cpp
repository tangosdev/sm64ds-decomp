//cpp
struct Vector3 { int x,y,z; };
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,struct Vector3*);
int _ZN6Player16St_BurnFire_InitEv(char* c){
  *(char*)(c+0x708)=1;
  *(char*)(c+0x6de)=1;
  *(char*)(c+0x6df)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x16,0,0x1000,0);
  *(char*)(c+0x6e3)=0;
  *(short*)(c+0x6a4)=0x51;
  if(*(int*)(c+0x658)!=1){
    *(int*)(c+0xa8)=0x1e000;
  } else {
    *(int*)(c+0x60)=*(int*)(c+0x644);
  }
  *(int*)(c+0x98)=0;
  *(char*)(c+0x6e5)=0;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9),0x23,(struct Vector3*)(c+0x74));
  return 1;
}
}
