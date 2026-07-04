//cpp
extern "C" {
struct State;
extern State data_ov002_021100c4;
extern int func_ov002_020d82f0(void* c);
extern int func_ov002_020d91e0(void* c, int a, int b, int d);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, State* st);
int _ZN6Player5ShockEj(char* c, unsigned int j){
  if (func_ov002_020d82f0(c) == 0) return 0;
  if (*(unsigned char*)(c+0x6f9)) j = 0;
  *(unsigned char*)(c+0x6e5) = 0;
  if (func_ov002_020d91e0(c, j<<8, 1, 0)) *(unsigned char*)(c+0x6e5) = 1;
  if (*(unsigned char*)(c+0x706)) {
    unsigned char* p = (unsigned char*)(((int)c + 0x6e5) & 0xFFFFFFFFFFFFFFFF);
    *p |= 2;
  }
  _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021100c4);
  return 1;
}
}
