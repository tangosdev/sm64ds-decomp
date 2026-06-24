extern void _Z14ApproachLinearRiii(int* p, int t, int step);
extern void func_0203d6d0(int* o, int* a, int* b);
extern void func_ov004_020b1b40(int c);
extern void func_ov004_020b51e4(char* p);
struct P2 { int a, b; };
extern struct P2 data_ov004_020bc7f0;
void func_ov004_020b51f0(char* c) {
  int out[2];
  _Z14ApproachLinearRiii((int*)(c + 8), *(int*)(c + 0x10), *(int*)(c + 0x18));
  _Z14ApproachLinearRiii((int*)(c + 0xc), *(int*)(c + 0x14), *(int*)(c + 0x1c));
  func_0203d6d0(out, (int*)(c + 0x10), (int*)(c + 8));
  if (out[0] != 0) return;
  if (out[1] != 0) return;
  func_ov004_020b1b40(1);
  func_ov004_020b51e4(c);
  *(struct P2*)c = data_ov004_020bc7f0;
}
