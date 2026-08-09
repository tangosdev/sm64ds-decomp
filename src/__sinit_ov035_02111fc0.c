extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int RotatingClockHand_ModelFile[];
extern int func_02017ab4[];
extern int data_ov035_02112cc0[];
extern int data_ov035_02112cb8[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int data_ov035_02112ccc[];
void __sinit_ov035_02111fc0(void)
{
    func_02017acc(RotatingClockHand_ModelFile, 1488);
    func_020731dc(RotatingClockHand_ModelFile, func_02017ab4, data_ov035_02112cc0);
    func_02017b4c(data_ov035_02112cb8, 1489);
    func_020731dc(data_ov035_02112cb8, SharedFilePtr_Destruct_Clsn, data_ov035_02112ccc);
}
