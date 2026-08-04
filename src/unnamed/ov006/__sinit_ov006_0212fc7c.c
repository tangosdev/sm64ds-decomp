extern void func_020733a8(void* a, int b, int c, void* d, void* e);
extern void func_020731dc(int a, void* b, void* node);
extern int func_02017acc(void* a, int b);
extern int func_ov006_020d1008(void);
extern int func_ov006_020d100c(void);
extern int func_ov006_020d0fe4(void);
extern int func_02017ab4(void);
extern int data_ov006_02140990[];
extern int data_ov006_021408cc[];
extern int data_ov006_02140850[];
extern int data_ov006_0214092c[];
extern int data_ov006_02140858[];
extern int data_ov006_02140944[];
extern int data_ov006_02140848[];
extern int data_ov006_02140950[];

void __sinit_ov006_0212fc7c(void){
  func_020733a8(data_ov006_02140990, 4, 0x32c, (void*)func_ov006_020d100c, (void*)func_ov006_020d1008);
  func_020731dc(0, (void*)func_ov006_020d0fe4, data_ov006_021408cc);
  func_02017acc(data_ov006_02140850, 0x21a);
  func_020731dc((int)data_ov006_02140850, (void*)func_02017ab4, data_ov006_0214092c);
  func_02017acc(data_ov006_02140858, 0x21b);
  func_020731dc((int)data_ov006_02140858, (void*)func_02017ab4, data_ov006_02140944);
  func_02017acc(data_ov006_02140848, 0x21c);
  func_020731dc((int)data_ov006_02140848, (void*)func_02017ab4, data_ov006_02140950);
}
