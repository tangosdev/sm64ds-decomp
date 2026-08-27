extern int data_ov004_020bc7d4;
extern short data_ov004_020bf9e4;
extern int data_ov004_020bf9fc;
extern int data_ov004_020bfa04;
extern int data_ov004_020bf9e8;
extern int data_ov004_020bfa24;
extern int data_ov004_020bf9f0;
extern void* data_ov004_020bfa20;
extern void func_ov004_020b58c4(void);

void func_ov004_020b5e40(int x){
  int cond = data_ov004_020bc7d4;
  data_ov004_020bf9e4 = 2;
  data_ov004_020bfa04 = 0;
  data_ov004_020bf9fc = x;
  if (cond != 0) {
    data_ov004_020bf9e8 = -x;
    data_ov004_020bfa24 = 1;
  }
  data_ov004_020bf9f0 = 1;
  data_ov004_020bfa20 = (void*)func_ov004_020b58c4;
}
