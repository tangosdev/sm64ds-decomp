extern void _Z14ApproachLinearRiii(int* x, int target, int step);
extern void func_0203d6d0(int* o, int* a, int* b);
extern void func_ov004_020b1b40(int c);
extern void func_ov004_020b5334(char* c);
extern int data_ov004_020bfa18;

void func_ov004_020b4c30(char* r4){
  int v[2];
  _Z14ApproachLinearRiii((int*)(r4 + 8), *(int*)(r4 + 0x10), *(int*)(r4 + 0x18));
  _Z14ApproachLinearRiii((int*)(r4 + 0xc), *(int*)(r4 + 0x14), *(int*)(r4 + 0x1c));
  func_0203d6d0(v, (int*)(r4 + 0x10), (int*)(r4 + 8));
  if (v[0] != 0) return;
  if (v[1] != 0) return;
  _Z14ApproachLinearRiii(&data_ov004_020bfa18, 0, 1);
  func_ov004_020b1b40(1);
  func_ov004_020b5334(r4);
}
