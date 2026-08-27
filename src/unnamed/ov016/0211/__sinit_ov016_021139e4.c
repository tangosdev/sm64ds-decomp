extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int data_ov016_02114e44[];
extern int func_02017ab4[];
extern int data_ov016_02114e54[];
extern int data_ov016_02114e4c[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int data_ov016_02114e60[];
void __sinit_ov016_021139e4(void)
{
    func_02017acc(data_ov016_02114e44, 1601);
    func_020731dc(data_ov016_02114e44, func_02017ab4, data_ov016_02114e54);
    func_02017b4c(data_ov016_02114e4c, 1602);
    func_020731dc(data_ov016_02114e4c, SharedFilePtr_Destruct_Clsn, data_ov016_02114e60);
}
