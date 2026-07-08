typedef unsigned char u8;
extern void func_02013868(int t, int x);
extern void _ZN6Player17SetNoControlStateEhih(void* p, u8 a, int b, u8 c);
extern int SAVE_DATA[];
int func_ov085_0212d038(char* c){
  int t;
  *(int*)(c+0xa4) = 0;
  *(int*)(c+0xa8) = 0;
  *(int*)(c+0xac) = 0;
  *(short*)(c+0x8c) = 0;
  t = *(int*)(c+0x19c);
  if (t != 0x4d) {
    if (t != 7) {
      func_02013868(t, *(int*)(*(char**)(c+0x18c)+8));
      _ZN6Player17SetNoControlStateEhih(*(void**)(c+0x18c), 2, 0x189, 1);
      *(int*)(c+0x194) = 0;
    } else {
      SAVE_DATA[1] |= 0x40;
      _ZN6Player17SetNoControlStateEhih(*(void**)(c+0x18c), 2, 0x18c, 1);
      *(int*)(c+0x194) = 6;
    }
  } else {
    SAVE_DATA[1] |= 0x80;
    _ZN6Player17SetNoControlStateEhih(*(void**)(c+0x18c), 2, 0x149, 1);
    *(int*)(c+0x194) = 6;
  }
  return 1;
}
