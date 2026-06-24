extern int data_ov022_021145a8[];
extern int data_ov022_021145bc[];
extern int data_ov022_021145a0[];
extern int data_ov022_021145b0[];
extern int func_02017ab4[];
extern int func_02017b34[];

extern int func_02017acc();
extern void func_020731dc();
extern int func_02017b4c();

void __sinit_ov022_02112e58(void)
{
    func_02017acc(data_ov022_021145a8, 0x610);
    func_020731dc(data_ov022_021145a8, func_02017ab4, data_ov022_021145bc);
    func_02017b4c(data_ov022_021145a0, 0x611);
    func_020731dc(data_ov022_021145a0, func_02017b34, data_ov022_021145b0);
}
