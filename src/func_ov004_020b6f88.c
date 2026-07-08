extern void func_02012790(int x);
extern void func_ov004_020b0aa0(int arg);
extern void func_ov004_020b7460(void* c);
extern unsigned char data_020a0e40;
extern unsigned char TOUCH_INPUT_ARR[];
extern unsigned char data_020a0de9[];
struct P2 { int a, b; };
extern struct P2 data_02086b58;
void func_ov004_020b6f88(char* c) {
  int ok;
  unsigned char idx;
  int i;
  if (*(int*)(c + 0x1c) != 0) return;
  idx = data_020a0e40;
  ok = 0;
  i = idx * 4;
  if (TOUCH_INPUT_ARR[i] != 0) {
    if (data_020a0de9[i] != 0) ok = 1;
  }
  if (ok == 0) return;
  func_02012790(0x62);
  func_ov004_020b0aa0(7);
  *(struct P2*)(c + 0x10) = data_02086b58;
  func_ov004_020b7460(c);
}
