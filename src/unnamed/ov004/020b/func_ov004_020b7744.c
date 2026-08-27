void func_ov004_020b0cac(int a, int b, int c, int d, int e, int f);
void func_ov004_020ae274(int x);
extern int data_ov004_020bc894;
extern int data_ov004_020bc874;
struct P2 { int a, b; };
extern struct P2 data_ov004_020bc9fc;
void func_ov004_020b7744(char* c){
  *(int*)(c+0x1c) = 0xb4;
  func_ov004_020ae274(3);
  func_ov004_020b0cac(8, data_ov004_020bc894, data_ov004_020bc874, -1, -1, 0xb);
  *(struct P2*)(c+8) = data_ov004_020bc9fc;
}
