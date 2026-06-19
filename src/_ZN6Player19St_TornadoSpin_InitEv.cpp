//cpp
extern "C" {
extern int func_ov002_020dab14(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int data_0209f318[];
extern void func_0200d6b4(void*,unsigned char);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
int _ZN6Player19St_TornadoSpin_InitEv(char* c){
  func_ov002_020dab14(c);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x5f,0,0x1000,0);
  *(int*)(c+0xa8)=0;
  *(int*)(c+0x9c)=0;
  *(unsigned char*)(c+0x6de)=1;
  *(unsigned char*)(c+0x6df)=0;
  func_0200d6b4(*(void**)data_0209f318, *(unsigned char*)(c+0x6d8));
  *(int*)(c+0x688)=*(int*)(c+0x60) - *(int*)(*(int*)(c+0x364)+0x60);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9),0x26,(void*)(c+0x74));
  return 1;
}
}
