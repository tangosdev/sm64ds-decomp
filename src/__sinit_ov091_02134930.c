extern int func_02017acc(void* p, int x);
extern void func_020731dc(void* a, void* b, void* node);
extern int func_02017b4c(void* p, int x);
extern int func_02017934(void* p, int x);
extern int func_02017ab4(void);
extern int func_02017b34(void);
extern int func_0201791c(void);
extern int data_ov091_02135620;
extern int data_ov091_02135628;
extern int data_ov091_02135618;
extern int data_ov091_02135640;
extern int data_ov091_02135610;
extern int data_ov091_02135634;

void __sinit_ov091_02134930(void){
  func_02017acc(&data_ov091_02135620, 0x2f4);
  func_020731dc(&data_ov091_02135620, (void*)func_02017ab4, &data_ov091_02135628);
  func_02017b4c(&data_ov091_02135618, 0x2f6);
  func_020731dc(&data_ov091_02135618, (void*)func_02017b34, &data_ov091_02135640);
  func_02017934(&data_ov091_02135610, 0x2f5);
  func_020731dc(&data_ov091_02135610, (void*)func_0201791c, &data_ov091_02135634);
}
