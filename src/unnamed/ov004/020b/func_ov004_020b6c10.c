extern int data_ov004_020bc8a0;
extern int data_ov004_020bc880;
extern int data_ov004_020bc884;
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void func_ov004_020ae274(int x);

struct P2 { int a, b; };
extern struct P2 data_ov004_020bca34;

void func_ov004_020b6c10(char* r4){
  int r3, c;
  *(int*)(r4 + 0x1c) = 0xb4;
  r3 = data_ov004_020bc8a0;
  c = 3;
  if (r3 != 0x1d) { data_ov004_020bc8a0 = 0x1d; c = r3; }
  func_ov004_020b0cac(c, data_ov004_020bc880, data_ov004_020bc884, -1, -1, 0xd);
  func_ov004_020ae274(4);
  *(struct P2*)(r4 + 8) = data_ov004_020bca34;
}
