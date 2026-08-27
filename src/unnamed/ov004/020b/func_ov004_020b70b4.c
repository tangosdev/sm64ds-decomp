void func_ov004_020b0cac(int a, int b, int c, int d, int e, int f);
void func_ov004_020ae274(int x);
extern int data_ov004_020bc8b8;
extern int data_ov004_020bc8b4;
struct P2 { int a, b; };
extern struct P2 data_ov004_020bca04;
void func_ov004_020b70b4(char* c){
  *(int*)(c+0x1c) = 0x3c;
  func_ov004_020b0cac(0xe, data_ov004_020bc8b8, data_ov004_020bc8b4, -1, -1, 0xd);
  func_ov004_020ae274(3);
  *(struct P2*)(c+8) = data_ov004_020bca04;
}
