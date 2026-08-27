//cpp
extern "C" {
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* thiz, unsigned int a, int b, int fix, unsigned int d);
extern short GetAngleToCamera(int i);
void func_ov002_020c9d68(char* c){
  if(*(unsigned char*)(c+0x6de) == 0){
    int b = (int)(*(int*)(c+0x354) != 0);
    if(b == 0) goto tail;
  }
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x54, 0x40000000, 0x1000, 0);
tail:
  *(short*)(c+0x8e) = GetAngleToCamera(*(unsigned char*)(c+0x6d8));
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  *(unsigned char*)(c+0x6e3) = 0;
  *(unsigned char*)(c+0x70c) = 0;
  *(int*)(c+0x98) = 0;
}
}
