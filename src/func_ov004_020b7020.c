extern int func_ov004_020adbe0(void);
extern void func_ov004_020b6f14(void*);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, int arg6);
extern void func_ov004_020ad90c(void);
struct P2 { int a, b; };
extern struct P2 data_ov004_020bc8e4;
extern struct P2 data_ov004_020bc91c;
void func_ov004_020b7020(char* c) {
  if (func_ov004_020adbe0()) {
    func_ov004_020b6f14(c);
    return;
  }
  func_ov004_020b0cac(7, 0x80, 0x14, -1, -1, 0xd);
  *(int*)(c+0x1c) = 0x78;
  *(struct P2*)(c+8) = data_ov004_020bc8e4;
  *(struct P2*)(c+0x10) = data_ov004_020bc91c;
  func_ov004_020ad90c();
}
