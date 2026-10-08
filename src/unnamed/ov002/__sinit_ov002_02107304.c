extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int data_ov002_02110acc[];
extern int func_02017ab4[];
extern int data_ov002_02110ae0[];
extern int data_ov002_02110ac4[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int data_ov002_02110ad4[];
void __sinit_ov002_02107304(void)
{
    func_02017acc(data_ov002_02110acc, 1084);
    func_020731dc(data_ov002_02110acc, func_02017ab4, data_ov002_02110ae0);
    func_02017b4c(data_ov002_02110ac4, 1085);
    func_020731dc(data_ov002_02110ac4, SharedFilePtr_Destruct_Clsn, data_ov002_02110ad4);
}
