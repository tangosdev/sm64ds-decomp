/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov022_02113050_AssetId {
    ASSET_FL_LOG_BMD = 0x05fb,
    ASSET_FL_LOG_KCL = 0x05fc
};

extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern int gFlLogModelFile[];
extern int func_02017ab4[];
extern int data_ov022_02114678[];
extern int gFlLogCollisionFile[];
extern int SharedFilePtr_Destruct_Clsn[];
extern int data_ov022_02114684[];
void __sinit_ov022_02113050(void)
{
    func_02017acc(gFlLogModelFile, ASSET_FL_LOG_BMD);
    func_020731dc(gFlLogModelFile, func_02017ab4, data_ov022_02114678);
    func_02017b4c(gFlLogCollisionFile, ASSET_FL_LOG_KCL);
    func_020731dc(gFlLogCollisionFile, SharedFilePtr_Destruct_Clsn, data_ov022_02114684);
}
