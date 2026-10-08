//cpp
extern "C" {
/* local extern: the call passes an untyped this, so it cannot use the header method. */
extern void _ZN9dEntObj_c19func_ov075_02114fa8Ev(void*);
extern void func_0203d918(void);
}
extern int *data_0209fc68;
extern char data_0208a0e0[];
extern "C" void func_ov075_02118c80(char *c) {
  _ZN9dEntObj_c19func_ov075_02114fa8Ev(*(void**)(c + 0x50));
  *(int*)(c + 0x264) = 0xf0;
  *(char*)(c + 0x282) = 0;
  *(int*)(c + 0x268) = 0;
  if (data_0209fc68 != 0) {
    data_0208a0e0[0] = 4;
  }
  func_0203d918();
}
