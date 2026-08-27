extern void FS_InitFile(int* s);
extern int func_0205d714(int* buf, int a, int b, int c);

int func_0205d644(int a, int b){
  int buf[0x11];
  FS_InitFile(buf);
  return func_0205d714(buf, b, a, 0) != 0;
}
