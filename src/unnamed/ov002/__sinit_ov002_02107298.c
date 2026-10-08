extern void func_02017acc();
extern void func_020731dc();
extern void SharedFilePtr_Construct_TexSeq();
extern int data_ov002_02110aa4[];
extern int func_02017ab4[];
extern int data_ov002_02110aac[];
extern int data_ov002_02110a9c[];
extern int SharedFilePtr_Destruct_TexSeq[];
extern int data_ov002_02110ab8[];
void __sinit_ov002_02107298(void)
{
    func_02017acc(data_ov002_02110aa4, 1078);
    func_020731dc(data_ov002_02110aa4, func_02017ab4, data_ov002_02110aac);
    SharedFilePtr_Construct_TexSeq(data_ov002_02110a9c, 1079);
    func_020731dc(data_ov002_02110a9c, SharedFilePtr_Destruct_TexSeq, data_ov002_02110ab8);
}
