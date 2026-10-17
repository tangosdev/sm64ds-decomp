/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov022_02112f78_AssetId {
    ASSET_FL_UKI_KI_BMD = 0x0612,
    ASSET_FL_UKI_KI_KCL = 0x0613
};

extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int data_ov022_02114620[];
extern int func_02017ab4[];
extern int data_ov022_02114634[];
extern int data_ov022_02114618[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int data_ov022_02114628[];
void __sinit_ov022_02112f78(void)
{
    func_02017acc(data_ov022_02114620, ASSET_FL_UKI_KI_BMD);
    func_020731dc(data_ov022_02114620, func_02017ab4, data_ov022_02114634);
    func_02017b4c(data_ov022_02114618, ASSET_FL_UKI_KI_KCL);
    func_020731dc(data_ov022_02114618, SharedFilePtr_Destruct_Clsn, data_ov022_02114628);
}
