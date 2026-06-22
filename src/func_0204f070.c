extern int data_020a4d44;
extern void func_0205a82c(void);
extern void func_0204f1e8(void);
extern void func_02050950(void);
extern void func_0204fc40(void);

void func_0204f070(void){
  if (data_020a4d44 != 0) return;
  data_020a4d44 = 1;
  func_0205a82c();
  func_0204f1e8();
  func_02050950();
  func_0204fc40();
}
