//cpp
extern "C" {
struct State;
extern State data_ov002_021101b4;
extern int func_ov002_020c6538(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, State* st);
int func_ov002_020c6908(char* c){
  if (*(unsigned char*)(c+0x6e5) == 1) {
    int old;
    *(int*)(c+0x60) = *(int*)(c+0x688);
    old = *(int*)(c+0x60);
    if (func_ov002_020c6538(c) == 0) {
      _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
      return 1;
    }
    *(int*)(c+0x60) = old;
    goto ret0;
  } else if (*(unsigned char*)(c+0x6e5) == 2) {
    if (func_ov002_020c6538(c) == 0) {
      _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
      return 1;
    }
    goto ret0;
  }
ret0:
  return 0;
}
}
