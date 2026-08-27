extern int func_0205d5e8(int a, int b, int c, int d, int e);
extern void func_02018e68(void* c);
extern void Crash(void);
extern int data_0208ecc4;
int func_02018e00(int a, int b, int c, int d, int e){
  int r = func_0205d5e8(a, b, c, d, e);
  if(r == 0){
    func_02018e68(&data_0208ecc4);
    Crash();
  }
  return r;
}
