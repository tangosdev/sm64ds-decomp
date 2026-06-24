extern int data_ov004_020bc8a0;
extern int data_ov004_020bc88c;
extern int data_ov004_020bc860;
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

struct P2 { int a, b; };
extern struct P2 data_ov004_020bc8ec;

void func_ov004_020b7f5c(char* r4){
  int r3, c;
  *(int*)(r4 + 0x1c) = 0x78;
  r3 = data_ov004_020bc8a0;
  c = 9;
  if (r3 != 0x1d) { data_ov004_020bc8a0 = 0x1d; c = r3; }
  func_ov004_020b0cac(c, data_ov004_020bc88c, data_ov004_020bc860, -1, -1, 0xd);
  _ZN5Sound12PlayBank2_2DEj(0x136);
  *(struct P2*)(r4 + 8) = data_ov004_020bc8ec;
}
