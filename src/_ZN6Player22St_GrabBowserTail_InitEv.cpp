//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern void func_0200d10c(void* cam, unsigned char id);
extern int* CAMERA;
int _ZN6Player22St_GrabBowserTail_InitEv(char* c){
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x81,0x40000000,0x1000,0);
  *(unsigned char*)(c+0x6e3)=0;
  *(short*)(c+0x69c)=0;
  *(unsigned char*)(c+0x6e5)=0;
  *(unsigned char*)(c+0x70c)=0;
  func_0200d10c((void*)CAMERA, *(unsigned char*)(c+0x6d8));
  return 1;
}
}
