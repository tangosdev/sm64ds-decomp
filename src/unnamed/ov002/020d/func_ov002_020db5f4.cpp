//cpp
extern "C" {
struct State;
extern State data_ov002_0211016c;
extern int func_ov002_020d82f0(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, State* st);
int func_ov002_020db5f4(char* c, char* arg){
  int b = (*(unsigned short*)(arg + 0xc) == 0xbf);
  if (b == 0){
    if (func_ov002_020d82f0(c) == 0) return 0;
  }
  if (*(int*)(c + 0x35c) != 0) return 0;
  *(int*)(c + 0x35c) = (int)arg;
  _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211016c);
  return 1;
}
}
