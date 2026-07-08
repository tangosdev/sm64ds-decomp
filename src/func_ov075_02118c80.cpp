//cpp
extern void func_ov075_02114fa8(void*);
extern void func_0203d918(void);
extern int *DP_STATE;
extern char data_0208a0e0[];
extern "C" void func_ov075_02118c80(char *c) {
  func_ov075_02114fa8(*(void**)(c + 0x50));
  *(int*)(c + 0x264) = 0xf0;
  *(char*)(c + 0x282) = 0;
  *(int*)(c + 0x268) = 0;
  if (DP_STATE != 0) {
    data_0208a0e0[0] = 4;
  }
  func_0203d918();
}
