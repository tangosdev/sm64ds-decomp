extern void func_02017acc();
extern void func_020731dc();
extern void func_02017ab4(void);
extern int data_ov006_02142190[];
extern int data_ov006_02142198[];
extern int data_ov006_02142188[];
extern int data_ov006_021421a4[];
void __sinit_ov006_02130e9c(void)
{
    func_02017acc(data_ov006_02142190, 0x203);
    func_020731dc(data_ov006_02142190, &func_02017ab4, data_ov006_02142198);
    func_02017acc(data_ov006_02142188, 0x204);
    func_020731dc(data_ov006_02142188, &func_02017ab4, data_ov006_021421a4);
}
