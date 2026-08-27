extern int func_02018a24(void);
extern void func_020185c0(void *buf, int x);
extern void func_020184e0(void *buf, int a, int b);
void func_02018270(int self, int a, int b){
  char buf[0x44];
  int r = func_02018a24();
  func_020185c0(buf, r);
  func_020184e0(buf, a, b);
}
