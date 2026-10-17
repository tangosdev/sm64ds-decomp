/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov079_02127a10_AssetId {
    ASSET_KILLER_BODY_BMD = 0x0352,
    ASSET_KILLER_FACE_BMD = 0x0353,
    ASSET_BK_KILLER_DAI_BMD = 0x0597,
    ASSET_BK_KILLER_DAI_KCL = 0x0598
};

extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int data_ov079_021282f0[];
extern int func_02017ab4[];
extern int data_ov079_0212831c[];
extern int data_ov079_02128300[];
extern int data_ov079_02128328[];
extern int gBkKillerDaiModelFile[];
extern int data_ov079_02128334[];
extern int gBkKillerDaiCollisionFile[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int data_ov079_02128310[];
void __sinit_ov079_02127a10(void)
{
    func_02017acc(data_ov079_021282f0, ASSET_KILLER_FACE_BMD);
    func_020731dc(data_ov079_021282f0, func_02017ab4, data_ov079_0212831c);
    func_02017acc(data_ov079_02128300, ASSET_KILLER_BODY_BMD);
    func_020731dc(data_ov079_02128300, func_02017ab4, data_ov079_02128328);
    func_02017acc(gBkKillerDaiModelFile, ASSET_BK_KILLER_DAI_BMD);
    func_020731dc(gBkKillerDaiModelFile, func_02017ab4, data_ov079_02128334);
    func_02017b4c(gBkKillerDaiCollisionFile, ASSET_BK_KILLER_DAI_KCL);
    func_020731dc(gBkKillerDaiCollisionFile, SharedFilePtr_Destruct_Clsn, data_ov079_02128310);
}
