extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int data_ov036_0211408c[];
extern int func_02017ab4[];
extern int data_ov036_021140a0[];
extern int data_ov036_02114084[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int data_ov036_02114094[];
void __sinit_ov036_021126c8(void)
{
    func_02017acc(data_ov036_0211408c, 0x6af);
    func_020731dc(data_ov036_0211408c, func_02017ab4, data_ov036_021140a0);
    func_02017b4c(data_ov036_02114084, 0x6b0);
    func_020731dc(data_ov036_02114084, SharedFilePtr_Destruct_Clsn, data_ov036_02114094);
}
