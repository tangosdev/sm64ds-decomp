extern void func_ov007_020b283c(void);
extern void func_ov007_020b76a0(void);
extern void func_ov007_020bcba8(int i);
extern void func_ov007_020bca40(int i);
extern void func_ov007_020bd1c0(int i);
extern void func_ov007_020bfacc(int a, int b, int c);
extern void func_ov007_020aec00(void);
extern void func_ov007_020c1d78(int i);
extern char data_ov007_0210342c[];

void func_ov007_020af434(void) {
  char* p34;
  char* a;
  (*(void(**)(int))(*(char**)data_ov007_0210342c + 0x2c))(0x12);
  func_ov007_020b283c();
  func_ov007_020b76a0();
  func_ov007_020bcba8(3);
  func_ov007_020bca40(3);
  func_ov007_020bd1c0(2);
  func_ov007_020bfacc(1, 2, 0);
  func_ov007_020aec00();
  p34 = *(char**)(*(char**)(*(char**)data_ov007_0210342c + 0x74) + 0x34);
  a = *(char**)(p34 + 0x18);
  *(int*)(a+0x28) = 0;
  *(int*)(a+0x2c) = 0;
  *(int*)(a+0x24) = 0;
  *(int*)(a+0x20) = *(int*)(a+0x24);
  a = *(char**)(p34 + 0x1c);
  *(int*)(a+0x28) = 0;
  *(int*)(a+0x2c) = 0;
  *(int*)(a+0x24) = 0;
  *(int*)(a+0x20) = *(int*)(a+0x24);
  func_ov007_020c1d78(0);
}
