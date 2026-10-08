//cpp
extern "C" {
struct State;
extern State data_ov002_02110244;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int func_ov002_020df7f4(char* c) {
  if (!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110244) || *(int*)(c + 0x358) == 0)
    return -1;
  return *(unsigned char*)(c + 0x6e3) != 0;
}
}
