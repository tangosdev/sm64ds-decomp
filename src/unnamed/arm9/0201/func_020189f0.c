extern void func_02018e3c(void*, int);
extern int data_0209d3b4;
int func_020189f0(int a){
  int buf[3];
  func_02018e3c(buf, a);
  data_0209d3b4 = buf[0];
  return buf[1];
}
