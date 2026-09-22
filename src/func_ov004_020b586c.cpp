//cpp
int ApproachLinear(int& ref, int target, int step);
extern int data_ov004_020bfa04;
extern short data_ov004_020bf9e4;
extern int data_ov004_020bfa20;
extern "C" void func_ov004_020b586c(void) {
    if (ApproachLinear(data_ov004_020bfa04, 0, 1) == 0)
        return;
    data_ov004_020bf9e4 = 1;
    data_ov004_020bfa20 = 0;
}
