extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, int arg6);
extern void func_ov004_020ad90c(void);
struct P2 { int a, b; };
extern struct P2 data_ov004_020bc9cc;
extern struct P2 data_ov004_020bca2c;
void func_ov004_020b6f14(char* c) {
  func_ov004_020b0cac(7, 0x80, 0x14, -1, -1, 0xd);
  *(int*)(c+0x1c) = 0x12c;
  *(struct P2*)(c+8) = data_ov004_020bc9cc;
  *(struct P2*)(c+0x10) = data_ov004_020bca2c;
  func_ov004_020ad90c();
}
