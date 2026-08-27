extern int func_0205d518(void);
extern void func_02018e68(void*);
extern void Crash(void);
extern int data_0208eb8c[];
int func_02018d98(void){
  int r = func_0205d518();
  if(!r){
    func_02018e68(data_0208eb8c);
    Crash();
  }
  return r;
}
