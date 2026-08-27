extern void _Z14ApproachLinearRiii(int *dst, int target, int step);
extern int data_ov004_020bc7d0;
extern int data_ov004_020bfa18;
extern int data_ov004_020bfa1c;
extern int data_ov004_020bfa00;
extern int data_ov004_020bfa04;
extern int data_ov004_020bfa10;
extern void func_ov004_020b5f6c(void);
extern void *data_ov004_020bfa20;
void func_ov004_020b6324(int c) {
    data_ov004_020bc7d0 = 5;
    _Z14ApproachLinearRiii(&data_ov004_020bfa18, 5, c);
    data_ov004_020bfa00 = 0;
    data_ov004_020bfa1c = c;
    data_ov004_020bfa10 = 1;
    data_ov004_020bfa04 = 0;
    data_ov004_020bfa20 = (void*)&func_ov004_020b5f6c;
}
