extern void CleanCommonModelDataArr(void);
extern void func_ov004_020b0840(int a, int b);
void func_ov006_0210a608(int a, int b) {
    if (b == 2) {
        *(volatile int*)0x40004c8 = 0x296a5800;
        *(volatile int*)0x40004cc = 0x7fff;
        CleanCommonModelDataArr();
    }
    func_ov004_020b0840(a, b);
}
