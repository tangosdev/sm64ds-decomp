//cpp
extern "C" {
extern void func_ov002_020dab14(void*);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned,unsigned,void*);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern void func_0200d768(void*,unsigned char);
extern int CAMERA[];
int _ZN6Player12St_Swim_InitEv(char* c){
  func_ov002_020dab14(c);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9),0x2e,c+0x74);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0xa7,0x40000000,0x1000,0);
  *(char*)(c+0x6e3)=0;
  *(int*)(c+0x9c)=0;
  *(int*)(c+0xa0)=-0xc000;
  *(char*)(c+0x706)=1;
  func_0200d768(*(void**)CAMERA,*(unsigned char*)(c+0x6d8));
  *(int*)(c+0x640)=*(int*)(c+0xa8);
  *(int*)(c+0x98)=0;
  *(int*)(c+0xa8)=0;
  *(short*)(c+0x69c)=0;
  *(char*)(c+0x6e5)=0;
  *(char*)(c+0x70c)=0;
  *(short*)(c+0x6a8)=0;
  return 1;
}
}
