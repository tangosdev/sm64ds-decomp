extern int _Z14ApproachLinearRiii(int* p, int a, int b);
extern int func_ov004_020ae1a8(void);
extern void func_ov004_020adb1c(int self);
extern int data_ov004_020bfa04;
extern char* data_ov004_020beb68;
extern int data_ov004_020bfa20;
extern short data_ov004_020bf9e4;
void func_ov004_020b5368(void){
  if(_Z14ApproachLinearRiii(&data_ov004_020bfa04, 0, 1)==0) return;
  if(func_ov004_020ae1a8()==2){
    char* p = data_ov004_020beb68;
    func_ov004_020adb1c(p ? *(int*)(p+0xa8) : 0);
  }
  data_ov004_020bfa04 = 0;
  data_ov004_020bf9e4 = 1;
  data_ov004_020bfa20 = 0;
}
