extern void func_0205d874(int* s);
extern int func_0205d714(int* buf, int a, int b, int c);

int func_0205d644(int a, int b){
  int buf[0x11];
  func_0205d874(buf);
  return func_0205d714(buf, b, a, 0) != 0;
}
