extern int func_0205d644(void);
extern void func_02018e68(void*);
extern void Crash(void);
extern int data_0208ecd8[];
int func_02018e3c(void){
  int r = func_0205d644();
  if(!r){
    func_02018e68(data_0208ecd8);
    Crash();
  }
  return r;
}
