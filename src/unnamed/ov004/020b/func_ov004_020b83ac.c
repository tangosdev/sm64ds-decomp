void func_ov004_020b0cac(int a, int b, int c, int d, int e, int f);
void func_ov004_020ae274(int x);
extern int data_ov004_020bc854;
extern int data_ov004_020bc87c;
struct P2 { int a, b; };
extern struct P2 data_ov004_020bc8c4;
void func_ov004_020b83ac(char* c){
  *(int*)(c+0x1c) = 0xb4;
  func_ov004_020b0cac(5, data_ov004_020bc854, data_ov004_020bc87c, -1, -1, 0xd);
  func_ov004_020ae274(1);
  *(struct P2*)(c+8) = data_ov004_020bc8c4;
}
