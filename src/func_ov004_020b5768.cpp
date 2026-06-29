//cpp
int ApproachLinear(int &, int, int);
extern "C" {
extern void func_ov004_020b4cc4(char* r4);
extern int data_ov004_020bf9fc;
extern int data_ov004_020bfa04;
extern int data_ov004_020bfa00;
extern char data_ov004_020bfa34[];
extern int data_ov004_020bfa08;
extern unsigned char data_ov004_020bfa56[];
extern short data_ov004_020bf9e4;
extern int data_ov004_020bfa20;

void func_ov004_020b5768() {
    if (data_ov004_020bf9fc > 0) {
        if (ApproachLinear(data_ov004_020bfa04, 0, 1) == 0)
            return;
        func_ov004_020b4cc4(&data_ov004_020bfa34[data_ov004_020bfa00 * 0x24]);
        data_ov004_020bfa08 = data_ov004_020bfa00;
        ApproachLinear(data_ov004_020bfa00, 0, 1);
        if (ApproachLinear(data_ov004_020bf9fc, 0, 1) == 0) {
            data_ov004_020bfa04 = 0xc;
        }
        return;
    }
    if (data_ov004_020bfa56[data_ov004_020bfa08 * 0x24] != 0)
        return;
    data_ov004_020bf9e4 = 1;
    data_ov004_020bfa20 = 0;
}
}
