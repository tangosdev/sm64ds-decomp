extern void func_ov007_020bfacc(int a, int b, void* p);
extern void func_ov007_020c1d8c(int i);
extern char data_ov007_0210342c[];
extern char func_ov007_020b1604[];

void func_ov007_020b155c(void) {
  char* g = *(char**)data_ov007_0210342c;
  if (*(int*)(*(char**)(g+8) + 0xc) != 0) return;
  {
    int v = *(int*)(g+0x10);
    if (v == 8) {
      func_ov007_020bfacc(1, 0, func_ov007_020b1604);
    } else {
      if ((unsigned int)(v - 0xa) <= 1) {
        func_ov007_020bfacc(2, 0, func_ov007_020b1604);
      } else {
        func_ov007_020bfacc(1, 0, func_ov007_020b1604);
      }
      (*(void(**)(int))(*(char**)data_ov007_0210342c + 0x2c))(0x13);
    }
  }
  func_ov007_020c1d8c(0);
}
