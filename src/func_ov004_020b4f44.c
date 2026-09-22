extern int __aeabi_idiv(int a, int b);
extern int _Z15ApproachLinear2Rsss(short* dst, short to, short step);
extern int _Z14ApproachLinearRiii(int* dst, int to, int step);
extern void func_ov004_020b5334(char* c);
extern int data_ov004_020b9488[];
extern int data_ov004_020bfa18[];

void func_ov004_020b4f44(char* c){
  int* p = (int*)((int)c + 0xc);
  int g = data_ov004_020b9488[0];
  int n = -(((short)*(short*)(c+0x20) - (g >> 1)) << 12);
  int q;
  int v;
  n = p ? n : n;
  q = __aeabi_idiv(n, g);
  v = (int)(((long long)(q - 0x600) * 0x4000 + 0x800) >> 12);
  *p += v;
  if (_Z15ApproachLinear2Rsss((short*)(c+0x20), 0, 1) == 0)
    return;
  _Z14ApproachLinearRiii(data_ov004_020bfa18, 0, 1);
  func_ov004_020b5334(c);
}
