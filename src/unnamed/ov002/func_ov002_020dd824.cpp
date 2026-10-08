//cpp
extern "C" {
struct State;
extern State data_ov002_0211034c;
extern int func_ov002_020d82f0(char* c);
extern int _ZN6Player11ChangeStateERNS_5StateE(void* c, State* st);
int func_ov002_020dd824(char* c){
  if (*(unsigned char*)(c+0x6de)) return 0;
  if (func_ov002_020d82f0(c) == 0) return 0;
  if (*(unsigned char*)(c+0x6f9)) return 0;
  if (*(int*)(c+0x2f8) == 0) return 0;
  if (*(int*)(c+0x2f4) & 0x10000) {
    return _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211034c) != 0;
  }
  return 0;
}
}
