//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
void func_ov002_020c9840(char* c){
  *(unsigned char*)(c+0x71a) = 0;
  *(unsigned char*)(c+0x6e3) = 0x18;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
  *(unsigned short*)(c+0x600+0xa4) = 2;
  *(int*)(c+0x98) = 0;
  *(int*)(c+0xa8) = 0;
  *(unsigned char*)(c+0x70c) = 0;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 4, (void*)(c+0x74));
}
}
