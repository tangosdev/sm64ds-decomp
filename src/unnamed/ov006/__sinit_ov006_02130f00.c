extern void func_02017acc();
extern void func_020731dc();
extern void func_02017ab4(void);
extern int data_ov006_021421cc[];
extern int data_ov006_021421d4[];
extern int data_ov006_021421c4[];
extern int data_ov006_021421e0[];
void __sinit_ov006_02130f00(void)
{
    func_02017acc(data_ov006_021421cc, 0x203);
    func_020731dc(data_ov006_021421cc, &func_02017ab4, data_ov006_021421d4);
    func_02017acc(data_ov006_021421c4, 0x204);
    func_020731dc(data_ov006_021421c4, &func_02017ab4, data_ov006_021421e0);
}
