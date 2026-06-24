extern int func_02017acc();
extern void func_020731dc();
extern int func_02017b4c();
extern char data_ov015_02114adc[];
extern char data_ov015_02114ae4[];
extern char data_ov015_02114ad4[];
extern char data_ov015_02114af0[];
extern void func_02017ab4(void);
extern void func_02017b34(void);
void __sinit_ov015_021133a8(void) {
    func_02017acc(data_ov015_02114adc, 0x58f);
    func_020731dc(data_ov015_02114adc, func_02017ab4, data_ov015_02114ae4);
    func_02017b4c(data_ov015_02114ad4, 0x590);
    func_020731dc(data_ov015_02114ad4, func_02017b34, data_ov015_02114af0);
}
