//cpp
extern "C" {
typedef int Fix12i;
extern void _ZN9Animation7AdvanceEv(void* a);
extern void func_ov002_020e8098(void* c);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, Fix12i v);
void func_ov002_020ea06c(char* c){
  int* s = (int*)(((int)*(void**)(c+0x438) + 0x5c));
  *(int*)(c+0x5c) = s[0];
  *(int*)(c+0x60) = s[1];
  *(int*)(c+0x64) = s[2];
  *(short*)(c+0x94) = *(short*)((char*)*(void**)(c+0x438) + 0x8e);
  *(int*)(c+0x440) = 7;
  *(short*)(c+0x490) = 0;
  *(unsigned char*)(c+0x49b) = 0;
  _ZN9Animation7AdvanceEv(c+0x35c);
  func_ov002_020e8098(c);
  if (*(int*)(c+0x43c) == 9) {
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0, 0x7f000);
  }
  ++*(unsigned char*)(((int)c + 0x4a1));
}
}
