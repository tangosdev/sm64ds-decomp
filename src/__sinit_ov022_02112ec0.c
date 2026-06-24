extern int data_ov022_021145e0[];
extern int data_ov022_021145e8[];
extern int data_ov022_021145d0[];
extern int data_ov022_021145f4[];
extern int data_ov022_021145d8[];
extern int data_ov022_02114600[];
extern int data_ov022_021145c8[];
extern int data_ov022_0211460c[];
extern int func_02017ab4[];
extern int func_02017b34[];

extern int func_02017acc();
extern void func_020731dc();
extern int func_02017b4c();

void __sinit_ov022_02112ec0(void)
{
    func_02017acc(data_ov022_021145e0, 0x614);
    func_020731dc(data_ov022_021145e0, func_02017ab4, data_ov022_021145e8);
    func_02017b4c(data_ov022_021145d0, 0x615);
    func_020731dc(data_ov022_021145d0, func_02017b34, data_ov022_021145f4);
    func_02017acc(data_ov022_021145d8, 0x610);
    func_020731dc(data_ov022_021145d8, func_02017ab4, data_ov022_02114600);
    func_02017b4c(data_ov022_021145c8, 0x611);
    func_020731dc(data_ov022_021145c8, func_02017b34, data_ov022_0211460c);
}
