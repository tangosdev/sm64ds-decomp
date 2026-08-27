//cpp
extern "C" {
struct State;
extern State data_ov002_0211013c;
extern short _Z15ApproachLinear2Rsss(short& v, short t, short s);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, State* st);
int func_ov002_020c9128(char* c){
  if (*(unsigned char*)(c+0x6e3) == 4) {
    _Z15ApproachLinear2Rsss(*(short*)(c+0x69c), 0x1000, 0x400);
  } else {
    if (_Z15ApproachLinear2Rsss(*(short*)(c+0x69c), 0x4000, 0x100)) {
      _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
      return 1;
    }
  }
  *(short*)(c+0x762) = 0;
  *(short*)(c+0x764) = 0;
  *(short*)(c+0x766) = *(short*)(c+0x69c);
  *(unsigned char*)(c+0x742) = 2;
  return 0;
}
}
