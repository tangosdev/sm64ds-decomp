struct P { int a, b; };
extern void func_02018e3c(struct P *buf, int x);
extern void func_020182ec(struct P p, int c);
void func_020182bc(int arg){
  struct P buf;
  func_02018e3c(&buf, arg);
  func_020182ec(buf, 1);
}
