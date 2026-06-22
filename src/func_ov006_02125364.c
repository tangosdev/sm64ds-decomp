extern int func_ov004_020ad878(void);
extern void func_ov004_020adb1c(int self);
extern void func_ov004_020b66d4(void);
extern int func_020bc7d4;
void func_ov006_02125364(char* c, int mode){
  if(mode != 4 && mode != 5 && mode == 3){
    *(int*)(c+0xb4) = func_ov004_020ad878();
    func_ov004_020adb1c(*(int*)(c+0xb4));
  }
  func_ov004_020b66d4();
  func_020bc7d4 = 1;
  *(int*)(c+0x5000+0x1b8) = 0;
}
